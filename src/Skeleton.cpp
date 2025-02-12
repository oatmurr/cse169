#include "Skeleton.h"

Skeleton::Skeleton() {
    printf("Skeleton::Skeleton() - constructing skeleton\n");
    root = nullptr;
}

Skeleton::~Skeleton() {
    printf("Skeleton::~Skeleton() - destroying skeleton\n");
    delete root;
}

bool Skeleton::Load(const char* filename) {
    printf("Skeleton::Load - loading skeleton from file: %s\n", filename);
    
    if (!filename) {
        // test.skel used for project 1
        // filename = "test.skel";
        return true;
    }
    
    // open .skel file
    Tokenizer token;
    token.Open(filename);
    printf("Skeleton::Load - searching for balljoint token\n");
    token.FindToken("balljoint");

    // parse tree
    printf("Skeleton::Load - creating root joint\n");
    root = new Joint;
    printf("Skeleton::Load - loading joint data\n");
    root->Load(token);

    // finish
    printf("Skeleton::Load - closing tokenizer\n");
    token.Close();
    return true;
}

void Skeleton::Update() {
    // printf("Skeleton::Update - updating skeleton\n");
    // traverse tree and update all joint matrices
    glm::mat4 identity = glm::mat4(1.0f);
    if (root == nullptr) {
        // printf("Skeleton::Update - ERROR: root is null in Update()\n");
        return;
    }
    root->Update(identity);
    // printf("Skeleton::Update - finished updating skeleton\n");
}

void Skeleton::Draw(const glm::mat4& viewProjMtx, GLuint shader) {
    // printf("Skeleton::Draw - drawing skeleton\n");
    if (root == nullptr) {
        // printf("Skeleton::Draw - ERROR: root is null in Draw()\n");
        return;
    }
    root->Draw(viewProjMtx, shader);
    // printf("Skeleton::Draw - finished drawing skeleton\n");
}

void Skeleton::PopulateJointList() {
    printf("Skeleton::PopulateJointList - populating joint list\n");
    if (root) {
        root->PopulateJointList(jointList);
    }
}

glm::mat4 Skeleton::GetWorldMatrix(int jointIndex) {
    return jointList[jointIndex]->GetWorldMatrix();
}

std::vector<Joint*> Skeleton::GetJointList() {
    return jointList;
}