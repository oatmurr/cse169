#include "core.h"
#include <vector>

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

    // Skinning data: jointIndices[4] + weights[4] = 8 bytes
    
    // number of attachments (max 4)
    int numAttachments;

    // joint indices (max 4), one byte each, limited to 256 joints
    uint8_t jointIndices[4];

    // weights (max 4), one byte each, precision of 1/256 = ~0.004%
    uint8_t weights[4];
    // fourth weight is stored as 1.0 - (weight[0] - weight[1] - weight[2])
    // uint8_t weights[3];

    
public:
    // constructors
    Vertex();
    Vertex(glm::vec3 position, glm::vec3 normal, int numAttachments, const std::vector<uint8_t>& jointIndices, const std::vector<uint8_t>& weights);

    // functions
    
};