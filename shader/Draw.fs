#version 330 core

out vec4 FragColor;
in vec2 texcoords;
in int frameCounter;
uniform sampler2D drawColor;
uniform float exposure;
void main(){
	vec3 hdrColor=texture(drawColor,texcoords).rgb;
	const float gamma = 2.2;
	//vec3 mapped = vec3(1.0) - exp(-hdrColor*exposure);
	//mapped = pow(mapped, vec3(1.0 / gamma));
	//FragColor = vec4(mapped, 1.0);
	FragColor = vec4(sqrt(hdrColor), 1.0);
}

