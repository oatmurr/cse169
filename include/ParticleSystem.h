#pragma once

#include "Particle.h"

struct ParticleSystem
{
public:
    // GL data
    GLuint VAO, VBO;
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 color;

    // ----- SPH PARAMETERS -----
    // radius of influence around each particle (m)
    float smoothingRadius;
    // mass of each particle (kg)
    float mass;
    // rest density of fluid (kg/m^3)
    float restDensity;
    // viscosity coefficient (Pa * s)
    float viscosity;
    // gas constant in pressure equation(m^2/s^2)
    float gasConstant;
    // gravitational acceleration (m/s^2)
    glm::vec3 gravity;

    // ----- BOUNDARY CONDITIONS -----
    // stiffness of boundary (N/m)
    float boundaryStiffness;
    // damping of boundary
    float boundaryDamping;
    // minimum and maximum coordinates of bounding box (m)
    glm::vec3 boxMin;
    glm::vec3 boxMax;

    // data 
    int size;
    std::vector<Particle*> particles;

    // constructor/destructor
    ParticleSystem(int size, glm::vec3 color, float smoothingRadius, float mass, float restDensity, float viscosity, float gasConstant, glm::vec3 gravity, float boundaryStiffness, float boundaryDamping, glm::vec3 boxMin, glm::vec3 boxMax);
    ~ParticleSystem();

    // core
    void Draw(const glm::mat4& viewProjMtx, GLuint shader);
    void Update();

    // SPH
    void ComputeDensityPressure();
    void ComputeForces();
    void Integrate(float dt);

    // boundary handling
    void HandleBoundaryConditions(float dt);
    void EnforceHardBoundaries(Particle* p);

    // ----- KERNELS -----
    // support radius = 2h
    // q = r/h

    // W_ij = W((‖x_i - x_j‖)/(h)) = W(q) = (1/(h^d)) * f(q)

    // density - W(r, h)
    // f(q) = (3/(2π)) * {
    //     (2/3 - q^2 + 1/2 * q^3)              for 0 ≤ q < 1
    //     (1/6) * (2 - q)^3                    for 1 ≤ q < 2
    //     0                                    for q ≥ 2
    // }
    float KernelFunction(float r, float h);

    // pressure - ∇W(r, h)
    // f(q) = (3/(2π)) * {
    //     (-2q * (3/2) * q^2)                  for 0 ≤ q < 1
    //     (-1/2) * (2 - q)^2                   for 1 ≤ q < 2
    //     0                                    for q ≥ 2
    glm::vec3 KernelGradient(glm::vec3 r, float h);

    // viscosity - ∇^2W(r, h)
    // f(q) = (3/(2π)) * {
    //     (-2 + 3q)                            for 0 ≤ q < 1
    //     -(2 - q)^2                           for 1 ≤ q < 2
    //     0                                    for q ≥ 2
    float KernelLaplacian(float r, float h);

    // utility
    void Reset();
};
