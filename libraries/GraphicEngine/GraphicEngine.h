#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// define general colors
#define COLOR_RESET strip.Color(0,0,0)
#define COLOR_GC strip.Color(255,255,255)
#define COLOR_PLAYER strip.Color(255,0,0)

// define world colors
#define COLOR_WORLD_WATER strip.Color(0,0,125)
#define COLOR_WORLD_ICE strip.Color(0,125,125)

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

  void initStrip() {
    strip.begin();
    strip.show();
  }

  void clearStrip() {
    for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, COLOR_RESET);
    }
  }
}

// converts transferred game-data into LED-data
namespace Renderer {

  //--------------------------------------------//
  // *** methods for transferring game data *** //
  //--------------------------------------------//

  void setBackground(uint8_t *bg, int wSize) {
    background = bg;
    worldSize = wSize;
  }

  void updatePlayer(signed int pos) {
    playerPos = pos;
  }

  void updateGc(signed int pos) {
    gcPos = pos;
  }

  //------------------------------------------//
  // *** methods for the actual rendering *** //
  //------------------------------------------//

  void renderBG() {
    for(int i = 0; i < worldSize; i++) {
      switch(background[i]) {
        case 'W':
          strip.setPixelColor(i, COLOR_WORLD_WATER);
        case 'I':
          strip.setPixelColor(i, COLOR_WORLD_ICE);
        default:
          strip.setPixelColor(i, COLOR_RESET);
      }
    }
  }

  void renderGC() {
    if(gcPos < 0) return;
    for(int i = 0; i < gcPos+1; i++) {
      strip.setPixelColor(i, COLOR_GC);
    }
  }

  void renderWorld() {
    renderBG();
    renderGC();
  }

  void renderEntities() {

  }

  void renderPlayer() {
    strip.setPixelColor(playerPos, COLOR_PLAYER);
  }

  // ** main render-method | called to render the whole game **
  void renderTick() {
    renderWorld();
    renderEntities();
    renderPlayer();
  }
}
