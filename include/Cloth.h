#include "ClothTriangle.h"
#include <vector>

class Cloth {
private:
    std::vector<Particle*> particles;
    std::vector<SpringDamper*> springs;
    std::vector<ClothTriangle*> triangles;

    glm::vec3 wind;
    glm::vec3 gravity;

public:
    Cloth(int width, int height, float particleSpacing, float mass, float springConstant, float dampingConstant);
    ~Cloth();

    void SetWind(glm::vec3 wind);
    void Simulate(float dt);
};
