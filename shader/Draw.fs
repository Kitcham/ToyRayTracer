#version 330 core

out vec4 FragColor;
in vec2 texcoords;
uniform sampler2D drawColor;

void main(){
	FragColor=vec4(texture(drawColor,texcoords).rgb,1.0f);
}

