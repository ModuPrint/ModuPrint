#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();  // Crear objeto

void setup() {
  tft.init();               // Inicializa pantalla
  tft.setRotation(1);       // Orientación
  tft.fillScreen(TFT_BLACK); // Fondo negro

  tft.setTextColor(TFT_WHITE); // Color del texto
  tft.setTextSize(1);          // Tamaño del texto
  tft.setCursor(10, 60);       // Posición
  tft.println("HOLA, WILLY");  // Texto a mostrar
}

void loop() {
  // Nada más que mostrar el texto
}
