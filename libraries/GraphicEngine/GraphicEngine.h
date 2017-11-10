#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// define general colors
#define COLOR_RESET strip.Color(0,0,0)
#define COLOR_GC_0 strip.Color(255,255,255)
#define COLOR_GC_1 strip.Color(40,40,50)
#define COLOR_GC_2 strip.Color(8,8,10)
#define COLOR_PLAYER strip.Color(0,255,0)

// define world colors
#define COLOR_WORLD_WATER strip.Color(0,0,125)
#define COLOR_WORLD_ICE strip.Color(0,125,125)

// define led-strip parameters
#define LED_PIN 6

// ---=[ only edit above this line]=---

Adafruit_NeoPixel strip = Adafruit_NeoPixel(WORLD_SIZE, LED_PIN, NEO_GRB + NEO_KHZ800);

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

  //------------------------------------------//
  // *** methods for the actual rendering *** //
  //------------------------------------------//

  void renderBG() {
    for(int i = 0; i < WORLD_SIZE; i++) {
      switch(world.level->bg[i]) {
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
      strip.setPixelColor(i, COLOR_GC_0);
      if(gcPos < WORLD_SIZE+1) strip.setPixelColor(i+1, COLOR_GC_1);
      if(gcPos < WORLD_SIZE+2) strip.setPixelColor(i+2, COLOR_GC_2);
    }
  }

  void renderEntities() {

  }

  void renderPlayer() {
    strip.setPixelColor(world.player.pos, COLOR_PLAYER);
  }

  // ** main render-method | called to render the whole game **
  void renderTick() {

    // update GC

    if(world.aGc > WORLD_SIZE) return;
    uint8_t aGc = WORLD_SIZE - world.aGc;
    gcPos = aGc;

    // start rendering

    LED::clearStrip();

    renderBG();
    renderEntities();
    renderPlayer();
    renderGC();

    strip.show();
  }
}
