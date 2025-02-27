#include "Window.h"

// Window Properties
int Window::width;
int Window::height;
const char* Window::windowTitle = "Model Environment";

// Objects to render
// Cube* Window::cube;

#ifdef INCLUDE_SKELETON
Skeleton* Window::skeleton;
#endif

#ifdef INCLUDE_SKIN
Skin* Window::skin;
#endif

#ifdef INCLUDE_ANIMATION
Rig* Window::rig;
AnimationClip* Window::clip;
AnimationPlayer* Window::player;
#endif

#ifdef INCLUDE_SKELETON
bool Window::drawSkeleton = true;
#endif

// project 2 lighting stuff
#ifdef INCLUDE_SKIN
glm::vec3 Window::lightDirection1;
glm::vec3 Window::lightColor1;
glm::vec3 Window::lightDirection2;
glm::vec3 Window::lightColor2;
#endif

// Camera Properties
Camera* Cam;

// Interaction Variables
bool LeftDown, RightDown;
int MouseX, MouseY;

// The shader program id
GLuint Window::shaderProgram;

// imgui stuff
#ifdef INCLUDE_SKELETON
static Joint* selectedJoint = nullptr;
static int currentJointIndex = 0;
#endif

// #ifdef INCLUDE_CLOTH
Cloth* Window::cloth;
glm::vec3 Window::wind = glm::vec3(0.0f, 0.0f, 0.0f);
bool Window::pauseSimulation = false;
// #endif

// Constructors and desctructors
// bool Window::initializeProgram() {
bool Window::initializeProgram(GLFWwindow* window) {
    // Create a shader program with a vertex shader and a fragment shader.
    shaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");

    // project 2 lighting stuff
    #ifdef INCLUDE_SKIN
    lightDirection1 = glm::normalize(glm::vec3(1, 5, 2));
    lightColor1 = glm::vec3(1.0f, 0.0f, 0.0f); // red light
    lightDirection2 = glm::normalize(glm::vec3(-2, -3, 1));
    lightColor2 = glm::vec3(0.0f, 0.0f, 1.0f); // blue light
    #endif

    // set up imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Check the shader program.
    if (!shaderProgram) {
        std::cerr << "Failed to initialize shader program" << std::endl;
        return false;
    }

    // #ifdef INCLUDE_CLOTH
    wind = glm::vec3(0.0f, 0.0f, 0.0f);
    pauseSimulation = false;
    // #endif

    return true;
}

bool Window::initializeObjects() {
    // Create a cube
    // cube = new Cube();
    // cube = new Cube(glm::vec3(-1, 0, -2), glm::vec3(1, 1, 1));

    #ifdef INCLUDE_SKELETON
    skeleton = new Skeleton();
    #endif

    #ifdef INCLUDE_SKIN
    skin = new Skin();
    #endif

    #ifdef INCLUDE_ANIMATION
    if (skeleton == nullptr) {
        std::cerr << "failed to initialize skeleton" << std::endl;
        return false;
    }

    if (skin == nullptr) {
        std::cerr << "failed to initialize skin" << std::endl;
        return false;
    }

    rig = new Rig(skeleton, skin);
    clip = new AnimationClip();
    player = new AnimationPlayer();
    player->SetClip(clip);
    player->SetRig(rig);
    #endif

    // #ifdef INCLUDE_CLOTH
    cloth = nullptr;
    // #endif

    return true;
}

