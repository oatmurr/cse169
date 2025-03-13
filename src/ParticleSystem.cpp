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

    // blob parameters
    float particleSpacing = smoothingRadius * 0.7f;
    int blobSize = (int)ceil(cbrt(size));
    float blobLength = blobSize * particleSpacing;

    // blob position
    float blobCenterX = (boxMax.x - boxMin.x) * 0.5f + boxMin.x;
    float blobCenterY = (boxMax.y - boxMin.y) * 0.5f + boxMin.y;
    float blobCenterZ = (boxMax.z - boxMin.z) * 0.5f + boxMin.z;

    // check if blob fits within boundaries
    if (blobCenterX - blobLength * 0.5f <= boxMin.x || blobCenterX + blobLength * 0.5f > boxMax.x)
    {
        std::cout << "blob exceeds x-boundary: length = " << blobLength << ", box x-bounds = [" << boxMin.x << ", " << boxMax.x << "]" << std::endl;
    }
    if (blobCenterY - blobLength * 0.5f <= boxMin.y || blobCenterY + blobLength * 0.5f > boxMax.y)
    {
        std::cout << "blob exceeds y-boundary: length = " << blobLength << ", box y-bounds = [" << boxMin.y << ", " << boxMax.y << "]" << std::endl;
    }
    if (blobCenterZ - blobLength * 0.5f <= boxMin.z || blobCenterZ + blobLength * 0.5f > boxMax.z)
    {
        std::cout << "blob exceeds z-boundary: length = " << blobLength << ", box z-bounds = [" << boxMin.z << ", " << boxMax.z << "]" << std::endl;
    }

    // create blob
    for (int i = 0; i < size; i++)
    {
        // grid position
        int x = i % blobSize;
        int y = (i / blobSize) % blobSize;
        int z = (i / (blobSize * blobSize));

        // convert to world position (centered above box center)
        glm::vec3 position = glm::vec3
        (
            blobCenterX - blobLength * 0.5f + x * particleSpacing,
            blobCenterY - blobLength * 0.5f + y * particleSpacing,
            blobCenterZ - blobLength * 0.5f + z * particleSpacing
        );

        // create particle
        particles[i] = new Particle(position, mass, false);
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
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

void ParticleSystem::Draw(const glm::mat4& viewProjMtx, GLuint shader)
{
    // create buffer of positions
    std::vector<glm::vec3> positions(size);
    for (int i = 0; i < size; i++)
    {
        positions[i] = particles[i]->GetPosition();
    }

    std::cout << "drawing " << positions.size() << " particles" << std::endl;
    for (int i = 0; i < 5 && i < positions.size(); i++)
    {
        std::cout << "particle " << i << " position: ("
                << positions[i].x << ", " << positions[i].y << ", " << positions[i].z << ")" << std::endl;
    }

    // *** GL jobs ***
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // Bind to the VAO.
    glBindVertexArray(VAO);

    // Bind to VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_STATIC_DRAW);
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

    glUniform3fv(glGetUniformLocation(shader, "particleColor"), 1, &color[0]);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(30.0f);

    // draw points
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, positions.size());
    glBindVertexArray(0);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "OpenGL error after drawing: " << err << std::endl;
    }

    // Unbind the VAO and shader program
    glBindVertexArray(0);
    glUseProgram(0);
}

void ParticleSystem::Update()
{   
    float dt = 0.001f;

    ComputeDensityPressure();
    ComputeForces();
    Integrate(dt);
    HandleBoundaryConditions(dt);
}

