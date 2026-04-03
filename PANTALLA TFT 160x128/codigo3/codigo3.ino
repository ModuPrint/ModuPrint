#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();  // Crear objeto TFT

const int numBarras = 5;
const int barraAncho = 20;
const int margen = 10;      // margen general: izquierda, derecha, superior
const int textoAlto = 8;    // alto aprox del texto con setTextSize(1)

void setup() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  // Dibujar texto arriba
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(margen, 2);   // margen superior seguro
  tft.print("Willy, no llores...");

  // Dibujar ejes
  int ejeX = 160 - margen;     // posición Y del eje X (abajo)
  int ejeY = margen + textoAlto + 2; // margen superior + texto
  tft.drawLine(margen, ejeY, margen, ejeX, TFT_WHITE);        // eje Y
  tft.drawLine(margen, ejeX, 128 - margen, ejeX, TFT_WHITE);  // eje X
}

void loop() {
  int barras[numBarras];
  int ejeX = 160 - margen;
  int ejeY = margen + textoAlto + 2;
  int espacioTotal = 128 - 2 * margen;
  int espacioEntreBarras = (espacioTotal - barraAncho * numBarras) / (numBarras - 1);

  // Generar alturas aleatorias seguras
  for (int i = 0; i < numBarras; i++) {
    barras[i] = random(10, ejeX - ejeY - 5);
  }

  // Limpiar zona de barras
  tft.fillRect(margen + 1, ejeY + 1, espacioTotal - 2, ejeX - ejeY - 2, TFT_BLACK);

  // Dibujar barras
  for (int i = 0; i < numBarras; i++) {
    int x = margen + i * (barraAncho + espacioEntreBarras);
    int y = ejeX - barras[i];
    int h = barras[i];

    tft.fillRect(x, y, barraAncho, h, TFT_BLUE);
    tft.drawRect(x, y, barraAncho, h, TFT_WHITE);
  }

  delay(1000); // actualizar cada segundo
}
