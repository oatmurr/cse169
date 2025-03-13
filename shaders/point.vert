#version 330 core

// vertex shader for drawing point/particle

layout (location = 0) in vec3 position;

uniform mat4 viewProj;
uniform mat4 model;

void main()
{
    gl_Position = viewProj * model * vec4(position, 1.0);
    gl_PointSize = 25.0; // Adjust the size of points
}
