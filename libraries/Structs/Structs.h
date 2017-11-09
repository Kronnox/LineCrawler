struct Entity 
{
  uint8_t pos;
  uint8_t moveState;
  uint8_t movePattern;
  uint8_t race : 7;
  uint8_t isAlive : 1;
};

struct Player
{
  union
  {
    uint16_t realPos;
    struct
    {
        uint8_t bpos;
        uint8_t pos;
    };
  };
  int8_t cooldown;
};

struct Level
{
  uint8_t bg[WORLD_SIZE];
  Entity e[MAX_ENTITY_NUMBER];
  bool gc;
};

struct World
{
  Level *level;
  Entity e[MAX_ENTITY_NUMBER];
  uint8_t aGc;
  Player player;
  uint16_t ticks;
};