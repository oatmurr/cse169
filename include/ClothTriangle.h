#pragma once

#include "SpringDamper.h"

class ClothTriangle {
private:
    // particles
    Particle* p1;
    Particle* p2;
    Particle* p3;

    // indices
    int indexP1;
    int indexP2;
    int indexP3;

    // drag coefficient 𝑐𝑑 for a flat plate is around 1.28, so we should use at least 1.0 or more
    float dragCoefficient;

    // fluid density 𝜌 of air at 15o C and a pressure of 101.325 kPa (14.696 psi) is 1.225 kg/m3
    float fluidDensity;

public:
    ClothTriangle(Particle* p1, Particle* p2, Particle* p3, int indexP1, int indexP2, int indexP3);

    // aerodynamicForce = (-1/2) * 𝜌 * |𝐯|^2 * 𝑐𝑑 * 𝑎 * 𝐧
    void ComputeAerodynamicForce(glm::vec3 wind);

    glm::vec3 ComputeNormal();

    Particle* GetP1();
    Particle* GetP2();
    Particle* GetP3();

    int GetIndexP1();
    int GetIndexP2();
    int GetIndexP3();
};
