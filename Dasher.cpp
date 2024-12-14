#include "raylib.h"
#include <string>



struct AnimData
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

// >= windowDimArr[1] - scarfyData.rec.height
bool isOnGround(AnimData data, int windowHeight) 
{
    return data.pos.y >= windowHeight - data.rec.height;
};

AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame)
{
    // update RunningTime
    data.runningTime += deltaTime;
    if (data.runningTime >= data.updateTime)
    {
        data.runningTime = 0.0;
        // Set animation frame
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        if (data.frame > maxFrame) data.frame = 0;        
    }
    return data;
};

int main()
{
    // windowdim array
    const int windowDimArr[2] {512, 380};

    // Acceleration due to gravity (pixels/f/f)
    const int gravity{1'000};

    const int jumpVel {-600};

    int velocity{0};

    // Boolean flags
    bool is_on_ground = true;
    bool collision = false;    
    bool player_won = false;
   

    InitWindow(windowDimArr[0], windowDimArr[1], "Dapper Dasher");
    SetTargetFPS(60);
    // Textures

    // Backgrounds
    Texture2D Background = LoadTexture("textures/far-buildings.png");
    Texture2D midGrd = LoadTexture("textures/back-buildings.png");
    Texture2D foreGrd = LoadTexture("textures/foreground.png");


    //Scarfy
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    AnimData scarfyData;
    scarfyData.rec.width = scarfy.width/6;
    scarfyData.rec.height = scarfy.height;  
    scarfyData.rec.x = 0;
    scarfyData.rec.y = 0;
    scarfyData.pos.x = windowDimArr[0]/2 - scarfyData.rec.width/2;
    scarfyData.pos.y = windowDimArr[1] - scarfyData.rec.height;
    scarfyData.frame = 0;
    scarfyData.updateTime = 1.0/12.0;
    scarfyData.runningTime = 0.0;


    // Nebula
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");
    float bgX{};
    float midbgX{};
    float forebgX{};

    // Neb X vel (px/s)
    int nebVel {-200}; 

    // number of nebulae
    int n {20};

    // Animdata for Nebula
      
    AnimData nebulae[n]{};    

    for (int i{0};i<n+1;i++)
    {
        nebulae[i].rec = {0.0, 0.0, nebula.width/8, nebula.height/8};
        nebulae[i].pos.x = windowDimArr[0] + (500 * i);
        nebulae[i].pos.y = windowDimArr[1] - nebula.height / 8;              
        nebulae[i].frame = 0;
        nebulae[i].updateTime =  1.0/16.0;
        nebulae[i].runningTime = 0.0;        
    }
    
    float finishLine {nebulae[n-1].pos.x + 50};

    while(!WindowShouldClose())
    {   
        // Delta time (time since last frame)
        const float dT {GetFrameTime()};
        // Beginning drawing
        BeginDrawing();
        ClearBackground(WHITE);

        // Background handling
        bgX -= 20 * dT*1.1;
        midbgX -= 20 * dT*1.5;
        forebgX -= 20 * dT*1.90;
        
        if (bgX <= - Background.width * 2) bgX = 0.0;
        if (midbgX <= - Background.width * 2) midbgX = 0.0;
        if (forebgX <= - Background.width * 2) forebgX = 0.0;

        // Far background
        Vector2 bg1Pos{bgX,0.0};
        Vector2 bg2Pos{bgX + Background.width * 2, 0.0};
        DrawTextureEx(Background, bg1Pos, 0.0, 2.0, WHITE);
        DrawTextureEx(Background, bg2Pos, 0.0, 2.0, WHITE);

        // Middle background
        Vector2 midbg1Pos{midbgX,0.0};
        Vector2 midbg2Pos{midbgX + Background.width * 2, 0.0};
        DrawTextureEx(midGrd, midbg1Pos, 0.0, 2.0, WHITE);
        DrawTextureEx(midGrd, midbg2Pos, 0.0, 2.0, WHITE);

        // Foreground
        Vector2 forebg1Pos{forebgX,0.0};
        Vector2 forebg2Pos{forebgX + Background.width * 2, 0.0};
        DrawTextureEx(foreGrd, forebg1Pos, 0.0, 2.0, WHITE);
        DrawTextureEx(foreGrd, forebg2Pos, 0.0, 2.0, WHITE);
        
        /*Game Logic Start*/   

        // Perform ground check
        if ( isOnGround(scarfyData, windowDimArr[1])) 
        {
            is_on_ground = true,
            velocity = 0;
            
        }  
        else
        {
            is_on_ground = false;
            velocity += gravity * dT;            
        }

        if (IsKeyPressed(KEY_SPACE) && is_on_ground)
        {
            is_on_ground = false;
            velocity = jumpVel;                       
        }

        // Finishline crossing
        finishLine += nebVel * dT;

        for (AnimData nebula : nebulae)
        {
            int pad{50};
            Rectangle nebRec
            {
                nebula.pos.x + pad,
                nebula.pos.y + pad,
                nebula.rec.width - pad * 2,
                nebula.rec.height - pad * 2
            };
            Rectangle scarfyRec
            {
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rec.width,
                scarfyData.rec.height
            };
            if (CheckCollisionRecs(nebRec, scarfyRec)) collision = true;
        }

        // Update position
        scarfyData.pos.y += velocity * dT;        

        // Update scarfy animation frame
        scarfyData.runningTime += dT;
        if (!is_on_ground) scarfyData.frame = 0;
        scarfyData = updateAnimData(scarfyData, dT*0.5, 5);  
        if (collision && !player_won)
        {
            DrawText("GAME OVER",windowDimArr[0]/2 - 150,windowDimArr[1]/2,50,WHITE);            
        }
        else if (scarfyData.pos.x > finishLine)
        {
            DrawText("YOU WIN",windowDimArr[0]/2 - 125,windowDimArr[1]/2,50,WHITE);
            player_won = true;
        }
        else
        {
            for (int i{0};i<n+1;i++)
            {   
                nebulae[i].pos.x += nebVel * dT;
                nebulae[i] = updateAnimData(nebulae[i], dT*0.5, 7);
            
                // Draw the nebula
                DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);

                // Draw scarfy
                DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);
            }      
        }  
     
        /*Game Logic End*/

        // Ending drawing
        EndDrawing();
    }
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(Background);
    CloseWindow();
}   