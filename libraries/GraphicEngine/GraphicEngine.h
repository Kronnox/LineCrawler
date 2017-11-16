#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel strip = Adafruit_NeoPixel(WORLD_SIZE, LED_PIN, NEO_GRB + NEO_KHZ800);

signed int gcPos;

byte finishHue;

//--------------------//
// *** Namespaces *** //
//--------------------//

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

  //------------------------------------//
  // *** methods for color modifing *** //
  //------------------------------------//

  double threeway_max(double a, double b, double c) {
      return max(a, max(b, c));
  }

  double threeway_min(double a, double b, double c) {
      return min(a, min(b, c));
  }

  double hue2rgb(double p, double q, double t) {
      if(t < 0) t += 1;
      if(t > 1) t -= 1;
      if(t < 1/6.0) return p + (q - p) * 6 * t;
      if(t < 1/2.0) return q;
      if(t < 2/3.0) return p + (q - p) * (2/3.0 - t) * 6;
      return p;
  }

  // Hue-Wheel for more dynamic color changes
  // (copied from Adafruit_NeoPixel stardtest)
  uint32_t hueWheel(byte WheelPos) {
    WheelPos = 255 - WheelPos;
    if(WheelPos < 85) {
      return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    if(WheelPos < 170) {
      WheelPos -= 85;
      return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }

  // modify lightness of a rgb color (with HSL)
  // (uses code fragments of the RGBConverter library)
  uint32_t changeLightness(uint32_t c, uint8_t li) {
    //RGB to HSL
    double rd = (double) (c >> 16)/255;
    double gd = (double) (c >>  8)/255;
    double bd = (double) c/255;
    double max = threeway_max(rd, gd, bd);
    double min = threeway_min(rd, gd, bd);
    double h, s, l = (max + min) / 2;

    if (max == min) {
        h = s = 0; // achromatic
    } else {
        double d = max - min;
        s = l > 0.5 ? d / (2 - max - min) : d / (max + min);
        if (max == rd) {
            h = (gd - bd) / d + (gd < bd ? 6 : 0);
        } else if (max == gd) {
            h = (bd - rd) / d + 2;
        } else if (max == bd) {
            h = (rd - gd) / d + 4;
        }
        h /= 6;
    }

    // HSL to RGB
    if (s == 0) {
        rd = gd = bd = li; // achromatic
    } else {
        double q = li < 0.5 ? li * (1 + s) : li + s - li * s;
        double p = 2 * li - q;
        rd = hue2rgb(p, q, h + 1/3.0);
        gd = hue2rgb(p, q, h);
        bd = hue2rgb(p, q, h - 1/3.0);
    }

    return strip.Color(rd*255, gd*255, bd*255);
  }

  // modify brightness of a rgb color (0-100)
  uint32_t changeBrightness(uint32_t c, uint8_t br) {
    double rd = (double) (c >> 16)/100;
    double gd = (double) (c >>  8)/100;
    double bd = (double) c/100;

    return strip.Color(rd*br, gd*br, bd*br);
  }
}

// converts transferred game-data into LED-data
namespace Renderer {

  // turns the actual GC position into the needed render position
  void updateGC() {
    if(!(world.aGc > WORLD_SIZE)) {
      uint8_t aGc = WORLD_SIZE - world.aGc;
      gcPos = aGc;
    }
  }

  //-----------------------------------------------//
  // *** methods for the actual Game rendering *** //
  //-----------------------------------------------//

  // renders the BackGround at pixel 'pPos' as defined in the level data
  void renderBgAt(uint16_t pPos) {
    switch(world.level->bg[pPos]) {
      case 'W':
        strip.setPixelColor(pPos, COLOR_WORLD_WATER);
        break;
      case 'I':
        strip.setPixelColor(pPos, COLOR_WORLD_ICE);
        break;
      default:
        strip.setPixelColor(pPos, COLOR_RESET);
    }
  }

  // renders "GarbageCompactor" at pixel 'pPos'
  void renderGcAt(uint16_t pPos) {
    if(gcPos < 0) return;
    if(pPos <= gcPos) strip.setPixelColor(pPos, COLOR_GC);
    if(pPos = gcPos+1) strip.setPixelColor(pPos, changeBrightness(COLOR_GC, 50));
    if(pPos = gcPos+2) strip.setPixelColor(pPos, changeBrightness(COLOR_GC, 10));
  }

  // renders all Entities as defined in the level data
  void renderEntities() {
    for(uint8_t i = 0; i < MAX_ENTITY_NUMBER; i++) {
      if(world.e[i].isAlive) {
        strip.setPixelColor(world.e[i].pos, COLOR_ENT_0);
      }
    }
  }

  // renders Player and all Player dedicated animations
  void renderPlayer() {
    if(!world.player.cooldown) {
      strip.setPixelColor(world.player.pos, COLOR_PLAYER);
    } else {
      // play attack animation if needed
      for(int z = 0; z <= PLAYER_ATTACK_RANGE; z++) {
        if(world.player.cooldown == COOLDOWN_TICKS-z) strip.setPixelColor(world.player.pos+z, COLOR_PLAYER);
        if(world.player.cooldown == COOLDOWN_TICKS-z) strip.setPixelColor(world.player.pos-z, COLOR_PLAYER);
      }

      if(world.player.cooldown <= 3) {
        // player lights up shortly if cooldown is over
        strip.setPixelColor(world.player.pos, strip.Color(255,255,255));
      } else {
        // player fades for Cooldown effect
        strip.setPixelColor(world.player.pos, strip.Color(0,(int (225-COLOR_COOLDOWN_STEP*world.player.cooldown)),0));
      }
    }
  }

  // renders Background and "GarbageCompactor" at pixel 'pPos'
  void renderWorldAt(uint16_t pPos) {
    renderBgAt(pPos);
    renderGcAt(pPos);
  }

  //-----------------------------------------//
  // *** methods for Animation rendering *** //
  //-----------------------------------------//

  // renders the color changing pixel at the end of the LED-strip
  void playFinishGlow() {
    strip.setPixelColor(WORLD_SIZE-1, LED::hueWheel(finishHue));
    if(finishHue >= 254) {
      finishHue = 0;
    } else {
      finishHue++;
    }
  }

  // called when a new level is loaded
  void playResetWipe() {
    for(uint16_t i=strip.numPixels()-1; i>0; i--) {
      strip.setPixelColor(i, LED::hueWheel(i%255));
      strip.show();
      delay(RESET_WIPE_DELAY);
    }

    updateGC();

    for(uint16_t i=strip.numPixels()-1; i>0; i--) {
      renderBgAt(i);
      strip.show();
      delay(RESET_WIPE_DELAY);
    }

  }

  //--------------------------------------------------------------//
  // *** main render-method | called to render the whole game *** //
  //--------------------------------------------------------------//

  void renderTick() {

    updateGC();

    // start rendering

    LED::clearStrip();

    for(int i = 0; i < WORLD_SIZE; i++)
        renderWorldAt(i);

    renderEntities();
    renderPlayer();

    playFinishGlow();

    strip.show();
  }
}
