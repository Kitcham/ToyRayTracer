#pragma once
#include "BasePart.h"
class Sphere :
    public BasePart
{
public:
    Sphere(MatrialType matrial, glm::vec3 color) :BasePart(matrial, color) {
        Creat();
    }
    virtual void Creat();
public:
    float R;
    glm::vec3 Pos;
};

