#include <FS.h>
#include <SPIFFS.h> // Sistema de archivos en la memoria interna del ESP32
#include <TFT_eSPI.h>
#include <ChronosESP32.h>

TFT_eSPI tft = TFT_eSPI();
ChronosESP32 watch("GPS Moto ESP32");

// VARIABLES DE CONTROL Y TEMPORIZADOR
unsigned long ultimoMensajeTiempo = 0;       
const unsigned long TIEMPO_ESPERA = 20000;   // 20 segundos
bool pantallaActiva = false;                 
bool enLlamada = false;
uint32_t nav_crc = 0xFFFFFFFF;

// Variables para el parpadeo de la llamada
unsigned long ultimoParpadeoTiempo = 0;
bool colorParpadeo = false;
String nombreContacto = "";

// FUNCIÓN AUXILIAR DE LA LIBRERÍA PARA LEER VALORES DEL COMPACTO .BMP
uint16_t read16(File &f) {
  uint16_t result;
  f.read((uint8_t *)&result, sizeof(result));
  return result;
}

uint32_t read32(File &f) {
  uint32_t result;
  f.read((uint8_t *)&result, sizeof(result));
  return result;
}

// FUNCIÓN QUE RENDERIZA EL ARCHIVO .BMP DESDE LA MEMORIA INTERNA (SPIFFS)
void drawBmp(const char *filename, int x, int y) {
  File bmpFile = SPIFFS.open(filename, "r");
  
  if (!bmpFile) {
    Serial.println("Error: No se encontro el archivo BMP");
    return;
  }

  // Comprobar cabecera del BMP (Debe empezar por 'BM')
  if (read16(bmpFile) != 0x4D42) {
    Serial.println("Error: El archivo no es un BMP valido");
    bmpFile.close();
    return;
  }

  read32(bmpFile); // Saltar tamaño del archivo
  read32(bmpFile); // Saltar datos reservados
  uint32_t bmpImageoffset = read32(bmpFile); // Inicio de los píxeles
  read32(bmpFile); // Saltar tamaño de cabecera
  uint32_t imgWidth  = read32(bmpFile);
  uint32_t imgHeight = read32(bmpFile);

  // Asegurarse de que es de 24 bits de color (Formato estándar)
  if (read16(bmpFile) != 1 || read16(bmpFile) != 24) {
    Serial.println("Error: El BMP debe ser de 24 bits");
    bmpFile.close();
    return;
  }

  read32(bmpFile); // Saltar compresión

  // Empezamos a pintar
  uint32_t rowSize = (imgWidth * 3 + 3) & ~3;
  uint8_t sRGB[3];

  tft.setSwapBytes(true); // Ajuste de color para pantallas ST7735/ST7789

  // Los BMP se guardan de abajo hacia arriba, lo leemos en consecuencia
  for (int32_t row = 0; row < imgHeight; row++) {
    uint32_t pos = bmpImageoffset + (imgHeight - 1 - row) * rowSize;
    bmpFile.seek(pos);
    
    for (int32_t col = 0; col < imgWidth; col++) {
      bmpFile.read(sRGB, 3);
      // Convertir color de 24 bits (RGB888) a 16 bits (RGB565 para la TFT)
      uint16_t color = ((sRGB[2] & 0xF8) << 8) | ((sRGB[1] & 0xFC) << 3) | (sRGB[0] >> 3);
      tft.drawPixel(x + col, y + row, color);
    }
  }
  bmpFile.close();
}

// PANTALLA DE BIENVENIDA DE 5 SEGUNDOS
void mostrarPantallaBienvenida() {
  tft.fillScreen(TFT_BLACK);
  
  // Intentamos pintar tu logo.bmp guardado en SPIFFS de forma centrada
  // Cambia el "x" e "y" (0, 0) si tu imagen ya mide 128x160 y ocupa toda la pantalla
  drawBmp("/logo.bmp", 0, 0); 
  
  // Barra de carga cosmética de 5 segundos abajo del todo
  tft.drawRect(14, tft.height() - 20, tft.width() - 28, 8, TFT_DARKGREY);
  for (int i = 0; i < (tft.width() - 32); i++) {
    tft.fillRect(16, tft.height() - 18, i, 4, TFT_GREEN);
    delay(5000 / (tft.width() - 32));
  }
  
  tft.fillScreen(TFT_BLACK);
}

// DIBUJA LA FLECHA (48x48) - ESQUINA SUPERIOR IZQUIERDA
void drawScaledPixel(int x, int y, uint16_t color) {
  int scale = 1; 
  int offsetX = 5;  
  int offsetY = 5;  
  tft.drawPixel(offsetX + x, offsetY + y, color);
}

