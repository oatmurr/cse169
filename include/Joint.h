#pragma once

#include <vector>

#include "core.h"
#include "Tokenizer.h"
#include "DOF.h"


class Joint {
private:
    // local and world matrices
    glm::mat4 localMatrix;
    glm::mat4 worldMatrix;

    // constant positional offset to add to the local joint transformation. location of joint's pivot point described relative to parent joint space.
    // default to (0, 0, 0).
    glm::vec3 offset;

    // min and max corners of box representing  the bone to render for hte particular joint.
    // default to (-0.1, -0.1, -0.1) and (0.1, 0.1, 0.1).
    glm::vec3 boxMin;
    glm::vec3 boxMax;

    // pose (xyz values to pose DOFs)
    std::vector<DOF> pose{DOF(), DOF(), DOF()};
    
    // tree data
    std::vector<Joint*> children;
    Joint* parent;

public:
    // constructors/destructors
    Joint();
    ~Joint();

    // functions
    bool Load(Tokenizer& token);
    void Update(glm::mat4& parent);  // recursively generate local matrix & concatenate    void load();
    void AddChild(Joint* child);
    void Draw();
};