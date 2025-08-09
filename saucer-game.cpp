#include "saucer-game.h"

#include "mgdl/ccVector/ccVector.h"
#include "mgdl/mgdl-color.h"
#include "mgdl/mgdl-controller.h"
#include "mgdl/mgdl-gui.h"
#include "mgdl/mgdl-main.h"
#include "mgdl/mgdl-node.h"
#include "mgdl/mgdl-opengl_util.h"
#include "mgdl/mgdl-scene.h"
#include "mgdl/mgdl-types.h"
#include "mgdl/mgdl-util.h"
#include "mgdl/mgdl-vector.h"
#include "mgdl/mgdl-vectorfunctions.h"
#include <GL/gl.h>
#include <algorithm>
#include <cmath>
#include <cstddef>

#include "start-screen.h"
#include "end-screen.h"

// Uncomment to show debug text
//#define SHOW_DEBUG_TEXT

#define UNUSED(x) (void)(x)

const float gravity = -1.8;

static void randomizeCowTargetPosition(SaucerGame::CowState &cow) {
    cow.targetPosition.z = 0.0f;
    cow.targetPosition.y = Random_FloatNormalized() * SaucerGame::MAP_SIZE - SaucerGame::MAP_SIZE / 2.0f;
    cow.targetPosition.x = Random_FloatNormalized() * SaucerGame::MAP_SIZE - SaucerGame::MAP_SIZE / 2.0f;
}

