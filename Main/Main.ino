#include "Defines.h"
#include "Structs.h"

World world;
Level levels[1];
long lastTick;

bool button_last;
bool blackout = true;

#include "NunchukController.h"
#include "GraphicEngine.h"

void setup() 
{
  for(int i = 30; i < 35; i++) {
    levels[0].bg[i]='W';
  }
  for(int i = 35; i < 45; i++) {
    levels[0].bg[i]='I';
  }
  for(int i = 65; i < 95; i++) {
    levels[0].bg[i]='W';
  }
  for(int i = 115; i < 145; i++) {
    levels[0].bg[i]='I';
  }
  
  levels[0].gc = true;
  //Test Maik:
  levels[0].e[0].isAlive=true;
  levels[0].e[0].pos = 18;
  levels[0].e[0].race = MAIK;

  //Test Heischkamp:
  levels[0].e[1].isAlive=true;
  levels[0].e[1].pos = 53;
  levels[0].e[1].race = HEISCHKAMP;
  levels[0].e[1].movePattern = 10;

  //Test Hannig2:
  levels[0].e[2].isAlive=true;
  levels[0].e[2].pos = 80;
  levels[0].e[2].race = HANNIG;
  levels[0].e[2].movePattern = 2;

  //Test Heischkamp:
  levels[0].e[3].isAlive=true;
  levels[0].e[3].pos = 93;
  levels[0].e[3].race = HEISCHKAMP;
  levels[0].e[3].movePattern = 10;

  //Test Krause:
  levels[0].e[4].isAlive=true;
  levels[0].e[4].pos = 142;
  levels[0].e[4].race = KRAUSE;
  levels[0].e[4].movePattern = 60;
  
  NunchukController::initncc();
  NunchukController::get_data();
  //NunchukController::calibrate();
  LED::initStrip();
  Serial.begin(9600);
}

void loadLevel(Level *level)
{
  world.level = level;
  memcpy(world.e, level->e, MAX_ENTITY_NUMBER*sizeof(Entity));
  world.aGc = WORLD_SIZE+5;
  world.ticks = 0;
  world.player.realPos = NOT_GO_DOWN_AREA;
  world.player.cooldown = 0;
  Renderer::playResetWipe();
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
		if(self->moveState >= (self->movePattern * 4))
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
		if(self->moveState)
			self->moveState--;
		else
      if(world.ticks % LUKANNEK_SPEED_TICKS_PER_LED == 0)
			  self->pos--;
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


void checkCbutton() {
  if(!button_last && NunchukController::button_c()) {
    blackout = !blackout;
    if(blackout) LED::clearStrip(true);
  }
  button_last = NunchukController::button_c();
}

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
      checkCbutton();
      if(!blackout) {
        Renderer::renderTick();
      }
      while(blackout) {
        NunchukController::get_data();
        checkCbutton();
        delay(1);
      }
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
  world.ticks++;
  collisionRequest();
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
  if(abs(NunchukController::joy_y())>REACTION_THRESHOLD && world.player.cooldown < COOLDOWN_TICKS-(PLAYER_ATTACK_RANGE*4))
  {
    if(world.player.realPos>NOT_GO_DOWN_AREA || NunchukController::joy_y()>0)
    {
      switch(world.level->bg[world.player.pos]) {
        case 'W':
          world.player.realPos += NunchukController::joy_y()*MOVEMENT_WATER_MULTIPLIER;
          break;
        case 'I':
          world.player.realPos += NunchukController::joy_y()*MOVEMENT_ICE_MULTIPLIER;
          break;
        default:
          world.player.realPos += NunchukController::joy_y()*MOVEMENT_MULTIPLIER;
      }
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
  if(world.player.pos >= WORLD_SIZE-1)
  {
	  //Start Animation
   
	  //Load next level
   loadLevel(world.level);
  }
  for(uint8_t i = 0; i < MAX_ENTITY_NUMBER; i++)
  {
    for(int z = 1; z <= PLAYER_ATTACK_RANGE; z++) {
      if(world.player.cooldown == COOLDOWN_TICKS-z+1 && world.e[i].pos - world.player.pos == PLAYER_ATTACK_RANGE-z) {
        if(world.e[i].race == KRAUSE)
        {
          world.e[i].race = LUKANNEK;
          world.e[i].moveState = 40;
        }
        else
        {
          world.e[i].isAlive=false;
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
        //Renderer::playDeathWipe(world.player.pos);
  		  loadLevel(world.level);
  	  }
  	  if(world.e[i].race == KRAUSE && world.e[i].moveState == 0)
  	  {
  		  if(world.e[i].pos - world.player.pos <= KRAUSE_ATTACK_RANGE)
  		  {
  			  //ANIMATION
  			  loadLevel(world.level);
  		  }
  	  }
	  }
	}  
  if(WORLD_SIZE - world.aGc >= world.player.pos)
  {
  	//Start Animation
  	loadLevel(world.level);
  }
}
