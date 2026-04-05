#include <TFT_eSPI.h>
#include <SPIFFS.h>

TFT_eSPI tft = TFT_eSPI();  // Inicializa la librería TFT

// Funciones para leer BMP
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

// Función para dibujar BMP optimizado
void drawBmp(const char *filename, int x, int y) {
  fs::File bmpFS = SPIFFS.open(filename, "r");
  if(!bmpFS){ 
    Serial.println("Archivo BMP NO encontrado!"); 
    return; 
  }

  if(read16(bmpFS) != 0x4D42){ 
    Serial.println("No es un BMP valido"); 
    bmpFS.close(); 
    return; 
  }

  read32(bmpFS); // tamaño archivo
  read32(bmpFS); // reservado
  uint32_t bmpImageoffset = read32(bmpFS);

  read32(bmpFS); // header size
  int bmpWidth  = read32(bmpFS);
  int bmpHeight = read32(bmpFS);
  if(read16(bmpFS) != 1){ 
    Serial.println("BMP con mas de un plano"); 
    bmpFS.close(); 
    return; 
  }

  uint16_t bmpDepth = read16(bmpFS);
  if(bmpDepth != 24){ 
    Serial.println("Solo BMP 24 bits"); 
    bmpFS.close(); 
    return; 
  }

  if(read32(bmpFS) != 0){ 
    Serial.println("BMP comprimido"); 
    bmpFS.close(); 
    return; 
  }

  // Buffer de colores para una fila
  uint16_t rowColors[bmpWidth];

  tft.startWrite();
  for(int row = bmpHeight - 1; row >= 0; row--){
    bmpFS.seek(bmpImageoffset + row * bmpWidth * 3);
    for(int col = 0; col < bmpWidth; col++){
      uint8_t b = bmpFS.read();
      uint8_t g = bmpFS.read();
      uint8_t r = bmpFS.read();
      // Convierte a RGB565 directamente
      rowColors[col] = tft.color565(r, g, b);
    }
    // Dibuja toda la fila de golpe
    tft.setAddrWindow(x, y + (bmpHeight - 1 - row), bmpWidth, 1);
    tft.pushColors(rowColors, bmpWidth, true);
  }
  tft.endWrite();
  bmpFS.close();
}

void setup() {
  Serial.begin(115200);

  if(!SPIFFS.begin(true)){
    Serial.println("Error montando SPIFFS"); 
    return; 
  }

  tft.init();
  tft.setRotation(1);  // horizontal 160x128
  tft.fillScreen(TFT_BLACK);

  drawBmp("/imagen.bmp", 0, 0); // dibuja en la esquina superior izquierda
}

void loop() {
  // nada que hacer aquí
}
