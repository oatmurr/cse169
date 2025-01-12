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
    root->Update();
}

void Skeleton::Draw() {
    root->Draw();
}