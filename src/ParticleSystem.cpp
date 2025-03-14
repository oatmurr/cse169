#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(int size, float dt, glm::vec3 color, float smoothingRadius, float mass, float restDensity, float viscosity, float gasConstant, glm::vec3 gravity, float boundaryStiffness, float boundaryDamping, glm::vec3 boxMin, glm::vec3 boxMax)
{
    this->size = size;
    this->particles = std::vector<Particle*>(size);

    this->dt = dt;

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
    float particleSpacing = smoothingRadius;
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

        // random position
        position.x += ((float)rand() / RAND_MAX * 0.02f - 0.01f) * particleSpacing;
        position.y += ((float)rand() / RAND_MAX * 0.02f - 0.01f) * particleSpacing;
        position.z += ((float)rand() / RAND_MAX * 0.02f - 0.01f) * particleSpacing;

        // random velocity
        glm::vec3 randomVelocity
        (
            (float)rand() / RAND_MAX * 0.1f - 0.05f,
            (float)rand() / RAND_MAX * 0.1f - 0.05f,
            (float)rand() / RAND_MAX * 0.1f - 0.05f
        );

        particles[i] = new Particle(position, mass, false);
        particles[i]->SetVelocity(randomVelocity);
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    boxVAO = 0;
    boxVBO = 0;
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

    if (boxVAO != 0)
    {
        glDeleteVertexArrays(1, &boxVAO);
        glDeleteBuffers(1, &boxVBO);
    }
}

void ParticleSystem::Draw(const glm::mat4& viewProjMtx, GLuint shader)
{
    // create buffer of positions
    std::vector<glm::vec3> positions(size);
    for (int i = 0; i < size; i++)
    {
        positions[i] = particles[i]->GetPosition();
    }

    // std::cout << "drawing " << positions.size() << " particles" << std::endl;
    // for (int i = 0; i < 5 && i < positions.size(); i++)
    // {
    //     std::cout << "particle " << i << " position: ("
    //             << positions[i].x << ", " << positions[i].y << ", " << positions[i].z << ")" << std::endl;
    // }

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

    // draw points
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, positions.size());
    glBindVertexArray(0);

    // GLenum err = glGetError();
    // if (err != GL_NO_ERROR)
    // {
    //     std::cout << "OpenGL error after drawing: " << err << std::endl;
    // }

    // Unbind the VAO and shader program
    glBindVertexArray(0);
    glUseProgram(0);
}

void ParticleSystem::DrawBoundaries(const glm::mat4& viewProjMtx, GLuint shader)
{
    if (boxVAO == 0)
    {
        SetupBoxBuffers();
    }

    glUseProgram(shader);

    glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&viewProjMtx);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);

    glUniform3f(glGetUniformLocation(shader, "DiffuseColor"), 1.0f, 0.0f, 0.0f);

    // draw box wireframe
    glBindVertexArray(boxVAO);
    glDrawArrays(GL_LINES, 0, 24);
    glBindVertexArray(0);

    glUseProgram(0);
}

