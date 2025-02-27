#include "ClothTriangle.h"

ClothTriangle::ClothTriangle(Particle* p1, Particle* p2, Particle* p3, int indexP1, int indexP2, int indexP3) {
    this->p1 = p1;
    this->p2 = p2;
    this->p3 = p3;
    this->indexP1 = indexP1;
    this->indexP2 = indexP2;
    this->indexP3 = indexP3;
    dragCoefficient = 1.28;
    fluidDensity = 1.225;
}

void ClothTriangle::ComputeAerodynamicForce(glm::vec3 wind) {
    // for velocity (v) of triangle use average of three particle velocities
    glm::vec3 surfaceVelocity = (p1->GetVelocity() + p2->GetVelocity() + p3->GetVelocity()) / 3.0f;

    // subtract off velocity of air
    glm::vec3 velocity = surfaceVelocity - wind;

    // speed (|v|)
    float speed = glm::length(velocity);

    // triangle normal
    glm::vec3 normal = glm::normalize(glm::cross(p2->GetPosition() - p1->GetPosition(), p3->GetPosition() - p1->GetPosition()));

    // area (a0) of triangle
    float area = 0.5f * glm::length(glm::cross(p2->GetPosition() - p1->GetPosition(), p3->GetPosition() - p1->GetPosition()));

    // cross-sectional area (a) which is the area viewed from the direction of the airflow (clamp to 0 if negative)
    float cosTheta = glm::dot(glm::normalize(velocity), normal);
    cosTheta = glm::max(0.0f, cosTheta);
    float crossSectionalArea = area * cosTheta;

    // aerodynamic force
    glm::vec3 aerodynamicForce = (-1.0f/2.0f) * fluidDensity * speed * speed * dragCoefficient * crossSectionalArea * normal;

    // apply 1/3 of the total force to each of the three particles connecting the triangle
    p1->ApplyForce(aerodynamicForce / 3.0f);
    p2->ApplyForce(aerodynamicForce / 3.0f);
    p3->ApplyForce(aerodynamicForce / 3.0f);
}

Particle* ClothTriangle::GetP1() {
    return p1;
}

Particle* ClothTriangle::GetP2() {
    return p2;
}

Particle* ClothTriangle::GetP3() {
    return p3;
}

int ClothTriangle::GetIndexP1() {
    return indexP1;
}

int ClothTriangle::GetIndexP2() {
    return indexP2;
}

int ClothTriangle::GetIndexP3() {
    return indexP3;
}

glm::vec3 ClothTriangle::ComputeNormal() {

    glm::vec3 position1 = p1->GetPosition();
    glm::vec3 position2 = p2->GetPosition();
    glm::vec3 position3 = p3->GetPosition();

    glm::vec3 normal = glm::normalize(glm::cross(position2 - position1, position3 - position1));

    return normal;
}