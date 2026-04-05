#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

const int numBarras = 5;
const int barraAncho = 14;
const int margen = 8;

void setup() {
  tft.init();
  tft.setRotation(0); // vertical
  tft.fillScreen(TFT_BLACK);
}

void loop() {

  // LIMPIAR TODO
  tft.fillScreen(TFT_BLACK);

  // ===== TEXTOS =====
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  // Arriba
  tft.setTextSize(1);
  tft.setCursor(margen, 5);
  tft.print("ESP32 TFT");

  // Medio
  tft.setTextSize(2);
  tft.setCursor(margen, 30);
  tft.print("Grafico");

  // Abajo
  tft.setTextSize(1);
  tft.setCursor(margen, 140);
  tft.print("Test OK");

  // ===== GRAFICO =====
  int ejeX = 130;   // altura del eje X
  int ejeY = 60;    // donde empieza grafico (debajo del texto medio)

  // Ejes
  tft.drawLine(margen, ejeY, margen, ejeX, TFT_WHITE);
  tft.drawLine(margen, ejeX, 128 - margen, ejeX, TFT_WHITE);

  // Barras
  int espacioTotal = 128 - 2 * margen;
  int espacioEntre = (espacioTotal - barraAncho * numBarras) / (numBarras - 1);

  for (int i = 0; i < numBarras; i++) {
    int altura = random(10, ejeX - ejeY - 5);

    int x = margen + i * (barraAncho + espacioEntre);
    int y = ejeX - altura;

    tft.fillRect(x, y, barraAncho, altura, TFT_BLUE);
    tft.drawRect(x, y, barraAncho, altura, TFT_WHITE);
  }

  delay(1500);
}
