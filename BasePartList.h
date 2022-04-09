/*
用同个shader渲染的Part都存到一起然后统一渲染
目前向shader中固定传送 projection cameraView modelMat
后期考虑对model偏移进行合批降低GPU负载（在带宽仍然充裕的情况下）
*/
#pragma once
#include "BasePart.h"
#include "camera.h"
#include <glm/glm.hpp>
#include <vector>
#include "shader.h"
class Bvh;
class BasePartList
{
	struct LightXZ {
		float x0, x1, z0, z1, y;
		int flag;
	};
public:
	BasePartList(){}
	void Draw(Shader shader,Camera camera, glm::mat4 projection);
	void LoadLight(Shader shader);
public:
	std::vector<glm::mat4>modelList;
	std::vector<std::shared_ptr<BasePart> >PartList;
	std::shared_ptr<Bvh> bvh;
	LightXZ Light;
};

