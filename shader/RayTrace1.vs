#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 Color;
//layout (location = 3) in mat4 model;//实例化

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;
out vec3 color;
out vec4 pix;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    TexCoords = aTexCoords;
    vec4 worldPos = model * vec4(aPos, 1.0f);
    WorldPos = worldPos.xyz;
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalMatrix * aNormal;
    color = Color;
    pix = projection * view  * worldPos;
    gl_Position =  projection * view  * worldPos;
}
