#include "end-screen.h"
#include <mgdl.h>

static Menu* endmenu;
static const int AMOUNT = 8;
static const char* costs[AMOUNT] = {
    "Saucer fuel and repairs......",
    "Jump gate offcial bribes.....",
    "Terrestial microbe removal...",
    "Protection money.............",
    "Saucer warranty..............",
    "Saucer Hazard warranty.......",
    "Rent.........................",
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

void End_Init()
{
    Font* df = DefaultFont_GetDefaultFont();
    endmenu = Menu_Create(df, 2, 1.2f);
    Random_CreateGenerator();
}

void End_CalculateScore(float iceCreamAmount, float time)
{
    score = iceCreamAmount * 1000 - time;

    int budget = (int)floor(score) - Random_Int(10, 100);
    for (int i = 0; i < AMOUNT; i++ )
    {
        minus_targets[i] = Random_Int(10, (budget/20) * (i+1));
        budget -= minus_targets[i];
    }
    score_target = score;
}


bool End_Run()
{
    Color4f *color_sky = Color_GetDefaultColor(Color_Black);
    mgdl_glClearColor4f(color_sky);
    mgdl_InitOrthoProjection();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    Menu_Start(endmenu, 32, mgdl_GetScreenHeight()-32, mgdl_GetScreenWidth()/2);

    int visible = 0;


    Menu_SetColors(endmenu, Color_GetDefaultColor(Color_Black),Color_GetDefaultColor(Color_White), Color_GetDefaultColor(Color_White));
    Menu_Text(endmenu, "Ice cream sold to highest bidder!");

    Menu_SetColors(endmenu, Color_GetDefaultColor(Color_Black),Color_GetDefaultColor(Color_Green), Color_GetDefaultColor(Color_White));
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

    Menu_SetColors(endmenu, Color_GetDefaultColor(Color_Black),Color_GetDefaultColor(Color_White), Color_GetDefaultColor(Color_White));
    Menu_Text(endmenu, "---------------------------------");
    Menu_SetColors(endmenu, Color_GetDefaultColor(Color_Black),Color_GetDefaultColor(Color_Red), Color_GetDefaultColor(Color_White));
    Menu_Text(endmenu, "Costs:");

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
        Menu_SetColors(endmenu, Color_GetDefaultColor(Color_Black),Color_GetDefaultColor(Color_White), Color_GetDefaultColor(Color_White));
        Menu_Text(endmenu, "---------------------------------");
        Menu_TextF(endmenu, "Total :                         %d", profit);
    }

    if (WiiController_ButtonPress(mgdl_GetController(0), ButtonA))
    {
        return true;
    }
    return false;
}

