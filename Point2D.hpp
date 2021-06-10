#pragma once

#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"

#include "glm/glm.hpp"

#include "FrameBuffer.hpp"
#include "ZBuffer.hpp"
#include "Texture.hpp"
#include "LightSource.hpp"
#include "auxiliary.hpp"

#include <cmath>
#include <iostream>

class Point2D
{
public:
    uint32_t x, y;
    double z;
    double u, v;
    glm::dvec3 coord;
    glm::dvec3 norm;
public:
    Point2D(uint32_t x = 0, uint32_t y = 0, double z = 0, glm::dvec3 crd = {0, 0, 0},  double u = 0, double v = 0, glm::dvec3 n = {0., 0., 0.}):
    x(x), y(y), z(z), coord(crd), u(u), v(v), norm(glm::normalize(n))
    {}

    void draw(FrameBuffer& buffer, ZBuffer& zb, const Texture& text, const std::vector<LightSource*>& lights, const std::vector<glm::dvec3>& lights_pos) {
        if (zb.fill_at(x, y, z)) {
            glm::dvec3 c(0.);
            for (uint32_t i = 0; i < lights.size(); ++i) {
                LightSource* l = lights[i];
                auto l_pos = lights_pos[i];
                switch (l->getType())
                {
                case LightSourceType::Ambient: {
                    c += text.getPixelUV_d(u, v) * l->colorAt(l_pos, coord);
                    break;
                }
                // case LightSourceType::Directional: {
                //     break;
                // }
                // case LightSourceType::Point:
                //     break;
                default: {
                    auto to_v = glm::normalize(-coord), l_dir = l->dirTo(l_pos, coord), h = glm::normalize(l_dir + to_v);
                    double m = 30;
                    glm::dvec3 t = l->colorAt(l_pos, coord) * (text.getPixelUV_d(u, v) * std::max(glm::dot(norm, l_dir), 0.) + (double)(glm::dot(l_dir, norm) > 0) * std::pow(std::max(glm::dot(h, norm), 0.), m));
                    c += t;
                    break;
                }
                }
            }
            c = glm::clamp(c, 0., 1.);
            buffer.fill_at_d(x, y, c);
        }
    }

    bool operator==(Point2D other) {
        return (x == other.x) && (y == other.y) && (z == other.z);
    }

    bool operator!=(Point2D other) {
        return !(*this == other);
    }

};

std::ostream& operator<<(std::ostream& os, const Point2D& p) {
    os << std::dec << "xyz " << p.x << " " << p.y << " " << p.z << " uv:" << p.u << " " << p.v;
    return os;
}