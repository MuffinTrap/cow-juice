#include "start-screen.h"
#include <mgdl.h>

static Menu* mainmenu;
static Texture* logo;

void Start_Init()
{
    Font* df = DefaultFont_GetDefaultFont();
    mainmenu = Menu_Create(df, 4.0f, 1.2f);
    Menu_SetActive(mainmenu);
    Menu_SetColors(Color_GetDefaultColor(Color_Black),Color_GetDefaultColor(Color_White), Color_GetDefaultColor(Color_White));

    logo = mgdl_LoadTexture("assets/MooJuice_Logo_BoxArtStyle.png", TextureFilterModes::Linear);
    // TODO  Menu_SetColors();

}

bool Start_Run()
{

    Color4f *color_sky = Color_GetDefaultColor(Color_Blue);
    mgdl_glClearColor4f(color_sky);

    mgdl_InitOrthoProjection();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    mgdl_glSetAlphaTest(true);

    Texture_Draw2DAligned(logo, mgdl_GetScreenWidth()/2, mgdl_GetScreenHeight()/2 + logo->height/2-40, 0.8f, Centered, Centered );
    mgdl_glSetAlphaTest(false);

    Menu_SetActive(mainmenu);
    Menu_Start(32, mgdl_GetScreenHeight()/2-20, mgdl_GetScreenWidth()/3);
    Menu_Text("Press A to start");

    if (WiiController_ButtonPress(mgdl_GetController(0), ButtonA))
    {
        return true;
    }
    return false;
}


