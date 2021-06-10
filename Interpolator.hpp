#pragma once

#include "Point2D.hpp"

class Interpolator
{
private:
    double A, B;
public:
    Interpolator(double a, double b):
    A(a), B(b)
    {}

    template <typename T>
    T interpolate(const Point2D& p1, const Point2D& p2, T a1, T a2, double t) const {
        double z1 = p1.z, z2 = p2.z;
        double z = (1 - t) * z1 + t * z2;
        T a = ((1 - t) * a1 * (z1 - B) + t * a2 * (z2 - B)) / (z - B);
        return a;
    }
};
