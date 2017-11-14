#define WORLD_SIZE 180
#define MAX_ENTITY_NUMBER 16
#define NOT_GO_DOWN_AREA 1000
#define REACTION_THRESHOLD 3
#define MOVEMENT_MULTIPLIER 2
#define PLAYER_ATTACK_RANGE 2
#define GARBAGE_COMPACTOR_SPEED_TICKS_PER_LED 40

#define TICKS_PER_SEC 20
#define COOLDOWN_SEC 5

#define MS_PER_TICK (1000/TICKS_PER_SEC)
#define COOLDOWN_TICKS (COOLDOWN_SEC*TICKS_PER_SEC)

#define KRAUSE_ATTACK_RANGE 5
#define HEISCHKAMP_SPEED_LEDS_PER_SEC 1
#define HEISCHKAMP_SPEED_TICKS_PER_LED (int)(TICKS_PER_SEC/HEISCHKAMP_SPEED_LEDS_PER_SEC)
#define HANNIG_SPEED_LEDS_PER_SEC 2
#define HANNIG_SPEED_TICKS_PER_LED (int)(TICKS_PER_SEC/HANNIG_SPEED_LEDS_PER_SEC)
#define LUKANNEK_SPEED_LEDS_PER_SEC 2/3
#define LUKANNEK_SPEED_TICKS_PER_LED (int)(TICKS_PER_SEC/LUKANNEK_SPEED_LEDS_PER_SEC)

#define MAIK 0
#define HEISCHKAMP 1
#define HANNIG 2
#define LUKANNEK 3
#define KRAUSE 4
#define SCHLAGER 5

//--------------------------------//
// *** GraphicsEngine Defines *** //
//--------------------------------//

// define general colors
#define COLOR_RESET strip.Color(0,0,0)
#define COLOR_GC_0 strip.Color(255,255,255)
#define COLOR_GC_1 strip.Color(40,40,50)
#define COLOR_GC_2 strip.Color(8,8,10)
#define COLOR_PLAYER strip.Color(0,255,0)
#define COLOR_COOLDOWN_STEP (150/COOLDOWN_TICKS)
#define COLOR_ENT_0 strip.Color(255,0,0)

// define world colors
#define COLOR_WORLD_WATER strip.Color(0,0,25)
#define COLOR_WORLD_ICE strip.Color(0,25,25)

// define led-strip parameters
#define LED_PIN 6
