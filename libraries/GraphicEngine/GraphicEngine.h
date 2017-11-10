#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

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
          break;
        case 'I':
          strip.setPixelColor(i, COLOR_WORLD_ICE);
          break;
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
    for(uint8_t i = 0; i < MAX_ENTITY_NUMBER; i++) {
      if(world.e[i].isAlive) {
        strip.setPixelColor(world.e.pos, COLOR_ENT_0);
        /**
        switch(world.e[i].race) {
          case 0:

        }
        **/
      }
    }
  }

  void renderPlayer() {
    if(!world.player.cooldown) {
      strip.setPixelColor(world.player.pos, COLOR_PLAYER);
    } else {
      strip.setPixelColor(world.player.pos, strip.Color((int (COLOR_COOLDOWN_STEP*world.player.cooldown)),255,0));
    }
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
