#include "Window.h"
#include "core.h"
#include <iostream>

void error_callback(int error, const char* description)
{
    // Print error.
    std::cerr << description << std::endl;
}

void setup_callbacks(GLFWwindow* window)
{
    // Set the error callback.
    glfwSetErrorCallback(error_callback);
    // Set the window resize callback.
    glfwSetWindowSizeCallback(window, Window::resizeCallback);

    // Set the key callback.
    glfwSetKeyCallback(window, Window::keyCallback);

    // Set the mouse and cursor callbacks
    glfwSetMouseButtonCallback(window, Window::mouse_callback);
    glfwSetCursorPosCallback(window, Window::cursor_callback);
}

void setup_opengl_settings()
{
    // Enable depth buffering.
    glEnable(GL_DEPTH_TEST);
    // such that we can choose particle size
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    // Related to shaders and z value comparisons for the depth buffer.
    glDepthFunc(GL_LEQUAL);
    // Set polygon drawing mode to fill front and back of each polygon.
    // for different choices for these 2 param, see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glPolygonMode.xhtml
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // Set clear color to black.
    glClearColor(0.0, 0.0, 0.0, 0.0);
}

void print_versions()
{
    // Get info of GPU and supported OpenGL version.
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL version supported: " << glGetString(GL_VERSION)
              << std::endl;

    // If the shading language symbol is defined.
#ifdef GL_SHADING_LANGUAGE_VERSION
    std::cout << "Supported GLSL version is: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
#endif
}

int main(int argc, char* argv[])
{
    // Create the GLFW window.
    GLFWwindow* window = Window::createWindow(800, 600);
    if (!window) exit(EXIT_FAILURE);

    // Print OpenGL and GLSL versions.
    print_versions();
    // Setup callbacks.
    setup_callbacks(window);
    // Setup OpenGL settings.
    setup_opengl_settings();

    // Initialize the shader program; exit if initialization fails.
    // if (!Window::initializeProgram()) exit(EXIT_FAILURE);
    if (!Window::initializeProgram(window)) exit(EXIT_FAILURE);
    // Initialize objects/pointers for rendering; exit if initialization fails.
    if (!Window::initializeObjects()) exit(EXIT_FAILURE);

    // command line stuff
    if (argc > 1)
    {
        std::string filename = argv[1];

        #ifdef INCLUDE_SKELETON
        // if there is a .skel file
        if ((filename.find(".skel") != std::string::npos) && Window::skeleton)
        {
            Window::skeleton->Load(argv[1]);
            Window::skeleton->PopulateJointList();

            #ifdef INCLUDE_SKIN
            // if there is a .skel file and a .skin file
            if (argc > 2 && Window::skin)
            {
                Window::skin->Load(argv[2], Window::skeleton);

                #ifdef INCLUDE_ANIMATION
                // if there is a .skel file, a .skin file, and a .anim file
                if (argc > 3 && Window::clip)
                {
                    Window::clip->Load(argv[3]);
                }
                #endif
            }
            #endif
        }
        #endif

        #ifdef INCLUDE_SKIN
        // if there is a .skin file but no .skel file
        if ((filename.find(".skin") != std::string::npos) && Window::skin)
        {
            Window::skin->Load(argv[1], nullptr);
        }
        #endif

        #ifdef INCLUDE_CLOTH
        if (filename == "-cloth")
        {
            // default cloth
            int width = 20;
            int height = 20;
            float spacing = 0.2f;
            float mass = 1.0f;
            float springConstant = 300.0f;
            float dampingConstant = 15.0f;

            // parse additional parameters if provided
            if (argc > 2) width = std::stoi(argv[2]);
            if (argc > 3) height = std::stoi(argv[3]);
            if (argc > 4) spacing = std::stof(argv[4]);
            if (argc > 5) mass = std::stof(argv[5]);
            if (argc > 6) springConstant = std::stof(argv[6]);
            if (argc > 7) dampingConstant = std::stof(argv[7]);

            // create cloth
            Window::cloth = new Cloth(width, height, spacing, mass, springConstant, dampingConstant);
            
            if (Window::cloth)
            {
                std::cout << "cloth created with dimensions: " << width << "x" << height << std::endl;
            }
            else
            {
                std::cout << "failed to create cloth!" << std::endl;
            }
        }
        #endif

        #ifdef INCLUDE_SPH
        if (filename == "-sph")
        {
            // default sph
            int size = 1000;
            glm::vec3 color = glm::vec3(0.0f, 0.5f, 1.0f);
            float smoothingRadius = 0.1f;
            float mass = 0.02f;
            float restDensity = 30.0f;
            float viscosity = 0.01f;
            float gasConstant = 1.0f;
            glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f);
            float boundaryStiffness  = 10000.0f;
            float boundaryDamping = 0.5f;
            glm::vec3 boxMin = glm::vec3(-1.0f, -1.0f, -1.0f);
            glm::vec3 boxMax = glm::vec3(1.0f, 1.0f, 1.0f);

            // parse additional parameters if provided

            Window::particleSystem = new ParticleSystem
            (
                size, color, smoothingRadius, mass, restDensity,
                viscosity, gasConstant, gravity, boundaryStiffness,
                boundaryDamping, boxMin, boxMax
            );

            if (Window::particleSystem)
            {
                std::cout << "particle system created with size: " << size << std::endl;
            }
            else
            {
                std::cout << "failed to create particle system!" << std::endl;
            }
        }
        #endif
    }

    // Loop while GLFW window should stay open.
    while (!glfwWindowShouldClose(window))
    {
        // Main render display callback. Rendering of objects is done here.
        Window::displayCallback(window);

        // Idle callback. Updating objects, etc. can be done here.
        Window::idleCallback();
    }

    Window::cleanUp();
    // Destroy the window.
    glfwDestroyWindow(window);
    // Terminate GLFW.
    glfwTerminate();

    exit(EXIT_SUCCESS);
}