#include "Triangle.h"
#include "Skeleton.h"
#include <vector>

class Skin {
private:
    // raw data from .skin file
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<Triangle> triangles;
    std::vector<glm::mat4> bindings;

    // processed vertex data
    std::vector<Vertex> vertices;
    std::vector<glm::vec3> transformedPositions;
    std::vector<glm::vec3> transformedNormals;

    // skinning data
    Skeleton* skeleton;
    std::vector<glm::mat4> skinningMatrices;
public:
    // constructors/destructors
    Skin();
    ~Skin();

    // functions
    // optional skeleton parameter
    bool Load(const char* filename, Skeleton* skeleton = nullptr);
    void Update();
    void Draw();
};