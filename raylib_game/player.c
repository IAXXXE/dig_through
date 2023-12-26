#include "player.h"

GridPosition WorldToCell(Vector2 worldPosition)
{
    GridPosition gridPostion;
    gridPostion.x = (int)(worldPosition.x + GRID_SIZE / 2) / GRID_SIZE;
    gridPostion.y = (int)(worldPosition.y + GRID_SIZE / 2) / GRID_SIZE;
    return gridPostion;
}

void UpdateTile(int gridX,int gridY)
{
    Tile targetTile = tiles[gridX][gridY];
    if(targetTile.type == Empty || targetTile.isHollowed || targetTile.hasRuby) return;
    TileType type = tiles[gridX][gridY].type;
    Tile tileUp = tiles[gridX][gridY - 1];
    Tile tileLeft = tiles[gridX - 1][gridY];
    Tile tileRight = tiles[gridX + 1][gridY];
    Tile tileDown = tiles[gridX][gridY + 1];

    if(tileLeft.type == type && tileRight.type == type && tileDown.type == type)
        tiles[gridX][gridY].texture = GetTileTexture(type,2);
    else if(tileUp.type == type && tileLeft.type == type && tileRight.type == type)
        tiles[gridX][gridY].texture = GetTileTexture(type,8);
    else if(tileRight.type == type && tileDown.type == type && tileUp.type == type)
        tiles[gridX][gridY].texture = GetTileTexture(type,4);
    else if(tileUp.type == type && tileLeft.type == type && tileDown.type == type)
        tiles[gridX][gridY].texture = GetTileTexture(type,6);
    else if(tileUp.type == type && tileLeft.type == type )
        tiles[gridX][gridY].texture = GetTileTexture(type,9);
    else if(tileRight.type == type && tileDown.type == type)
        tiles[gridX][gridY].texture = GetTileTexture(type,1);
    else if(tileDown.type == type && tileLeft.type == type )
        tiles[gridX][gridY].texture = GetTileTexture(type,3);
    else if(tileRight.type == type && tileUp.type == type)
        tiles[gridX][gridY].texture = GetTileTexture(type,7);
}

void UpdateAroundTiles(int gridX,int gridY)
{
    UpdateTile(gridX + 1,gridY);
    UpdateTile(gridX,gridY + 1);
    UpdateTile(gridX - 1,gridY);
    UpdateTile(gridX,gridY - 1);
}

void DigGrid(int gridX,int gridY)
{
    Tile targetTile = tiles[gridX][gridY];
    if (targetTile.type != Empty && targetTile.type != Lava && stamina > 0)
    {
        if(targetTile.hasRuby) 
        {
            stamina += 3;
            score += 100;
        }
        // canDig = false;
        stamina -= 1;
        tiles[gridX][gridY].isHollowed = true;
        tiles[gridX][gridY].type = Empty;
        tiles[gridX][gridY].texture = GetTileTexture(targetTile.type,0);
        
        // === Update Tile ===
        UpdateAroundTiles(gridX,gridY);
        // if (targetTile.type == Sand) canDig = true;
    }
}

void BuildGrid(int gridX,int gridY)
{
    Tile targetGrid = tiles[gridX][gridY];
    Tile GridDown = tiles[gridX][gridY + 1];
    if(targetGrid.type == Empty && GridDown.type != Empty && stamina > 0)
    {
        stamina -= 1;
        tiles[gridX][gridY].type = GridDown.type;
        tiles[gridX][gridY].isHollowed = false;
        tiles[gridX][gridY].texture = GetTileTexture(GridDown.type,1);
        UpdateTile(gridX,gridY);
        UpdateTile(gridX,gridY + 1);
    }
}

void CheckGravityArea(void)
{
    if (player.grid.y < GRID_COUNT / 3)
        player.gravityArea = North;
    else if (player.grid.y < GRID_COUNT * 2 / 3 && player.grid.x < GRID_COUNT / 2)
        player.gravityArea = West;
    else if (player.grid.y < GRID_COUNT * 2 / 3 && player.grid.x >= GRID_COUNT / 2)
        player.gravityArea = East;
    else
        player.gravityArea = South;
}