void SaucerGame::init() {
    
    Random_SetSeed(WiiController_GetRoll(mgdl_GetController(0)) * WiiController_GetPitch(mgdl_GetController(0)));

    debugMenu = Menu_Create(DefaultFont_GetDefaultFont(), 2.0f, 2.1f);

    mainScene = Scene_CreateEmpty();
    
    ufoTexture = mgdl_LoadTexture("assets/Ufo.png", TextureFilterModes::Linear);
    tilesTexture = mgdl_LoadTexture("assets/Tiles.png", TextureFilterModes::Linear);
    rightHandControls = mgdl_LoadTexture("assets/right_hand.png", TextureFilterModes::Nearest);

    terrainScene = mgdl_LoadFBX("assets/Plane.fbx");
    Scene_SetMaterialTexture(terrainScene, "Material", tilesTexture);
    Scene_AddChildNode(mainScene, mainScene->rootNode, terrainScene->rootNode);
    
    cowScene = mgdl_LoadFBX("assets/Cow.fbx");
    Scene_SetMaterialTexture(cowScene, "Material", ufoTexture);
    for (size_t i = 0; i < cows.size(); ++i) {
        cows[i].behavior = CowState::BehaviorState::grace;
        cows[i].speed = V3f_Create(0, 0, 0);
        cows[i].stress = 0;
        cows[i].node = Node_Clone(Scene_GetRootNode(cowScene), (NodeTransform|NodeChildren));
        cows[i].node->transform->position.x = Random_FloatNormalized() * MAP_SIZE - MAP_SIZE / 2.0f;
        cows[i].node->transform->position.y = Random_FloatNormalized() * MAP_SIZE - MAP_SIZE / 2.0f;
        cows[i].node->transform->rotationDegrees.z = Rad2Deg(Random_Float(0.0f, 180.0f));
        cows[i].parachute = Node_FindChildByIndex(cows[i].node, 7);
        Node_DisableDrawing(cows[i].parachute);
        randomizeCowTargetPosition(cows[i]);
        Scene_AddChildNode(mainScene, mainScene->rootNode, cows[i].node);
    }

    ufoScene = mgdl_LoadFBX("assets/Ufo.fbx");
    Scene_SetMaterialTexture(ufoScene, "Material", ufoTexture);
    Scene_AddChildNode(mainScene, mainScene->rootNode, ufoScene->rootNode);

    treeScene = mgdl_LoadFBX("assets/Tree.fbx");
    Scene_SetMaterialTexture(treeScene, "Material", ufoTexture);
    treeNodes = (Node**)malloc(sizeof(Node*) * TREE_MODEL_AMOUNT);
    for(int i = 0; i < TREE_MODEL_AMOUNT; ++i)
    {
        treeNodes[i] = Node_Clone(Scene_GetRootNode(treeScene), (NodeTransform|NodeChildren));
        treeNodes[i]->transform->position.x = Random_FloatNormalized() * MAP_SIZE - MAP_SIZE / 2.0f;
        treeNodes[i]->transform->position.y = Random_FloatNormalized() * MAP_SIZE - MAP_SIZE / 2.0f;
        treeNodes[i]->transform->position.z = 0.0f;
        treeNodes[i]->transform->rotationDegrees.z = Rad2Deg(Random_Float(0.0f, 180.0f));
        treeNodes[i]->transform->rotationDegrees.y = 0.0f;
        treeNodes[i]->transform->rotationDegrees.x = 0.0f;
        float scale = 0.75f + Random_Float(0.0f, 0.3f);
        treeNodes[i]->transform->scale.z = scale;
        treeNodes[i]->transform->scale.y = scale;
        treeNodes[i]->transform->scale.x = scale;
        Scene_AddChildNode(mainScene, mainScene->rootNode, treeNodes[i]);
    }

    bushScene = mgdl_LoadFBX("assets/Bush.fbx");
    Scene_SetMaterialTexture(bushScene, "Material", ufoTexture);
    bushNodes = (Node**)malloc(sizeof(Node*) * BUSH_MODEL_AMOUNT);
    for(int i = 0; i < BUSH_MODEL_AMOUNT; ++i)
    {
        bushNodes[i] = Node_Clone(Scene_GetRootNode(bushScene), (NodeTransform|NodeChildren));
        bushNodes[i]->transform->position.x = Random_FloatNormalized() * MAP_SIZE - MAP_SIZE / 2.0f;
        bushNodes[i]->transform->position.y = Random_FloatNormalized() * MAP_SIZE - MAP_SIZE / 2.0f;
        bushNodes[i]->transform->position.z = 0.0f;
        bushNodes[i]->transform->rotationDegrees.z = Rad2Deg(Random_Float(0.0f, 180.0f));
        bushNodes[i]->transform->rotationDegrees.y = 0.0f;
        bushNodes[i]->transform->rotationDegrees.x = 0.0f;
        float scale = 0.75f + Random_Float(0.0f, 0.3f);
        bushNodes[i]->transform->scale.z = scale;
        bushNodes[i]->transform->scale.y = scale;
        bushNodes[i]->transform->scale.x = scale;
        Scene_AddChildNode(mainScene, mainScene->rootNode, bushNodes[i]);
    }

    cowFaceSprite = mgdl_LoadSprite("assets/CowFace.png", 60, 64);
    cowBubbleSprite = mgdl_LoadSprite("assets/CowBubble.png", 80, 80);
    iceCreamMeter = mgdl_LoadSprite("assets/IceCreamMeter.png", 280, 128);
    iceCreamMeterBackground = mgdl_LoadSprite("assets/IceCreamMeterBackground.png", 280, 128);
    grassSprite = mgdl_LoadSprite("assets/Grass.png", 16, 16);

    grassSprites = (AnimatedWorldSprite*)malloc(sizeof(AnimatedWorldSprite) * GRASS_SPRITE_AMOUNT);
    for(int i = 0; i < GRASS_SPRITE_AMOUNT; ++i)
    {
        grassSprites[i].sprite = grassSprite;
        grassSprites[i].position.x = Random_FloatNormalized() * MAP_SIZE - MAP_SIZE / 2.0f;
        grassSprites[i].position.y = Random_FloatNormalized() * MAP_SIZE - MAP_SIZE / 2.0f;
        // HAX use these as star positions
        grassSprites[i].position.z = Random_Float(-300.0f, 0.0f);
        grassSprites[i].euler.x = 90.0f;
        grassSprites[i].euler.y = 0.0f;
        grassSprites[i].euler.z = Random_Float(0.0f, 180.0f);
        grassSprites[i].scale = 0.33f;
        grassSprites[i].frame = Random_Int(0, GRASS_SPRITE_FRAME_COUNT);
    }

    sfxCommonMoos[0] = mgdl_LoadSound("assets/sfx/deepmoo01.wav");
    sfxCommonMoos[1] = mgdl_LoadSound("assets/sfx/deepmoo04.wav");
    sfxCommonMoos[2] = mgdl_LoadSound("assets/sfx/deepmoo06.wav");
    sfxCommonMoos[3] = mgdl_LoadSound("assets/sfx/deepmoo09.wav");
    sfxCommonMoos[4] = mgdl_LoadSound("assets/sfx/semimoo08.wav");
    sfxCommonMoos[5] = mgdl_LoadSound("assets/sfx/semimoo09.wav");
    sfxMeltMoos[0] = mgdl_LoadSound("assets/sfx/supermeltmoo05.wav");
    sfxMeltMoos[1] = mgdl_LoadSound("assets/sfx/wahmeltmoo10.wav");
    sfxMeltMoos[2] = mgdl_LoadSound("assets/sfx/wahmeltmoo11.wav");
    sfxBeam = mgdl_LoadSound("assets/sfx/wavloop_beam.wav");
    Sound_SetLooping(sfxBeam, true);

    music = mgdl_LoadOgg("assets/blossom_mountain_140bpm.ogg");
    Music_Play(music, true);

    mooSfxTimer = 0.0f;

    ufo.state = Idle;
    ufo.movementSpeed = 2.0f;
    ufo.handedness = LeftHanded;

    Start_Init();
    End_Init();

    currentState = StartScreen;
}

