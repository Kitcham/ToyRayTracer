#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "camera.h"
#include "model.h"
#include "shader.h"
#include "stb_image.h"
#include "OpenRender.h"
#include "Sphere.h"
#include "Quad.h"
using namespace std;

extern unsigned int SCR_WIDTH = 1600;
extern unsigned int SCR_HEIGHT = 900;
void setQueue(OpenRender &render) {
	auto Red = glm::vec3(.65, .05, .05);
	auto White = glm::vec3(.73, .73, .73);
	auto Green = glm::vec3(.12, .45, .15);
	auto iron = glm::vec3(0.8, 0.85, 0.88);

	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(0.5, -0.5, -3));
	trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
	shared_ptr<BasePart> sphere1;
	sphere1 = make_shared<Sphere>(Dielectric, iron);
	shared_ptr<BasePartList>partList = make_shared<BasePartList>();//球体
	//partList->Add(sphere1, trans);

	trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(0.0f, 1.01f, -3.f));
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
	shared_ptr<BasePart> quad = make_shared<Quad>(Lambertian, White);//顶面
	partList->Add(quad, trans);

	trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(0.0f, 1.f, -3.f));
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
	trans = glm::scale(trans, glm::vec3(0.25, 0.25f, 0.25));
	shared_ptr<BasePart> quad1 = make_shared<Quad>(Light, glm::vec3(17, 17, 17));//光源
	partList->Add(quad1, trans);

	trans = glm::mat4(1.0f);//背面
	trans = glm::translate(trans, glm::vec3(0.0, 0, -4));
	shared_ptr<BasePart> quad2 = make_shared<Quad>(Lambertian, White);
	partList->Add(quad2, trans);

 
	trans = glm::mat4(1.0f);//右面
	trans = glm::translate(trans, glm::vec3(1, 0, -3));
	trans = glm::rotate(trans, glm::radians(-90.f), glm::vec3(0, 1, 0));
	shared_ptr<BasePart> quad3 = make_shared<Quad>(Lambertian, Red);
	partList->Add(quad3, trans);

	trans = glm::mat4(1.0f);//底面
	trans = glm::translate(trans, glm::vec3(0, -1, -3));
	trans = glm::rotate(trans, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	shared_ptr<BasePart> quad4 = make_shared<Quad>(Lambertian, White);
	partList->Add(quad4, trans);

	trans = glm::mat4(1.0f);//左面
	trans = glm::translate(trans, glm::vec3(-1, 0.f, -3));
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));	
	shared_ptr<BasePart> quad5 = make_shared<Quad>(Lambertian, Green);
	partList->Add(quad5, trans);

	trans = glm::mat4(1.0f);
	trans = glm::scale(trans, glm::vec3(0.25, 0.5, 0.25));
	trans = glm::translate(trans, glm::vec3(0, -1, -12));
	trans = glm::rotate(trans, glm::radians(20.0f), glm::vec3(0, 1, 0));
	Model TestModel("model/test.obj", Metal, iron);
	//partList->Add(TestModel.meshes, trans);

	render.setRenderQueue(partList);
}
int main() {
	Camera camera(glm::vec3(0.0f, 0.0f, 2.0f));
	glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT, 0.1f, 100.0f);
	OpenRender render(camera, projection);
	setQueue(render);
	render.Loop();
}


