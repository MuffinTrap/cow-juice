#include "start-screen.h"
#include <mgdl.h>

static Menu* mainmenu;
static Texture* logo;

void Start_Init()
{
    Font* df = DefaultFont_GetDefaultFont();
    mainmenu = Menu_Create(df, 4.0f, 1.2f);
    Menu_SetColors(mainmenu, Color_GetDefaultColor(Color_Black),Color_GetDefaultColor(Color_White), Color_GetDefaultColor(Color_White));

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

    Texture_Draw2DAligned(logo, mgdl_GetScreenWidth()/2, mgdl_GetScreenHeight(), 0.8f, Centered, LJustify );
    mgdl_glSetAlphaTest(false);

    Menu_Start(mainmenu, 32, mgdl_GetScreenHeight()/2-20, mgdl_GetScreenWidth()/3);
    Menu_Text(mainmenu, "Press A to start");

    float r = mgdl_GetElapsedSeconds();
    float ss = r + cos(r/2.0f) * 10.0f;
    float h = mgdl_GetScreenHeight()/4.0f;
    glBegin(GL_LINES);
    mgdl_glColor4f(Color_GetDefaultColor(Color_White));

    for(int i = 0; i < mgdl_GetScreenWidth(); i++)
    {
        glVertex2f(i, 0.0f);
        glVertex2f(i, h + sin(ss + i * 0.025f) * (h/3.0f)*sin(r+i*0.01f)*1.0f);
    }

    glEnd();

    if (WiiController_ButtonPress(mgdl_GetController(0), ButtonA))
    {
        return true;
    }
    return false;
}


