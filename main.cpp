#include <mgdl.h>
//#include "example.h"
#include "saucer-game.h"

static SaucerGame game;

void init()
{
    glViewport(0, 0, mgdl_GetScreenWidth(), mgdl_GetScreenHeight());

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    game.init();
    //example.Init();
}

void frame()
{
    //example.Update();

    // NOTE Use the mgdl_glClear to assure depth buffer working correctly on Wii
    mgdl_glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    //example.Draw();

    game.update();
    game.draw();
}

void quit()
{
    // Called before program exits
    // Use this to free any resources and disconnect rocket
    //example.Quit();

    game.quit();
}


int main()
{
    mgdl_InitSystem("Moo Juice",
        ScreenAspect::Screen4x3,
            init,
            frame,
            quit,
        FlagNone
        // | FlagFullScreen
        // | FlagSplashScreen
        // | FlagPauseUntilA
    );
}
