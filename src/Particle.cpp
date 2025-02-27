#include "Particle.h"

Particle::Particle(glm::vec3 position, float mass, bool fixed) {
    this->position = position;
    this->velocity = glm::vec3(0);
    this->force = glm::vec3(0);
    this->mass = mass;
    this->fixed = fixed;
}

void Particle::ApplyForce(glm::vec3 force) {
    if (!fixed) {
        this->force += force;
    }
}

void Particle::Integrate(float dt) {
    
    if (!fixed) {
        // apply newton's second law (f = ma)
        glm::vec3 acceleration = force / mass;

        // forward euler integration to get new velocity
        velocity += acceleration * dt;

        // forward euler integration to get new position
        position += velocity * dt;
    }

    // zero force out so next frame will start fresh
    force = glm::vec3(0);
}

glm::vec3 Particle::GetPosition() {
    return position;
}

glm::vec3 Particle::GetVelocity() {
    return velocity;
}

float Particle::GetMass() {
    return mass;
}

void Particle::SetPosition(glm::vec3 position) {
    this->position = position;
}

void Particle::SetVelocity(glm::vec3 velocity) {
    this->velocity = velocity;
}