bool CheckCollision(void)
{
    GridPosition checkGridPos = WorldToCell(player.position);
    player.grid = checkGridPos;
    checkGridPos.x -= 1;
    checkGridPos.y -= 1;

    for (int i = 0; i <= 2; i++)
    {
        for (int j = 0; j <= 2; j++)
        {
            if ((checkGridPos.x + i) < 0 || (checkGridPos.x + i) >= GRID_COUNT || (checkGridPos.y + j) < 0 || (checkGridPos.y + j) >= GRID_COUNT)
                continue;
            Tile tile = tiles[checkGridPos.x + i][checkGridPos.y + j];
            if (tile.type == Empty || tile.isHollowed)
            {
                continue;
            }
            else if (CheckCollisionRecs(ToRectangle(tile.position, GRID_SIZE), ToRectangle(player.position, PLAYER_SIZE)))
            {
                if (player.canDig)
                { 
                    DigGrid(checkGridPos.x + i,checkGridPos.y + j);
                    player.canDig = false;
                    return true;
                }
                else
                {
                    // normal collision 
                    switch (player.gravityArea)
                    {
                        case North: player.position.y = tile.position.y - GRID_SIZE / 2 - PLAYER_SIZE / 2; break;
                        case West: player.position.x = tile.position.x - GRID_SIZE / 2 - PLAYER_SIZE / 2; break;
                        case East: player.position.x = tile.position.x + GRID_SIZE / 2 + PLAYER_SIZE / 2; break;
                        case South: player.position.y = tile.position.y + GRID_SIZE / 2 + PLAYER_SIZE / 2; break;
                        default: break;
                    }
                    return true;
                }
            }
        }
    }
    return false;
}



void UpdatePlayer(Player *player, float delta)
{
    // // === Gravity Area ===
    CheckGravityArea();

    // === Move ===
    switch (player->gravityArea)
    {
    case North:

        break;
    case West:

        break;
    case East:

        break;
    case South:

        break;
    default:
        break;
    }

    if (IsKeyPressed(KEY_A))
        BuildGrid(player->grid.x - 1 , player->grid.y);
    if (IsKeyPressed(KEY_D))
        BuildGrid(player->grid.x + 1 , player->grid.y);

    if (IsKeyDown(KEY_LEFT) && player->canMove)
    {
        if(IsKeyDown(KEY_LEFT_SHIFT))
        {
            switch (player->gravityArea)
            {
                case North: DigGrid(player->grid.x - 1, player->grid.y); break;
                case West:  DigGrid(player->grid.x , player->grid.y + 1); break;
                case East:  DigGrid(player->grid.x , player->grid.y - 1 ); break;
                case South: DigGrid(player->grid.x + 1 , player->grid.y); break;
                default: break;
            }
        }
        switch (player->gravityArea)
        {
            case North: player->position.x -= PLAYER_HOR_SPD * delta; break;
            case West:  player->position.y += PLAYER_HOR_SPD * delta; break;
            case East:  player->position.y -= PLAYER_HOR_SPD * delta; break;
            case South: player->position.x += PLAYER_HOR_SPD * delta; break;
            default: break;
        }
    }
    if (IsKeyDown(KEY_RIGHT) && player->canMove)
    {
        if(IsKeyDown(KEY_LEFT_SHIFT))
        {
            switch (player->gravityArea)
            {
                case North: DigGrid(player->grid.x + 1, player->grid.y); break;
                case West: DigGrid(player->grid.x , player->grid.y - 1); break;
                case East: DigGrid(player->grid.x , player->grid.y + 1 ); break;
                case South: DigGrid(player->grid.x - 1 , player->grid.y); break;
                default: break;
            }
        }
        switch (player->gravityArea)
        {
            case North: player->position.x += PLAYER_HOR_SPD * delta; break;
            case West: player->position.y -= PLAYER_HOR_SPD * delta; break;
            case East: player->position.y += PLAYER_HOR_SPD * delta; break;
            case South: player->position.x -= PLAYER_HOR_SPD * delta; break;
            default: break;
        }
    }
    // === Jump ===
    if (IsKeyDown(KEY_SPACE) && player->canJump)
    {
        player->speed = -PLAYER_JUMP_SPD;
        player->canMove = false;
        player->canDig = true;
        player->canJump = false;
    }

    bool colled = CheckCollision();
    int hitObstacle = 0;
    if (colled)
    {
        hitObstacle = 1;
        player->speed = 0.0f;
    }

    // === Gravity ===
    if (!hitObstacle)
    {
        switch (player->gravityArea)
        {
            case North: player->position.y += player->speed * delta; break;
            case West: player->position.x += player->speed * delta; break;
            case East: player->position.x -= player->speed * delta; break;
            case South: player->position.y -= player->speed * delta; break;
            default: break;
        }
        player->speed += G * delta;
        player->canJump = false;
    }
    else
    {
        player->canMove = true;
        player->canJump = true;
    }
}