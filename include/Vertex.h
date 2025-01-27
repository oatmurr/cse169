#include "core.h"

class Vertex {
private:    
    // Rendering data (position, normal, color, texture coordinates, tangents...)
    
    // (x, y, z)
    glm::vec3 position;

    // (x, y, z)
    glm::vec3 normal;

    // (r, g, b)
    glm::vec3 color;

    // (u, v)
    glm::vec2 textureCoordinates;

    // (x, y, z)
    glm::vec3 tangent;

    // (x, y, z)
    glm::vec3 bitangent;

    // note: normal, tangent, bitangent are orthogonal to each other
    // https://i.sstatic.net/tAQNN.png

    // Skinning data
    
    // number of attachments (max 4)
    int numAttachments;

    // joint indices (max 4), one byte each, limited to 256 joints
    unsigned char jointIndices[4];

    // weights (max 4), one byte each, precision of 1/256 = ~0.004%
    unsigned char weights[4];
    
    // with skinningData[7]; fourth weight is stored as 1.0 - (weight[0] - weight[1] - weight[2])
    // unsigned char weights[3];

    // jointIndices[4] + weights[4] = 8 bytes
    unsigned char skinningData[8];

    // with weights[3]
    // unsigned char skinningData[7];
    
public:
    // constructors/destructors
    Vertex();
    ~Vertex();

    // functions
    
};