void ParticleSystem::ComputeDensityPressure()
{
    // compute density and pressure for each particle
    for (int i = 0; i < particles.size(); i++)
    {   
        Particle* pi = particles[i];
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
                float kernelW = normalisationFactor * KernelFunction(r, smoothingRadius);
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
            // std::cout << "ParticleSystem::computeDensityPressure - negative pressure" << std::endl;
            pi->SetPressure(0.0f);
        }

        // std::cout << "particle " << i << " density: " << density 
        //           << " (rest: " << restDensity << ")" << std::endl;
    }
}
void ParticleSystem::ComputeForces()
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
                glm::vec3 gradientW = normalisationFactor * KernelGradient(r_ij, smoothingRadius);
                // F_pressure = -m * (p_i/ρ_i² + p_j/ρ_j²) * ∇W
                pressureForce += -mass * (pi->GetPressure() / (pi->GetDensity() * pi->GetDensity()) + 
                                          pj->GetPressure() / (pj->GetDensity() * pj->GetDensity())) * gradientW;

                // ----- VISCOSITY FORCE -----
                // W_ij = ∇^2W(r, h) = (1/(h^d)) * f(q)
                float laplacianW = normalisationFactor * KernelLaplacian(r, smoothingRadius);
                // F_viscosity = μ * m * (v_j - v_i)/ρ_j * ∇²W
                viscosityForce += viscosity * mass * (pj->GetVelocity() - pi->GetVelocity()) / pj->GetDensity() * laplacianW;
            }
        }

        // sum of all forces: navier-stokes equation in lagrangian form
        // dv/dt = -∇p/ρ + μ∇²v/ρ + g
        glm::vec3 totalForce = pressureForce + viscosityForce + gravityForce;

        pi->ApplyForce(totalForce);
    }
}

void ParticleSystem::Integrate(float dt)
{
    for (Particle* p : particles)
    {
        p->Integrate(dt);
    }
}

