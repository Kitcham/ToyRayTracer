#version 330 core
layout (location=0) in vec3 aposition;
layout (location=1) in vec3 Normal;
layout (location=2) in vec2 aTexCoords;

out vec2 texcoords;
void main(){
	gl_Position=vec4(aposition,1.0f);
	texcoords=aTexCoords;

}