#pragma once

#include "core.h"
#include <iostream>

class Particle {
private:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 force;
    float mass;
    bool fixed;

    // for SPH
    float density;
    float pressure;

public:
    Particle(glm::vec3 position, float mass, bool fixed = false);

    // apply a force to the particle
    void ApplyForce(glm::vec3 force);

    // integrate the particle forward in time, also checks ground collision
    void Integrate(float dt);

    // getters and setters
    glm::vec3 GetPosition();
    glm::vec3 GetVelocity();
    glm::vec3 GetForce();
    float GetMass();
    float GetDensity();
    float GetPressure();

    void SetPosition(glm::vec3 position);
    void SetVelocity(glm::vec3 velocity);
    void SetForce(glm::vec3 force);
    void SetMass(float mass);
    void SetDensity(float density);
    void SetPressure(float pressure);


    bool IsFixed();

};
