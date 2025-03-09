#define GLFW_INCLUDE_GLCOREARB
#define GLM_ENABLE_EXPERIMENTAL
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

// such that we can use glm::to_string() to print vec or mat
#include <glm/gtx/string_cast.hpp>

#include <vector>   // std::vector
#include <iostream> // std::cout