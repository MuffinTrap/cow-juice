#include "saucer-game.h"

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
#include <cstdio>

const float pi = 3.141;

void SaucerGame::init() {
    debugMenu = Menu_CreateDefault();

    mainScene = Scene_CreateEmpty();
    
    ufoTexture = mgdl_LoadTexture("assets/Ufo.png", TextureFilterModes::Linear);

    terrainScene = mgdl_LoadFBX("assets/Plane.fbx");
    Scene_SetMaterialTexture(terrainScene, "Material", ufoTexture);
    Scene_AddChildNode(mainScene, mainScene->rootNode, terrainScene->rootNode);
    
    cowScene = mgdl_LoadFBX("assets/Cow.fbx");
    Scene_SetMaterialTexture(cowScene, "Material", ufoTexture);
    Scene_AddChildNode(mainScene, mainScene->rootNode, cowScene->rootNode);
    
    ufoScene = mgdl_LoadFBX("assets/Ufo.fbx");
    Scene_SetMaterialTexture(ufoScene, "Material", ufoTexture);
    Scene_AddChildNode(mainScene, mainScene->rootNode, ufoScene->rootNode);

    for(int i = 0; i < TREE_MODEL_AMOUNT; ++i)
    {
        treeScenes[i] = mgdl_LoadFBX("assets/Tree.fbx");
        Scene_SetMaterialTexture(treeScenes[i], "Material", ufoTexture);
        Scene_AddChildNode(mainScene, mainScene->rootNode, treeScenes[i]->rootNode);
        treeScenes[i]->rootNode->transform->position.x = (float)rand()/(float)(RAND_MAX/10.0f) - 5.0f;
        treeScenes[i]->rootNode->transform->position.y = (float)rand()/(float)(RAND_MAX/10.0f) - 5.0f;
        treeScenes[i]->rootNode->transform->position.z = 0.0f;
        treeScenes[i]->rootNode->transform->rotationDegrees.z = Rad2Deg((float)(rand() % 180));
        treeScenes[i]->rootNode->transform->rotationDegrees.y = 0.0f;
        treeScenes[i]->rootNode->transform->rotationDegrees.x = 0.0f;
        float scale = 0.75f + (float)rand()/(float)(RAND_MAX/0.3f);
        treeScenes[i]->rootNode->transform->scale.z = scale;
        treeScenes[i]->rootNode->transform->scale.y = scale;
        treeScenes[i]->rootNode->transform->scale.x = scale;
    }

    for(int i = 0; i < BUSH_MODEL_AMOUNT; ++i)
    {
        bushScenes[i] = mgdl_LoadFBX("assets/Bush.fbx");
        Scene_SetMaterialTexture(bushScenes[i], "Material", ufoTexture);
        Scene_AddChildNode(mainScene, mainScene->rootNode, bushScenes[i]->rootNode);
        bushScenes[i]->rootNode->transform->position.x = (float)rand()/(float)(RAND_MAX/10.0f) - 5.0f;
        bushScenes[i]->rootNode->transform->position.y = (float)rand()/(float)(RAND_MAX/10.0f) - 5.0f;
        bushScenes[i]->rootNode->transform->position.z = 0.0f;
        bushScenes[i]->rootNode->transform->rotationDegrees.z = Rad2Deg((float)(rand() % 180));
        bushScenes[i]->rootNode->transform->rotationDegrees.y = 0.0f;
        bushScenes[i]->rootNode->transform->rotationDegrees.x = 0.0f;
        float scale = 0.75f + (float)rand()/(float)(RAND_MAX/0.3f);
        bushScenes[i]->rootNode->transform->scale.z = scale;
        bushScenes[i]->rootNode->transform->scale.y = scale;
        bushScenes[i]->rootNode->transform->scale.x = scale;
    }

    iceCreamMeter = mgdl_LoadSprite("assets/IceCreamMeter.png", 280, 128);
    iceCreamMeterBackground = mgdl_LoadSprite("assets/IceCreamMeterBackground.png", 280, 128);
    grassSprite = mgdl_LoadSprite("assets/Grass.png", 16, 16);

    for(int i = 0; i < GRASS_SPRITE_AMOUNT; ++i)
    {
        grassSprites[i].sprite = grassSprite;
        grassSprites[i].position.x = (float)rand()/(float)(RAND_MAX/10.0f) - 5.0f;
        grassSprites[i].position.y = (float)rand()/(float)(RAND_MAX/10.0f) - 5.0f;
        grassSprites[i].position.z = 0.0f;
        grassSprites[i].euler.x = 90.0f;
        grassSprites[i].euler.y = 0.0f;
        grassSprites[i].euler.z = (float)(rand() % 180);
        grassSprites[i].scale = 0.33f;
        grassSprites[i].frame = rand() % GRASS_SPRITE_FRAME_COUNT;
    }
}

