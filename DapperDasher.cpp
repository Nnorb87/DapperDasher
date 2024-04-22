#include <iostream>
#include "raylib.h"
#include <vector>

struct AnimData {
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
    Color color;
};

bool isOnGround(AnimData data, int windowHeight) {
    return data.pos.y >= windowHeight - data.rec.height;
}

void updateAnimData(AnimData& data, const int maxFrameCount, const float& deltaTime) {
    data.runningTime += deltaTime;
    data.rec.x = data.frame * data.rec.width;
    if (data.runningTime >= data.updateTime) {
        data.frame++;
        if (data.frame > maxFrameCount) {
            data.frame = 0;
        }
        data.runningTime = 0.f;
    }

}

void moveBackground(const Texture2D& bg, float& CoordX, const float& deltaTime, const float& scrollSpeed) {
    CoordX -= scrollSpeed * deltaTime;

    if (CoordX <= -bg.width * 2) {
        CoordX = 0;
    }

    Vector2 bg1Pos{ CoordX, 0.f };
    Vector2 bg2Pos{ CoordX + bg.width * 2, 0.f };

    DrawTextureEx(bg, bg1Pos, 0.f, 2.f, WHITE);
    DrawTextureEx(bg, bg2Pos, 0.f, 2.f, WHITE);
}

int main()
{  

    const int WINDOW_WIDTH{ 512 };
    const int WINDOW_HEIGHT{ 380 };

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Dapper Dasher");
    SetTargetFPS(60);

    // scarfy section
    const int gravity{ 1'000 };                     // (pixels/s)/s
    const int jumpVel{ -600 };                      // pixels/s

    bool collision{};
    bool reachedFinishLine{};
    int velocity{};
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    AnimData scarfyData{ {0, 0, scarfy.width / 6, scarfy.height}, { WINDOW_WIDTH / 2 - (scarfy.width / 6) / 2,  WINDOW_HEIGHT - scarfy.height}, 0, 1.f / 12.f, 0.f };

    // nebula section
    const Color randomColorPalette[4]{ WHITE, RED, BLUE, GREEN };
    const int nebuleaNumber{ 30 };

    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");
    int nebVel{ -250 }; // pixels/s
    int rangeBetweenNebulea{};
    AnimData nebulea[nebuleaNumber]{};

    for (int i = 0; i < nebuleaNumber; i++) {
        nebulea[i].rec.x = 0.f;
        nebulea[i].rec.y = 0.f;
        nebulea[i].rec.width = nebula.width / 8;
        nebulea[i].rec.height = nebula.height / 8;
        nebulea[i].pos.x = WINDOW_WIDTH + rangeBetweenNebulea;
        nebulea[i].pos.y = WINDOW_HEIGHT - nebula.height / 8;
        nebulea[i].frame = 0;
        nebulea[i].runningTime = 0.f;
        nebulea[i].updateTime = 0.f;
        nebulea[i].color = randomColorPalette[GetRandomValue(0, 4)];

        rangeBetweenNebulea += GetRandomValue(300, 500);
    }

    // finish line
    float finishLine{nebulea[nebuleaNumber - 1].pos.x};
    
    // background section
    Texture2D background = LoadTexture("textures/far-buildings.png");
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    Texture2D foreground = LoadTexture("textures/foreground.png");
    float bgX{}, mgX{}, fgX{};

    // game loop
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);
        const float dT{ GetFrameTime() };

        // background
        moveBackground(background, bgX, dT, 20);
        moveBackground(midground, mgX, dT, 50);
        moveBackground(foreground, fgX, dT, 100);

        // scarfy gravity
        if (isOnGround(scarfyData, WINDOW_HEIGHT)) {
            velocity = 0;
        } else {
            velocity += gravity * dT;
        }

        // scarfy jump
        if (IsKeyPressed(KEY_SPACE) && isOnGround(scarfyData, WINDOW_HEIGHT)) {
            velocity += jumpVel;
        }

        // scarfy update
        scarfyData.pos.y += velocity * dT;

        // scarfy anim
        if (isOnGround(scarfyData, WINDOW_HEIGHT)) {
            updateAnimData(scarfyData, 5, dT);
        }

        // nebula drawing & anim & collison
        if (!collision || reachedFinishLine) {

            for (AnimData& neb : nebulea) {
                neb.pos.x += nebVel * dT;
                updateAnimData(neb, 7, dT);
                DrawTextureRec(nebula, neb.rec, neb.pos, neb.color);
                float pad{ 50 };
                Rectangle nebRec{ neb.pos.x + pad, neb.pos.y + pad, neb.rec.width - 2 * pad, neb.rec.width - 2 * pad };
                Rectangle scarfyRec{ scarfyData.pos.x, scarfyData.pos.y, scarfyData.rec.width, scarfyData.rec.height };

                if (CheckCollisionRecs(nebRec, scarfyRec)) {
                    collision = true;
                }
            }

            // scarfy draw
            DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);
        }


        if (collision) {
            DrawText("GAME OVER", (WINDOW_WIDTH / 2) - 180, WINDOW_HEIGHT / 2 - 50, 60, RED);
        }
        else if (reachedFinishLine) {
            DrawText("YOU WON", (WINDOW_WIDTH / 2) - 150, WINDOW_HEIGHT / 2 - 50, 60, GREEN);
        }


        // update finish line
        finishLine += nebVel * dT;

        reachedFinishLine = scarfyData.pos.x >= finishLine;



        EndDrawing();
    }
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    CloseWindow();
}