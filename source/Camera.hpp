#pragma once

#include <cmath>

#include "glm/glm.hpp"

class Camera
{
private:
    double fov; // Field of view andle in radians
    double near, far; // Distances to near and far frustrum planes
    double aspect_ratio; // Ascpect ratio
    double tg_fov_2; // tg(fov / 2)
    constexpr static double pi = std::acos(-1);
public:
    Camera(double theta = 3. * pi / 4., double n = 0.1, double f = 100., double ar = 16. / 9.):
    fov(theta), near(n), far(f), aspect_ratio(ar), tg_fov_2(std::tan(fov / 2.))
    {}

    glm::dmat4 projectionMatrix() const {
        glm::dmat4 m(1. / tg_fov_2);
        m[1][1] *= aspect_ratio;
        m[2][2] = (near + far) / (far - near);
        m[2][3] = 1.;
        m[3][2] = 2. * far * near / (near - far);
        m[3][3] = 0;
        return m;
    }

    void setAspectRatio(double ar) {
        aspect_ratio = ar;
    }

    std::vector<glm::dvec3> getFrustrumNormals() const {
        return 
        {
            {0., 0., -1},                       // near
            {0., 0., 1.},                       // far
            {1., 0., -tg_fov_2},                // left
            {-1., 0., -tg_fov_2},               // right
            {0., 1., -tg_fov_2 / aspect_ratio}, // top
            {0., -1., -tg_fov_2 / aspect_ratio} // bottom
        };
    }

    std::vector<double> getFrustrumOffsets() const {
        return
        {
            near, // near
            -far, // far
            0.,   // left
            0.,   // right
            0.,   // top
            0.    // bottom
        };
    }

    double B() const {
        return (near + far) / (far - near);
    }

    double A() const {
        return 2. * far * near / (near - far);
    }
};
