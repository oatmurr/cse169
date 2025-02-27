#pragma once

#include "core.h"

class Particle {
private:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 force;
    float mass;
    bool fixed;

public:
    Particle(glm::vec3 position, float mass, bool fixed = false);

    // apply a force to the particle
    void ApplyForce(glm::vec3 force);

    // integrate the particle forward in time
    void Integrate(float dt);

    // getters and setters
    glm::vec3 GetPosition();
    glm::vec3 GetVelocity();
    float GetMass();
    void SetPosition(glm::vec3 position);
    void SetVelocity(glm::vec3 velocity);
};
