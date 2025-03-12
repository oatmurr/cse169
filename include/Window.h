#pragma once

#include "Camera.h"
#include "Shader.h"
// #include "Cube.h"

#ifdef INCLUDE_SKELETON
#include "Skeleton.h"
#endif

#ifdef INCLUDE_SKIN
#include "Skin.h"
#endif

#ifdef INCLUDE_ANIMATION
#include "AnimationPlayer.h"
#endif

#ifdef INCLUDE_CLOTH
#include "Cloth.h"
#endif

// imgui stuff
#include "imgui/imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

class Window
{
public:
    // Window Properties
    static int width;
    static int height;
    static const char* windowTitle;

    // Objects to render
    // static Cube* cube;

    #ifdef INCLUDE_SKELETON
    static Skeleton* skeleton;
    #endif

    #ifdef INCLUDE_SKIN
    static Skin* skin;
    #endif

    #ifdef INCLUDE_ANIMATION
    static Rig* rig;
    static AnimationClip* clip;
    static AnimationPlayer* player;
    #endif

    #ifdef INCLUDE_CLOTH
    static Cloth* cloth;
    static glm::vec3 wind;
    static bool pauseSimulation;
    static void RenderClothControls();
    #endif

    // Shader Program
    static GLuint shaderProgram;

    // Act as Constructors and destructors
    // static bool initializeProgram();
    static bool initializeProgram(GLFWwindow* window);
    static bool initializeObjects();
    static void cleanUp();

    // for the Window
    static GLFWwindow* createWindow(int width, int height);
    static void resizeCallback(GLFWwindow* window, int width, int height);

    // update and draw functions
    static void idleCallback();
    static void displayCallback(GLFWwindow*);

    // helper to reset the camera
    static void resetCamera();

    // callbacks - for interaction
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouse_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_callback(GLFWwindow* window, double currX, double currY);

    // imgui stuff
    #ifdef INCLUDE_SKELETON
    static void RenderJointControls();
    static bool drawSkeleton;
    #endif

    // project 2 lighting stuff
    #ifdef INCLUDE_SKIN
    static glm::vec3 lightDirection1;
    static glm::vec3 lightColor1;
    static glm::vec3 lightDirection2;
    static glm::vec3 lightColor2;
    #endif
};