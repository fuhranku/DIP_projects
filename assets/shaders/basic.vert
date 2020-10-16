#version 330 core
// Atributte 0 of the vertex
layout (location = 0) in vec3 vertexPosition;
// Atributte 1 of the vertex
layout (location = 1) in vec3 vertexColor;
// Atributte 1 of the vertex
layout (location = 2) in vec2 vertexUV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Vertex texture position
out vec2 TexCoords;

void main()
{
    TexCoords = vertexUV;  
    gl_Position = projection * view * vec4(vertexPosition,1.0);
}