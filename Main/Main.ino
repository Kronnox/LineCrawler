#include "Defines.h"
#include "Structs.h"

World world;
Level levels[1];
long lastTick;

#include "NunchukController.h"
#include "GraphicEngine.h"

void setup() 
{
  for(int i = 0; i < 10; i++)
  {
    levels[0].bg[i+20]='W';
    levels[0].bg[i+35]='I';
  }
  levels[0].gc = true;
  NunchukController::initncc();
  NunchukController::get_data();
  //NunchukController::calibrate();
  LED::initStrip();
  Serial.begin(9600);
  //Renderer::setBackground(background, WORLD_SIZE);
  //Renderer-Function set world
  
}

void loadLevel(Level *level)
{
  world.level = level;
  memcpy(world.e, level->e, MAX_ENTITY_NUMBER*sizeof(Entity));
  world.aGc = 180;
  world.ticks = 0;
  world.player.realPos = 0;
  world.player.cooldown = 0;
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
		if(self->moveState >= (self->movePattern << 1))
			self->moveState=0;
		if(self->moveState<self->movePattern)
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
		if(self->moveState >= (self->movePattern << 2))
			self->moveState=0;
		switch(self->moveState/self->movePattern)
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


void (*EntityActions[])(Entity *) = {&MAIK_FUNCTION, &Entity1ActionHeischkamp, &Entity1ActionHannig, &Entity1ActionLukannek, &Entity1ActionKrause};


void loop() 
{
  long t = millis();
  if(lastTick + MS_PER_TICK < t)
  {
    lastTick = t;
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
}

void gameMainLoop()
{
  playerActions();
  entityActions();
  if(world.level->gc && world.ticks % GARBAGE_COMPACTOR_SPEED_TICKS_PER_LED == 0)
  {
	  world.aGc--;
  }
  collisionRequest();
  world.ticks++;
}

void playerActions()
{
  if(world.player.cooldown)
  {
    world.player.cooldown--;
  }
  if(NunchukController::shake() && !world.player.cooldown)
  {
    world.player.cooldown=COOLDOWN_TICKS;
  }
  if(abs(NunchukController::joy_y())>REACTION_THRESHOLD)
  {
    if(world.player.realPos>NOT_GO_DOWN_AREA || NunchukController::joy_y()>0)
    {
      world.player.realPos += NunchukController::joy_y()*MOVEMENT_MULTIPLIER;
    }
  }
}

void entityActions()
{
  for(uint8_t i = 0; i < MAX_ENTITY_NUMBER; i++)
  {
    if(world.e[i].isAlive)
    {
      EntityActions[world.e[i].race](world.e+i);
    }  
  }
}

void collisionRequest()
{
  if(world.player.pos >= WORLD_SIZE)
  {
	  //Start Animation
   
	  //Load next level
   loadLevel(world.level);
  }
  if(world.player.cooldown == COOLDOWN_TICKS)
  {
	  for(uint8_t i = 0; i < MAX_ENTITY_NUMBER; i++)
	  {
  		if(world.e[i].isAlive)
  		{
  		  if(world.e[i].pos - world.player.pos <= PLAYER_ATTACK_RANGE)
        {
          
        }
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
  /*if(world.aGc - world.player.pos <= WORLD_SIZE)
  {
  	//Start Animation
  	//loadLevel(world.level);
  }*/
}
