#include "Cloth.h"

Cloth::Cloth(int width, int height, float particleSpacing, float mass, float springConstant, float dampingConstant) {
    
    // initialise wind with zero velocity, can be set later by ui
    wind = glm::vec3(0.0f);

    // initialise gravity
    gravity = glm::vec3(0.0f, -9.81f, 0.0f);

    // create grid of particles
    for (int y = 0; y < width; y++) {
        for (int x = 0; x < height; x++) {
            
            // position particles in grid
            glm::vec3 position = glm::vec3(x * particleSpacing, 0.0f, y * particleSpacing);

            // create and store particles (fix top row of particles)
            if (y == 0) {
                Particle* particle = new Particle(position, mass, true);
                particles.push_back(particle);
            } else {
                Particle* particle = new Particle(position, mass, false);
                particles.push_back(particle);
            }
        }
    }

    // create springs between particles
    for (int y = 0; y < width; y++) {
        for (int x = 0; x < height; x++) {

            // particle indiex
            int i = y * width + x;

            // p1 --- p2
            // |      |
            // |      |
            // p3 --- p4

            // horizontal spring: p1 --- p2
            if (x < width - 1) {
                SpringDamper* p1p2 = new SpringDamper(particles[i], particles[i + 1], springConstant, dampingConstant);
                springs.push_back(p1p2);
            }

            // vertical spring: p1 --- p3
            if (y < height - 1) {
                SpringDamper* p1p3 = new SpringDamper(particles[i], particles[i + width], springConstant, dampingConstant);
                springs.push_back(p1p3);
            }

            // diagonal springs: p1 --- p4, p2 --- p3
            if (x < width - 1 && y < height - 1) {
                
                // p1 --- p4
                SpringDamper* p1p4 = new SpringDamper(particles[i], particles[i + width + 1], springConstant, dampingConstant);
                springs.push_back(p1p4);

                // p2 --- p3
                SpringDamper* p2p3 = new SpringDamper(particles[i + 1], particles[i + width], springConstant, dampingConstant);
                springs.push_back(p2p3);
            }
        }
    }

    // create triangles: (p1, p2, p3) and (p2, p3, p4)
    for (int y = 0; y < height - 1; y++) {
        for (int x = 0; x < width - 1; x++) {

            // particle index
            int i = y * width + x;

            // p1 --- p2
            // |      |
            // |      |
            // p3 --- p4

            // p1, p2, p3
            ClothTriangle* p1p2p3 = new ClothTriangle(particles[i], particles[i + 1], particles[i + width]);
            triangles.push_back(p1p2p3);

            // p2, p3, p4
            ClothTriangle* p2p3p4 = new ClothTriangle(particles[i + 1], particles[i + width], particles[i + width + 1]);
            triangles.push_back(p2p3p4);
        }
    }

    // initialise rendering data
    model = glm::mat4(1.0f);
    color = glm::vec3(0.7f, 0.7f, 0.9f);

    SetupBuffers();
}