void SaucerGame::update()
{
    switch(currentState)
    {
        case StartScreen:

            break;

        case SpaceTravel:
            update_space();
            break;

        case SaucerEnter:

            break;

        case CowHunt:
            update_gameloop();
            break;

        case SaucerExit:
            update_saucerExit();
            break;

        case EndScreen:

            break;
    }
}

void SaucerGame::draw() {
    switch(currentState)
    {
        case StartScreen:
        {
            bool startGame = Start_Run();
            if (startGame)
            {
                currentState = SpaceTravel;
            }
        }
        break;
        case SpaceTravel:
            drawSpace();
            break;
        case SaucerEnter:
           // TODO
            break;
        case CowHunt:
            draw_gameloop();
            break;
        case SaucerExit:
            draw_gameloop();
            break;
        case EndScreen:
            End_Run();
            break;
    }
}

void SaucerGame::update_gameloop() {
    float time = mgdl_GetElapsedSeconds();
    float timeDelta = mgdl_GetDeltaTime();

    #ifdef SHOW_DEBUG_TEXT
    debugstream.str(std::string());


    debugstream << "UPDATE\ntime: " << time << "\n  delta: " << timeDelta << std::endl;
    debugstream << "Press + to win" << std::endl;
    #endif
   
    //////// INPUT ///////////

    moveUfo(time, timeDelta);

    updateCowBeaming(timeDelta);


    // Camera
    V3f cam_pos_target;
    V3f_Add(ufoScene->rootNode->transform->position, V3f_Create(-3., 0., 2.), cam_pos_target);
    V3f cam_pos_diff;
    V3f_Sub(cam_pos_target, mainCameraTransform.position, cam_pos_diff);
    V3f cam_pos_delta;
    const float cam_chase_speed = 1.8;
    V3f_Scale(cam_pos_diff, cam_chase_speed * timeDelta, cam_pos_delta);
    V3f_Add(mainCameraTransform.position, cam_pos_delta, mainCameraTransform.position);
    mainCameraTransform.rotationDegrees.y = -15.;

    if (iceCreamMeterProgress >= 1.0f)
    {
        // YOU WIN!
        End_CalculateScore(iceCreamMeterProgress, time);
        if(!isBeamSoundPaused)
        {
            isBeamSoundPaused = true;
            Sound_Stop(sfxBeam);
        }
        saucerExitStartTime = time;
        currentState = SaucerExit;

    }
    
    if (ufo.state != Idle) {
        // Melt ice cream
        iceCreamMeterProgress = std::max(0.f, iceCreamMeterProgress - timeDelta * 0.0025f);
    }

    // AUDIO
    mooSfxTimer += timeDelta;
}

