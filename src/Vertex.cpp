#include "Vertex.h"

Vertex::Vertex() {
    this->position = glm::vec3(0.0f, 0.0f, 0.0f);
    this->normal = glm::vec3(0.0f, 0.0f, 0.0f);

    this->color = glm::vec3(0.0f, 0.0f, 0.0f);
    this->textureCoordinates = glm::vec2(0.0f, 0.0f);

    this->tangent = glm::vec3(0.0f, 0.0f, 0.0f);

    this->bitangent = glm::vec3(0.0f, 0.0f, 0.0f);

    this->numAttachments = 0;

    for (int i = 0; i < 4; i++) {
        this->jointIndices[i] = 0;
        this->weights[i] = 0;
    }
}   

Vertex::Vertex(glm::vec3 position, glm::vec3 normal, int numAttachments, const std::vector<int>& jointIndices, const std::vector<float>& weights) {
    this->position = position;
    this->normal = normal;

    this->color = glm::vec3(0.0f, 0.0f, 0.0f);
    this->textureCoordinates = glm::vec2(0.0f, 0.0f);

    // calculate tangent and bitangent from normal (orthonormal basis for normal mapping)

    // cross normal with y-axis (0,1,0)
    glm::vec3 normalCrossY = glm::cross(normal, glm::vec3(0.0f, 1.0f, 0.0f));
    // cross normal with z-axis (0,0,1)
    glm::vec3 normalCrossZ = glm::cross(normal, glm::vec3(0.0f, 0.0f, 1.0f));

    // choose the cross product that gives the longest vector (most perpendicular) and normalise
    if (glm::length(normalCrossZ) > glm::length(normalCrossY)) {
        this->tangent = glm::normalize(normalCrossZ);
    } else {
        this->tangent = glm::normalize(normalCrossY);
    }

    // calculate bitangent by crossing normal with tangent and normalise
    this->bitangent = glm::normalize(glm::cross(normal, tangent));

    this->numAttachments = numAttachments;

    for (int i = 0; i < numAttachments; i++) {
        // cast as uint8_t
        this->jointIndices[i] = uint8_t(jointIndices[i]);
        // convert float weight value (0.0-1.0) to byte weight value (0-255)
        this->weights[i] = uint8_t(weights[i] * 255.0f);
    }
}

int Vertex::GetNumAttachments() {
    return this->numAttachments;
}

float Vertex::GetWeight(int index) {
    return this->weights[index] / 255.0f;
}

int Vertex::GetJointIndex(int index) {
    return this->jointIndices[index];
}