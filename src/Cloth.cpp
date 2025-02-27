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
}


