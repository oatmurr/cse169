#include "Joint.h"
#include <glm/gtc/matrix_transform.hpp>

Joint::Joint() {
    localMatrix = glm::mat4(1.0f);
    worldMatrix = glm::mat4(1.0f);

    offset = glm::vec3(0.0f, 0.0f, 0.0f);

    boxMin = glm::vec3(-0.1f, -0.1f, -0.1f);
    boxMax = glm::vec3(0.1f, 0.1f, 0.1f);

    parent = nullptr;
}

Joint::~Joint() {
    for (Joint* child : children) {
        delete child;
    }
}

bool Joint::Load(Tokenizer& token) {
    token.FindToken("{");
    while (true) {
        char temp[256];
        token.GetToken(temp);
        if(strcmp(temp, "offset") == 0) {
            offset.x = token.GetFloat();
            offset.y = token.GetFloat();
            offset.z = token.GetFloat();
        // check for other tokens
        } else if(strcmp(temp, "balljoint") == 0) {
            Joint *child = new Joint();
            child->Load(token);
            AddChild(child);
        } else if(strcmp(temp, "}") == 0) {
            return true;
        } else { // unrecognised token
            token.SkipLine();
        }
    }
}

void Joint::Update(glm::mat4& parent) {
    // computer local matrix L
    // compute world matrix W
    // recursively call update() on children
}

void Joint::AddChild(Joint* child) {
    child->parent = this;
    children.push_back(child);
}

void Joint::Draw() {
    // draw orientated box with OpenGL
    // recursively call draw() on children
}