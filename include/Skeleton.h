#pragma once

#include "Joint.h"

class Skeleton
{
private:
    // joint tree
    Joint* root;

    // joint list for imgui
    std::vector<Joint*> jointList;

public:
    // constructors/destructors
    Skeleton();
    ~Skeleton();

    // functions
    bool Load(const char* filename);
    void Update(); // traverse tree and compute all join matrices
    void Draw(const glm::mat4& viewProjMtx, GLuint shader);

    // added when implementing gui (imgui)
    // populate joint list for imgui
    void PopulateJointList();

    // added for skinning
    glm::mat4 GetWorldMatrix(int jointIndex);
    std::vector<Joint*> GetJointList();
};