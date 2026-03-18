#include <Adafruit_NeoPixel.h>

#define PIN 6
#define NUM_LEDS 329

Adafruit_NeoPixel strip(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

struct Letra {
  int topStart;
  int topEnd;
  int bottomStart;
  int bottomEnd;
};

Letra letras[] = {
  {0,15,297,328},    // M
  {16,27,289,296},   // O
  {29,42,276,288},   // D
  {43,68,262,275},   // U
  {69,81,250,261},  // P
  {82,90,231,249},  // R
  {91,99,222,230},  // I
  {100,118,203,221},  // N
  {119,132,195,202},  // T
  {133,135,192,194},  // .
  {136,155,186,191},  // E
  {156,179,180,185},  // S
};

#define NUM_LETRAS (sizeof(letras) / sizeof(letras[0]))

uint32_t wheel(byte pos) {
  pos = 255 - pos;
  if (pos < 85) return strip.Color(255 - pos * 3, 0, pos * 3);
  if (pos < 170) {
    pos -= 85;
    return strip.Color(0, pos * 3, 255 - pos * 3);
  }
  pos -= 170;
  return strip.Color(pos * 3, 255 - pos * 3, 0);
}

void pintarLetra(int index, uint32_t color) {
  Letra l = letras[index];

  for (int i = l.topStart; i <= l.topEnd; i++) {
    strip.setPixelColor(i, color);
  }

  for (int i = l.bottomStart; i <= l.bottomEnd; i++) {
    strip.setPixelColor(i, color);
  }
}

void setup() {
  strip.begin();
  strip.show();
}

void loop() {
  static uint8_t offset = 0;

  for (int i = 0; i < NUM_LETRAS; i++) {
    uint8_t colorIndex = (i * 20 + offset) & 255;
    uint32_t color = wheel(colorIndex);

    pintarLetra(i, color);
  }

  strip.show();

  offset++;
  delay(20);
}
