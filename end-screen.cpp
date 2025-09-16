#include "end-screen.h"
#include <mgdl.h>

static Menu* endmenu;
static const int AMOUNT = 8;
static const char* costs[AMOUNT] = {
    "Saucer fuel and repairs......",
    "Jump gate official bribes....",
    "Terrestial microbe removal...",
    "Protection money.............",
    "Saucer warranty..............",
    "Saucer Hazard warranty.......",
    "Hyperway motel room..........",
    "Ice cream addiction treatment",
};
static int minus_targets[AMOUNT] = {
    1,2,3,4,5,6,7,8};

static int minus_draws[AMOUNT] = {
    1,2,3,4,5,6,7,8};

static float score = 0;
static int score_draw = 0;
static int score_target= 0;
static const int increase = 4;
static float milkTimer = 0;

void DrawMilk();
bool DrawScores();

void End_Init()
{
    Font* df = DefaultFont_GetDefaultFont();
    endmenu = Menu_Create(df, 2, 1.2f);
    Random_CreateGenerator();
}
void End_Reset()
{
    milkTimer = 0;
    score_draw = 0;
    score_target = 0;
}

void End_CalculateScore(float iceCreamAmount, float time)
{
    score = iceCreamAmount * 1000 - time;

    int budget = (int)floor(score) - Random_Int(10, 100);
    for (int i = 0; i < AMOUNT; i++ )
    {
        minus_draws[i] = 0;
        minus_targets[i] = Random_Int(10, (budget/20) * (i+1));
        budget -= minus_targets[i];
    }
    score_target = score;
}


bool End_Run()
{
    bool restart = DrawScores();
    DrawMilk();
    return restart;

}
bool DrawScores()
{

    Color4f LightGrey = Color_HexToFloats(0xd8d8d8ff);
    Color4f DarkGreen = Color_Create4f((float)0x16/255.0f, (float)0x63/255.0f, (float)0x18/255.0f, 1.0f);
    mgdl_glClearColor4f(&DarkGreen);
    mgdl_InitOrthoProjection();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    Menu_Start(endmenu, 32, mgdl_GetScreenHeight()-32, mgdl_GetScreenWidth()/2);

    int visible = 0;


    Menu_SetColors(endmenu, &DarkGreen,Color_GetDefaultColor(Color_White), Color_GetDefaultColor(Color_White));
    Menu_Text(endmenu, "Ice cream sold to highest bidder!");

    Menu_SetColors(endmenu, &DarkGreen,Color_GetDefaultColor(Color_White), Color_GetDefaultColor(Color_White));
    Menu_TextF(endmenu, "Black Market Profit %d", score_draw);

    if (score_draw < score_target)
    {
        score_draw += increase;
        if (score_draw > score_target)
        {
            score_draw = score_target;
        }
        return false;
    }
    // Make first cost visible
    visible = 1;

    Menu_SetColors(endmenu, &DarkGreen,Color_GetDefaultColor(Color_Black), Color_GetDefaultColor(Color_White));
    Menu_Text(endmenu, "---------------------------------");
    Menu_Text(endmenu, "Costs:");
    Menu_SetColors(endmenu, &DarkGreen,&LightGrey, Color_GetDefaultColor(Color_White));

    int profit = score_target;
    // Increase counters
    for (int i = 0; i < AMOUNT; i++ )
    {
        if (minus_draws[i] < minus_targets[i])
        {
            minus_draws[i] += increase;
            if (minus_draws[i] > minus_targets[i])
            {
                minus_draws[i] = minus_targets[i];
            }
            break;
        }
        // Make this score visible
        visible += 1;
        profit -= minus_targets[i];
    }

    bool allcosts = false;
    if (visible >= AMOUNT)
    {
        visible = AMOUNT;
        allcosts = true;
    }

    for (int i = 0; i < visible; i++ )
    {
        Menu_TextF(endmenu, "%s : -%i", costs[i], minus_draws[i]);
    }
    if (allcosts)
    {
        Menu_SetColors(endmenu, &DarkGreen,Color_GetDefaultColor(Color_White), Color_GetDefaultColor(Color_White));
        Menu_Text(endmenu, "---------------------------------");
        Menu_TextF(endmenu, "Total :                         %d", profit);

        Menu_Skip(endmenu, 64);
        Menu_Text(endmenu, "Press A to try again!");

        if (WiiController_ButtonPress(mgdl_GetController(0), ButtonA))
        {
            return true;
        }
    }
    return false;
}

void DrawMilk()
{
    milkTimer += mgdl_GetDeltaTime() * 80.0f;
    float r = mgdl_GetElapsedSeconds();
    float ss = r + cos(r/2.0f) * 10.0f;
    float h = mgdl_GetScreenHeight() / 4.0f;
    float level = mgdl_GetScreenHeight() - 80.0f;
    float bottom = 0.0f - milkTimer;
    if (bottom + level + h > -mgdl_GetScreenHeight())
    {

        glBegin(GL_LINES);
        mgdl_glColor4f(Color_GetDefaultColor(Color_White));

        for(int i = 0; i < mgdl_GetScreenWidth(); i++)
        {
            glVertex2f(i, bottom);
            glVertex2f(i, bottom + level + h + sin(ss + i * 0.025f) * (h/3.0f)*sin(r+i*0.01f)*1.0f);
        }

        glEnd();
    }
}

