#include <TFT_eSPI.h>
#include <SPIFFS.h>

TFT_eSPI tft = TFT_eSPI();  // Inicializa TFT

// --- Funciones de lectura BMP (Originales, NO tocar) ---
uint16_t read16(fs::File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read();
  ((uint8_t *)&result)[1] = f.read();
  return result;
}

uint32_t read32(fs::File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read();
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read();
  return result;
}

// --- Función BMP Optimizada Original (Restaurada para arreglar colores) ---
void drawBmp(const char *filename, int x, int y) {
  fs::File bmpFS = SPIFFS.open(filename, "r");
  if(!bmpFS){ Serial.println("Archivo BMP NO encontrado!"); return; }
  if(read16(bmpFS) != 0x4D42){ Serial.println("No es un BMP valido"); bmpFS.close(); return; }
  read32(bmpFS); read32(bmpFS);
  uint32_t bmpImageoffset = read32(bmpFS);
  read32(bmpFS); int bmpWidth = read32(bmpFS); int bmpHeight = read32(bmpFS);
  if(read16(bmpFS)!=1){ Serial.println("BMP con mas de un plano"); bmpFS.close(); return; }
  uint16_t bmpDepth = read16(bmpFS);
  if(bmpDepth != 24){ Serial.println("Solo BMP 24 bits"); bmpFS.close(); return; }
  if(read32(bmpFS) != 0){ Serial.println("BMP comprimido"); bmpFS.close(); return; }

  uint16_t rowColors[bmpWidth];
  
  // A menudo los BMP tienen padding al final de cada fila para alinear a 4 bytes.
  // Calculamos el ancho real de la fila en el archivo.
  uint32_t rowSize = (bmpWidth * 3 + 3) & ~3; 

  tft.startWrite();
  for(int row=0; row<bmpHeight; row++){
    // Los BMP se guardan de abajo hacia arriba. Saltamos a la fila correcta.
    // Usamos el offset + (alto - 1 - fila_actual) * tamaño_fila
    bmpFS.seek(bmpImageoffset + (bmpHeight - 1 - row) * rowSize);
    
    for(int col=0; col<bmpWidth; col++){
      uint8_t b = bmpFS.read(), g = bmpFS.read(), r = bmpFS.read();
      rowColors[col] = tft.color565(r,g,b);
    }
    // Empujamos la fila entera a la pantalla
    tft.setAddrWindow(x, y + row, bmpWidth, 1);
    tft.pushColors(rowColors, bmpWidth, true);
  }
  tft.endWrite();
  bmpFS.close();
}

// --- Barra de carga HORIZONTAL con Márgenes ---
// progress: 0.0 a 1.0
void drawProgressBarHorizontal(int x, int y, int w, int h, float progress) {
  int barFillWidth = (int)(w * progress);
  
  tft.startWrite();
  // Parte llena (Verde)
  if (barFillWidth > 0) {
    tft.fillRect(x, y, barFillWidth, h, TFT_RED);
  }
  // Parte vacía (Gris Oscuro)
  if (barFillWidth < w) {
    tft.fillRect(x + barFillWidth, y, w - barFillWidth, h, TFT_DARKGREY);
  }
  // Borde (Blanco)
  tft.drawRect(x, y, w, h, TFT_WHITE);
  tft.endWrite();
}

void setup() {
  Serial.begin(115200);
  if(!SPIFFS.begin(true)){ Serial.println("Error montando SPIFFS"); return; }

  tft.init();
  tft.setRotation(0);  // Vertical (Asegúrate que tu BMP sea para esta orientación)
  tft.fillScreen(TFT_BLACK);

  // 1️⃣ Mostrar splash BMP
  drawBmp("/imagen.bmp", 0, 0);

  // 2️⃣ Configuración de Barra Horizontal con Márgenes de 7px
  int margen = 7;
  int barH = 15; // Alto de la barra

  // El ancho es el ancho de pantalla menos margen izquierdo y derecho (7+7)
  int barW = tft.width() - (2 * margen); 
  
  // La posición X es el margen izquierdo
  int barX = margen; 

  // La posición Y es el alto de pantalla menos el alto de barra menos margen inferior
  int barY = tft.height() - barH - margen; 

  int steps = 50;
  for(int i=0; i<=steps; i++){
    float progress = (float)i/steps;
    drawProgressBarHorizontal(barX, barY, barW, barH, progress);
    delay(70); // Total ~2.5s
  }

  // 3️⃣ Limpiar pantalla
  tft.fillScreen(TFT_BLACK);

  // 4️⃣ Mostrar menú
  
  tft.setTextColor(TFT_PURPLE);
  tft.setTextSize(3);
  tft.setCursor(0, 3);
  tft.println("MiTul");

tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(5, 50);
  tft.println("1->WiFI");
  tft.setCursor(5, 68);
  tft.println("2->BT");
  tft.setCursor(5, 86);
  tft.println("3->Ajustes");

tft.setTextColor(TFT_RED);
  tft.setTextSize(1);
  tft.setCursor(47, 150);
  tft.println("MiTul_OS v1.0");
}

void loop() {
  // Lógica del menú
}
