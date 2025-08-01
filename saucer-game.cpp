#include "saucer-game.h"

#include "mgdl/mgdl-color.h"
#include "mgdl/mgdl-controller.h"
#include "mgdl/mgdl-node.h"
#include "mgdl/mgdl-opengl_util.h"
#include "mgdl/mgdl-scene.h"
#include "mgdl/mgdl-util.h"
#include "mgdl/mgdl-vectorfunctions.h"
#include <GL/gl.h>
#include <cmath>

void SaucerGame::init() {
    mainScene = Scene_CreateEmpty();

    ufoScene = mgdl_LoadFBX("assets/Ufo.fbx");
    ufoTexture = mgdl_LoadTexture("assets/Ufo.png", TextureFilterModes::Nearest);
    Scene_SetMaterialTexture(ufoScene, "Material", ufoTexture);
    Scene_AddChildNode(mainScene, mainScene->rootNode, ufoScene->rootNode);
}

void SaucerGame::update() {
    float time = mgdl_GetElapsedSeconds();


    //////// INPUT ///////////

    WiiController *controller = mgdl_GetController(0);
    float roll = WiiController_GetRoll(controller);
    float pitch = WiiController_GetPitch(controller);
    //float yaw = WiiController_GetYaw(controller);

    //////// SCENE ///////////

    mainCameraTransform.position.x = -3.;

    ufoScene->rootNode->transform->position.x = sin(time);
    ufoScene->rootNode->transform->position.y = cos(time * 1.2);
    ufoScene->rootNode->transform->position.z = cos(time * 0.7);

    ufoScene->rootNode->transform->rotationDegrees.y = Rad2Deg(-roll);
    ufoScene->rootNode->transform->rotationDegrees.x = Rad2Deg(pitch);
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
}

void SaucerGame::quit() {

    
}