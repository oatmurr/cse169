#include "SpringDamper.h"

SpringDamper::SpringDamper(Particle* p1, Particle* p2, float springConstant, float dampingConstant, float restLength) {
    this->p1 = p1;
    this->p2 = p2;
    this->springConstant = springConstant;
    this->dampingConstant = dampingConstant;
    this->restLength = restLength;
}

void SpringDamper::ComputeForce() {
    // compute current length (l) and unit vector (e)
    glm::vec3 distance = p2->GetPosition() - p1->GetPosition();
    float currentLength = glm::length(distance);
    glm::vec3 unitVector = distance / currentLength;

    // compute closing velocity (vclose)
    float closingVelocity = glm::dot(p2->GetVelocity() - p1->GetVelocity(), unitVector);

    // -ks(l0 - l)
    glm::vec3 springForce = -springConstant * (currentLength - restLength) * unitVector;
    // -kd * vclose
    glm::vec3 dampingForce = -dampingConstant * closingVelocity * unitVector;

    // apply force f1 = fe
    p1->ApplyForce(springForce + dampingForce);

    // apply force f2 = -f1
    p2->ApplyForce(-springForce - dampingForce);
}

Particle* SpringDamper::GetP1() {
    return p1;
}

Particle* SpringDamper::GetP2() {
    return p2;
}