#ifndef IMAGE_H
#define IMAGE_H
#include "dig.h"

typedef struct PalyerTexture
{
    Texture2D idle[4];
    Texture2D dig[3];
}PalyerTexture;

typedef struct TileTexture
{
    Texture2D tile[11];
}TileTexture;

typedef struct TextureLib
{
    PalyerTexture player;

    TileTexture ice;
    TileTexture grass;
    TileTexture clok;
    TileTexture stone;
    TileTexture sand;
    TileTexture igneous;
    TileTexture lava;

}TextureLib;

extern TextureLib textureLib;

void InitTexture(void);

const char *TileTypeToString(TileType type);

Texture2D PathToTexture(char* path, int size);

Texture2D GetTileTexture(TileType type,int style);

#endif