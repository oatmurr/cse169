#include "Vertex.h"

class Triangle {
private:
    // vertex indices
    unsigned int vertexIndex1;
    unsigned int vertexIndex2;
    unsigned int vertexIndex3;

public:
    // constructors
    Triangle();
    Triangle(unsigned int vertexIndex1, unsigned int vertexIndex2, unsigned int vertexIndex3);

    // functions
    unsigned int GetVertexIndex1();
    unsigned int GetVertexIndex2();
    unsigned int GetVertexIndex3();
};