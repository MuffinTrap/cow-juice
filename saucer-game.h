#pragma once

#include "mgdl/mgdl-gui.h"
#include "mgdl/mgdl-texture.h"
#include "mgdl/mgdl-transform.h"
#include <mgdl.h>
#include <sstream>

class SaucerGame {
public:
    SaucerGame() = default;
    void init();
    void update();
    void draw();
    void quit();
    
    Scene *mainScene;
    Transform mainCameraTransform;
    
    Scene *ufoScene;
    Texture *ufoTexture;

    Menu *debugMenu;

    std::stringstream debugstream;
};
