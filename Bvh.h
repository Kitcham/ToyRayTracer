#pragma once
#include "tool.h"
#include "BasePartList.h"

class aabb {
public:
	aabb(){}
	aabb(const glm::vec3& a, const glm::vec3& b) { minimum = a; maximum = b; }
	glm::vec3 minimum, maximum;
};

class Triangle {
public:
	Triangle() = default;
	bool bounding_box(aabb& output_box) {
		double minx = fmin(v1[0], fmin(v2[0], v3[0]));
		double miny = fmin(v1[1], fmin(v2[1], v3[1]));
		double minz = fmin(v1[2], fmin(v2[2], v3[2]));

		double maxx = fmax(v1[0], fmax(v2[0], v3[0]));
		double maxy = fmax(v1[1], fmax(v2[1], v3[1]));
		double maxz = fmax(v1[2], fmax(v2[2], v3[2]));
		double eps = 1e-5;

		glm::vec3 min = glm::vec3(minx - eps, miny - eps, minz - eps);
		glm::vec3 max = glm::vec3(maxx + eps, maxy + eps, maxz + eps);
		output_box = aabb(min, max);
		return true;
	}
public:
	glm::vec3 Matrial;
	glm::vec3 v1,v2,v3;
	glm::vec3 Color;
};
inline bool box_compare(Triangle& a, Triangle& b, int axis) {
	aabb boxA, boxB;
	if(!a.bounding_box(boxA)||!b.bounding_box(boxB))
		std::cout << "No bounding box in bvh_node constructor.\n";
	return boxA.minimum[axis] < boxB.minimum[axis];
}
inline bool box_x_compare(Triangle& a,Triangle& b) {
	return box_compare(a, b, 0);
}
inline bool box_y_compare(Triangle& a,Triangle& b) {
	return box_compare(a, b, 1);
}
inline bool box_z_compare(Triangle& a, Triangle& b) {
	return box_compare(a, b, 2);
}

class Bvh
{
struct node {
	aabb AABB;
	Triangle Tri;
	float TriFlag;//0aabb 1 Triangle
	float duiqi[2] = { 0,0 };
};
public:
	Bvh(std::shared_ptr<BasePartList> basepartList) {
		//根据点集和index，用model偏转，构建出所有三角形
		for (int i = 0; i < basepartList->PartList.size(); i++) {
			auto basepart = basepartList->PartList[i];
			for (int j = 2; j < basepart->indices.size(); j++) {
				Triangle tri;
				tri.Matrial[0] = basepart->Matrial;
				tri.Color = basepart->Color;
				if (j % 2) {
					tri.v1 = basepart->positions[basepart->indices[j - 1]];
					tri.v2 = basepart->positions[basepart->indices[j - 2]];
					tri.v3 = basepart->positions[basepart->indices[j]];
					tri.v1 = basepartList->modelList[i] * glm::vec4(tri.v1[0], tri.v1[1], tri.v1[2], 1.0);
					tri.v2 = basepartList->modelList[i] * glm::vec4(tri.v2[0], tri.v2[1], tri.v2[2], 1.0);
					tri.v3 = basepartList->modelList[i] * glm::vec4(tri.v3[0], tri.v3[1], tri.v3[2], 1.0);
				}
				else {
					tri.v1 = basepart->positions[basepart->indices[j - 2]];
					tri.v2 = basepart->positions[basepart->indices[j - 1]];
					tri.v3 = basepart->positions[basepart->indices[j]];
					tri.v1 = basepartList->modelList[i] * glm::vec4(tri.v1[0], tri.v1[1], tri.v1[2], 1.0);
					tri.v2 = basepartList->modelList[i] * glm::vec4(tri.v2[0], tri.v2[1], tri.v2[2], 1.0);
					tri.v3 = basepartList->modelList[i] * glm::vec4(tri.v3[0], tri.v3[1], tri.v3[2], 1.0);
				}
				
				TriList.push_back(tri);
			}
		}

		TriNum = TriList.size();
		BVHList.resize(TriNum <<4);
		BVHList[1].AABB=build(1, 0, TriNum);
		BVHList[1].TriFlag = 2;

		GLuint tbo0,tbo1;
		glGenBuffers(1, &tbo0);
		glBindBuffer(GL_TEXTURE_BUFFER, tbo0);
		glBufferData(GL_TEXTURE_BUFFER, TriNum * sizeof(Triangle), &TriList[0], GL_STATIC_DRAW);
		glGenTextures(1, &textureTri);
		glBindTexture(GL_TEXTURE_BUFFER, textureTri);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, tbo0);

		glGenBuffers(1, &tbo1);
		glBindBuffer(GL_TEXTURE_BUFFER, tbo1);
		glBufferData(GL_TEXTURE_BUFFER, BVHList.size() * sizeof(node), &BVHList[0], GL_STATIC_DRAW);
		glGenTextures(1, &textureBvh);
		glBindTexture(GL_TEXTURE_BUFFER, textureBvh);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, tbo1);

	}
	aabb build(int now, int start, int end);
	int Bvh::SAH(int now, int start, int end);
public:
	std::vector<Triangle> TriList;
	std::vector<node> BVHList;
	int TriNum;
	GLuint textureTri, textureBvh;
};

