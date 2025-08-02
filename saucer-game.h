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
    Scene *cowScene;
    Scene *terrainScene;
    Sprite* iceCreamMeter;
    Sprite* grassSprite;

    static const int TREE_MODEL_AMOUNT = 16;
    Scene *treeScenes[TREE_MODEL_AMOUNT];

    Menu *debugMenu;

    struct AnimatedWorldSprite
    {
        Sprite* sprite;
        V3f position;
        V3f euler;
        float scale;
        int frame;
    };
    static const int GRASS_SPRITE_AMOUNT = 128;
    static const int GRASS_SPRITE_FRAME_COUNT = 4;
    AnimatedWorldSprite grassSprites[GRASS_SPRITE_AMOUNT];

    std::stringstream debugstream;
};
