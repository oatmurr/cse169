#include "Joint.h"

Joint::Joint() {
    printf("Joint::Joint() - constructing joint\n");
    localMatrix = glm::mat4(1.0f);
    worldMatrix = glm::mat4(1.0f);

    offset = glm::vec3(0.0f, 0.0f, 0.0f);

    boxMin = glm::vec3(-0.1f, -0.1f, -0.1f);
    boxMax = glm::vec3(0.1f, 0.1f, 0.1f);

    parent = nullptr;
}

Joint::~Joint() {
    printf("Joint::~Joint() - destroying joint\n");
    for (Joint* child : children) {
        delete child;
    }
}

bool Joint::Load(Tokenizer& token) {
    printf("Joint::Load - loading joint data\n");
    // find '{' token
    token.FindToken("{");

    // loop through tokens until '}' token
    while (true) {
        char temp[256];
        token.GetToken(temp);
        printf("Joint::Load - processing token: %s\n", temp);
        // check for 'offset' token
        if(strcmp(temp, "offset") == 0) {
            offset.x = token.GetFloat();
            offset.y = token.GetFloat();
            offset.z = token.GetFloat();
            printf("Joint::Load - offset: %f, %f, %f\n", offset.x, offset.y, offset.z);
        // check for 'boxmin'' token
        } else if(strcmp(temp, "boxmin") == 0) {
            boxMin.x = token.GetFloat();
            boxMin.y = token.GetFloat();
            boxMin.z = token.GetFloat();
            printf("Joint::Load - boxMin: %f, %f, %f\n", boxMin.x, boxMin.y, boxMin.z);
        // check for 'boxmax' token
        } else if(strcmp(temp, "boxmax") == 0) {
            boxMax.x = token.GetFloat();
            boxMax.y = token.GetFloat();
            boxMax.z = token.GetFloat();
            printf("Joint::Load - boxMax: %f, %f, %f\n", boxMax.x, boxMax.y, boxMax.z);
        // check for 'rotxlimit' token
        } else if(strcmp(temp, "rotxlimit") == 0) {
            float min = token.GetFloat();
            float max = token.GetFloat();
            pose[0].SetMinMax(min, max);
            printf("Joint::Load - rotxlimit: %f, %f\n", min, max);
        // check for 'rotylimit' token
        } else if(strcmp(temp, "rotylimit") == 0) {
            float min = token.GetFloat();
            float max = token.GetFloat();
            pose[1].SetMinMax(min, max);
            printf("Joint::Load - rotylimit: %f, %f\n", min, max);
        // check for 'rotzlimit' token
        } else if(strcmp(temp, "rotzlimit") == 0) {
            float min = token.GetFloat();
            float max = token.GetFloat();
            pose[2].SetMinMax(min, max);
            printf("Joint::Load - rotzlimit: %f, %f\n", min, max);
        // check for 'pose' token
        } else if(strcmp(temp, "pose") == 0) {
            pose[0].SetValue(token.GetFloat());
            pose[1].SetValue(token.GetFloat());
            pose[2].SetValue(token.GetFloat());
            printf("Joint::Load - pose: %f, %f, %f\n", pose[0].GetValue(), pose[1].GetValue(), pose[2].GetValue());
        // check for child 'balljoint; token
        } else if(strcmp(temp, "balljoint") == 0) {
            printf("Joint::Load - creating child joint\n");
            Joint *child = new Joint();
            child->Load(token);
            AddChild(child);
        // check for '}' token
        } else if(strcmp(temp, "}") == 0) {
            printf("Joint::Load - finished loading joint\n");
            return true;
        // unrecognised token
        } else {
            printf("Joint::Load - unrecognised token: %s\n", temp);
            token.SkipLine();
        }
    }
}

void Joint::Update(glm::mat4& parentWorld) {
    printf("Joint::Update - updating joint\n");
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
    printf("Joint::Draw - drawing joint\n");
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