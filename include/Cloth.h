#pragma once

#include "ClothTriangle.h"
#include <vector>
#include <iostream>

class Cloth {
private:
    std::vector<Particle*> particles;
    std::vector<SpringDamper*> springs;
    std::vector<ClothTriangle*> triangles;

    glm::vec3 wind;
    glm::vec3 gravity;

    // rendering
    GLuint VAO;
    GLuint VBO_positions;
    GLuint VBO_normals;
    GLuint EBO;
    std::vector<glm::vec3> vertexPositions;
    std::vector<glm::vec3> vertexNormals;
    std::vector<unsigned int> triangleIndices;

    glm::mat4 model;
    glm::vec3 color;

public:
    Cloth(int width, int height, float particleSpacing, float mass, float springConstant, float dampingConstant);
    ~Cloth();

    void SetWind(glm::vec3 wind);

    void Simulate(float dt);

    void Draw(glm::mat4 viewProjMtx, GLuint shader);

    void SetupBuffers();

    void UpdateBuffers();
};