void SaucerGame::moveUfo(float time, float timeDelta)
{
    WiiController *controller = mgdl_GetController(0);
    float roll = WiiController_GetRoll(controller);
    float pitch = WiiController_GetPitch(controller);
    if (ufo.handedness == RightHanded)
    {
        roll *= -1.0f;
        pitch *= -1.0f;
    }

    ///////////////////////
    const float tilt_normal_max = 0.5f;
    float tilt_forward = std::max(-0.5f, std::min(roll, 0.5f));
    float tilt_side = std::max(-0.5f, std::min(pitch, 0.5f));
    float tilt_forward_norm = tilt_forward / tilt_normal_max;
    float tilt_side_norm = tilt_side / tilt_normal_max;

    V3f move_desire = V3f_Create(-tilt_forward_norm, -tilt_side_norm, 0.);
    V3f move_delta = V3f_Create(0,0,0);
    V3f_Scale(move_desire, ufo.movementSpeed * timeDelta, move_delta);

    //////// SCENE ///////////

    // UFO
    V3f_Add(
        ufoScene->rootNode->transform->position,
        move_delta,
        ufoScene->rootNode->transform->position
    );
    ufoScene->rootNode->transform->position.z = cos(time * 12.f) * 0.25 + 1.8;
    ufoScene->rootNode->transform->rotationDegrees.y = Rad2Deg(-tilt_forward);
    ufoScene->rootNode->transform->rotationDegrees.x = Rad2Deg(tilt_side);

    Node_FindChildByName(ufoScene->rootNode, "Disc")->transform->rotationDegrees.z += 150.0f * V3f_Length(move_delta);

    //float yaw = WiiController_GetYaw(controller);
    bool button_beam_pressed = WiiController_ButtonHeld(controller, WiiButtons::ButtonAny);


    if (button_beam_pressed)
    {
        ufo.state = Beaming;
        if(isBeamSoundPaused)
        {
            isBeamSoundPaused = false;
            Sound_Play(sfxBeam);
        }
    }
    else
    {
        ufo.state = Idle;
        if(!isBeamSoundPaused)
        {
            isBeamSoundPaused = true;
            Sound_Stop(sfxBeam);
        }
    }


    // DANGER DEBUG
    if (WiiController_ButtonPress(controller, ButtonPlus))
    {
        End_CalculateScore(1, 1);
        saucerExitStartTime = time;
        currentState = SaucerExit;
    }
}


void SaucerGame::update_saucerExit()
{
    V3f move_desire = V3f_Create(0.0f, 0.0f, 1.0f);
    V3f move_delta = V3f_Create(0,0,1);
    V3f_Scale(move_desire, ufo.movementSpeed * mgdl_GetDeltaTime(), move_delta);
    V3f_Add(
        ufoScene->rootNode->transform->position,
        move_delta,
        ufoScene->rootNode->transform->position
    );
    ufoScene->rootNode->transform->rotationDegrees.y = 0.0f;
    ufoScene->rootNode->transform->rotationDegrees.x = 0.0f;

    Node_FindChildByName(ufoScene->rootNode, "Disc")->transform->rotationDegrees.z += 150.0f * 1.0f;

    float exitElapsed = mgdl_GetElapsedSeconds() - saucerExitStartTime;
    if (exitElapsed > saucerExitDuration)
    {
        currentState = EndScreen;
    }
}


