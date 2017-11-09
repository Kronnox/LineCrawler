#include "GraphicEngine.h"
#include "NunchukController.h"
#include "Structs.h"
#include <binary.h>


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

void setup() 
{
  NunchukController::initncc();
  NunchukController::get_data();
  NunchukController::calibrate();
  LED::initStrip();
  //Renderer::setBackground(background, WORLD_SIZE);
  //Renderer-Function set world
  
}

void MAIK_FUNCTION(Entity *self)
{
    //DO Nothing
}

void Entity1ActionHeischkamp(Entity *self)
{
	if(world.ticks % HEISCHKAMP_SPEED_TICKS_PER_LED == 0)
	{
		self->moveState++;
		if(self->moveState >= (world.e[i].movePattern << 1))
			self->moveState=0;
		if(self->moveState<world.e[i].movePattern)
			self->pos--;
		else
			self->pos++;
	}
}

void Entity1ActionHannig(Entity *self)
{
	if(world.ticks % HANNIG_SPEED_TICKS_PER_LED == 0)
	{
		self->moveState++;
		if(self->moveState >= (world.e[i].movePattern << 2))
			self->moveState=0;
		switch(self->moveState/world.e[i].movePattern)
		{
			case 0:
				self->pos--;
				break;
				
			case 2:
				self->pos++;
				break;
		}
	}
}

void Entity1ActionLukannek(Entity *self)
{
	if(world.ticks % LUKANNEK_SPEED_TICKS_PER_LED == 0)
	{
		if(self->moveState)
			self->moveState--;
		else
			self->pos--;
		
	}
}

void Entity1ActionKrause(Entity *self)
{
	self->moveState++;
	if(self->moveState >= self->movePattern)
	{
		self->moveState=0;
	}
}


Level levels[1];
void (*EntityActions[])(Entity *) = {&MAIK_FUNCTION, &Entity1ActionHeischkamp, &Entity1ActionHannig, &Entity1ActionLukannek, &Entity1ActionKrause};

World world;


void loop() 
{
  if(world.level)
  {
    NunchukController::get_data();
    gameMainLoop();
    Renderer::renderTick();
  }
  else
  {
    loadLevel(&levels[0]);
  }
}

void gameMainLoop()
{
  playerActions();
  entityActions();
  if(world.level->gc && world.ticks % GARBAGE_COMPACTOR_SPEED_TICKS_PER_LED == 0)
	  world.aGc--;
  collisionRequest();
}

void playerActions()
{
  if(player.cooldown)
    player.cooldown--;
  if(NunchukController::shake() && !cooldown)
  {
    player.cooldown=COOLDOWN_TICKS;
  }
  if(abs(NunchukController::joy_y())>REACTION_THRESHOLD)
  {
    if(player.realPos>NOT_GO_DOWN_AREA || NunchukController::joy_y()>0)
      player.realPos += NunchukController::joy_y()*MOVEMENT_MULTIPLIER;
  }
}

void entityActions()
{
  for(uint8_t i = 0; i < MAX_ENTITY_NUMBER; i++)
  {
    if(world.e[i].isAlive)
    {
      EntityActions[world.e[i].race]((world.e+i);
    }  
  }
}

void collisionRequest()
{
  if(world.player.pos == WORLD_SIZE)
  {
	  //Start Animation
	  //Load next level
  }
  if(world.player.cooldown == COOLDOWN_TICKS)
  {
	  for(uint8_t i = 0; i < MAX_ENTITY_NUMBER; i++)
	  {
		if(world.e[i].isAlive)
		{
		  if(world.e[i].pos - world.player.pos <= PLAYER_ATTACK_RANGE)
		}  
	  }
  }
  for(uint8_t i = 0; i < MAX_ENTITY_NUMBER; i++)
  {
	if(world.e[i].isAlive)
	{
	  if(world.e[i].pos == world.player.pos)
	  {
		//Start Animation
		loadLevel(world.level);
	  }
	  if(world.e[i].race == KRAUSE)
	  {
		  if(world.e[i].pos - world.player.pos <= KRAUSE_ATTACK_RANGE)
		  {
			//Start Animation
			loadLevel(world.level);
		  }
	  }
	}  
  }
  if(world.aGc - world.player.pos <= WORLD_SIZE)
  {
	//Start Animation
	loadLevel(world.level);
  }
}

void loadLevel(Level *level)
{
  world.level = level;
  memcpy(world.e, (*level).e, MAX_ENTITY_NUMBER*sizeof(Entity));
  world.aGc = 0xFF;
  world.ticks = 0;
  world.player.realPos = 0;
  world.player.cooldown = 0;
}