Cloth::~Cloth() {
    for (Particle* particle : particles) {
        delete particle;
    }
    for (SpringDamper* spring : springs) {
        delete spring;
    }
    for (ClothTriangle* triangle : triangles) {
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
}

void Cloth::SetWind(glm::vec3 wind) {
    this->wind = wind;
}

void Cloth::Simulate(float dt) {
    
    // apply gravity: F = m * g
    for (Particle* particle : particles) {
        particle->ApplyForce(gravity * particle->GetMass());
    }

    // compute and apply spring-damper forces
    for (SpringDamper* spring : springs) {
        spring->ComputeForce();
    }

    // compute and apply aerodynamic forces using wind
    for (ClothTriangle* triangle : triangles) {
        triangle->ComputeAerodynamicForce(wind);
    }

    // update particle positions by integrating forces
    for (Particle* particle : particles) {
        particle->Integrate(dt);
    }

    // after simulation is done, update buffers
    UpdateBuffers();

        // Print positions of a few corner particles to check where the cloth is
        if (!particles.empty()) {
            std::cout << "First particle position: "
                      << particles[0]->GetPosition().x << ", "
                      << particles[0]->GetPosition().y << ", "
                      << particles[0]->GetPosition().z << std::endl;
                      
            int lastIdx = particles.size() - 1;
            std::cout << "Last particle position: "
                      << particles[lastIdx]->GetPosition().x << ", "
                      << particles[lastIdx]->GetPosition().y << ", "
                      << particles[lastIdx]->GetPosition().z << std::endl;
        }
}

void Cloth::Draw(glm::mat4 viewProjMtx, GLuint shader) {

    glUseProgram(shader);
    
    glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&viewProjMtx);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);
    
    // triangles
    glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, triangleIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    // particles
    glm::vec3 particleColor = glm::vec3(0.0f, 1.0f, 0.0f);
    glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &particleColor[0]);
    
    // particles as points
    glPointSize(8.0f);
    
    // setup point rendering
    GLuint pointVAO, pointVBO;
    glGenVertexArrays(1, &pointVAO);
    glGenBuffers(1, &pointVBO);
    
    glBindVertexArray(pointVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertexPositions.size(), vertexPositions.data(), GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    // draw points
    glDrawArrays(GL_POINTS, 0, vertexPositions.size());
    
    // clean up point VAO/VBO
    glDeleteBuffers(1, &pointVBO);
    glDeleteVertexArrays(1, &pointVAO);
    
    // springs
    glm::vec3 springColor = glm::vec3(0.0f, 0.5f, 1.0f);
    glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &springColor[0]);
    
    // setup line rendering for springs
    std::vector<glm::vec3> springLines;
    for (auto spring : springs) {
        springLines.push_back(spring->GetP1()->GetPosition());
        springLines.push_back(spring->GetP2()->GetPosition());
    }
    
    // create VAO, VBO for springs
    GLuint lineVAO, lineVBO;
    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);
    
    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * springLines.size(), springLines.data(), GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    // draw lines for springs
    glLineWidth(1.5f); // Make lines a bit thicker
    glDrawArrays(GL_LINES, 0, springLines.size());
    
    // clean up line VAO/VBO
    glDeleteBuffers(1, &lineVBO);
    glDeleteVertexArrays(1, &lineVAO);
    
    // reset point size and line width
    glPointSize(1.0f);
    glLineWidth(1.0f);
    
    // unbind shader
    glUseProgram(0);
}
void Cloth::SetupBuffers() {

    // clear old data
    vertexPositions.clear();
    vertexNormals.clear();
    triangleIndices.clear();

    // for each particle, store position and initialise normal to zero
    for (Particle* particle : particles) {
        vertexPositions.push_back(particle->GetPosition());
        vertexNormals.push_back(glm::vec3(0.0f));
    }

    // for each triangle, store indices of vertices
    for (ClothTriangle* triangle : triangles) {
        
        // get the three particles for this triangle
        Particle* a = triangle->GetP1();
        Particle* b = triangle->GetP2();
        Particle* c = triangle->GetP3();

        // find index of a
        int indexA = -1;
        for (int i = 0; i < particles.size(); i++) {
            if (particles[i] == a) {
                indexA = i;
                break;
            }
        }

        // find index of b
        int indexB = -1;
        for (int i = 0; i < particles.size(); i++) {
            if (particles[i] == b) {
                indexB = i;
                break;
            }
        }

        // find index of c
        int indexC = -1;
        for (int i = 0; i < particles.size(); i++) {
            if (particles[i] == c) {
                indexC = i;
                break;
            }
        }

        // make sure all three particles were found and store indices in triangleIndices
        if (indexA != -1 && indexB != -1 && indexC != -1) {
            triangleIndices.push_back(indexA);
            triangleIndices.push_back(indexB);
            triangleIndices.push_back(indexC);
        }
    }

    // generate VAO and VBOs
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

    // unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    std::cout << "Set up " << triangleIndices.size() << " indices for drawing" << std::endl;
}

void Cloth::UpdateBuffers() {

    // update positions
    for (int i = 0; i < particles.size(); i++) {
        vertexPositions[i] = particles[i]->GetPosition();
    }

    // reset normals
    for (int i = 0; i < vertexNormals.size(); i++) {
        vertexNormals[i] = glm::vec3(0.0f);
    }

    // calculate normals by averaging triangle contributions
    for (ClothTriangle* triangle : triangles) {

        // get the three particles for this triangle
        Particle* a = triangle->GetP1();
        Particle* b = triangle->GetP2();
        Particle* c = triangle->GetP3();

        // find index of a
        int indexA = -1;
        for (int i = 0; i < particles.size(); i++) {
            if (particles[i] == a) {
                indexA = i;
                break;
            }
        }

        // find index of b
        int indexB = -1;
        for (int i = 0; i < particles.size(); i++) {
            if (particles[i] == b) {
                indexB = i;
                break;
            }
        }

        // find index of c
        int indexC = -1;
        for (int i = 0; i < particles.size(); i++) {
            if (particles[i] == c) {
                indexC = i;
                break;
            }
        }

        if (indexA != -1 && indexB != -1 && indexC != -1) {

            // calculate face normal
            glm::vec3 normal = triangle->ComputeNormal();

            // add to vertex normals
            vertexNormals[indexA] += normal;
            vertexNormals[indexB] += normal;
            vertexNormals[indexC] += normal;
        }
    }

    // normalise vertex normals
    for (int i = 0; i < vertexNormals.size(); i++) {
        vertexNormals[i] = glm::normalize(vertexNormals[i]);
    }

    // update VBOs
    glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * vertexPositions.size(), vertexPositions.data());

    glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * vertexNormals.size(), vertexNormals.data());

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
