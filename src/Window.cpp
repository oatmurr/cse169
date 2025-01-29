#include "Window.h"

// Window Properties
int Window::width;
int Window::height;
const char* Window::windowTitle = "Model Environment";

// Objects to render
// Cube* Window::cube;
Skeleton* Window::skeleton;

// Camera Properties
Camera* Cam;

// Interaction Variables
bool LeftDown, RightDown;
int MouseX, MouseY;

// The shader program id
GLuint Window::shaderProgram;

// imgui stuff
static Joint* selectedJoint = nullptr;
static int currentJointIndex = 0;

// Constructors and desctructors
// bool Window::initializeProgram() {
bool Window::initializeProgram(GLFWwindow* window) {
    // Create a shader program with a vertex shader and a fragment shader.
    shaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");

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

    return true;
}

bool Window::initializeObjects() {
    // Create a cube
    // cube = new Cube();
    // cube = new Cube(glm::vec3(-1, 0, -2), glm::vec3(1, 1, 1));
    skeleton = new Skeleton();
    return true;
}

void Window::cleanUp() {
    // Deallcoate the objects.
    // delete cube;
    delete skeleton;

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
    // Perform any updates as necessary.
    Cam->Update();

    // cube->update();
    skeleton->Update();
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
    skeleton->Draw(Cam->GetViewProjectMtx(), Window::shaderProgram);

    // create imgui window
    ImGui::Begin("joints");

    RenderJointControls();

    ImGui::End();

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

void Window::RenderJointControls() {
    
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