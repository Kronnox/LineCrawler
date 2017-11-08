#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// define general colors
#define COLOR_RESET strip.Color(0,0,0)
#define COLOR_GC strip.Color(255,255,255)
#define COLOR_PLAYER strip.Color(255,0,0)

// define world colors
#define COLOR_WORLD_WATER strip.Color(0,0,125)
#define COLOR_WORLD_IDE strip.Color(0,125,125)

// define led-strip parameters
#define LED_PIN 6
#define LED_COUNT 100

// ---=[ only edit above this line]=---

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

int worldSize;
uint8_t *background;

signed int playerPos;

signed int gcPos;

// LED-Strip handler
namespace LED {

  void init() {
    strip.begin();
    strip.show();
  }

  void setLEDPixel(int i, uint32_t c) {
    strip.setPixelColor(i, c);
  }

  void clear() {
    for(uint16_t i=0; i<strip.numPixels(); i++) {
      LED::setLEDPixel(i, COLOR_RESET);
    }
  }
}

// converts transferred game-data into LED-data
namespace Renderer {

  // methods for transferring game data

  void setBackground(uint8_t *bg, int wSize) {
    background = bg;
    worldSize = wSize;
  }

  void updatePlayerInfo(signed int pos) {
    playerPos = pos;
  }

  void updateGcPos(signed int pos) {
    gcPos = pos;
  }

  // methods for the actual rendering

  // main render-method | called to render the whole game
  void renderTick() {
    renderWorld();
    renderEntities();
    renderPlayer();
  }

  void renderWorld() {
    renderBG();
    renderGC();
  }

  void renderBG() {
    for(int i = 0; i < worldSize; i++) {
      switch(bg[i]) {
        case 'W':
          LED::setLEDPixel(i, COLOR_WORLD_WATER);
        case 'I':
          LED::setLEDPixel(i, COLOR_WORLD_ICE);
        default:
          LED::setLEDPixel(i, COLOR_RESET);
      }
    }
  }

  void renderGC() {
    if(gcPos < 0) return;
    for(int i = 0; i < gcPos+1; i++) {
      LED::setLEDPixel(i, COLOR_GC);
    }
  }

  void renderEntities() {

  }

  void renderPlayer() {
    LED::setLEDPixel(playerPos, COLOR_PLAYER);
  }
}
