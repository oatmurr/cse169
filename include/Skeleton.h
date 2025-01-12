#pragma once

#include "core.h"
#include "DOF.h"
#include "Joint.h"
#include "Tokenizer.h"

class Skeleton {
private:
    // joint tree
    Joint* root;

public:
    // constructors/destructors
    Skeleton();
    ~Skeleton();

    // functions
    bool Load(const char* filename);
    void Update(); // traverse tree and compute all join matrices
    void Draw();
};