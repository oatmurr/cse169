#include "Cloth.h"

Cloth::Cloth(int width, int height, float particleSpacing, float mass, float springConstant, float dampingConstant)
{
    // initialise wind with zero velocity, can be set later by ui
    wind = glm::vec3(0.0f);

    // initialise gravity
    gravity = glm::vec3(0.0f, -9.81f, 0.0f);

    // initial height of cloth
    float initialHeight = 2.0f;

    // create grid of particles
    for (int y = 0; y < width; y++)
    {
        for (int x = 0; x < height; x++)
        {
            // position particles in grid
            glm::vec3 position = glm::vec3(x * particleSpacing, initialHeight, y * particleSpacing);

            // create and store particles (fix top row of particles)
            if (y == 0)
            {
                Particle* particle = new Particle(position, mass, true);
                particles.push_back(particle);
            }
            else
            {
                Particle* particle = new Particle(position, mass, false);
                particles.push_back(particle);
            }
        }
    }

    // create structural springs between particles
    for (int y = 0; y < width; y++)
    {
        for (int x = 0; x < height; x++)
        {
            // particle indiex
            int i = y * width + x;

            // p1 --- p2
            // |      |
            // |      |
            // p3 --- p4

            // horizontal spring: p1 --- p2
            if (x < width - 1)
            {
                SpringDamper* p1p2 = new SpringDamper(particles[i], particles[i + 1], springConstant, dampingConstant, particleSpacing);
                springs.push_back(p1p2);
            }

            // vertical spring: p1 --- p3
            if (y < height - 1)
            {
                SpringDamper* p1p3 = new SpringDamper(particles[i], particles[i + width], springConstant, dampingConstant, particleSpacing);
                springs.push_back(p1p3);
            }

            // diagonal springs: p1 --- p4, p2 --- p3
            if (x < width - 1 && y < height - 1)
            {    
                // p1 --- p4
                SpringDamper* p1p4 = new SpringDamper(particles[i], particles[i + width + 1], springConstant, dampingConstant, particleSpacing * sqrt(2.0f));
                springs.push_back(p1p4);

                // p2 --- p3
                SpringDamper* p2p3 = new SpringDamper(particles[i + 1], particles[i + width], springConstant, dampingConstant, particleSpacing * sqrt(2.0f));
                springs.push_back(p2p3);
            }
        }
    }

    // create bending springs between particles (skip one particle)
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            // particle index
            int i = y * width + x;

            // p1 --- p2 --- p3
            // |      |      |
            // |      |      |
            // p4 --- p5 --- p6
            // |      |      |
            // |      |      |
            // p7 --- p8 --- p9

            // horizontal bending spring: p1 --- p3
            if (x < width - 2)
            {
                SpringDamper* p1p3 = new SpringDamper(particles[i], particles[i + 2], springConstant * 0.5f, dampingConstant * 1.5f, particleSpacing * 2.0f);
                springs.push_back(p1p3);
            }

            // vertical bending spring: p1 --- p7
            if (y < height - 2)
            {
                SpringDamper* p1p7 = new SpringDamper(particles[i], particles[i + width * 2], springConstant * 0.5f, dampingConstant * 1.5f, particleSpacing * 2.0f);
                springs.push_back(p1p7);
            }

            // diagonal bending springs: p1 --- p9, p3 --- p7
            if (x < width - 2 && y < height - 2)
            {
                // p1 --- p9
                SpringDamper* p1p9 = new SpringDamper(particles[i], particles[i + width * 2 + 2], springConstant * 0.5f, dampingConstant * 1.5f, (particleSpacing * 2.0f) * sqrt(2.0f));
                springs.push_back(p1p9);

                // p3 --- p7
                SpringDamper* p3p7 = new SpringDamper(particles[i + 2], particles[i + width * 2], springConstant * 0.5f, dampingConstant * 1.5f, (particleSpacing * 2.0f) * sqrt(2.0f));
                springs.push_back(p3p7);
            }
        }
    }

    // create triangles: (p1, p2, p3) and (p2, p3, p4)
    for (int y = 0; y < height - 1; y++)
    {
        for (int x = 0; x < width - 1; x++)
        {
            // particle index
            int i = y * width + x;

            // p1 --- p2
            // |      |
            // |      |
            // p3 --- p4

            // indices
            int indexP1 = i;
            int indexP2 = i + 1;
            int indexP3 = i + width;
            int indexP4 = i + width + 1;

            // p1, p2, p3
            ClothTriangle* p1p2p3 = new ClothTriangle(particles[i], particles[i + 1], particles[i + width], indexP1, indexP2, indexP3);
            triangles.push_back(p1p2p3);

            // p2, p3, p4
            ClothTriangle* p2p3p4 = new ClothTriangle(particles[i + 1], particles[i + width], particles[i + width + 1], indexP2, indexP3, indexP4);
            triangles.push_back(p2p3p4);
        }
    }

    // initialise rendering data
    model = glm::mat4(1.0f);
    color = glm::vec3(0.7f, 0.7f, 0.9f);

    SetupBuffers();
}

Cloth::~Cloth()
{
    for (Particle* particle : particles)
    {
        delete particle;
    }
    for (SpringDamper* spring : springs)
    {
        delete spring;
    }
    for (ClothTriangle* triangle : triangles)
    {
        delete triangle;
    }

    particles.clear();
    springs.clear();
    triangles.clear();

    // clean up OpenGL stuff
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO_positions);
    glDeleteBuffers(1, &VBO_normals);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &springVAO);
    glDeleteBuffers(1, &springVBO);
}

