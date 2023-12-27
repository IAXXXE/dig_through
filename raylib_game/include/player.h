#ifndef PLAYER_H
#define PLAYER_H
#include "dig.h"
#include "image.h"

#define G 500
#define PLAYER_JUMP_SPD 150.0f
#define PLAYER_HOR_SPD 200.0f
#define PLAYER_SIZE 16

typedef enum PlayerAnim
{
    Idle,
    Walk,
    Dig,
} PlayerAnim;

typedef struct Player
{
    Vector2 position;
    Vector2 size;
    Texture2D texture;
    GravityArea gravityArea;
    GridPosition grid;
    float speed;
    bool canMove;
    bool canJump;
    bool canDig;
} Player;

extern GameState gameState;
extern Player player;
extern TextureLib textureLib;
extern Tile tiles[GRID_COUNT * 3][GRID_COUNT * 3];
extern int stamina;
extern int score;
extern int rubyAmount;
extern int gridCount;
extern int level;

void InitPlayer(void);
void UpdatePlayer(Player *player, float delta);

#endif