#include "dig.h"
#include "image.h"
#include "player.h"
#include <time.h>
#include <stdlib.h>

TextureLib textureLib = {0};
Tile tiles[GRID_COUNT][GRID_COUNT] = {0};


Player player = {0};
int stamina = 15;
int score = 0;
int rubyAmount = 0;

GameState gameState;
Camera2D camera = {0};

// === InitGame ===

void InitBlock(TileType type, Vector2 begin, Vector2 end)
{
    for(int x = begin.x ; x < end.x ; x++)
    {
        for(int y = begin.y ; y < end.y ; y++)
        {
            tiles[x][y].type = type;
            tiles[x][y].grid = (GridPosition){x, y};
            tiles[x][y].position = (Vector2){ x * GRID_SIZE, y * GRID_SIZE };

            if( type != Lava && rand()%100 < 3)
            {
                tiles[x][y].hasRuby = true;
                tiles[x][y].texture = GetTileTexture(type, 10);
                rubyAmount ++;
                continue;
            }

            // Texture
            if(x == begin.x)        tiles[x][y].texture = GetTileTexture(type, 4);
            else if(y == begin.y)   tiles[x][y].texture = GetTileTexture(type, 2);
            else if(x == end.x - 1) tiles[x][y].texture = GetTileTexture(type, 6);
            else if(y == end.y - 1) tiles[x][y].texture = GetTileTexture(type, 8);
            else                    tiles[x][y].texture = GetTileTexture(type, 5);
        }
    }

    tiles[(int)begin.x][(int)begin.y].texture = GetTileTexture(type, 1);
    tiles[(int)end.x - 1][(int)begin.y].texture = GetTileTexture(type, 3);
    tiles[(int)begin.x][(int)end.y - 1].texture = GetTileTexture(type, 7);
    tiles[(int)end.x - 1][(int)end.y - 1].texture = GetTileTexture(type, 9);
}

void InitTile(void)
{
    rubyAmount = 0;
    int blockCount = GRID_COUNT / 3;
    int rimCount = 2;    

    InitBlock(Ice,(Vector2){rimCount,rimCount},(Vector2){GRID_COUNT,blockCount});
    InitBlock(Sand,(Vector2){rimCount,blockCount},(Vector2){blockCount,blockCount*2});
    InitBlock(Lava,(Vector2){blockCount,blockCount},(Vector2){blockCount*2,blockCount*2});
    InitBlock(Igneous,(Vector2){blockCount*2,blockCount},(Vector2){GRID_COUNT,blockCount*2});
    InitBlock(Grass,(Vector2){rimCount,blockCount*2},(Vector2){GRID_COUNT,GRID_COUNT});
}