void Cloth::SetWind(glm::vec3 wind)
{
    this->wind = wind;
}

void Cloth::Simulate(float dt)
{
    
    // float subdt = dt / 10.0f;

    // for (int i = 0; i < 10; i++) {
    //     // apply gravity
    //     for (Particle* particle : particles)
    // {
    //         particle->ApplyForce(gravity * particle->GetMass());
    //     }
    
    //     // compute and apply spring-damper forces
    //     for (SpringDamper* spring : springs)
    //     {
    //         spring->ComputeForce();
    //     }
    
    //     // compute and apply aerodynamic forces using wind
    //     for (ClothTriangle* triangle : triangles)
    //     {
    //         triangle->ComputeAerodynamicForce(wind);
    //     }
    
    //     // update particle positions by integrating forces
    //     for (Particle* particle : particles)
    //     {
    //         particle->Integrate(subdt);
    //     }
    // }

    // printf("Cloth::Simulate - dt = %f\n", dt);

    // apply gravity: F = m * g
    for (Particle* particle : particles)
    {
        particle->ApplyForce(gravity * particle->GetMass());
    }

    // compute and apply spring-damper forces
    for (SpringDamper* spring : springs)
    {
        spring->ComputeForce();
    }

    // compute and apply aerodynamic forces using wind
    for (ClothTriangle* triangle : triangles)
    {
        triangle->ComputeAerodynamicForce(wind);
    }

    // update particle positions by integrating forces
    for (Particle* particle : particles)
    {
        particle->Integrate(dt);
    }

    // after simulation is done, update buffers
    UpdateBuffers();
}

void Cloth::Draw(glm::mat4 viewProjMtx, GLuint shader)
{
    glUseProgram(shader);
    
    glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&viewProjMtx);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);
    
    // triangles
    // glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);
    // glBindVertexArray(VAO);
    // glDrawElements(GL_TRIANGLES, triangleIndices.size(), GL_UNSIGNED_INT, 0);
    // glBindVertexArray(0);
    
    // springs
    glm::vec3 springColor = glm::vec3(0.0f, 0.5f, 1.0f);
    glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &springColor[0]);
    
    // line rendering for springs
    springLines.clear();
    for (SpringDamper* spring : springs)
    {
        springLines.push_back(spring->GetP1()->GetPosition());
        springLines.push_back(spring->GetP2()->GetPosition());
    }
    
    // spring VAO/VBO
    glBindVertexArray(springVAO);
    glBindBuffer(GL_ARRAY_BUFFER, springVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * springLines.size(), springLines.data(), GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glLineWidth(1.5f);
    glDrawArrays(GL_LINES, 0, springLines.size());
    glBindVertexArray(0);
    
    // reset point size and line width
    glPointSize(1.0f);
    glLineWidth(1.0f);
    
    // unbind shader
    glUseProgram(0);
}
void Cloth::SetupBuffers()
{
    // clear old data
    vertexPositions.clear();
    vertexNormals.clear();
    triangleIndices.clear();

    // for each particle, store position and initialise normal to zero
    for (Particle* particle : particles)
    {
        vertexPositions.push_back(particle->GetPosition());
        vertexNormals.push_back(glm::vec3(0.0f));
    }

    // for each triangle, store indices of particles
    for (ClothTriangle* triangle : triangles)
    {
        int indexP1 = triangle->GetIndexP1();
        int indexP2 = triangle->GetIndexP2();
        int indexP3 = triangle->GetIndexP3();
    }

    // generate VAO and VBOs for triangles
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO_positions);
    glGenBuffers(1, &VBO_normals);
    glGenBuffers(1, &EBO);

    // bind VAO
    glBindVertexArray(VAO);

    // bind VBO_positions
    glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertexPositions.size(), vertexPositions.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    // bind VBO_normals
    glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertexNormals.size(), vertexNormals.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    // bind EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * triangleIndices.size(), triangleIndices.data(), GL_DYNAMIC_DRAW);

    // spring VAO/VBO
    glGenVertexArrays(1, &springVAO);
    glGenBuffers(1, &springVBO);

    // unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Cloth::UpdateBuffers()
{
    // update positions
    for (int i = 0; i < particles.size(); i++)
    {
        vertexPositions[i] = particles[i]->GetPosition();
    }

    // reset normals
    for (int i = 0; i < vertexNormals.size(); i++)
    {
        vertexNormals[i] = glm::vec3(0.0f);
    }

    // calculate normals by averaging triangle contributions
    for (ClothTriangle* triangle : triangles)
    {
        // get triangle particle indices
        int indexP1 = triangle->GetIndexP1();
        int indexP2 = triangle->GetIndexP2();
        int indexP3 = triangle->GetIndexP3();

        // calculate triangle face normal
        glm::vec3 normal = triangle->ComputeNormal();

        // add to vertex normals
        vertexNormals[indexP1] += normal;
        vertexNormals[indexP2] += normal;
        vertexNormals[indexP3] += normal;
    }

    // normalise vertex normals
    for (int i = 0; i < vertexNormals.size(); i++)
    {
        vertexNormals[i] = glm::normalize(vertexNormals[i]);
    }

    // update VBOs
    glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * vertexPositions.size(), vertexPositions.data());

    glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * vertexNormals.size(), vertexNormals.data());

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Cloth::Translate(glm::vec3 translation)
{
    for (Particle* particle : particles)
    {
        if (particle->IsFixed())
        {
            particle->SetPosition(particle->GetPosition() + translation);
        }
    }

    UpdateBuffers();
}