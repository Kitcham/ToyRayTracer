#include "BasePartList.h"
void BasePartList::Draw(Shader shader, Camera camera, glm::mat4 projection)
{		
	shader.use();
	for (int i = 0; i < PartList.size(); i++) {
		PartList[i]->CreatVAO();
		//shader.setInt("texture1", PartList[i]->getTexture());
		shader.setMat4("projection", projection);
		shader.setMat4("view", camera.GetViewMatrix());
		shader.setMat4("model", modelList[i]);
		PartList[i]->Draw();
	}


}

