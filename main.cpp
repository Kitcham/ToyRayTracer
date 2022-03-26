#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "camera.h"

#include "shader.h"
#include "stb_image.h"
#include "OpenRender.h"
#include "Sphere.h"
#include "Quad.h"
using namespace std;

extern unsigned int SCR_WIDTH = 1600;
extern unsigned int SCR_HEIGHT = 900;
void setQueue(OpenRender &render) {
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(0.0, -0.5, -3));
	trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
	shared_ptr<BasePart> sphere1;
	sphere1 = make_shared<Sphere>();
	sphere1->Creat();
	sphere1->setTexture("texture/Earth.jpg");
	sphere1->Matrial = 3;
	sphere1->Color = glm::vec3(0.7, 0.7, 0.7);
	shared_ptr<BasePartList>partList = make_shared<BasePartList>();
	partList->PartList.push_back(sphere1);
	partList->modelList.push_back(trans);

	trans = glm::mat4(1.0f);
	trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
	trans = glm::translate(trans, glm::vec3(0.0f, 2.f, -5.f));
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
	shared_ptr<BasePart> quad1 = make_shared<Quad>();
	quad1->Creat();
	quad1->Matrial = 4;//光源
	quad1->Color = glm::vec3(1, 1, 1);
	partList->PartList.push_back(quad1);
	partList->modelList.push_back(trans);

	trans = glm::mat4(1.0f);//背面
	trans = glm::translate(trans, glm::vec3(0.0, 0, -4));
	shared_ptr<BasePart> quad2 = make_shared<Quad>();
	quad2->Creat();
	quad2->Matrial = 2;
	quad2->Color = glm::vec3(0.5, 0.5, 0.5);
	partList->PartList.push_back(quad2);
	partList->modelList.push_back(trans);
 
	trans = glm::mat4(1.0f);//右面
	trans = glm::translate(trans, glm::vec3(1, 0, -3));
	trans = glm::rotate(trans, glm::radians(-90.f), glm::vec3(0, 1, 0));
	shared_ptr<BasePart> quad3 = make_shared<Quad>();
	quad3->Creat();
	quad3->Matrial = 2;
	quad3->Color = glm::vec3(0.5, 0.0, 0.0);
	partList->PartList.push_back(quad3);
	partList->modelList.push_back(trans);

	trans = glm::mat4(1.0f);//底面
	trans = glm::translate(trans, glm::vec3(0, -1, -3));
	trans = glm::rotate(trans, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	shared_ptr<BasePart> quad4 = make_shared<Quad>();
	quad4->Creat();
	quad4->Matrial = 2;
	quad4->Color = glm::vec3(0.5, 0.5, 0.5);
	partList->PartList.push_back(quad4);
	partList->modelList.push_back(trans);

	trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(-1, 0.f, -3));
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));	
	shared_ptr<BasePart> quad5 = make_shared<Quad>();
	quad5->Creat();
	quad5->Matrial = 2;
	quad5->Color = glm::vec3(0.0, 0.5, 0.0);
	partList->PartList.push_back(quad5);
	partList->modelList.push_back(trans);


	render.setRenderQueue(partList);
}
int main() {
	Camera camera(glm::vec3(0.0f, 0.0f, 2.0f));
	glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT, 0.1f, 100.0f);
	OpenRender render(camera, projection);
	setQueue(render);
	render.Loop();
}