void SaucerGame::update() {
    debugstream.str(std::string());

    float time = mgdl_GetElapsedSeconds();
    float timeDelta = mgdl_GetDeltaTime();

    debugstream << "UPDATE\ntime: " << time << "\n  delta: " << timeDelta;
   
    //////// INPUT ///////////

    WiiController *controller = mgdl_GetController(0);
    float roll = WiiController_GetRoll(controller);
    float pitch = WiiController_GetPitch(controller);
    //float yaw = WiiController_GetYaw(controller);


    ///////////////////////
    const float tilt_normal_max = 0.5f;
    float tilt_forward = std::max(-0.5f, std::min(roll, 0.5f));
    float tilt_side = std::max(-0.5f, std::min(pitch, 0.5f));
    float tilt_forward_norm = tilt_forward / tilt_normal_max;
    float tilt_side_norm = tilt_side / tilt_normal_max;

    V3f move_desire = V3f_Create(-tilt_forward_norm, -tilt_side_norm, 0.);
    const float move_speed = 2.0;
    V3f move_delta = V3f_Create(0,0,0);
    V3f_Scale(move_desire, move_speed * timeDelta, move_delta);


    //////// SCENE ///////////

    // UFO
    V3f_Add(
        ufoScene->rootNode->transform->position,
        move_delta,
        ufoScene->rootNode->transform->position
    );
    ufoScene->rootNode->transform->position.z = cos(time * 12.f)*0.25 + 0.75;
    ufoScene->rootNode->transform->rotationDegrees.y = Rad2Deg(-tilt_forward);
    ufoScene->rootNode->transform->rotationDegrees.x = Rad2Deg(tilt_side);

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

    // UI
    // TODO: Connect meter progress to cows
    iceCreamMeterProgress += timeDelta;
    if(iceCreamMeterProgress > 1.0f)
    {
        iceCreamMeterProgress -= 1.0f;
    }
}

void SaucerGame::draw() {
    Color4f *color_sky = Color_GetDefaultColor(Color_Red);
    mgdl_glClearColor4f(color_sky);

    mgdl_InitPerspectiveProjection(90, 0.1, 128. );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    V3f cam_lookat;
    float cam_angle = Deg2Rad(mainCameraTransform.rotationDegrees.y);
    V3f_Add(mainCameraTransform.position, V3f_Create(cos(cam_angle), 0., sin(cam_angle)), cam_lookat);
    mgdl_InitCamera(mainCameraTransform.position, cam_lookat, V3f_Create(0., 0., 1.));

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

    glDisable(GL_CULL_FACE);
    bool animateGrassThisFrame = mgdl_GetElapsedFrames() % 15 == 1;
    for(int i = 0; i < GRASS_SPRITE_AMOUNT; ++i)
    {
        glPushMatrix();
        glTranslatef(grassSprites[i].position.x, grassSprites[i].position.y, grassSprites[i].position.z);
        glRotatef(grassSprites[i].euler.z, 0.0f, 0.0f, 1.0f);
        glRotatef(grassSprites[i].euler.y, 0.0f, 1.0f, 0.0f);
        glRotatef(grassSprites[i].euler.x, 1.0f, 0.0f, 0.0f);
        Sprite_Draw3D(grassSprites[i].sprite, grassSprites[i].frame, grassSprites[i].scale, Centered, RJustify, Color_GetDefaultColor(Color_White));
        if(animateGrassThisFrame)
        {
            grassSprites[i].frame++;
            if(grassSprites[i].frame >= GRASS_SPRITE_FRAME_COUNT)
            {
                grassSprites[i].frame = 0;
            }
        }
        glPopMatrix();
    }
    glEnable(GL_CULL_FACE);

    mgdl_InitOrthoProjection();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    Menu_SetActive(debugMenu);
    Menu_Start(0, mgdl_GetScreenHeight(), mgdl_GetScreenWidth());
    Menu_Text(debugstream.str().c_str());

    // Draw UI
    mgdl_glSetAlphaTest(true);
    mgdl_glSetTransparency(true);
    Sprite_Draw2D(iceCreamMeterBackground, 0, 200, 350, 100, LJustify, RJustify, Color_GetDefaultColor(Color_White));
    Sprite_Draw2DClipped(iceCreamMeter, 0, 200, 350, 100, iceCreamMeterProgress, LJustify, RJustify, Color_GetDefaultColor(Color_White));
}

void SaucerGame::quit() {

    
}

void SaucerGame::Sprite_Draw2DClipped(Sprite* sprite, u16 spriteIndex, short x, short y, float scale, float progress, AlignmentModes alignX, AlignmentModes alignY, Color4f* tintColor)
{
	V3f drawPos = Sprite_AdjustDrawingPosition(sprite, x, y, scale, alignX, alignY);
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