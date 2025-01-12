#include "DOF.h"

DOF::DOF() {
    // radians
    value = 0.0f;

    // default limits
    min = -100000.0f;
    max = 100000.0f;
}

GLfloat DOF::GetValue() {
    return value;
}

void DOF::SetValue(GLfloat value) {
    // clamp to limits
    if (value < min) {
        this->value = min;
    } else if (value > max) {
        this->value = max;
    } else {
        this->value = value;
    }
}

void DOF::SetMinMax(GLfloat min, GLfloat max) {
    this->min = min;
    this->max = max;
}