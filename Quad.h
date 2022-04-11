/*
生成一个平面，用于平面光源、Gbuffer写入
*/
#pragma once
#include "BasePart.h"
class Quad :
    public BasePart
{
public:
    Quad(MatrialType matrial, glm::vec3 color) :BasePart(matrial, color) {
        Creat();
    }
    Quad() = default;
    virtual void Creat();
};

