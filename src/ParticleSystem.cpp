#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(int size, glm::vec3 color, float smoothingRadius, float particleMass, float restDensity, float viscosity, float gasConstant, glm::vec3 gravity, float boundaryStiffness, float boundaryDamping, glm::vec3 boxMin, glm::vec3 boxMax)
{
    this->size = size;
    this->particles = std::vector<Particle*>(size);

    this->color = color;

    this->smoothingRadius = smoothingRadius;
    this->particleMass = particleMass;
    this->restDensity = restDensity;
    this->viscosity = viscosity;
    this->gasConstant = gasConstant;
    this->gravity = gravity;

    this->boundaryStiffness = boundaryStiffness;
    this->boundaryDamping = boundaryDamping;
    this->boxMin = boxMin;
    this->boxMax = boxMax;
}

ParticleSystem::~ParticleSystem()
{
    for (Particle* p : particles)
    {
        delete p;
    }
    
    // Delete the VBOs and the VAO.
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void ParticleSystem::draw(const glm::mat4& viewProjMtx, GLuint shader)
{
    // *** GL jobs ***
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // Bind to the VAO.
    glBindVertexArray(VAO);

    // Bind to VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(glm::vec3), particles.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    // Unbind the VBOs.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // actiavte the shader program
    glUseProgram(shader);

    // get the locations and send the uniforms to the shader
    glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&viewProjMtx);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);

    // draw points
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, particles.size());
    glBindVertexArray(0);

    // Unbind the VAO and shader program
    glBindVertexArray(0);
    glUseProgram(0);
}

void ParticleSystem::update()
{

}

float ParticleSystem::kernelFunction(float r, float h)
{
    float q = r / h;

    float kernelValue = 0.0f;

    float coefficient = 3.0f / (2.0f * M_PI);
    
    // (3/(2π)) * ((2/3 - q^2 + 1/2 * q^3)) for 0 ≤ q < 1
    if (q < 1.0f)
    {
        kernelValue = coefficient * (2.0f / 3.0f - q * q + 0.5f * q * q * q);
    }
    // (3/(2π)) * (1/6) * (2 - q)^3 for 1 ≤ q < 2
    else if (q < 2.0f)
    {
        kernelValue = coefficient * (1.0f / 6.0f) * std::pow(2.0f - q, 3.0f);
    }

    return kernelValue;
}

glm::vec3 ParticleSystem::kernelGradient(glm::vec3 r, float h)
{
    float q = glm::length(r) / h;

    float gradientMagnitude = 0.0f;

    float coefficient = 3.0f / (2.0f * M_PI);

    // (3/(2π)) * ((-2q * (3/2) * q^2)) for 0 ≤ q < 1
    if (q < 1.0f)
    {
        gradientMagnitude = coefficient * (-2.0f * q * 1.5f * q * q);
    }
    // (3/(2π)) * ((-1/2) * (2 - q)^2) for 1 ≤ q < 2
    else if (q < 2.0f)
    {
        gradientMagnitude = coefficient * (-0.5f) * std::pow(2.0f - q, 2.0f);
    }

    return gradientMagnitude * glm::normalize(r);
}

float ParticleSystem::kernelLaplacian(float r, float h)
{
    float q = r / h;

    float laplacianValue = 0.0f;

    float coefficient = 3.0f / (2.0f * M_PI);

    // (3/(2π)) * ((-2 + 3q)) for 0 ≤ q < 1
    if (q < 1.0f)
    {
        laplacianValue = coefficient * (-2.0f + 3.0f * q);
    }
    // (3/(2π)) * (-(2 - q)^2) for 1 ≤ q < 2
    else if (q < 2.0f)
    {
        laplacianValue = coefficient * -std::pow(2.0f - q, 2.0f);
    }

    return laplacianValue;
}