void ParticleSystem::Update()
{   
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
        // float pressure = gasConstant * (pow(density / restDensity, 5.0f) - 1.0f);
        float pressure = gasConstant * (pow(density / restDensity, 5.0f));
        // float pressure = gasConstant * (density - restDensity);

        pi->SetPressure(pressure);

        // ensure pressure is not negative
        // if (pi->GetPressure() < 0.0f)
        // {
        //     // std::cout << "ParticleSystem::computeDensityPressure - negative pressure" << std::endl;
        //     pi->SetPressure(0.0f);
        // }

        // std::cout << "particle " << i << " density: " << density 
        //           << " (rest: " << restDensity << "), " << "pressure: " << pressure << std::endl;
    }
}
void ParticleSystem::ComputeForces()
{
    // for debug
    float maxPressureX = 0.0f, maxPressureY = 0.0f, maxPressureZ = 0.0f;
    float maxViscosityX = 0.0f, maxViscosityY = 0.0f, maxViscosityZ = 0.0f;
    float maxTotalX = 0.0f, maxTotalY = 0.0f, maxTotalZ = 0.0f;
    
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

        // for debug
        maxPressureX = glm::max(maxPressureX, glm::abs(pressureForce.x));
        maxPressureY = glm::max(maxPressureY, glm::abs(pressureForce.y));
        maxPressureZ = glm::max(maxPressureZ, glm::abs(pressureForce.z));

        maxViscosityX = glm::max(maxViscosityX, glm::abs(viscosityForce.x));
        maxViscosityY = glm::max(maxViscosityY, glm::abs(viscosityForce.y));
        maxViscosityZ = glm::max(maxViscosityZ, glm::abs(viscosityForce.z));

        // sum of all forces: navier-stokes equation in lagrangian form
        // dv/dt = -∇p/ρ + μ∇²v/ρ + g
        glm::vec3 totalForce = pressureForce + viscosityForce + gravityForce;

        // for debug
        maxTotalX = glm::max(maxTotalX, glm::abs(totalForce.x));
        maxTotalY = glm::max(maxTotalY, glm::abs(totalForce.y));
        maxTotalZ = glm::max(maxTotalZ, glm::abs(totalForce.z));

        // print debug
        // std::cout << "===== SAMPLE PARTICLE FORCES =====" << std::endl;
        // std::cout << "Pressure Force: (" << pressureForce.x << ", " 
        //             << pressureForce.y << ", " << pressureForce.z << ")" << std::endl;
        // std::cout << "Viscosity Force: (" << viscosityForce.x << ", " 
        //             << viscosityForce.y << ", " << viscosityForce.z << ")" << std::endl;
        // std::cout << "Gravity Force: (" << gravityForce.x << ", " 
        //             << gravityForce.y << ", " << gravityForce.z << ")" << std::endl;
        // std::cout << "Total Force: (" << totalForce.x << ", " 
        //             << totalForce.y << ", " << totalForce.z << ")" << std::endl;
        // std::cout << "Position: (" << pi->GetPosition().x << ", " 
        //             << pi->GetPosition().y << ", " << pi->GetPosition().z << ")" << std::endl;
        // std::cout << "Velocity: (" << pi->GetVelocity().x << ", " 
        //             << pi->GetVelocity().y << ", " << pi->GetVelocity().z << ")" << std::endl;
        // std::cout << "=================================" << std::endl;

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
    // float epsilon = 100.0f;
    // σ = characteristic interaction distance
    // float sigma = smoothingRadius * 0.5f;
    // d_max = cutoff distance beyond which force is ignored
    // float d_max = 2.5f * sigma;
    // r = distance from boundary
    // r̂ = normalised direction vector pointing away from boundary
    
    // from section 4 of "SPH Fluids in Computer Graphics" by Ihmsen et al. 2014
    // "[Mon94, Mon05, MK09] compute distance-based penalty forces, e.g., Lennard-Jones forces which scale polynomially with the distance to the fluid particle."
    // "Generally, these methods require small integration time steps to pro- duce smooth pressure distributions."
    // for (Particle* p : particles)
    // {
    //     glm::vec3 position = p->GetPosition();
    //     glm::vec3 force = glm::vec3(0.0f);

    //     // x-min boundary
    //     force += CalculateLennardJonesForce(position, boxMin.x, 0, true, epsilon, sigma, d_max);
    //     // x-max boundary
    //     force += CalculateLennardJonesForce(position, boxMax.x, 0, false, epsilon, sigma, d_max);
    //     // y-min boundary
    //     force += CalculateLennardJonesForce(position, boxMin.y, 1, true, epsilon, sigma, d_max);
    //     // y-max boundary
    //     force += CalculateLennardJonesForce(position, boxMax.y, 1, false, epsilon, sigma, d_max);
    //     // z-min boundary
    //     force += CalculateLennardJonesForce(position, boxMin.z, 2, true, epsilon, sigma, d_max);
    //     // z-max boundary
    //     force += CalculateLennardJonesForce(position, boxMax.z, 2, false, epsilon, sigma, d_max);

    //     p->ApplyForce(force);
    // }

    // ----- HARD BOUNDARY WITH VELOCITY DAMPING -----
    // as a fallback, still enforce hard boundaries to prevent particles from escaping
    // "In order to overcome the issues of penalty-based methods and to have more control on the boundary condition, direct forcing has been proposed in [BTT09]"
    for (Particle* p : particles)
    {
        glm::vec3 boundaryForce = glm::vec3(0.0f);

        if (p->GetPosition().x < boxMin.x + 0.1f)
        {
            boundaryForce.x += boundaryStiffness * (boxMin.x + 0.1 - p->GetPosition().x) * boundaryDamping;
        }
        if (p->GetPosition().x > boxMax.x - 0.1f)
        {
            boundaryForce.x += boundaryStiffness * (boxMax.x - 0.1 - p->GetPosition().x) * boundaryDamping;
        }
        if (p->GetPosition().y < boxMin.y + 0.1f)
        {
            boundaryForce.y += boundaryStiffness * (boxMin.y + 0.1 - p->GetPosition().y) * boundaryDamping;
        }
        if (p->GetPosition().y > boxMax.y - 0.1f)
        {
            boundaryForce.y += boundaryStiffness * (boxMax.y - 0.1 - p->GetPosition().y) * boundaryDamping;
        }
        if (p->GetPosition().z < boxMin.z + 0.1f)
        {
            boundaryForce.z += boundaryStiffness * (boxMin.z + 0.1 - p->GetPosition().z) * boundaryDamping;
        }
        if (p->GetPosition().z > boxMax.z - 0.1f)
        {
            boundaryForce.z += boundaryStiffness * (boxMax.z - 0.1 - p->GetPosition().z) * boundaryDamping;
        }


        // if (p->GetPosition().z > boxMax.z)
        // {
        //     boundaryForce.z += boundaryStiffness * (boxMax.z - p->GetPosition().z) * boundaryDamping;
        // }

        // if (p->GetPosition().x < boxMin.x)
        // {
        //     boundaryForce.x += boundaryStiffness * (boxMin.x - p->GetPosition().x);
        // }

        // if (p->GetPosition().x > boxMax.x)
        // {
        //     boundaryForce.x += boundaryStiffness * (boxMax.x - p->GetPosition().x);
        // }

        // if (p->GetPosition().y < boxMin.y)
        // {
        //     boundaryForce.y += boundaryStiffness * (boxMin.y - p->GetPosition().y);
        // }

        // if (p->GetPosition().y > boxMax.y)
        // {
        //     boundaryForce.y += boundaryStiffness * (boxMax.y - p->GetPosition().y);
        // }

        // if (p->GetPosition().z < boxMin.z)
        // {
        //     boundaryForce.z += boundaryStiffness * (boxMin.z - p->GetPosition().z);
        // }

        // if (p->GetPosition().z > boxMax.z)
        // {
        //     boundaryForce.z += boundaryStiffness * (boxMax.z - p->GetPosition().z);
        // }

        p->ApplyForce(boundaryForce);

        EnforceHardBoundaries(p);
    }
}

