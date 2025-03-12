#include "Triangle.h"

Triangle::Triangle()
{
    this->vertexIndex1 = 0;
    this->vertexIndex2 = 0;
    this->vertexIndex3 = 0;
}

Triangle::Triangle(unsigned int vertexIndex1, unsigned int vertexIndex2, unsigned int vertexIndex3) 
{
    this->vertexIndex1 = vertexIndex1;
    this->vertexIndex2 = vertexIndex2;
    this->vertexIndex3 = vertexIndex3;
}

unsigned int Triangle::GetVertexIndex1()
{
    return this->vertexIndex1;
}

unsigned int Triangle::GetVertexIndex2()
{
    return this->vertexIndex2;
}

unsigned int Triangle::GetVertexIndex3()
{
    return this->vertexIndex3;
}