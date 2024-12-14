#include "raylib.h"
#include <stdlib.h> 
#include <string.h> 

// Struct to hold animation data
typedef struct AnimData 
{
	Rectangle rec;
	Vector2 pos;
	int frame;
	float updateTime;
	float runningTime;
} AnimData;

// Get an IsOnGround bool
bool IsOnGround(const AnimData* data, int windowHeight) 
{
	return data->pos.y >= windowHeight - data->rec.height;
}

// Update animation logic
void UpdateAnimData(AnimData* data, float deltaTime, int maxFrame) 
{
	data->runningTime += deltaTime;
	if (data->runningTime >= data->updateTime) 
    {
		data->runningTime = 0.0f;
		data->frame = (data->frame + 1) % (maxFrame + 1);
		data->rec.x = data->frame * data->rec.width;
	}
}

// Parralax BGs
void DrawScrollingBackground(Texture2D texture, float* offset, float speed, float scale) 
{
	*offset -= speed;
	if (*offset <= -texture.width * scale) {
		*offset = 0.0f;
	}
	Vector2 pos1 = {*offset, 0.0f};
	Vector2 pos2 = {*offset + texture.width * scale, 0.0f};
	DrawTextureEx(texture, pos1, 0.0f, scale, WHITE);
	DrawTextureEx(texture, pos2, 0.0f, scale, WHITE);
}

int main() 
{
	// Window dimensions
	const int windowWidth = 512;
	const int windowHeight = 380;

	InitWindow(windowWidth, windowHeight, "Dapper Dasher");
	SetTargetFPS(60);

	// Constants
	const int gravity = 1'000;
	const int jumpVel = -600;
	const int nebVel = -200;
	const int numNebulae = 20;

    // Boolean flags
	bool collision = false;
	bool playerWon = false;

	// Background textures
	Texture2D bg = LoadTexture("textures/far-buildings.png");
	Texture2D mid = LoadTexture("textures/back-buildings.png");
	Texture2D fore = LoadTexture("textures/foreground.png");
	float bgX = 0, midX = 0, foreX = 0;

	// Character textures
	Texture2D scarfy = LoadTexture("textures/scarfy.png");
	AnimData scarfyData = 
    {
		{0.0f, 0.0f, scarfy.width / 6.0f, (float)scarfy.height},
		{windowWidth / 2.0f - scarfy.width / 12.0f, (float)(windowHeight - scarfy.height)},
		0,
		1.0f / 12.0f,
		0.0f
	};
	int velocity = 0;

	// Nebulae textures
	Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");
    
    // C style Dynamic memory allocation, in C++ would be: AnimData* nebulae = new AnimData[numNebulae]; but it can add an overhead. For a simple struct prefer C style.
	AnimData* nebulae = (AnimData*)malloc(numNebulae * sizeof(AnimData));  
	if (nebulae == NULL) 
    {
		CloseWindow();
		return -1; // if allocation failed
	}    
	memset(nebulae, 0, numNebulae * sizeof(AnimData));// Preventing UB

	for (int i = 0; i < numNebulae; i++) 
    {
		nebulae[i].rec = (Rectangle){0.0f, 0.0f, nebula.width / 8.0f, nebula.height / 8.0f};
		nebulae[i].pos.x = windowWidth + i * 500.0f;
		nebulae[i].pos.y = windowHeight - nebula.height / 8.0f;
		nebulae[i].updateTime = 1.0f / 16.0f;
	}

	float finishLine = nebulae[numNebulae - 1].pos.x + 50;

	// Game loop
	while (!WindowShouldClose()) {
		const float deltaTime = GetFrameTime();

		// Parrallax scrolling
		DrawScrollingBackground(bg, &bgX, 20 * deltaTime * 1.1f, 2.0f);
		DrawScrollingBackground(mid, &midX, 20 * deltaTime * 1.5f, 2.0f);
		DrawScrollingBackground(fore, &foreX, 20 * deltaTime * 1.9f, 2.0f);

		// Ground check logic
		if (IsOnGround(&scarfyData, windowHeight)) 
        {
			velocity = 0;
			if (IsKeyPressed(KEY_SPACE)) 
            {
				velocity = jumpVel;
			}
		} 
        else 
        {
			velocity += gravity * deltaTime;
            scarfyData.frame = 1;
		}

		// Update positions
		scarfyData.pos.y += velocity * deltaTime;

		// Update animations
		UpdateAnimData(&scarfyData, deltaTime, 5);
		for (int i = 0; i < numNebulae; i++) 
        {
			nebulae[i].pos.x += nebVel * deltaTime;
			UpdateAnimData(&nebulae[i], deltaTime, 7);
		}

		// Check collisions
		for (int i = 0; i < numNebulae; i++) 
        {
			Rectangle nebRect = {
				nebulae[i].pos.x + 50, nebulae[i].pos.y + 50,
				nebulae[i].rec.width - 100, nebulae[i].rec.height - 100
			};
			Rectangle scarfyRect = {
				scarfyData.pos.x, scarfyData.pos.y,
				scarfyData.rec.width, scarfyData.rec.height
			};
			if (CheckCollisionRecs(nebRect, scarfyRect)) collision = 1;
		}

		// Draw game state
		BeginDrawing();
		ClearBackground(WHITE);

		if (collision && !playerWon) 
        {
			DrawText("GAME OVER", windowWidth / 2 - 150, windowHeight / 2, 50, RED);
		} else if (scarfyData.pos.x > finishLine) 
        {
			DrawText("YOU WIN!", windowWidth / 2 - 125, windowHeight / 2, 50, GREEN);
			playerWon = 1;
		} else 
        {
			for (int i = 0; i < numNebulae; i++) {
				DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
			}
			DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);
		}

		EndDrawing();
	}

	// Cleanup
	free(nebulae);
	UnloadTexture(scarfy);
	UnloadTexture(nebula);
	UnloadTexture(bg);
	UnloadTexture(mid);
	UnloadTexture(fore);
	CloseWindow();

	return 0;
}
