#pragma once

#include "Triangle.h"
#include "Skeleton.h"
#include <vector>

class Skin {
private:
    // raw data from .skin file
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<Triangle> triangles;
    std::vector<glm::mat4> bindings;

    // processed vertex data
    std::vector<Vertex> vertices;
    std::vector<glm::vec3> transformedPositions;
    std::vector<glm::vec3> transformedNormals;

    // skinning data
    Skeleton* skeleton;
    std::vector<glm::mat4> skinningMatrices;

    // rendering stuff - based off Cube.h
    GLuint VAO;
    GLuint VBO_positions, VBO_normals, EBO;

    glm::mat4 model;
    glm::vec3 color;
    // glDrawElements expects GL_UNSIGNED_INT
    std::vector<unsigned int> triangleIndices;

public:
    // constructors/destructors
    Skin();
    ~Skin();

    // functions
    // optional skeleton parameter
    bool Load(const char* filename, Skeleton* skeleton = nullptr);
    void Update();
    void Draw(const glm::mat4& viewProjMtx, GLuint shader, const glm::vec3& lightDirection1, const glm::vec3& lightColor1, const glm::vec3& lightDirection2, const glm::vec3& lightColor2);
    // can't do in constructor because skin file is not loaded yet
    void SetupBuffers();
};