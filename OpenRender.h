/*
封装了延迟渲染的OpenGL光线追踪框架
可直接在场景中生成体积光、平面、球体
*/
#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include<iostream>
#include "BasePartList.h"
#include "camera.h"
#include <glm/glm.hpp>
#include <vector>
extern unsigned int SCR_WIDTH;
extern unsigned int SCR_HEIGHT;
class OpenRender
{
public:
	OpenRender(Camera camera1, glm::mat4 projection1);
	~OpenRender(){ delete quad; }
	void Draw(std::shared_ptr<BasePartList> partlist, Shader shader);
	void setRenderQueue(std::shared_ptr<BasePartList> basepart);
	int Loop();
public:
	GLuint gBuffer, lastColorBuffer;
	GLuint rbo, rbo1;
	GLuint gPosition, gNormal, gColorSpec, gLastColor;
	glm::mat4 projection;
	Shader defferedShader, drawShader;
	BasePart* quad;
	std::vector<std::shared_ptr<BasePartList> > renderQueue;

};

