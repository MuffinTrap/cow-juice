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
    SpaceTravel,    // Try out controls
    SaucerEnter,    // Show map and put saucer down
    CowHunt,        // Main game
    SaucerExit,     // Fly saucer up
    EndScreen       // Stonks!
};

enum UfoState
{
    Idle,
    Beaming,
    Lifting,
    Milking
};

enum Handedness
{
    LeftHanded, // A button on the right side
    RightHanded // A button on the left side
};

struct Ufo
{
    UfoState state;
    Handedness handedness;
    float movementSpeed;
};

class SaucerGame {
public:
    SaucerGame() = default;
    void init();
    void update();
    void draw();
    void drawSpace();
    void setCamera();
    void update_space();
    void update_gameloop();
    void update_saucerExit();
    void moveUfo(float time, float timeDelta);
    void draw_gameloop();
    void draw_grass();
    void drawUI();
    void quit();

    void Sprite_Draw2DClipped(Sprite* sprite, u16 spriteIndex, short x, short y, float scale, float progress, AlignmentModes alignX, AlignmentModes alignY, Color4f* tintColor);
    void updateCowBeaming(float timeDelta);
    void addMilkTick(float timeDelta, float stress);
    void PlayMooSfx(bool melted);

    float iceCreamMeterProgress = 0.0f;
    static const int COW_STRESS_NONE = -1;
    static const int COW_STRESS_FRAME_COUNT = 7;
    int cowStressUiState = COW_STRESS_NONE; // COW_STRESS_NONE Nothing in UI, 0 - COW_STRESS_FRAME_COUNT shows UI

    GameState currentState = CowHunt;
    float saucerExitStartTime = 0.0f;
    float saucerExitDuration = 3.0f;

    Ufo ufo;
    Scene *mainScene;
    Transform mainCameraTransform;
    
    Scene *ufoScene;
    Texture *ufoTexture;
    Texture *tilesTexture;
    Scene *cowScene;
    Scene *terrainScene;
    Scene *treeScene;
    Scene *bushScene;
    Sprite* iceCreamMeter;
    Sprite* iceCreamMeterBackground;
    Sprite* grassSprite;
    Sprite* cowBubbleSprite;
    Sprite* cowFaceSprite;
    Texture* rightHandControls;

    static const int MAP_SIZE = 40;

    static const int TREE_MODEL_AMOUNT = 128;
    static const int BUSH_MODEL_AMOUNT = 64;
    Node **treeNodes;
    Node **bushNodes;

    Menu *debugMenu;

    struct CowState {
        Node *node;
        Node *parachute;
        enum class BehaviorState { grace, panic, lifted } behavior;
        V3f speed;
        float stress;
        V3f targetPosition;
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
    static const int GRASS_SPRITE_AMOUNT = 512;
    static const int GRASS_SPRITE_FRAME_COUNT = 4;
    AnimatedWorldSprite* grassSprites;

    Sound* sfxCommonMoos[6];
    Sound* sfxMeltMoos[3];
    Sound* sfxBeam;
    Music* music;
    bool isBeamSoundPaused = false;
    float mooSfxTimer;

    std::stringstream debugstream;
};
