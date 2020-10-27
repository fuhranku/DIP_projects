#version 330 core
// Vertex color (interpolated/fragment)
in vec2 TexCoords;


// Fragment Color
out vec4 fragColor;

uniform sampler2D image;
uniform int channels;

void main()
{
	if(channels == 1){
		fragColor = vec4(vec3(texture(image,TexCoords).r),1.0f);
	}
	else if(channels == 3){
		fragColor = vec4(vec3(texture(image,TexCoords).rgb),1.0f);
	}
	else if(channels == 4){
		fragColor = vec4(texture(image,TexCoords).rgba);
	}
	
}