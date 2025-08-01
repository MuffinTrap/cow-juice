#include "saucer-game.h"

#include "mgdl/mgdl-color.h"
#include "mgdl/mgdl-controller.h"
#include "mgdl/mgdl-gui.h"
#include "mgdl/mgdl-main.h"
#include "mgdl/mgdl-node.h"
#include "mgdl/mgdl-opengl_util.h"
#include "mgdl/mgdl-scene.h"
#include "mgdl/mgdl-util.h"
#include "mgdl/mgdl-vectorfunctions.h"
#include <GL/gl.h>
#include <algorithm>
#include <cmath>
#include <cstdio>

const float pi = 3.141;

void SaucerGame::init() {
    debugMenu = Menu_CreateDefault();

    mainScene = Scene_CreateEmpty();

    ufoScene = mgdl_LoadFBX("assets/Ufo.fbx");
    ufoTexture = mgdl_LoadTexture("assets/Ufo.png", TextureFilterModes::Nearest);
    Scene_SetMaterialTexture(ufoScene, "Material", ufoTexture);
    Scene_AddChildNode(mainScene, mainScene->rootNode, ufoScene->rootNode);
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

    // Camera
    mainCameraTransform.position.x = -3.;
    mainCameraTransform.position.z = 2.;
    mainCameraTransform.rotationDegrees.y = -15.;

    // UFO
    V3f_Add(
        ufoScene->rootNode->transform->position,
        move_delta,
        ufoScene->rootNode->transform->position
    );
    ufoScene->rootNode->transform->position.z = cos(time * 12.f)*0.25 + 0.75;
    ufoScene->rootNode->transform->rotationDegrees.y = Rad2Deg(-tilt_forward);
    ufoScene->rootNode->transform->rotationDegrees.x = Rad2Deg(tilt_side);
}

void SaucerGame::draw() {
    Color4f *color_sky = Color_GetDefaultColor(Color_Red);
    mgdl_glClearColor4f(color_sky);

    mgdl_InitPerspectiveProjection(90, 1., 128. );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    mgdl_InitCamera(
        mainCameraTransform.position,
        V3f_Create(0., 0., 0.),
        V3f_Create(0., 0., 1.)
    );

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glShadeModel(GL_FLAT);

    glColor3f(1.0f, 1.0f, 1.0f);

    glPushMatrix();
    Scene_Draw(mainScene);
    glPopMatrix();


    mgdl_InitOrthoProjection();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    Menu_SetActive(debugMenu);
    Menu_Start(0, mgdl_GetScreenHeight(), mgdl_GetScreenWidth());
    Menu_Text("fdfsdffdfdfd");
    Menu_Text(debugstream.str().c_str());
}

void SaucerGame::quit() {

    
}