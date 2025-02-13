#pragma once

#include "Camera.h"
#include "Shader.h"
// #include "Skeleton.h"
// #include "Skin.h"
#include "AnimationPlayer.h"

// imgui stuff
#include "imgui/imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

class Window {
public:
    // Window Properties
    static int width;
    static int height;
    static const char* windowTitle;

    // Objects to render
    // static Cube* cube;
    static Skeleton* skeleton;
    static Skin* skin;
    static Rig* rig;
    static AnimationClip* clip;
    static AnimationPlayer* player;

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
    static void RenderJointControls();
    static bool drawSkeleton;

    // project 2 lighting stuff
    static glm::vec3 lightDirection1;
    static glm::vec3 lightColor1;
    static glm::vec3 lightDirection2;
    static glm::vec3 lightColor2;
};