void SaucerGame::drawSpace()
{
    Color4f *color_space = Color_GetDefaultColor(Color_Black);
    mgdl_glClearColor4f(color_space);

    mgdl_InitPerspectiveProjection(90, 0.1, 128. );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    V3f cam_lookat = V3f_Create(0.0f, 0.0f, -1.0f);
    V3f cam_pos = V3f_Create(0.0f, 0.0f, 0.0f);
    mgdl_InitCamera(cam_pos, cam_lookat, V3f_Create(0., 1., 0.));

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glShadeModel(GL_FLAT);

    // TODO draw starfield
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for(int i = 0; i < GRASS_SPRITE_AMOUNT; ++i)
    {
        glVertex3f(grassSprites[i].position.x, grassSprites[i].position.y, grassSprites[i].position.z);
    }
    glEnd();
    glPointSize(1.0f);
    glPopMatrix();
    mgdl_glSetTransparency(true);

    ufoScene->rootNode->transform->position.z = -3.0f;
    ufoScene->rootNode->transform->position.y = -2.0f;
    ufoScene->rootNode->transform->rotationDegrees.y = 0.0f;
    ufoScene->rootNode->transform->rotationDegrees.x = -90.0f;

    glPushMatrix();
    Scene_Draw(ufoScene);
    glPopMatrix();

    mgdl_InitOrthoProjection();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Select if right or left handed
    glDisable(GL_CULL_FACE);
    Menu_Start(debugMenu, 64,  mgdl_GetScreenHeight()-64, mgdl_GetScreenWidth()/3);

    Menu_BeginRow(debugMenu);
    if (Menu_TexturedButton(debugMenu, rightHandControls, FlipVertical))
    {
        ufo.handedness = LeftHanded;
    }
    Menu_Skip(debugMenu, 64);
    if (Menu_TexturedButton(debugMenu, rightHandControls, FlipNone))
    {
        ufo.handedness = RightHanded;
    }

    Menu_EndRow(debugMenu);

    if (ufo.handedness == RightHanded)
    {
        Menu_Text(debugMenu, "Right handed controls");
    }
    else
    {
        Menu_Text(debugMenu, "Left handed controls");
    }
    Menu_Text(debugMenu, "Hold B (trigger) to confirm");
    Menu_DrawCursor(debugMenu);
    glEnable(GL_CULL_FACE);
}

void SaucerGame::update_space()
{
    float delta = mgdl_GetDeltaTime();
    float starSpeed = 32.0f;

    for(int i = 0; i < GRASS_SPRITE_AMOUNT; ++i)
    {
        grassSprites[i].position.z += delta * starSpeed;
        if (grassSprites[i].position.z > 0.0f)
        {
            grassSprites[i].position.z = -300.0f;
        }
    }
    Node_FindChildByName(ufoScene->rootNode, "Disc")->transform->rotationDegrees.z += 150.0f * 0.4f * delta;
    Node_FindChildByName(ufoScene->rootNode, "Beam")->transform->scale.z = 0.0f;

    if (WiiController_ButtonHeld(mgdl_GetController(0), ButtonB))
    {
        for(int i = 0; i < GRASS_SPRITE_AMOUNT; ++i)
        {
            grassSprites[i].position.z = 0.0f;
        }
        // TODO saucer enter
        currentState = CowHunt;
    }
}

void SaucerGame::setCamera()
{
    V3f cam_lookat;
    float cam_angle = Deg2Rad(mainCameraTransform.rotationDegrees.y);
    V3f_Add(mainCameraTransform.position, V3f_Create(cos(cam_angle), 0., sin(cam_angle)), cam_lookat);
    mgdl_InitCamera(mainCameraTransform.position, cam_lookat, V3f_Create(0., 0., 1.));
}


void SaucerGame::draw_gameloop()
{
    Color4f *color_sky = Color_GetDefaultColor(Color_Blue);
    mgdl_glClearColor4f(color_sky);

    mgdl_InitPerspectiveProjection(90, 0.1, 128. );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    setCamera();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glShadeModel(GL_FLAT);
    mgdl_glSetTransparency(true);

    glColor3f(1.0f, 1.0f, 1.0f);

    glPushMatrix();
    Scene_Draw(mainScene);
    glPopMatrix();

    draw_grass();

    drawUI();

}

