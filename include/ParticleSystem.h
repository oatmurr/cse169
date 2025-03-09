#pragma once

#include <vector>
#include "core.h"

struct ParticleSystem
{
public:
    // GL data
    GLuint VAO, VBO;
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 color;

    // data 
    int size;
    std::vector<glm::vec3> positions;

    // my example code let points attach to and rotate with the cube
    ParticleSystem(int n, std::vector<glm::vec3> cubePos);
    ~ParticleSystem();

    void draw(const glm::mat4& viewProjMtx, GLuint shader);
    void update(glm::mat3 spinMat);

    void spin(float deg);
};
