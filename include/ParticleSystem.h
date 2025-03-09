#pragma once

#include "Particle.h"

struct ParticleSystem
{
public:
    // GL data
    GLuint VAO, VBO;
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 color;

    // SPH stuff
    float smoothingRadius;
    float particleMass;
    float restDensity;
    float viscosity;
    float gasConstant;
    glm::vec3 gravity;

    // boundary
    float boundaryStiffness;
    float boundaryDamping;
    glm::vec3 boxMin;
    glm::vec3 boxMax;

    // data 
    int size;
    std::vector<Particle*> particles;

    // constructor/destructor
    ParticleSystem(int size, glm::vec3 color, float smoothingRadius, float particleMass, float restDensity, float viscosity, float gasConstant, glm::vec3 gravity, float boundaryStiffness, float boundaryDamping, glm::vec3 boxMin, glm::vec3 boxMax);
    ~ParticleSystem();

    // core
    void draw(const glm::mat4& viewProjMtx, GLuint shader);
    void update();

    // SPH
    void computeDensityPressure();
    void computeForces();
    void integrate(float dt);
    void handleBoundaryConditions(float dt);

    // utility
    void reset();
};
