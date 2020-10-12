#version 330 core
// Vertex color (interpolated/fragment)
in vec2 TexCoords;

// Fragment Color
out vec4 fragColor;

uniform sampler2D image;

void main()
{
	fragColor = vec4(vec3(texture(image,TexCoords).rgb),1.0f);
}