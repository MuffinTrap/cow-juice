#include "end-screen.h"
#include <mgdl.h>

static Menu* endmenu;
static const int AMOUNT = 8;
static const char* costs[AMOUNT] = {
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
};
static int minus_targets[AMOUNT] = {
    1,2,3,4,5,6,7,8};

static int minus_draws[AMOUNT] = {
    1,2,3,4,5,6,7,8};

static float score = 0;

void End_Init()
{
    Font* df = DefaultFont_GetDefaultFont();
    endmenu = Menu_Create(df, 16, 1.2f);
    Random_CreateGenerator();
}

void End_CalculateScore(float iceCreamAmount, float time)
{
    score = iceCreamAmount * 1000 - time;

    int budget = (int)floor(score) - Random_Int(10, 100);
    for (int i = 0; i < AMOUNT; i++ )
    {
        minus_targets[i] = Random_Int(10, budget/4);
        budget -= minus_targets[i];
    }
}


bool End_Run()
{
    Color4f *color_sky = Color_GetDefaultColor(Color_Blue);
    mgdl_glClearColor4f(color_sky);
    mgdl_InitOrthoProjection();
    Menu_SetActive(endmenu);
    Menu_Start(32, mgdl_GetScreenHeight()-32, mgdl_GetScreenWidth()/2);

    static int visible = 1;

    // Increase counters
    for (int i = 0; i < AMOUNT; i++ )
    {
        if (minus_draws[i] < minus_targets[i])
        {
            minus_draws[i] += 1;
            visible = i+1;
            break;
        }
    }

    Menu_TextF("Profit %d", (int)floor(score));
    Menu_Text("----------------------------");
    Menu_Text("Costs:");

    for (int i = 0; i < visible; i++ )
    {
        Menu_TextF("%s : -%i", costs[i], minus_draws[i]);
    }

    if (WiiController_ButtonPress(mgdl_GetController(0), ButtonA))
    {
        return true;
    }
    return false;
}