void SaucerGame::draw_grass()
{
    glDisable(GL_CULL_FACE);
    bool animateGrassThisFrame = mgdl_GetElapsedFrames() % 15 == 1;
    Sprite* sprite = grassSprite;
    float scale = 0.33f;
    float width = sprite->_font->_aspect * scale;
    float height = scale;
    const float uvW = sprite->_font->_uvWidth;
    const float uvH = sprite->_font->_uvHeight;
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    mgdl_glColor4f(Color_GetDefaultColor(Color_White));
    Sprite_BeginDrawBatch(grassSprite);
    for(int i = 0; i < GRASS_SPRITE_AMOUNT; ++i)
    {
        // glTranslatef(grassSprites[i].position.x, grassSprites[i].position.y, grassSprites[i].position.z);

        // Sprite_Draw3DBatched(grassSprites[i].sprite, grassSprites[i].frame, grassSprites[i].position, grassSprites[i].scale, Centered, RJustify, Color_GetDefaultColor(Color_White));
        V3f drawPos = Sprite_AdjustDrawingPosition3D(sprite, grassSprites[i].position, scale, Centered, RJustify);

        V2f tx= _Font_GetTextureCoordinate(sprite->_font, grassSprites[i].frame); //LOW LEFT!

        // LOW LEFT!
        glTexCoord2f(V2f_X(tx), V2f_Y(tx));
        glVertex3f(V3f_X(drawPos), V3f_Y(drawPos), V3f_Z(drawPos));

        // LOW RIGHT
        glTexCoord2f(V2f_X(tx) + uvW, V2f_Y(tx));
        glVertex3f(V3f_X(drawPos) + width, V3f_Y(drawPos), V3f_Z(drawPos));

        // TOP RIGHT
        glTexCoord2f(V2f_X(tx) + uvW, V2f_Y(tx) + uvH);
        glVertex3f(V3f_X(drawPos) + width, V3f_Y(drawPos), V3f_Z(drawPos) + height);

        // TOP LEFT
        glTexCoord2f(V2f_X(tx), V2f_Y(tx) + uvH);
        glVertex3f(V3f_X(drawPos), V3f_Y(drawPos), V3f_Z(drawPos) + height);
        if(animateGrassThisFrame)
        {
            grassSprites[i].frame++;
            if(grassSprites[i].frame >= GRASS_SPRITE_FRAME_COUNT)
            {
                grassSprites[i].frame = 0;
            }
        }
    }
    Sprite_EndDrawBatch();
    glPopMatrix();
    glEnable(GL_CULL_FACE);

}

void SaucerGame::drawUI()
{
    mgdl_InitOrthoProjection();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    Menu_Start(debugMenu, 24, mgdl_GetScreenHeight()-8, mgdl_GetScreenWidth());

    #ifdef SHOW_DEBUG_TEXT
    Menu_Text(debugstream.str().c_str());
    #endif

    // Draw UI
    mgdl_glSetAlphaTest(true);
    mgdl_glSetTransparency(true);
    Sprite_Draw2D(iceCreamMeterBackground, 0, 200, 350, 100, LJustify, RJustify, Color_GetDefaultColor(Color_White));
    Sprite_Draw2DClipped(iceCreamMeter, 0, 200, 350, 100, iceCreamMeterProgress, LJustify, RJustify, Color_GetDefaultColor(Color_White));

    if(cowStressUiState >= 0)
    {
        if(cowStressUiState >= COW_STRESS_FRAME_COUNT)
        {
            cowStressUiState = COW_STRESS_FRAME_COUNT - 1;
        }
        Sprite_Draw2D(cowBubbleSprite, 0, 50, 230, 130, LJustify, RJustify, Color_GetDefaultColor(Color_White));
        Sprite_Draw2D(cowFaceSprite, cowStressUiState, 67, 250, 100, LJustify, RJustify, Color_GetDefaultColor(Color_White));
    }

    //Scene_DebugDraw(mainScene, debugMenu, 32, mgdl_GetScreenHeight()-8, 0);
}

