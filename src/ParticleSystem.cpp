#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(int size, glm::vec3 color, float smoothingRadius, float mass, float restDensity, float viscosity, float gasConstant, glm::vec3 gravity, float boundaryStiffness, float boundaryDamping, glm::vec3 boxMin, glm::vec3 boxMax)
{
    this->size = size;
    this->particles = std::vector<Particle*>(size);

    this->color = color;

    this->smoothingRadius = smoothingRadius;
    this->mass = mass;
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
    float dt = 0.01f;

    computeDensityPressure();
    computeForces();
    integrate(dt);
    handleBoundaryConditions(dt);
}

void ParticleSystem::computeDensityPressure()
{
    // compute density and pressure for each particle
    for (Particle* pi : particles)
    {   
        // reset density
        float density = 0.0f;

        // sum up contributions from all other particles (neighbor search not implemented yet)
        // kernel function: W_ij = W((‖x_i - x_j‖)/(h)) = W(q) = (1/(h^d)) * f(q)
        for (Particle* pj : particles)
        {               
            // distance betwen particles: x_i - x_j
            glm::vec3 r_ij = pj->GetPosition() - pi->GetPosition();

            // magnitude of r_ij: ‖x_i - x_j‖
            float r = glm::length(r_ij);

            // apply smoothing kernel
            if (r < smoothingRadius)
            {
                // normalisation factor: 1/(h^d)
                float normalisationFactor = 1.0f / (smoothingRadius * smoothingRadius * smoothingRadius);
                
                // ----- DENSITY -----
                // W_ij = W(r, h) = (1/(h^d)) * f(q)
                float kernelW = normalisationFactor * kernelFunction(r, smoothingRadius);
                // density = ∑(m_j * W)
                density += mass * kernelW;
            }
        }

        pi->SetDensity(density);

        // ----- PRESSURE -----
        // tait equation of state for water (γ = 7): pi = k[(ρ/ρ₀)^γ - 1]
        // from section 1.3 of "SPH Fluids in Computer Graphics" by Ihmsen et al. 2014
        float pressure = gasConstant * (pow(density / restDensity, 7.0f) - 1.0f);

        pi->SetPressure(pressure);

        // ensure pressure is not negative
        if (pi->GetPressure() < 0.0f)
        {
            std::cout << "ParticleSystem::computeDensityPressure - negative pressure" << std::endl;
            pi->SetPressure(0.0f);
        }
    }
}

void ParticleSystem::computeForces()
{
    // compute forces for each particle (pressure, viscosity, gravity)
    for (Particle* pi : particles)
    {
        glm::vec3 pressureForce = glm::vec3(0.0f);
        glm::vec3 viscosityForce = glm::vec3(0.0f);
        // ----- GRAVITY FORCE -----
        glm::vec3 gravityForce = mass * gravity;
        
        // sum up contributions from all other particles (neighbor search not implemented yet)
        // kernel function: W_ij = W((‖x_i - x_j‖)/(h)) = W(q) = (1/(h^d)) * f(q)
        for (Particle* pj : particles)
        {
            if (pi == pj)
            {
                continue;
            }

            // distance betwen particles: x_i - x_j
            glm::vec3 r_ij = pj->GetPosition() - pi->GetPosition();

            // magnitude of r_ij: ‖x_i - x_j‖
            float r = glm::length(r_ij);

            if (r < smoothingRadius && r > 0.0001f)
            {
                // normalisation factor: 1/(h^d)
                float normalisationFactor = 1.0f / (smoothingRadius * smoothingRadius * smoothingRadius);

                // ----- PRESSURE FORCE -----
                // W_ij = ∇W(r, h) = (1/(h^d)) * f(q)
                glm::vec3 gradientW = normalisationFactor * kernelGradient(r_ij, smoothingRadius);
                // F_pressure = -m * (p_i/ρ_i² + p_j/ρ_j²) * ∇W
                pressureForce += -mass * (pi->GetPressure() / (pi->GetDensity() * pi->GetDensity()) + 
                                          pj->GetPressure() / (pj->GetDensity() * pj->GetDensity())) * gradientW;

                // ----- VISCOSITY FORCE -----
                // W_ij = ∇^2W(r, h) = (1/(h^d)) * f(q)
                float laplacianW = normalisationFactor * kernelLaplacian(r, smoothingRadius);
                // F_viscosity = μ * m * (v_j - v_i)/ρ_j * ∇²W
                viscosityForce += viscosity * mass * (pj->GetVelocity() - pi->GetVelocity()) / pj->GetDensity() * laplacianW;
            }
        }

        glm::vec3 totalForce = pressureForce + viscosityForce + gravityForce;

        pi->ApplyForce(totalForce);
    }
}

void ParticleSystem::integrate(float dt)
{
    for (Particle* p : particles)
    {
        p->Integrate(dt);
    }
}

void ParticleSystem::handleBoundaryConditions(float dt)
{

}

void ParticleSystem::reset()
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