#include "raylib.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MAX_BULLETS 20
#define MAX_ENEMIES 5

typedef struct {
    Rectangle rect;
    Color color;
} Entity;

typedef struct {
    Rectangle rect;
    bool active;
    Color color;
} Projectile;

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Space Shooter");
    InitAudioDevice();
    SetTargetFPS(60);

    Sound shot = LoadSound("pew.mp3");
    Sound boom = LoadSound("gameOver.mp3");
    Sound loss = LoadSound("lifeLost.mp3");

    int shakeFrames = 0;
    int playerShakeX = 0;

    Texture2D shipTexture = LoadTexture("spaceship.png");

    Entity player = { {SCREEN_WIDTH/2 - 20, SCREEN_HEIGHT - 60, 40, 40}, BLUE };
    
    Projectile bullets[MAX_BULLETS] = {0};
    
    Entity enemies[MAX_ENEMIES] = {0};
    for(int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].rect = (Rectangle){GetRandomValue(0, SCREEN_WIDTH - 40), GetRandomValue(-1000, -100), 40, 40};
        enemies[i].color = RED;
    }

    int score = 0;
    int lives = 3;
    bool gameOver = false;

    while (!WindowShouldClose()) {
        if (!gameOver) {
            if (shakeFrames > 0) {
                playerShakeX = GetRandomValue(-5, 5);
                shakeFrames--;
            } else {
                playerShakeX = 0;
            }
            if (IsKeyDown(KEY_RIGHT)) player.rect.x += 8.0f;
            if (IsKeyDown(KEY_LEFT)) player.rect.x -= 8.0f;

            if (player.rect.x < 0) player.rect.x = 0;
            if (player.rect.x > SCREEN_WIDTH - player.rect.width) player.rect.x = SCREEN_WIDTH - player.rect.width;

            if (IsKeyPressed(KEY_SPACE)) {
                PlaySound(shot);
                for (int i = 0; i < MAX_BULLETS; i++) {
                    if (!bullets[i].active) {
                        bullets[i].rect = (Rectangle){ player.rect.x + 15, player.rect.y, 10, 20 };
                        bullets[i].active = true;
                        bullets[i].color = YELLOW;
                        break;
                    }
                }
            }

            for (int i = 0; i < MAX_BULLETS; i++) {
                if (bullets[i].active) {
                    bullets[i].rect.y -= 10.0f;
                    if (bullets[i].rect.y < 0) bullets[i].active = false;
                }
            }

            for (int i = 0; i < MAX_ENEMIES; i++) {
                enemies[i].rect.y += 4.0f;

                if (enemies[i].rect.y > SCREEN_HEIGHT) {
                    enemies[i].rect.y = GetRandomValue(-1000, -100);
                    enemies[i].rect.x = GetRandomValue(0, SCREEN_WIDTH - 40);
                }

                if (CheckCollisionRecs(player.rect, enemies[i].rect)) {
                    if(lives>1) PlaySound(loss);
                    lives--;
                    shakeFrames = 15;
                    enemies[i].rect.y = GetRandomValue(-1000, -100);
                    enemies[i].rect.x = GetRandomValue(0, SCREEN_WIDTH - 40);
                    if(lives <= 0) 
                    {
                        gameOver = true;
                        PlaySound(boom);
                    }
                }
            }

            for (int i = 0; i < MAX_BULLETS; i++) {
                if (!bullets[i].active) continue;
                for (int j = 0; j < MAX_ENEMIES; j++) {
                    if (CheckCollisionRecs(bullets[i].rect, enemies[j].rect)) {
                        bullets[i].active = false;
                        enemies[j].rect.y = GetRandomValue(-1000, -100);
                        enemies[j].rect.x = GetRandomValue(0, SCREEN_WIDTH - 40);
                        score += 100;
                    }
                }
            }
        } else {
            if (IsKeyPressed(KEY_R)) {
                gameOver = false;
                shakeFrames = 0;
                playerShakeX = 0;
                score = 0;
                player.rect.x = SCREEN_WIDTH/2 - 20;
                for(int i = 0; i < MAX_ENEMIES; i++) {
                    enemies[i].rect.y = GetRandomValue(-1000, -100);
                    enemies[i].rect.x = GetRandomValue(0, SCREEN_WIDTH - 40);
                }
                for(int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;
                lives = 3;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        if (!gameOver) {
            DrawTexturePro(shipTexture, 
                           (Rectangle){0, 0, (float)shipTexture.width, (float)shipTexture.height}, 
                           (Rectangle){player.rect.x+playerShakeX,player.rect.y,player.rect.width,player.rect.height}, 
                           (Vector2){0, 0}, 
                           0.0f, 
                           WHITE);
            
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (bullets[i].active) DrawRectangleRec(bullets[i].rect, bullets[i].color);
            }

            for (int i = 0; i < MAX_ENEMIES; i++) {
                DrawRectangleRec(enemies[i].rect, enemies[i].color);
            }

            DrawText(TextFormat("SCORE: %4d", score), 20, 20, 20, WHITE);
            DrawText(TextFormat("LIVES: %3d", lives), 20, 40, 20, WHITE);
        } else {
            DrawText("GAME OVER", SCREEN_WIDTH/2 - 125, SCREEN_HEIGHT/2 - 30, 40, WHITE);
            DrawText("Press 'R' to Restart", SCREEN_WIDTH/2 - 125, SCREEN_HEIGHT/2 + 20, 20, GRAY);
        }

        EndDrawing();
    }

    UnloadTexture(shipTexture);
    UnloadSound(shot);
    UnloadSound(loss);
    UnloadSound(boom);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}