// CALLBACK DE CONFIGURACIÓN (GOOGLE MAPS)
void configCallback(Config config, uint32_t a, uint32_t b) {
  if (enLlamada) return; 

  switch (config) {
    case CF_NAV_DATA:
      if (a) { 
        Navigation nav = watch.getNavigation();
        
        tft.fillRect(54, 0, tft.width() - 54, 62, TFT_BLACK);
        tft.fillRect(0, 65, tft.width(), tft.height() - 65, TFT_BLACK);
        
        tft.setTextFont(2);
        tft.setTextSize(1);
        tft.setTextColor(TFT_GREEN); 
        tft.setCursor(58, 18); 
        
        String dist = nav.distance;
        if (dist.indexOf(' ') == -1) {
          int index = 0;
          while (index < dist.length() && isDigit(dist[index])) { index++; }
          if (index > 0 && index < dist.length()) {
            dist = dist.substring(0, index) + " " + dist.substring(index);
          }
        }
        tft.print(dist); 
        
        tft.setTextFont(1);
        tft.setTextSize(1);
        tft.setTextColor(TFT_YELLOW); 
        tft.setCursor(58, 44); 
        tft.print(nav.duration);

        tft.setTextFont(1);
        tft.setTextSize(2); 
        tft.setTextColor(TFT_WHITE);
        tft.setCursor(10, 80); 
        tft.println(nav.directions);

        ultimoMensajeTiempo = millis();
        pantallaActiva = true;
      } else {
        tft.fillScreen(TFT_BLACK);
        pantallaActiva = false;
        nav_crc = 0xFFFFFFFF; 
      }
      break;

    case CF_NAV_ICON:
      if (a == 2) { 
        Navigation nav = watch.getNavigation();
        if (nav_crc != nav.iconCRC) {
          nav_crc = nav.iconCRC;

          tft.fillRect(0, 0, 54, 54, TFT_BLACK);

          for (int y = 0; y < 48; y++) {
            for (int x = 0; x < 48; x++) {
              int byte_index = (y * 48 + x) / 8;
              int bit_pos = 7 - (x % 8);
              bool px_on = (nav.icon[byte_index] >> bit_pos) & 0x01;
              
              tft.drawPixel(5 + x, 6 + y, px_on ? TFT_WHITE : TFT_BLACK);
            }
          }
          ultimoMensajeTiempo = millis();
          pantallaActiva = true;
        }
      }
      break;
  }
}

// CALLBACK DE NOTIFICACIONES GENERALES
void notificationCallback(Notification notification) {
  if (enLlamada) return;

  tft.fillScreen(TFT_BLACK);
  tft.setTextFont(1);
  tft.setTextSize(2);
  tft.setCursor(10, 40);
  tft.setTextColor(TFT_GREEN); 
  tft.println(notification.title); 
  tft.setCursor(10, 100);
  tft.setTextColor(TFT_WHITE); 
  tft.println(notification.message); 

  ultimoMensajeTiempo = millis(); 
  pantallaActiva = true;          
}

// CALLBACK DE LLAMADAS (RINGER)
void ringerCallback(String name, bool ringing) {
  if (ringing) {
    enLlamada = true;
    pantallaActiva = true;
    nombreContacto = (name.length() > 0) ? name : "Número Oculto";
  } else {
    enLlamada = false;
    pantallaActiva = false;
    tft.fillScreen(TFT_BLACK); 
    nav_crc = 0xFFFFFFFF; 
  }
  ultimoMensajeTiempo = millis();
}

void gestionarParpadeoLlamada() {
  if (millis() - ultimoParpadeoTiempo >= 350) {
    ultimoParpadeoTiempo = millis();
    colorParpadeo = !colorParpadeo;

    uint16_t fondo = colorParpadeo ? TFT_RED : TFT_BLACK;
    uint16_t textoAlerta = colorParpadeo ? TFT_WHITE : TFT_RED;
    uint16_t textoContacto = colorParpadeo ? TFT_YELLOW : TFT_GREEN;

    tft.fillScreen(fondo);
    tft.setTextFont(1);
    
    tft.setTextSize(2);
    tft.setTextColor(textoAlerta);
    tft.setCursor(15, 15); 
    tft.println("LLAMADA");
    tft.setCursor(15, 40); 
    tft.println("ENTRANTE");

    tft.fillRect(10, 75, tft.width() - 20, 3, textoAlerta);

    tft.setTextSize(2);
    tft.setTextColor(textoContacto);
    tft.setCursor(15, 95); 
    tft.println(nombreContacto);
  }
}

void setup() {
  Serial.begin(115200);
  
  // Inicializar el sistema de archivos de la memoria flash del ESP32
  if(!SPIFFS.begin(true)){
    Serial.println("Error al montar SPIFFS");
  }

  tft.init();
  tft.setRotation(0); 
  
  // Lanza el renderizado del BMP antes de conectar nada
  mostrarPantallaBienvenida();

  watch.setNotificationCallback(notificationCallback);
  watch.setConfigurationCallback(configCallback);
  watch.setRingerCallback(ringerCallback);
  watch.begin();
}

void loop() {
  watch.loop();

  if (enLlamada) {
    gestionarParpadeoLlamada();
  }

  if (pantallaActiva && !enLlamada && (millis() - ultimoMensajeTiempo >= TIEMPO_ESPERA)) {
    tft.fillScreen(TFT_BLACK); 
    pantallaActiva = false; 
    nav_crc = 0xFFFFFFFF; 
  }
}
