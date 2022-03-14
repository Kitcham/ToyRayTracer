#version 330 core
//数据结构
#define SIZE_TRIANGLE   4
struct Triangle {
	vec3 Matrial;
	vec3 v1, v2, v3;
	vec3 Color;
};
struct ray {
	vec3 position;
	vec3 direction;
	float Len;
};
//IO数据
in vec3 pix;
in vec2 texcoords;
out vec4 FragColor;
uniform samplerBuffer TriangleList;
uniform sampler2D position;
uniform sampler2D normal;
//解码输入数据
Triangle getTriangle(int i) {
    int offset = i * SIZE_TRIANGLE ;
    Triangle t;
    t.Matrial=texelFetch(TriangleList,offset+0).xyz;
    // 顶点坐标
    t.v1 = texelFetch(TriangleList, offset + 1).xyz;
    t.v2 = texelFetch(TriangleList, offset + 2).xyz;
    t.v3 = texelFetch(TriangleList, offset + 3).xyz;
    // 法线
    t.Color = texelFetch(TriangleList, offset + 4).xyz;
    return t;
}

void main(){
	FragColor=vec4(texture(position,texcoords).rgb,1.0f);
    //FragColor=vec4(vec3(255,0,0),1.0);
	//vec3 Frag=texture(gPosition,TexCoords).rgb;
	//if(Frag!=vec3(0,0,0))FragColor=vec4(gl_FragCoord.x/1000.0f,gl_FragCoord.y/1000.0f,gl_FragCoord.z/1000.0f,1.0f);
}