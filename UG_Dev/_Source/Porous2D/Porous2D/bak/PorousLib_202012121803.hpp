#pragma once
#ifndef POROUSE2DLIB_H_INCLUDED
#define POROUSE2DLIB_H_INCLUDED

#include <iostream>
#include <cmath>
#include <ctime>
#include "Porous2D.hpp"
#include "HuNXOpen.h"
#include "uf_all.h"

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
//bool Porous2D::CheckIsDistanceValid(CircleParticle& circle1, CircleParticle& circle2)
//{
//    float aimDistance2 = (circle1.point.x - circle2.point.x) *
//        (circle1.point.x - circle2.point.x) +
//        (circle1.point.y - circle2.point.y) *
//        (circle1.point.y - circle2.point.y);
//    float minDistance2 = (circle1.radius + circle2.radius) *
//        (circle1.radius + circle2.radius);
//    if (aimDistance2 > minDistance2)
//    {
//        return true;
//    }
//    else
//    {
//        return false;
//    }
//}
//bool Porous2D::CheckIsValid(vector<CircleParticle>& circles, CircleParticle& newCircle)
//{
//    for (auto circle : circles)
//    {
//        if (!Porous2D::CheckIsDistanceValid(circle, newCircle))
//        {
//            return false;
//        }
//    }
//    return true;
//}

void GenPorousSheet(NXOpen::Part* workPart)
{
    //--4 points creat plane
    NXOpen::FourPointSurfaceBuilder* fourPointSurfaceBuilder1;
    fourPointSurfaceBuilder1 = workPart->Bodies()->CreateFourPointSurfaceBuilder();
    //Create set points of surface
    faceLength = expressionFaceLength->Value();
    faceWidth = expressionFaceWidth->Value();
    //----Set setpoint1
    //NXOpen::Scalar* scalar11;
    //scalar11 = workPart->Scalars()->CreateScalar(0.0, NXOpen::Scalar::DimensionalityTypeNone, NXOpen::SmartObject::UpdateOptionWithinModeling);
    //NXOpen::Scalar* scalar12;
    //scalar12 = workPart->Scalars()->CreateScalar(0.0, NXOpen::Scalar::DimensionalityTypeNone, NXOpen::SmartObject::UpdateOptionWithinModeling);
    //NXOpen::Scalar* scalar13;
    //scalar13 = workPart->Scalars()->CreateScalar(0.0, NXOpen::Scalar::DimensionalityTypeNone, NXOpen::SmartObject::UpdateOptionWithinModeling);
    //NXOpen::Point* setPoint1 = workPart->Points()->CreatePoint(scalar11, scalar12, scalar13, NXOpen::SmartObject::UpdateOptionWithinModeling);
    ////fourPointSurfaceBuilder1->SetPoint1(&pointSheetStart);
    NXOpen::Point* setPoint1 = workPart->Points()->CreatePoint(pointSheetStart->Point());

    fourPointSurfaceBuilder1->SetPoint1(setPoint1);
    double x0 = setPoint1->Coordinates().X;
    double y0 = setPoint1->Coordinates().Y;
    double z0 = setPoint1->Coordinates().Z;


    //----Set setpoint2
    NXOpen::Scalar* scalar21;
    scalar21 = workPart->Scalars()->CreateScalar(x0 + faceLength, NXOpen::Scalar::DimensionalityTypeNone, NXOpen::SmartObject::UpdateOptionWithinModeling);
    NXOpen::Scalar* scalar22;
    scalar22 = workPart->Scalars()->CreateScalar(y0, NXOpen::Scalar::DimensionalityTypeNone, NXOpen::SmartObject::UpdateOptionWithinModeling);
    NXOpen::Scalar* scalar23;
    scalar23 = workPart->Scalars()->CreateScalar(z0, NXOpen::Scalar::DimensionalityTypeNone, NXOpen::SmartObject::UpdateOptionWithinModeling);
    NXOpen::Point* setPoint2 = workPart->Points()->CreatePoint(scalar21, scalar22, scalar23, NXOpen::SmartObject::UpdateOptionWithinModeling);
    fourPointSurfaceBuilder1->SetPoint2(setPoint2);
    //----Set setpoint3
    NXOpen::Scalar* scalar31;
    scalar31 = workPart->Scalars()->CreateScalar(x0 + faceLength, NXOpen::Scalar::DimensionalityTypeNone, NXOpen::SmartObject::UpdateOptionWithinModeling);
    NXOpen::Scalar* scalar32;
    scalar32 = workPart->Scalars()->CreateScalar(y0 + faceWidth, NXOpen::Scalar::DimensionalityTypeNone, NXOpen::SmartObject::UpdateOptionWithinModeling);
    NXOpen::Scalar* scalar33;
    scalar33 = workPart->Scalars()->CreateScalar(z0, NXOpen::Scalar::DimensionalityTypeNone, NXOpen::SmartObject::UpdateOptionWithinModeling);
    NXOpen::Point* setPoint3 = workPart->Points()->CreatePoint(scalar31, scalar32, scalar33, NXOpen::SmartObject::UpdateOptionWithinModeling);
    fourPointSurfaceBuilder1->SetPoint3(setPoint3);
    //----Set setpoint4
    NXOpen::Scalar* scalar41;
    scalar41 = workPart->Scalars()->CreateScalar(x0, NXOpen::Scalar::DimensionalityTypeNone, NXOpen::SmartObject::UpdateOptionWithinModeling);
    NXOpen::Scalar* scalar42;
    scalar42 = workPart->Scalars()->CreateScalar(y0 + faceWidth, NXOpen::Scalar::DimensionalityTypeNone, NXOpen::SmartObject::UpdateOptionWithinModeling);
    NXOpen::Scalar* scalar43;
    scalar43 = workPart->Scalars()->CreateScalar(z0, NXOpen::Scalar::DimensionalityTypeNone, NXOpen::SmartObject::UpdateOptionWithinModeling);
    NXOpen::Point* setPoint4 = workPart->Points()->CreatePoint(scalar41, scalar42, scalar43, NXOpen::SmartObject::UpdateOptionWithinModeling);
    fourPointSurfaceBuilder1->SetPoint4(setPoint4);
    //----Create surface object
    NXOpen::NXObject* nXObjectSheet;
    nXObjectSheet = fourPointSurfaceBuilder1->Commit();
    //----Free memory
    fourPointSurfaceBuilder1->Destroy();
}

double GetRandomFloatNumber(double max, double min = 0.0)
{
    return ((double)rand() / RAND_MAX) * (max - min) + min;
}


#endif
