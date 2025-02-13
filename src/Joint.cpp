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
    // printf("Joint::~Joint() - destroying joint\n");
    for (Joint* child : children) {
        delete child;
    }
}

bool Joint::Load(Tokenizer& token) {
    printf("Joint::Load - loading joint data\n");
    
    // get joint name for imgui
    char nameBuffer[256];
    token.GetToken(nameBuffer);
    name = std::string(nameBuffer);
    
    // find '{' token
    token.FindToken("{");

    // loop through tokens until '}' token
    while (true) {
        char temp[256];
        token.GetToken(temp);
        printf("Joint::Load - processing token: %s\n", temp);
        // check for 'offset' token
        if (strcmp(temp, "offset") == 0) {
            offset.x = token.GetFloat();
            offset.y = token.GetFloat();
            offset.z = token.GetFloat();
            printf("Joint::Load - offset: %f, %f, %f\n", offset.x, offset.y, offset.z);
        // check for 'boxmin'' token
        } else if (strcmp(temp, "boxmin") == 0) {
            boxMin.x = token.GetFloat();
            boxMin.y = token.GetFloat();
            boxMin.z = token.GetFloat();
            printf("Joint::Load - boxMin: %f, %f, %f\n", boxMin.x, boxMin.y, boxMin.z);
        // check for 'boxmax' token
        } else if (strcmp(temp, "boxmax") == 0) {
            boxMax.x = token.GetFloat();
            boxMax.y = token.GetFloat();
            boxMax.z = token.GetFloat();
            printf("Joint::Load - boxMax: %f, %f, %f\n", boxMax.x, boxMax.y, boxMax.z);
        // check for 'rotxlimit' token
        } else if (strcmp(temp, "rotxlimit") == 0) {
            float min = token.GetFloat();
            float max = token.GetFloat();
            pose[0].SetMinMax(min, max);
            printf("Joint::Load - rotxlimit: %f, %f\n", min, max);
        // check for 'rotylimit' token
        } else if (strcmp(temp, "rotylimit") == 0) {
            float min = token.GetFloat();
            float max = token.GetFloat();
            pose[1].SetMinMax(min, max);
            printf("Joint::Load - rotylimit: %f, %f\n", min, max);
        // check for 'rotzlimit' token
        } else if (strcmp(temp, "rotzlimit") == 0) {
            float min = token.GetFloat();
            float max = token.GetFloat();
            pose[2].SetMinMax(min, max);
            printf("Joint::Load - rotzlimit: %f, %f\n", min, max);
        // check for 'pose' token
        } else if (strcmp(temp, "pose") == 0) {
            initialPose[0].SetValue(token.GetFloat());
            initialPose[1].SetValue(token.GetFloat());
            initialPose[2].SetValue(token.GetFloat());
            printf("Joint::Load - initial pose: %f, %f, %f\n", initialPose[0].GetValue(), initialPose[1].GetValue(), initialPose[2].GetValue());
            pose[0].SetValue(initialPose[0].GetValue());
            pose[1].SetValue(initialPose[1].GetValue());
            pose[2].SetValue(initialPose[2].GetValue());
            printf("Joint::Load - pose: %f, %f, %f\n", pose[0].GetValue(), pose[1].GetValue(), pose[2].GetValue());
        // check for child 'balljoint; token
        } else if (strcmp(temp, "balljoint") == 0) {
            printf("Joint::Load - creating child joint\n");
            Joint *child = new Joint();
            child->Load(token);
            AddChild(child);
        // check for '}' token
        } else if (strcmp(temp, "}") == 0) {
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
    // printf("joint %s:\n", name.c_str());
    // printf("  initial pose: Z=%f Y=%f X=%f\n", initialPose[2].GetValue(), initialPose[1].GetValue(), initialPose[0].GetValue());
    // printf("  animation rotations: Z=%f Y=%f X=%f\n", pose[2].GetValue(), pose[1].GetValue(), pose[0].GetValue());

    // compute local matrix L
    // initialize local matrix with identity matrix and translate by offset vector
    localMatrix = glm::translate(glm::mat4(1.0f), offset);

    // apply initial pose zyx order
    localMatrix = glm::rotate(localMatrix, initialPose[2].GetValue(), glm::vec3(0.0f, 0.0f, 1.0f));
    localMatrix = glm::rotate(localMatrix, initialPose[1].GetValue(), glm::vec3(0.0f, 1.0f, 0.0f));
    localMatrix = glm::rotate(localMatrix, initialPose[0].GetValue(), glm::vec3(1.0f, 0.0f, 0.0f));

    // // apply initial pose xyz order
    // // localMatrix = glm::rotate(localMatrix, initialPose[0].GetValue(), glm::vec3(1.0f, 0.0f, 0.0f));
    // // localMatrix = glm::rotate(localMatrix, initialPose[1].GetValue(), glm::vec3(0.0f, 1.0f, 0.0f));
    // // localMatrix = glm::rotate(localMatrix, initialPose[2].GetValue(), glm::vec3(0.0f, 0.0f, 1.0f));

    // // // rotate around z-axis (roll) using pose z value
    // // localMatrix = glm::rotate(localMatrix, pose[2].GetValue(), glm::vec3(0.0f, 0.0f, 1.0f));
    // // // rotate around y-axis (yaw) using pose y value
    // // localMatrix = glm::rotate(localMatrix, pose[1].GetValue(), glm::vec3(0.0f, 1.0f, 0.0f));
    // // // rotate around x-axis (pitch) using pose x value
    // // localMatrix = glm::rotate(localMatrix, pose[0].GetValue(), glm::vec3(1.0f, 0.0f, 0.0f));

    // apply animation rotations zyx order
    localMatrix = glm::rotate(localMatrix, pose[2].GetValue(), glm::vec3(0.0f, 0.0f, 1.0f));
    localMatrix = glm::rotate(localMatrix, pose[1].GetValue(), glm::vec3(0.0f, 1.0f, 0.0f));
    localMatrix = glm::rotate(localMatrix, pose[0].GetValue(), glm::vec3(1.0f, 0.0f, 0.0f));

    // apply animation rotations xyz order
    // localMatrix = glm::rotate(localMatrix, pose[0].GetValue(), glm::vec3(1.0f, 0.0f, 0.0f));
    // localMatrix = glm::rotate(localMatrix, pose[1].GetValue(), glm::vec3(0.0f, 1.0f, 0.0f));
    // localMatrix = glm::rotate(localMatrix, pose[2].GetValue(), glm::vec3(0.0f, 0.0f, 1.0f));

    // compute world matrix W
    worldMatrix = parentWorld * localMatrix;
    // worldMatrix = localMatrix * parentWorld;

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
    // printf("Joint::Draw - drawing joint\n");
    // draw orientated box with OpenGL
    // create cube for this joint
    Cube cube = Cube(boxMin, boxMax);

    // set cube's model matrix to world matrix
    cube.update(worldMatrix);

    // draw cube
    cube.draw(viewProjMtx, shader);

    // recursively call draw() on children
    for (Joint* child : children) {
        child->Draw(viewProjMtx, shader);
    }
}

std::string Joint::GetName() {
    return name;
}

DOF& Joint::GetDOF(int index) {
    return pose[index];
}

void Joint::PopulateJointList(std::vector<Joint*>& jointList) {
    printf("Joint::PopulateJointList - adding joint %s at index %d\n", name.c_str(), jointList.size());
    jointList.push_back(this);
    for (Joint* child : children) {
        child->PopulateJointList(jointList);
    }
}

glm::mat4 Joint::GetWorldMatrix() {
    return worldMatrix;
}

void Joint::SetPose(float rotationX, float rotationY, float rotationZ) {
    pose[0].SetValue(rotationX);
    pose[1].SetValue(rotationY);
    pose[2].SetValue(rotationZ);
}

void Joint::SetOffset(float translationX, float translationY, float translationZ) {
    offset = glm::vec3(translationX, translationY, translationZ);
}
