#include "Joint.h"

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
    // find '{' token
    token.FindToken("{");

    // loop through tokens until '}' token
    while (true) {
        char temp[256];
        token.GetToken(temp);
        // check for 'offset' token
        if(strcmp(temp, "offset") == 0) {
            offset.x = token.GetFloat();
            offset.y = token.GetFloat();
            offset.z = token.GetFloat();
        // check for 'boxmin'' token
        } else if(strcmp(temp, "boxmin") == 0) {
            boxMin.x = token.GetFloat();
            boxMin.y = token.GetFloat();
            boxMin.z = token.GetFloat();
        // check for 'boxmax' token
        } else if(strcmp(temp, "boxmax") == 0) {
            boxMax.x = token.GetFloat();
            boxMax.y = token.GetFloat();
            boxMax.z = token.GetFloat();
        // check for 'rotxlimit' token
        } else if(strcmp(temp, "rotxlimit") == 0) {
            float min = token.GetFloat();
            float max = token.GetFloat();
            pose[0].SetMinMax(min, max);
        // check for 'rotylimit' token
        } else if(strcmp(temp, "rotylimit") == 0) {
            float min = token.GetFloat();
            float max = token.GetFloat();
            pose[1].SetMinMax(min, max);
        // check for 'rotzlimit' token
        } else if(strcmp(temp, "rotzlimit") == 0) {
            float min = token.GetFloat();
            float max = token.GetFloat();
            pose[2].SetMinMax(min, max);
        // check for 'pose' token
        } else if(strcmp(temp, "pose") == 0) {
            pose[0].SetValue(token.GetFloat());
            pose[1].SetValue(token.GetFloat());
            pose[2].SetValue(token.GetFloat());
        // check for child 'balljoint; token
        } else if(strcmp(temp, "balljoint") == 0) {
            Joint *child = new Joint();
            child->Load(token);
            AddChild(child);
        // check for '}' token
        } else if(strcmp(temp, "}") == 0) {
            return true;
        // unrecognised token
        } else {
            token.SkipLine();
        }
    }
}

void Joint::Update(glm::mat4& parentWorld) {
    // compute local matrix L
    // initialize local matrix with identity matrix and translate by offset vector
    localMatrix = glm::translate(glm::mat4(1.0f), offset);
    // rotate around x-axis (pitch) using first pose x value
    localMatrix = glm::rotate(localMatrix, pose[0].GetValue(), glm::vec3(1.0f, 0.0f, 0.0f));
    // rotate around y-axis (yaw) using second pose y value
    localMatrix = glm::rotate(localMatrix, pose[1].GetValue(), glm::vec3(0.0f, 1.0f, 0.0f));
    // rotate around z-axis (roll) using third pose z value
    localMatrix = glm::rotate(localMatrix, pose[2].GetValue(), glm::vec3(0.0f, 0.0f, 1.0f));

    // compute world matrix W
    worldMatrix = parentWorld * localMatrix;

    // recursively call update() on children
    for (Joint* child : children) {
        child->Update(worldMatrix);
    }
}

void Joint::AddChild(Joint* child) {
    child->parent = this;
    children.push_back(child);
}

void Joint::Draw(const glm::mat4& viewProjMtx, GLuint shader) {
    // draw orientated box with OpenGL
    // create cube for this joint
    Cube cube(boxMin, boxMax);

    // set cube's model matrix to world matrix
    cube.update(worldMatrix);

    // draw cube
    cube.draw(viewProjMtx, shader);

    // recursively call draw() on children
    for (Joint* child : children) {
        child->Draw(viewProjMtx, shader);
    }
}