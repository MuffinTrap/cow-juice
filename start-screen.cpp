#include "start-screen.h"
#include <mgdl.h>

static Menu* mainmenu;

void Start_Init()
{
    Font* df = DefaultFont_GetDefaultFont();
    mainmenu = Menu_Create(df, 32, 1.2f);
    // TODO  Menu_SetColors();

}

bool Start_Run()
{

    Color4f *color_sky = Color_GetDefaultColor(Color_Blue);
    mgdl_glClearColor4f(color_sky);

    mgdl_InitOrthoProjection();
    Menu_SetActive(mainmenu);
    Menu_Start(mgdl_GetScreenWidth(), mgdl_GetScreenHeight()/2, mgdl_GetScreenWidth()/3);
    Menu_Text("Cow Juice");
    Menu_Text("Press A to start");

    if (WiiController_ButtonPress(mgdl_GetController(0), ButtonA))
    {
        return true;
    }
    return false;
}