void Window::cleanUp() {
    // Deallcoate the objects.
    // delete cube;

    #ifdef INCLUDE_SKELETON
    delete skeleton;
    #endif

    #ifdef INCLUDE_SKIN
    delete skin;
    #endif

    #ifdef INCLUDE_ANIMATION
    delete rig;
    delete clip;
    delete player;
    #endif

    // #ifdef INCLUDE_CLOTH
    delete cloth;
    // #endif

    // Delete the shader program.
    glDeleteProgram(shaderProgram);

    // imgui stuff
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

// for the Window
GLFWwindow* Window::createWindow(int width, int height) {
    // Initialize GLFW.
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return NULL;
    }

    // 4x antialiasing.
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Apple implements its own version of OpenGL and requires special treatments
    // to make it uses modern OpenGL.
    // Ensure that minimum OpenGL version is 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Enable forward compatibility and allow a modern OpenGL context
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create the GLFW window.
    GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

    // Check if the window could not be created.
    if (!window) {
        std::cerr << "Failed to open GLFW window." << std::endl;
        glfwTerminate();
        return NULL;
    }

    // Make the context of the window.
    glfwMakeContextCurrent(window);

    // Set swap interval to 1.
    glfwSwapInterval(0);

    // set up the camera
    Cam = new Camera();
    Cam->SetAspect(float(width) / float(height));

    // initialize the interaction variables
    LeftDown = RightDown = false;
    MouseX = MouseY = 0;

    // Call the resize callback to make sure things get drawn immediately.
    Window::resizeCallback(window, width, height);

    return window;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height) {
#ifdef __APPLE__
    // In case your Mac has a retina display.
    glfwGetFramebufferSize(window, &width, &height);
#endif
    Window::width = width;
    Window::height = height;
    // Set the viewport size.
    glViewport(0, 0, width, height);

    Cam->SetAspect(float(width) / float(height));
}

// update and draw functions
void Window::idleCallback() {
    static float lastTime = glfwGetTime();
    float currentTime = glfwGetTime();
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    
    // Perform any updates as necessary.
    Cam->Update();

    // cube->update();

    #ifdef INCLUDE_ANIMATION
    if (player && clip) {
        player->Update(deltaTime);
    } else {

        #ifdef INCLUDE_SKELETON
        if (skeleton == nullptr) {
            std::cerr << "failed to initialize skeleton" << std::endl;
            return;
        }
        skeleton->Update();
        #endif

        #ifdef INCLUDE_SKIN
        if (skin == nullptr) {
            std::cerr << "failed to initialize skin" << std::endl;
            return;
        }
        skin->Update();
        #endif
    }
    #else
        #ifdef INCLUDE_SKELETON
        if (skeleton == nullptr) {
            std::cerr << "failed to initialize skeleton" << std::endl;
            return;
        }
        skeleton->Update();
        #endif

        #ifdef INCLUDE_SKIN
        if (skin == nullptr) {
            std::cerr << "failed to initialize skin" << std::endl;
            return;
        }
        skin->Update();
        #endif
    #endif

    // #ifdef INCLUDE_CLOTH
    if (cloth && !pauseSimulation) {
        cloth->SetWind(wind);
        cloth->Simulate(deltaTime);
    }
    // #endif
}

void Window::displayCallback(GLFWwindow* window) {
    
    // imgui new frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    // Clear the color and depth buffers.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the object.
    // cube->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);

    #ifdef INCLUDE_SKELETON
    if (drawSkeleton && skeleton) {
        skeleton->Draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
    }
    #endif

    #ifdef INCLUDE_SKIN
    if (skin) {
        skin->Draw(Cam->GetViewProjectMtx(), Window::shaderProgram, lightDirection1, lightColor1, lightDirection2, lightColor2);
    }
    #endif

    // create imgui window
    #ifdef INCLUDE_SKELETON
    ImGui::Begin("joints");
    RenderJointControls();
    ImGui::End();
    #endif

    // #ifdef INCLUDE_CLOTH
    if (cloth) {
        std::cout << "About to draw cloth..." << std::endl;
        cloth->Draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
        std::cout << "Finished drawing cloth" << std::endl;

        
    }
    // #endif

    // #ifdef INCLUDE_CLOTH
    ImGui::Begin("cloth");
    RenderClothControls();
    ImGui::End();
    // #endif

    ImGui::ShowDemoWindow();

    // render ui
    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Gets events, including input such as keyboard and mouse or window resizing.
    glfwPollEvents();
    // Swap buffers.
    glfwSwapBuffers(window);
}

// helper to reset the camera
void Window::resetCamera() {
    Cam->Reset();  

    Cam->SetAspect(float(Window::width) / float(Window::height));
}

