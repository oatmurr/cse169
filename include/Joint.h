#pragma once

#include "DOF.h"
#include "glm/gtc/matrix_transform.hpp"

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

    // name
    std::string name;

public:
    // constructors/destructors
    Joint();
    ~Joint();

    // functions
    bool Load(Tokenizer& token);
    void Update(glm::mat4& parentWorld);  // recursively generate local matrix & concatenate
    void AddChild(Joint* child);
    void Draw(const glm::mat4& viewProjMtx, GLuint shader);

    // added when implementing gui (imgui)
    std::string GetName();
    DOF& GetDOF(int index);
    void PopulateJointList(std::vector<Joint*>& jointList);

    // added for skinning
    glm::mat4 GetWorldMatrix();

    // added for animation
    void SetPose(float x, float y, float z);
};