void SaucerGame::quit() {

    for(int i = 0; i < BUSH_MODEL_AMOUNT; ++i)
    {
        free(bushNodes[i]);
    }
    for(int i = 0; i < TREE_MODEL_AMOUNT; ++i)
    {
        free(treeNodes[i]);
    }
    for (size_t i = 0; i < cows.size(); ++i) {
        free(cows[i].node);
    }

    free(treeScene);
    free(bushScene);
    free(cowScene);
}

void SaucerGame::Sprite_Draw2DClipped(Sprite* sprite, u16 spriteIndex, short x, short y, float scale, float progress, AlignmentModes alignX, AlignmentModes alignY, Color4f* tintColor)
{
	V3f drawPos = Sprite_AdjustDrawingPosition2D(sprite, x, y, scale, alignX, alignY);
	float width = sprite->_font->_aspect * scale * progress;
	float height = scale;
	const float uvW = sprite->_font->_uvWidth * progress;
	const float uvH = sprite->_font->_uvHeight;

	mgdl_glColor4f(tintColor);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, sprite->_font->_fontTexture->textureId);

	glBegin(GL_QUADS);

	vec2 tx = _Font_GetTextureCoordinate(sprite->_font, spriteIndex); //LOW LEFT!

	// LOW LEFT!
	glTexCoord2f(tx.x, tx.y);
	glVertex2f(drawPos.x, drawPos.y - height);

	// LOW RIGHT
	glTexCoord2f(tx.x + uvW, tx.y);
	glVertex2f(drawPos.x + width, drawPos.y - height);

	// TOP RIGHT
	glTexCoord2f(tx.x + uvW, tx.y + uvH);
	glVertex2f(drawPos.x + width, drawPos.y);

	// TOP LEFT
	glTexCoord2f(tx.x, tx.y + uvH);
	glVertex2f(drawPos.x, drawPos.y );

	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

}

