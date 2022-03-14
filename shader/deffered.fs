#version 330 core
layout(location=0) out vec3 gPosition;
layout(location=1) out vec3 gNormal;
layout(location=2) out vec3 Color;
//out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;
in vec3 color;
//uniform sampler2D texture1;

void main(){
	gPosition=WorldPos;
	gNormal=normalize(Normal);
	Color = color;
	//FragColor = vec4(color, 1.0);
	//color=texture(texture1,TexCoords).rgb;
	//FragColor=vec4(vec3(255,0,90),1.0f);
}

