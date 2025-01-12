#include "Skeleton.h"

Skeleton::Skeleton() {
    root = nullptr;
}

Skeleton::~Skeleton() {
    delete root;
}

bool Skeleton::Load(const char* filename) {
    // open .skel file
    Tokenizer token;
    token.Open(filename);
    token.FindToken("balljoint");

    // parse tree
    root = new Joint;
    root->Load(token);

    // finish
    token.Close();
    return true;
}

void Skeleton::Update() {
    // traverse tree and update all joint matrices
    glm::mat4 identity = glm::mat4(1.0f);
    root->Update(identity);
}

void Skeleton::Draw(const glm::mat4& viewProjMtx, GLuint shader) {
    root->Draw(viewProjMtx, shader);
}