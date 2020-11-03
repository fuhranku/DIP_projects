#version 330 core
// Atributte 0 of the vertex
layout (location = 0) in vec3 vertexPosition;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    mat4 MVP = projection * view;
    gl_Position = MVP * vec4(vertexPosition, 1.0f);
}