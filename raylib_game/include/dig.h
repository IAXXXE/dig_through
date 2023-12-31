#ifndef DIG_H
#define DIG_H
#include <stdio.h>
#include <string.h>
#include "raylib.h"
#include "raymath.h"

#define GRID_SIZE 40
#define GRID_COUNT 12

typedef enum GameState
{
    Start,
    Running,
    Over,
    Success
}GameState;

typedef enum GravityArea
{
    North,
    East,
    West,
    South
}GravityArea;

typedef enum TileType
{
    Empty,
    Ice,
    Grass,
    Clok,
    Stone,
    Sand,
    Igneous,
    Lava
}TileType;

// typedef struct TileMap
// {
//     /* data */
// };


typedef struct GridPosition
{
    int x;
    int y;
}GridPosition;

typedef struct Tile
{
    Vector2 position;
    GridPosition grid;
    Texture2D* texture;
    TileType type;
    bool isHollowed;
    bool hasRuby;
}Tile;

typedef struct EnvItem {
    Rectangle rect;
    int blocking;
    Color color;
} EnvItem;

Rectangle ToRectangle(Vector2 position, int size);

#endif