void SaucerGame::updateCowBeaming(float timeDelta) {

    if (ufo.state == Beaming) {
#ifdef SHOW_DEBUG_TEXT
        debugstream << "BEAMING!" << std::endl;
#endif
        Node_FindChildByName(ufoScene->rootNode, "Beam")->transform->scale.z = 1.0f;
    } else {
#ifdef SHOW_DEBUG_TEXT
        debugstream << "Not beaming." << std::endl;
#endif
        Node_FindChildByName(ufoScene->rootNode, "Beam")->transform->scale.z = 0.0f;
    }

    float stress_max = 0;

    for (size_t i = 0; i < cows.size(); ++i) {
        CowState &cow = cows[i];
#ifdef SHOW_DEBUG_TEXT
        debugstream << "Cow #" << i << ":" << std::endl;
#endif

        V3f cow_saucer_diff;
        V3f_Sub(ufoScene->rootNode->transform->position, cow.node->transform->position, cow_saucer_diff);
        float distance = V3f_Length(cow_saucer_diff);
        V3f cow_to_saucer_dir;
        V3f_Normalize(cow_saucer_diff, cow_to_saucer_dir);
        
        V3f cow_saucer_diff_plane = cow_saucer_diff;
        cow_saucer_diff_plane.z = 0;
        float distance_plane = V3f_Length(cow_saucer_diff_plane);

        // Stress
        float stressfulness = std::exp(-distance * 0.4);
        cows[i].stress = std::max(0.f, std::min(cows[i].stress - timeDelta * 0.2f + stressfulness * 0.4f * timeDelta, 1.f));
        stress_max = std::max(stress_max, cows[i].stress);

        if (ufo.state == Beaming) {
            if (distance < 0.4f) {
                #ifdef SHOW_DEBUG_TEXT
                debugstream << "MILKING!" << std::endl;
                #endif
                ufo.state = Milking;
                addMilkTick(timeDelta, cow.stress);
                PlayMooSfx(cow.stress > 0.5f);
            }
            else
            {
                ufo.state = Beaming;
            }

            if (distance_plane < 0.5f) {
                #ifdef SHOW_DEBUG_TEXT
                debugstream << "LIFTING!" << std::endl;
                #endif
                ufo.state = Lifting;
                cow.behavior = CowState::BehaviorState::lifted;
                
                cow.speed.x = cow_saucer_diff.x * 0.1f;
                cow.speed.y = cow_saucer_diff.y * 0.1f;
                cow.speed.z = 1.9f * cow_to_saucer_dir.z;

                cow.node->transform->rotationDegrees.z += 80.0f * timeDelta;

                PlayMooSfx(false);
            }
            else
            {
                ufo.state = Beaming;
            }
        }
        
        if (cow.behavior == CowState::BehaviorState::lifted) {
            // fall down
            cow.speed.z += gravity * timeDelta;

            float ground_height = 0.f; // TODO get from terrain
            V3f cow_pos_delta;
            V3f_Scale(cow.speed, timeDelta, cow_pos_delta);
            V3f_Add(cow.node->transform->position, cow_pos_delta, cow.node->transform->position);

            if (cow.node->transform->position.z < ground_height) {
                // touch ground
                cow.behavior = CowState::BehaviorState::grace;
                cow.node->transform->position.z = ground_height;
                cow.speed.z = 0.0f;
            }
        }

        if (cow.behavior == CowState::BehaviorState::grace)
        {
            float speed = 1.f;
            V3f dir = vec3Subtract(cow.targetPosition, cow.node->transform->position);
            float distanceToTarget = vec3Length(dir);
            dir.z = 0;
            dir = vec3Normalize(dir);
            cow.speed = vec3Multiply(dir, speed);
            V3f moveDelta = vec3Multiply(cow.speed, timeDelta);
            cow.node->transform->position = vec3Add(cow.node->transform->position, moveDelta);

            float heading_angle = std::atan2(dir.y, dir.x) + M_PI;
            cow.node->transform->rotationDegrees.z = Rad2Deg(heading_angle);

            if (distanceToTarget < 0.5) {
                randomizeCowTargetPosition(cow);
            }

        //
            // debugstream << "posX: " << cow.node->transform->position.x << "\n";
            // debugstream << "posY: " << cow.node->transform->position.y << "\n";
            // debugstream << "posZ: " << cow.node->transform->position.z << "\n";
            // debugstream << "targetX: " << cow.targetPosition.x << "\n";
            // debugstream << "targetY: " << cow.targetPosition.y << "\n";
            // debugstream << "targetZ: " << cow.targetPosition.z << "\n";
            // debugstream << "speedX: " << cow.speed.x << "\n";
            // debugstream << "speedY: " << cow.speed.y << "\n";
            // debugstream << "speedZ: " << cow.speed.z << "\n";
        }

        // When in air and falling, draw the parachute
        if (cow.behavior == CowState::BehaviorState::lifted
            && cow.speed.z < 0.0f
            && ufo.state != Milking
            && ufo.state != Lifting)
        {
            Node_EnableDrawing(cow.parachute);
        }
        else
        {
            Node_DisableDrawing(cow.parachute);
        }

#ifdef SHOW_DEBUG_TEXT
        debugstream << "stress: " << cows[i].stress;
        debugstream << std::endl;
#endif
    }

    cowStressUiState = (int)(stress_max * (float)COW_STRESS_FRAME_COUNT - 1);
}

void SaucerGame::addMilkTick(float timeDelta, float stress) {
    iceCreamMeterProgress += timeDelta * 0.2f * (1. - stress * 1.25);
    iceCreamMeterProgress = std::max(0.f, std::min(iceCreamMeterProgress, 1.0f));
}

void SaucerGame::PlayMooSfx(bool melted)
{
    if(mooSfxTimer > 1.0f)
    {
        if(melted)
        {
            Sound_Play(sfxMeltMoos[Random_Int(0,3)]);
        }
        else
        {
            Sound_Play(sfxCommonMoos[Random_Int(0,6)]);
        }
        mooSfxTimer = 0.0f;
    }
}
