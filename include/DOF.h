#pragma once

#include "Cube.h"
#include "Tokenizer.h"

class DOF
{
private:
    // value
    GLfloat value;

    // limits
    GLfloat min;
    GLfloat max;

public:
    // constructor
    DOF();

    // functions
    GLfloat GetValue();
    void SetValue(GLfloat value); // (could) clamp to joint limits
    void SetMinMax(GLfloat min, GLfloat max);
    float GetMin();
    float GetMax();
};