#ifndef MADMATH_H
#define MADMATH_H

#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <math.h>

class MadMath
{
public:
    static constexpr float RADIANT = 57.295779513082f;
    static float get3DDistance(float own_x, float own_y, float own_z, float entity_x, float entity_y, float entity_z);
    static std::vector<float> calcAngle(std::vector<float> src, std::vector<float> dst);
};

#endif