//----------------------------------------------------------------------------------
// Module functions declaration
//----------------------------------------------------------------------------------
void UpdateCameraCenter(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraCenterInsideMap(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraCenterSmoothFollow(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraEvenOutOnLanding(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraPlayerBoundsPush(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);

void InitGame(void)
{ 
    InitTile();
    player.position = (Vector2){200, 0};
    player.speed = 0;
    player.canJump = false;
    player.texture = PathToTexture("image/hero/idle_01.png", PLAYER_SIZE);
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;
    gameState = Start;
    InitWindow(screenWidth, screenHeight, "raylib [core] example - 2d camera");
    InitTexture();

    InitGame();

    EnvItem envItems[] = {
        {{0, 0, 1000, 400}, 0, LIGHTGRAY},
    };

    int envItemsLength = sizeof(envItems) / sizeof(envItems[0]);

    camera.target = player.position;
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // Store pointers to the multiple update camera functions
    void (*cameraUpdaters[])(Camera2D *, Player *, EnvItem *, int, float, int, int) = {
        UpdateCameraCenter,
        UpdateCameraCenterInsideMap,
        UpdateCameraCenterSmoothFollow,
        UpdateCameraEvenOutOnLanding,
        UpdateCameraPlayerBoundsPush};

    int cameraOption = 0;
    int cameraUpdatersLength = sizeof(cameraUpdaters) / sizeof(cameraUpdaters[0]);

    char *cameraDescriptions[] = {
        "Follow player center",
        "Follow player center, but clamp to map edges",
        "Follow player center; smoothed",
        "Follow player center horizontally; update player center vertically after landing",
        "Player push camera on getting too close to screen edge"};

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())
    {
        if(stamina == 0) gameState == Over;

        if(gameState == Start)
        {

        }
        else if (gameState == Running)
        {
            // ===* = * = *===  Update  ===* = * = *===
            float deltaTime = GetFrameTime();
            UpdatePlayer(&player, deltaTime);

            camera.zoom += ((float)GetMouseWheelMove() * 0.05f);
            if (camera.zoom > 3.0f)
                camera.zoom = 3.0f;
            else if (camera.zoom < 0.25f)
                camera.zoom = 0.25f;
            if (IsKeyPressed(KEY_R))
            {
                camera.zoom = 1.0f;
                player.position = (Vector2){400, 280};
            }
            if (IsKeyPressed(KEY_C))
                cameraOption = (cameraOption + 1) % cameraUpdatersLength;
            // Call update camera function by its pointer
            cameraUpdaters[cameraOption](&camera, &player, envItems, envItemsLength, deltaTime, screenWidth, screenHeight);

            //----------------------------------------------------------------------------------
            // Draw
            //----------------------------------------------------------------------------------
            BeginDrawing();

            ClearBackground(LIGHTGRAY);

            BeginMode2D(camera);

                for (int i = 0; i < GRID_COUNT; i++)
                {
                    for (int j = 0; j < GRID_COUNT; j++)
                    {
                        Tile tile = tiles[i][j];
                        //if (tile.type != Empty)
                        DrawTexture(tile.texture, tile.position.x - GRID_SIZE / 2, tile.position.y - GRID_SIZE / 2, WHITE);
                        // else
                        //     DrawRectangleRec(ToRectangle(tile.position, GRID_SIZE), LIGHTGRAY);

                        //  === Debug Info === Draw Grid position
                        // char gridPosStr[1024];
                        // sprintf(gridPosStr, "(%d, %d)", tile.grid.x, tile.grid.y);
                        // DrawText(gridPosStr, tile.position.x-15, tile.position.y, 3, BLACK);
                    }
                }

                // Draw Palyer
                Vector2 drawPos = (Vector2){player.position.x - PLAYER_SIZE / 2, player.position.y - PLAYER_SIZE / 2};
                switch (player.gravityArea)
                {
                case North:
                    DrawTextureEx(player.texture, drawPos, 0, 1, WHITE);
                    break;
                case West:
                    drawPos.y += PLAYER_SIZE;
                    DrawTextureEx(player.texture, drawPos, -90, 1, WHITE);
                    break;
                case East:
                    drawPos.x += PLAYER_SIZE;
                    DrawTextureEx(player.texture, drawPos, 90, 1, WHITE);
                    break;
                case South:
                    drawPos.x += PLAYER_SIZE;
                    drawPos.y += PLAYER_SIZE;
                    DrawTextureEx(player.texture, drawPos, 180, 1, WHITE);
                    break;
                default:
                    break;
                }
                // DrawTexture(player.texture, player.position.x - PLAYER_SIZE / 2, player.position.y - PLAYER_SIZE / 2, WHITE);

            EndMode2D();

            // Debug Info:
            char positionStr[1024];
            sprintf(positionStr, "Player position : x = %f , y = %f", player.position.x, player.position.y);
            DrawText(positionStr, 20, 20, 20, BLUE);

            char gridPositionStr[1024];
            sprintf(gridPositionStr, "Player grid position : x = %d , y = %d", player.grid.x, player.grid.y);
            DrawText(gridPositionStr, 20, 40, 20, BLUE);

            char scoreStr[1024];
            sprintf(scoreStr, "Score : %d", score);
            DrawText(scoreStr, 575, 10, 30, GOLD);

            char staminaStr[1024];
            sprintf(staminaStr, "Stamina : %d", stamina);
            DrawText(staminaStr, 575, 50, 30, BLUE);

            EndDrawing();
            //----------------------------------------------------------------------------------
        }
        else if(gameState = Over)
        {
            BeginDrawing();
                DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, GetScreenHeight()/2 - 50, 20, GRAY);
            EndDrawing();
        }
        
    }

    UnloadGame();
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

Rectangle ToRectangle(Vector2 position, int size)
{
    return (Rectangle){position.x - size / 2, position.y - size / 2, size, size};
}

void UpdateCameraCenter(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
    camera->offset = (Vector2){width / 2.0f, height / 2.0f};
    camera->target = player->position;
}

void UpdateCameraCenterInsideMap(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
    camera->target = player->position;
    camera->offset = (Vector2){width / 2.0f, height / 2.0f};
    float minX = 1000, minY = 1000, maxX = -1000, maxY = -1000;

    for (int i = 0; i < envItemsLength; i++)
    {
        EnvItem *ei = envItems + i;
        minX = fminf(ei->rect.x, minX);
        maxX = fmaxf(ei->rect.x + ei->rect.width, maxX);
        minY = fminf(ei->rect.y, minY);
        maxY = fmaxf(ei->rect.y + ei->rect.height, maxY);
    }

    Vector2 max = GetWorldToScreen2D((Vector2){maxX, maxY}, *camera);
    Vector2 min = GetWorldToScreen2D((Vector2){minX, minY}, *camera);

    if (max.x < width)
        camera->offset.x = width - (max.x - width / 2);
    if (max.y < height)
        camera->offset.y = height - (max.y - height / 2);
    if (min.x > 0)
        camera->offset.x = width / 2 - min.x;
    if (min.y > 0)
        camera->offset.y = height / 2 - min.y;
}

void UpdateCameraCenterSmoothFollow(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
    static float minSpeed = 30;
    static float minEffectLength = 10;
    static float fractionSpeed = 0.8f;

    camera->offset = (Vector2){width / 2.0f, height / 2.0f};
    Vector2 diff = Vector2Subtract(player->position, camera->target);
    float length = Vector2Length(diff);

    if (length > minEffectLength)
    {
        float speed = fmaxf(fractionSpeed * length, minSpeed);
        camera->target = Vector2Add(camera->target, Vector2Scale(diff, speed * delta / length));
    }
}

void UpdateCameraEvenOutOnLanding(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
    static float evenOutSpeed = 700;
    static int eveningOut = false;
    static float evenOutTarget;

    camera->offset = (Vector2){width / 2.0f, height / 2.0f};
    camera->target.x = player->position.x;

    if (eveningOut)
    {
        if (evenOutTarget > camera->target.y)
        {
            camera->target.y += evenOutSpeed * delta;

            if (camera->target.y > evenOutTarget)
            {
                camera->target.y = evenOutTarget;
                eveningOut = 0;
            }
        }
        else
        {
            camera->target.y -= evenOutSpeed * delta;

            if (camera->target.y < evenOutTarget)
            {
                camera->target.y = evenOutTarget;
                eveningOut = 0;
            }
        }
    }
    else
    {
        if (player->canJump && (player->speed == 0) && (player->position.y != camera->target.y))
        {
            eveningOut = 1;
            evenOutTarget = player->position.y;
        }
    }
}

void UpdateCameraPlayerBoundsPush(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
    static Vector2 bbox = {0.2f, 0.2f};

    Vector2 bboxWorldMin = GetScreenToWorld2D((Vector2){(1 - bbox.x) * 0.5f * width, (1 - bbox.y) * 0.5f * height}, *camera);
    Vector2 bboxWorldMax = GetScreenToWorld2D((Vector2){(1 + bbox.x) * 0.5f * width, (1 + bbox.y) * 0.5f * height}, *camera);
    camera->offset = (Vector2){(1 - bbox.x) * 0.5f * width, (1 - bbox.y) * 0.5f * height};

    if (player->position.x < bboxWorldMin.x)
        camera->target.x = player->position.x;
    if (player->position.y < bboxWorldMin.y)
        camera->target.y = player->position.y;
    if (player->position.x > bboxWorldMax.x)
        camera->target.x = bboxWorldMin.x + (player->position.x - bboxWorldMax.x);
    if (player->position.y > bboxWorldMax.y)
        camera->target.y = bboxWorldMin.y + (player->position.y - bboxWorldMax.y);
}
