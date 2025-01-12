#pragma once

#include "Joint.h"

class Skeleton {
private:
    // joint tree
    Joint* root;

public:
    // constructors/destructors
    Skeleton();
    ~Skeleton();

    // functions
    bool Load(const char* filename);
    void Update(); // traverse tree and compute all join matrices
    void Draw(const glm::mat4& viewProjMtx, GLuint shader);
};