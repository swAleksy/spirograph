#include <iostream>
#include <cmath>
#include "raylib.h"
#include <algorithm>

void resetTime(float* accumulatedTime, bool* firstDraw) {
    *accumulatedTime = 0.0f;
    *firstDraw = true;
}

float DrawSliderBar(float x, float y, float width, float minValue, float maxValue, float* value) {
    DrawRectangle(x, y, width, 10, LIGHTGRAY);
    float handlePosX = x + ((*value - minValue) / (maxValue - minValue)) * width;
    DrawCircle(handlePosX, y + 5, 8, DARKGRAY);

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();
        if (mousePos.x >= x && mousePos.x <= x + width && mousePos.y >= y - 10 && mousePos.y <= y + 20) {
            *value = ((mousePos.x - x) / width) * (maxValue - minValue) + minValue;
        }
    }

    return *value;
}

int main(void) {
    // Initialization
    srand((unsigned) time(NULL));
    const int screenWidth = 800;
    const int screenHeight = 800;
    Color col = RED;

    InitWindow(screenWidth, screenHeight, "raylib - Spirographs");

    float accumulatedTime = 0.0f;
    bool firstDraw = true;


    float fixedCircleRad = 200.0f;
    float movingCircleRad = 75.0f;
    Vector2 movingCirclePos;
    Vector2 fixedCirclePos = {400.0f, 400.0f};

    float penOffsetRad = 65.0f;
    float ratio = fixedCircleRad / movingCircleRad;
    Vector2 penOffset;

    Vector2 drawPoint;
    Vector2 oldDrawPoint;

    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);

    SetTargetFPS(60);   
    
    // Main game loop
    while (!WindowShouldClose()) {
        
        // Update
        if (IsKeyDown(KEY_SPACE)){
            accumulatedTime += GetFrameTime() * 10.0f;
            movingCirclePos.x = (fixedCircleRad - movingCircleRad) * cos(accumulatedTime);
            movingCirclePos.y = (fixedCircleRad - movingCircleRad) * sin(accumulatedTime);
            ratio = fixedCircleRad / movingCircleRad;
            penOffset.x = penOffsetRad * cos(-accumulatedTime * ratio);
            penOffset.y = penOffsetRad * sin(-accumulatedTime * ratio);
            drawPoint.x = fixedCirclePos.x + movingCirclePos.x + penOffset.x;
            drawPoint.y = fixedCirclePos.y + movingCirclePos.y + penOffset.y;
        }

        if (IsKeyDown(KEY_R)) {
            resetTime(&accumulatedTime, &firstDraw);
            BeginTextureMode(target);
            ClearBackground(BLANK);
            EndTextureMode();           
        }

        if (IsKeyDown(KEY_SPACE)){
            if (firstDraw){
                oldDrawPoint = drawPoint;
                firstDraw = false;
            }
            else {
                BeginTextureMode(target); // Draw to the RenderTexture2D
                if (col.a == 255) {
                    if(col.g < 255 )
                        col.g++;
                    else if (col.b < 255)
                        col.b++;
                    else
                        col.a = 254;
                }
                else {
                    if(col.g >0)
                        col.g--;
                    else if (col.b > 0)
                        col.b--;
                    else
                        col.a = 255;
                }
                DrawLineEx(oldDrawPoint, drawPoint, 6.0f, col);
                EndTextureMode();
            }
        }

        fixedCircleRad = DrawSliderBar(50, 700, 700, 50.0f, 300.0f, &fixedCircleRad);
        movingCircleRad = DrawSliderBar(50, 750, 700, 50.0f, 300.0f, &movingCircleRad);

        // Draw
        BeginDrawing();
            ClearBackground(DARKGRAY); // Clear background each frame

            DrawCircleLines(fixedCirclePos.x, fixedCirclePos.y, fixedCircleRad, RAYWHITE);
            DrawCircleLines(fixedCirclePos.x + movingCirclePos.x, fixedCirclePos.y + movingCirclePos.y, movingCircleRad, RAYWHITE);
            DrawCircleLines(drawPoint.x, drawPoint.y, 4, RAYWHITE);
            DrawText(TextFormat("Fixed Circle Radius"), 50, 675, 20, RAYWHITE);
            DrawText(TextFormat("Moving Circle Radius"), 50, 725, 20, RAYWHITE);

            // Draw the persistent lines from the RenderTexture2D
            DrawTextureRec(target.texture, { 0, 0, (float)target.texture.width, -(float)target.texture.height }, { 0, 0 }, WHITE);

        EndDrawing();
        oldDrawPoint = drawPoint;
    }

    // De-Initialization
    UnloadRenderTexture(target);
    CloseWindow();        // Close window and OpenGL context

    return 0;
}
