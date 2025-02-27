#pragma once

#include "Particle.h"

class SpringDamper {
private:
    // stiffness
    float springConstant;
    // damping
    float dampingConstant;
    float restLength;
    Particle* p1;
    Particle* p2;

public:
    SpringDamper(Particle* p1, Particle* p2, float springConstant, float dampingConstant);
    
    // compute and apply spring-damper forces
    void ComputeForce();

    // getters and setters
    Particle* GetP1();
    Particle* GetP2();
};