void ParticleSystem::HandleBoundaryConditions(float dt)
{
    // ----- LENNARD-JONES DISTANCE-BASED PENALTY FORCE -----
    // from "SPH particle boundary forces for arbitrary boundaries" by Monaghan and Kajtar 2009
    // lennard-jones penalty force parameters
    // F_LJ(r) = ε * [(12σ¹²)/(r¹³) - (6σ⁶)/(r⁷)] * r̂
    
    // ε = strength of repulsion force
    float epsilon = 100.0f;
    // σ = characteristic interaction distance
    float sigma = smoothingRadius * 0.5f;
    // d_max = cutoff distance beyond which force is ignored
    float d_max = 2.5f * sigma;
    // r = distance from boundary
    // r̂ = normalised direction vector pointing away from boundary
    
    // from section 4 of "SPH Fluids in Computer Graphics" by Ihmsen et al. 2014
    // "[Mon94, Mon05, MK09] compute distance-based penalty forces, e.g., Lennard-Jones forces which scale polynomially with the distance to the fluid particle."
    // "Generally, these methods require small integration time steps to pro- duce smooth pressure distributions."
    for (Particle* p : particles)
    {
        glm::vec3 position = p->GetPosition();
        glm::vec3 velocity = p->GetVelocity();
        glm::vec3 force = glm::vec3(0.0f);

        // x-min boundary
        if (position.x < boxMin.x)
        {   
            // r = distance from boundary
            float r = boxMin.x - position.x;

            // particle is inside the boundary
            if (r < 0.0f)
            {
                r = -r;
            }

            // apply force only within interaction range
            if (r < d_max)
            {
                // ε * ((12σ¹²)/(r¹³))
                float repulsionForce = epsilon * (12.0f * pow(sigma, 12.0f) / (pow(r, 13.0f)));
                // ε * ((6σ⁶)/(r⁷))
                float attractionForce = epsilon * (6.0f * pow(sigma, 6.0f) / (pow(r, 7.0f)));
                // ε * [(12σ¹²)/(r¹³) - (6σ⁶)/(r⁷)]
                float totalForce = repulsionForce - attractionForce;
                // r̂ points in +x direction
                glm::vec3 r_hat = glm::vec3(1.0f, 0.0f, 0.0f);
                // F_LJ = ε * [(12σ¹²)/(r¹³) - (6σ⁶)/(r⁷)] * r̂
                glm::vec3 F_LJ = totalForce * r_hat;
                
                force += F_LJ;
            }
        }

        // x-max boundary
        if (position.x > boxMax.x)
        {
            // r = distance from boundary
            float r = position.x - boxMax.x;

            // particle is inside the boundary
            if (r < 0.0f)
            {
                r = -r;
            }

            // apply force only within interaction range
            if (r < d_max)
            {
                // ε * ((12σ¹²)/(r¹³))
                float repulsionForce = epsilon * (12.0f * pow(sigma, 12.0f) / (pow(r, 13.0f)));
                // ε * ((6σ⁶)/(r⁷))
                float attractionForce = epsilon * (6.0f * pow(sigma, 6.0f) / (pow(r, 7.0f)));
                // ε * [(12σ¹²)/(r¹³) - (6σ⁶)/(r⁷)]
                float totalForce = repulsionForce - attractionForce;
                // r̂ points in -x direction
                glm::vec3 r_hat = glm::vec3(-1.0f, 0.0f, 0.0f);
                // F_LJ = ε * [(12σ¹²)/(r¹³) - (6σ⁶)/(r⁷)] * r̂
                glm::vec3 F_LJ = totalForce * r_hat;

                force += F_LJ;
            }
        }

        // y-min boundary
        if (position.y < boxMin.y)
        {   
            // r = distance from boundary
            float r = boxMin.y - position.y;

            // particle is inside the boundary
            if (r < 0.0f)
            {
                r = -r;
            }

            // apply force only within interaction range
            if (r < d_max)
            {
                // ε * ((12σ¹²)/(r¹³))
                float repulsionForce = epsilon * (12.0f * pow(sigma, 12.0f) / (pow(r, 13.0f)));
                // ε * ((6σ⁶)/(r⁷))
                float attractionForce = epsilon * (6.0f * pow(sigma, 6.0f) / (pow(r, 7.0f)));
                // ε * [(12σ¹²)/(r¹³) - (6σ⁶)/(r⁷)]
                float totalForce = repulsionForce - attractionForce;
                // r̂ points in +y direction
                glm::vec3 r_hat = glm::vec3(0.0f, 1.0f, 0.0f);
                // F_LJ = ε * [(12σ¹²)/(r¹³) - (6σ⁶)/(r⁷)] * r̂
                glm::vec3 F_LJ = totalForce * r_hat;
                
                force += F_LJ;
            }
        }

        // y-max boundary
        if (position.y > boxMax.y)
        {
            // r = distance from boundary
            float r = position.y - boxMax.y;

            // particle is inside the boundary
            if (r < 0.0f)
            {
                r = -r;
            }

            // apply force only within interaction range
            if (r < d_max)
            {
                // ε * ((12σ¹²)/(r¹³))
                float repulsionForce = epsilon * (12.0f * pow(sigma, 12.0f) / (pow(r, 13.0f)));
                // ε * ((6σ⁶)/(r⁷))
                float attractionForce = epsilon * (6.0f * pow(sigma, 6.0f) / (pow(r, 7.0f)));
                // ε * [(12σ¹²)/(r¹³) - (6σ⁶)/(r⁷)]
                float totalForce = repulsionForce - attractionForce;
                // r̂ points in -y direction
                glm::vec3 r_hat = glm::vec3(0.0f, -1.0f, 0.0f);
                // F_LJ = ε * [(12σ¹²)/(r¹³) - (6σ⁶)/(r⁷)] * r̂
                glm::vec3 F_LJ = totalForce * r_hat;

                force += F_LJ;
            }
        }

        // z-min boundary
        if (position.z < boxMin.z)
        {   
            // r = distance from boundary
            float r = boxMin.z - position.z;

            // particle is inside the boundary
            if (r < 0.0f)
            {
                r = -r;
            }

            // apply force only within interaction range
            if (r < d_max)
            {
                // ε * ((12σ¹²)/(r¹³))
                float repulsionForce = epsilon * (12.0f * pow(sigma, 12.0f) / (pow(r, 13.0f)));
                // ε * ((6σ⁶)/(r⁷))
                float attractionForce = epsilon * (6.0f * pow(sigma, 6.0f) / (pow(r, 7.0f)));
                // ε * [(12σ¹²)/(r¹³) - (6σ⁶)/(r⁷)]
                float totalForce = repulsionForce - attractionForce;
                // r̂ points in +z direction
                glm::vec3 r_hat = glm::vec3(0.0f, 0.0f, 1.0f);
                // F_LJ = ε * [(12σ¹²)/(r¹³) - (6σ⁶)/(r⁷)] * r̂
                glm::vec3 F_LJ = totalForce * r_hat;
                
                force += F_LJ;
            }
        }

        // z-max boundary
        if (position.z > boxMax.z)
        {
            // r = distance from boundary
            float r = position.z - boxMax.z;

            // particle is inside the boundary
            if (r < 0.0f)
            {
                r = -r;
            }

            // apply force only within interaction range
            if (r < d_max)
            {
                // ε * ((12σ¹²)/(r¹³))
                float repulsionForce = epsilon * (12.0f * pow(sigma, 12.0f) / (pow(r, 13.0f)));
                // ε * ((6σ⁶)/(r⁷))
                float attractionForce = epsilon * (6.0f * pow(sigma, 6.0f) / (pow(r, 7.0f)));
                // ε * [(12σ¹²)/(r¹³) - (6σ⁶)/(r⁷)]
                float totalForce = repulsionForce - attractionForce;
                // r̂ points in -z direction
                glm::vec3 r_hat = glm::vec3(0.0f, 0.0f, -1.0f);
                // F_LJ = ε * [(12σ¹²)/(r¹³) - (6σ⁶)/(r⁷)] * r̂
                glm::vec3 F_LJ = totalForce * r_hat;

                force += F_LJ;
            }
        }

        p->ApplyForce(force);
        
        // enforce hard boundaries as fallback to prevent particles from escaping
        EnforceHardBoundaries(p);
    }
}