void ParticleSystem::EnforceHardBoundaries(Particle* p)
{
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

glm::vec3 ParticleSystem::CalculateLennardJonesForce(glm::vec3 position, float boundary, int axis, bool isMin, float epsilon, float sigma, float d_max)
{
    glm::vec3 F_LJ = glm::vec3(0.0f);

    // calculate distance to boundary (min/max set by isMin)
    float distance;
    if (isMin)
    {
        // distance to min boundary
        distance = position[axis] - boundary;
    }
    else
    {
        // distance to max boundary
        distance = boundary - position[axis];
    }

    // apply force only within interaction range
    if (fabs(distance) < d_max)
    {
        // r = distance from boundary
        float r = fabs(distance);
        
        // ε * ((12σ¹²)/(r¹³))
        float repulsionForce = epsilon * (12.0f * pow(sigma, 12.0f) / (pow(r, 13.0f)));
        // ε * ((6σ⁶)/(r⁷))
        float attractionForce = epsilon * (6.0f * pow(sigma, 6.0f) / (pow(r, 7.0f)));
        // ε * [(12σ¹²)/(r¹³) - (6σ⁶)/(r⁷)]
        float totalForce = repulsionForce - attractionForce;

        // r̂ = direction vector pointing away from boundary
        glm::vec3 r_hat = glm::vec3(0.0f);
        
        // for minimum boundary
        if (isMin)
        {
            if (r < 0.0f)
            {
                // particle outside minimum boundary, force points inward (positive direction)
                r_hat[axis] = 1.0f;
            }
            else
            {
                // particle inside minimum boundary, force points outward (negative direction)
                r_hat[axis] = -1.0f;
            }
        }
        // for maximum boundary
        else
        {
            if (r < 0.0f)
            {
                // particle outside maximum boundary, force points inward (negative direction)
                r_hat[axis] = -1.0f;
            }
            else
            {
                // particle inside maximum boundary, force points outward (positive direction)
                r_hat[axis] = 1.0f;
            }
        }

        // F_LJ = ε * [(12σ¹²)/(r¹³) - (6σ⁶)/(r⁷)] * r̂
        F_LJ = totalForce * r_hat;
    }

    return F_LJ;
}

void ParticleSystem::SetupBoxBuffers()
{
    // define 8 vertices of box
    glm::vec3 vertices[8] =
    {
        glm::vec3(boxMin.x, boxMin.y, boxMin.z),
        glm::vec3(boxMax.x, boxMin.y, boxMin.z),
        glm::vec3(boxMax.x, boxMax.y, boxMin.z),
        glm::vec3(boxMin.x, boxMax.y, boxMin.z),
        glm::vec3(boxMin.x, boxMin.y, boxMax.z),
        glm::vec3(boxMax.x, boxMin.y, boxMax.z),
        glm::vec3(boxMax.x, boxMax.y, boxMax.z),
        glm::vec3(boxMin.x, boxMax.y, boxMax.z)
    };

    // define 12 edges of box
    std::vector<glm::vec3> edges =
    {
        vertices[0], vertices[1],
        vertices[1], vertices[2],
        vertices[2], vertices[3],
        vertices[3], vertices[0],
        vertices[4], vertices[5],
        vertices[5], vertices[6],
        vertices[6], vertices[7],
        vertices[7], vertices[4],
        vertices[0], vertices[4],
        vertices[1], vertices[5],
        vertices[2], vertices[6],
        vertices[3], vertices[7]
    };

    // create and setup buffers
    glGenVertexArrays(1, &boxVAO);
    glGenBuffers(1, &boxVBO);

    glBindVertexArray(boxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
    glBufferData(GL_ARRAY_BUFFER, edges.size() * sizeof(glm::vec3), edges.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ParticleSystem::Reset()
{
    // blob parameters
    float particleSpacing = smoothingRadius;
    int blobSize = (int)ceil(cbrt(size));
    float blobLength = blobSize * particleSpacing;

    // blob position
    float blobCenterX = (boxMax.x - boxMin.x) * 0.5f + boxMin.x;
    float blobCenterY = (boxMax.y - boxMin.y) * 0.5f + boxMin.y;
    float blobCenterZ = (boxMax.z - boxMin.z) * 0.5f + boxMin.z;

    // reset blob
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

        particles[i]->SetPosition(position);
        particles[i]->SetVelocity(glm::vec3(0.0f));
        particles[i]->SetForce(glm::vec3(0.0f));
    }
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
    float r_length = glm::length(r);

    if (r_length < 0.0001)
    {
        return glm::vec3(0.0f);
    }
    
    float q = r_length / h;

    float gradientMagnitude = 0.0f;

    float coefficient = 3.0f / (2.0f * M_PI);

    // (3/(2π)) * ((-2q + (3/2) * q^2)) for 0 ≤ q < 1
    if (q < 1.0f)
    {
        gradientMagnitude = coefficient * (-2.0f * q + 1.5f * q * q);
    }
    // (3/(2π)) * ((-1/2) * (2 - q)^2) for 1 ≤ q < 2
    else if (q < 2.0f)
    {
        gradientMagnitude = coefficient * (-0.5f) * std::pow(2.0f - q, 2.0f);
    }

    glm::vec3 direction = r / r_length;

    return gradientMagnitude * direction;
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
    // (3/(2π)) * (2 - q) for 1 ≤ q < 2
    else if (q < 2.0f)
    {
        laplacianValue = coefficient * (2.0f - q);
    }

    return laplacianValue;
}