#include "SpringDamper.h"

class ClothTriangle {
private:
    Particle* p1;
    Particle* p2;
    Particle* p3;
    float drag;

public:
    ClothTriangle(Particle* p1, Particle* p2, Particle* p3, float drag);
    void ComputeAerodynamicForce(glm::vec3 wind);
};