void ParticleSystem::EnforceHardBoundaries(Particle* p)
{
    // ----- HARD BOUNDARY WITH VELOCITY DAMPING -----
    // as a fallback, still enforce hard boundaries to prevent particles from escaping
    // "In order to overcome the issues of penalty-based methods and to have more control on the boundary condition, direct forcing has been proposed in [BTT09]"
    glm::vec3 position = p->GetPosition();
    glm::vec3 velocity = p->GetVelocity();
    bool collided = false;

    // x-boundary
    if (position.x < boxMin.x)
    {
        // adjust position to boundary
        position.x = boxMin.x;
        // reverse velocity with damping
        velocity.x = -velocity.x * boundaryDamping;
        collided = true;
    }
    else if (position.x > boxMax.x)
    {
        position.x = boxMax.x;
        velocity.x = -velocity.x * boundaryDamping;
        collided = true;
    }

    // y-boundary
    if (position.y < boxMin.y)
    {
        position.y = boxMin.y;
        velocity.y = -velocity.y * boundaryDamping;
        collided = true;
    }
    else if (position.y > boxMax.y)
    {
        position.y = boxMax.y;
        velocity.y = -velocity.y * boundaryDamping;
        collided = true;
    }

    // z-boundary
    if (position.z < boxMin.z)
    {
        position.z = boxMin.z;
        velocity.z = -velocity.z * boundaryDamping;
        collided = true;
    }
    else if (position.z > boxMax.z)
    {
        position.z = boxMax.z;
        velocity.z = -velocity.z * boundaryDamping;
        collided = true;
    }

    // update particle position and velocity if collided
    if (collided)
    {
        p->SetPosition(position);
        p->SetVelocity(velocity);
    }
}

void ParticleSystem::Reset()
{

}

float ParticleSystem::KernelFunction(float r, float h)
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

glm::vec3 ParticleSystem::KernelGradient(glm::vec3 r, float h)
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

float ParticleSystem::KernelLaplacian(float r, float h)
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