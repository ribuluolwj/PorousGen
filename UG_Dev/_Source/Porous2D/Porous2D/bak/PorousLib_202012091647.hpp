#pragma once
#ifndef POROUSE2DLIB_H_INCLUDED
#define POROUSE2DLIB_H_INCLUDED

#include <iostream>
#include <cmath>
#include "Porous2D.hpp"

//void Porous2D::GenerateCore(double* cx, double* cy, double x0 = 0.0, double y0 = 0.0)
//{
//    // Generate particle core coordination
//
//    // Generate random seed
//    std::srand((unsigned)std::time(0));
//
//    // 
//    cx = new double[porousParticleNumber] {};
//    cy = new double[porousParticleNumber] {};
//
//    // Generate random core coordination
//    for (int i = 0; i < porousParticleNumber; i++)
//    {
//        cx[i] = double(rand()) / double(RAND_MAX) * (faceLength - x0) + x0;
//        cy[i] = double(rand()) / double(RAND_MAX) * (faceWidth - y0) + x0;
//        //std::cout << i << '\t' << cx[i] << '\t' << cy[i] << endl;
//    }
//}
bool Porous2D::CheckIsDistanceValid(CircleParticle& circle1, CircleParticle& circle2)
{
    float aimDistance2 = (circle1.point.x - circle2.point.x) *
        (circle1.point.x - circle2.point.x) +
        (circle1.point.y - circle2.point.y) *
        (circle1.point.y - circle2.point.y);
    float minDistance2 = (circle1.radius + circle2.radius) *
        (circle1.radius + circle2.radius);
    if (aimDistance2 > minDistance2)
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool Porous2D::CheckIsValid(vector<CircleParticle>& circles, CircleParticle& newCircle)
{
    for (auto circle : circles)
    {
        if (!Porous2D::CheckIsDistanceValid(circle, newCircle))
        {
            return false;
        }
    }
    return true;
}

float GetRandomFloatNumber(double max, double min = 0.0)
{
    return ((float)rand() / RAND_MAX) * (max - min) + min;
}


#endif