// callbacks - for Interaction
void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    /*
     * TODO: Modify below to add your key callbacks.
     */

    // Check for a key press.
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                // Close the window. This causes the program to also terminate.
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;

            case GLFW_KEY_R:
                resetCamera();
                break;

            default:
                break;
        }
    }
}

void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        LeftDown = (action == GLFW_PRESS);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        RightDown = (action == GLFW_PRESS);
    }
}

void Window::cursor_callback(GLFWwindow* window, double currX, double currY) {
    int maxDelta = 100;
    int dx = glm::clamp((int)currX - MouseX, -maxDelta, maxDelta);
    int dy = glm::clamp(-((int)currY - MouseY), -maxDelta, maxDelta);

    MouseX = (int)currX;
    MouseY = (int)currY;

    // Move camera
    // NOTE: this should really be part of Camera::Update()
    if (LeftDown) {
        const float rate = 1.0f;
        Cam->SetAzimuth(Cam->GetAzimuth() + dx * rate);
        Cam->SetIncline(glm::clamp(Cam->GetIncline() - dy * rate, -90.0f, 90.0f));
    }
    if (RightDown) {
        const float rate = 0.005f;
        float dist = glm::clamp(Cam->GetDistance() * (1.0f - dx * rate), 0.01f, 1000.0f);
        Cam->SetDistance(dist);
    }
}

#ifdef INCLUDE_SKELETON
void Window::RenderJointControls() {
    
    ImGui::Checkbox("draw skeleton", &drawSkeleton);

    if (ImGui::Button("previous joint") && currentJointIndex > 0) {
        currentJointIndex--;
    }

    ImGui::SameLine();

    if (ImGui::Button("next joint") && currentJointIndex < skeleton->GetJointList().size() - 1) {
        currentJointIndex++;
    }
    
    if (!skeleton->GetJointList().empty()) {
        // get the selected joint
        selectedJoint = skeleton->GetJointList()[currentJointIndex];
        ImGui::Text("selected joint: %s", selectedJoint->GetName().c_str());

        // x-rotation
        float x = selectedJoint->GetDOF(0).GetValue();

        if (ImGui::Button("-##x")) {
            x -= 0.1f;
            selectedJoint->GetDOF(0).SetValue(x);
        }

        ImGui::SameLine();

        if (ImGui::Button("+##x")) {
            x += 0.1f;
            selectedJoint->GetDOF(0).SetValue(x);
        }

        if (ImGui::InputFloat("x-rotation", &x)) {
            selectedJoint->GetDOF(0).SetValue(x);
        }

        // y-rotation
        float y = selectedJoint->GetDOF(1).GetValue();

        if (ImGui::Button("-##y")) {
            y -= 0.1f;
            selectedJoint->GetDOF(1).SetValue(y);
        }

        ImGui::SameLine();

        if (ImGui::Button("+##y")) {
            y += 0.1f;
            selectedJoint->GetDOF(1).SetValue(y);
        }

        if (ImGui::InputFloat("y-rotation", &y)) {
            selectedJoint->GetDOF(1).SetValue(y);
        }

        // z-rotation
        float z = selectedJoint->GetDOF(2).GetValue();

        if (ImGui::Button("-##z")) {
            z -= 0.1f;
            selectedJoint->GetDOF(2).SetValue(z);
        }

        ImGui::SameLine();

        if (ImGui::Button("+##z")) {
            z += 0.1f;
            selectedJoint->GetDOF(2).SetValue(z);
        }

        if (ImGui::InputFloat("z-rotation", &z)) {
            selectedJoint->GetDOF(2).SetValue(z);
        }
    }
}
#endif

// #ifdef INCLUDE_CLOTH
void Window::RenderClothControls() {

    ImGui::Text("wind");
    ImGui::SliderFloat("wind-x", &wind.x, -10.0f, 10.0f);
    ImGui::SliderFloat("wind-y", &wind.y, -10.0f, 10.0f);
    ImGui::SliderFloat("wind-z", &wind.z, -10.0f, 10.0f);

    if (ImGui::Button("reset wind")) {
        wind = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    ImGui::Separator();

    ImGui::Checkbox("pause simulation", &pauseSimulation);
}
// #endif