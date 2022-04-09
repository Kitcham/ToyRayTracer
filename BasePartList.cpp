#include "BasePartList.h"
#include <algorithm>
void BasePartList::Draw(Shader shader, Camera camera, glm::mat4 projection)
{		
	shader.use();

	for (int i = 0; i < PartList.size(); i++) {
		PartList[i]->CreatVAO();
		//shader.setInt("texture1", PartList[i]->getTexture());
		shader.setMat4("projection", projection);
		shader.setMat4("view", camera.GetViewMatrix());
		shader.setMat4("model", modelList[i]);
		if (PartList[i]->Matrial == 4 && Light.flag==0) {
                float x0(1e8 + 7), x1(-1e8 + 7), z0(1e8 + 7), z1(-1e8 + 7);
                for (int j = 0; j < PartList[i]->positions.size(); j++) {
                    glm::vec4 v1 = glm::vec4(PartList[i]->positions[j], 1.0) * modelList[i];
                    x0 = std::min(x0, v1.x);
                    x1 = std::max(x1, v1.x);
                    z0 = std::min(z0, v1.z);
                    z1 = std::max(z1, v1.z);
                }
                Light.x0 = x0;
                Light.x1 = x1;
                Light.z0 = z0;
                Light.z1 = z1;
                Light.y = PartList[i]->positions[0].y;
                Light.flag = 1;
        }
        PartList[i]->Draw();
	}


}

void BasePartList::LoadLight(Shader shader)
{
    shader.setFloat("Light.x0", Light.x0);
    shader.setFloat("Light.x1", Light.x1);
    shader.setFloat("Light.z0", Light.z0);
    shader.setFloat("Light.z1", Light.z1);
    shader.setFloat("Light.y", Light.y);
}

