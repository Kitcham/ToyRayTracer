#include "Quad.h"
void Quad::Creat()
{
    setVAO(0);
    float quadVertices[] = {
        // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    for (int i = 0; i < 20; i=i+5) {
        positions.push_back(glm::vec3(quadVertices[i], quadVertices[i + 1], quadVertices[i + 2]));
        normals.push_back(glm::vec3(quadVertices[i], quadVertices[i + 1], quadVertices[i + 2]));
        uv.push_back(glm::vec2(quadVertices[i + 3], quadVertices[i + 4]));
    }
    indices.clear();
    indices = {
        0,1,2,3
    };
}
