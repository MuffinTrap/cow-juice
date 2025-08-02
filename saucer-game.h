#pragma once

#include "mgdl/mgdl-gui.h"
#include "mgdl/mgdl-texture.h"
#include "mgdl/mgdl-transform.h"
#include "mgdl/mgdl-vector.h"
#include <array>
#include <mgdl.h>
#include <sstream>

enum GameState
{
    StartScreen,
    CowHunt,
    EndScreen
};

class SaucerGame {
public:
    SaucerGame() = default;
    void init();
    void update();
    void draw();
    void update_gameloop();
    void draw_gameloop();
    void quit();

    void Sprite_Draw2DClipped(Sprite* sprite, u16 spriteIndex, short x, short y, float scale, float progress, AlignmentModes alignX, AlignmentModes alignY, Color4f* tintColor);
    void updateCowBeaming(float time, float timeDelta, bool button_beam_pressed);
    
    float iceCreamMeterProgress = 0.0f;
    static const int COW_STRESS_NONE = -1;
    static const int COW_STRESS_FRAME_COUNT = 7;
    int cowStressUiState = COW_STRESS_NONE; // COW_STRESS_NONE Nothing in UI, 0 - COW_STRESS_FRAME_COUNT shows UI

    GameState currentState = StartScreen;

    Scene *mainScene;
    Transform mainCameraTransform;
    
    Scene *ufoScene;
    Texture *ufoTexture;
    Scene *cowScene;
    Scene *terrainScene;
    Sprite* iceCreamMeter;
    Sprite* iceCreamMeterBackground;
    Sprite* grassSprite;
    Sprite* cowBubbleSprite;
    Sprite* cowFaceSprite;

    static const int TREE_MODEL_AMOUNT = 16;
    static const int BUSH_MODEL_AMOUNT = 16;
    Scene *treeScenes[TREE_MODEL_AMOUNT];
    Scene *bushScenes[BUSH_MODEL_AMOUNT];

    Menu *debugMenu;

    struct CowState {
        Node *node;
    };
    std::array<CowState, 16> cows;

    struct BeamingState {
        float power = 0;
    } beamingState;

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
