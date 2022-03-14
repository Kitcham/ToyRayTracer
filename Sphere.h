#pragma once
#include "BasePart.h"
class Sphere :
    public BasePart
{
public:
    virtual void Creat();
public:
    float R;
    glm::vec3 Pos;
};

