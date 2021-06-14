#pragma once

#include "glm/glm.hpp"

#include "Interpolator.hpp"
#include "LightSource.hpp"
#include "Point2D.hpp"
#include "Texture.hpp"
#include "Triangle2D.hpp"

#include <vector>

class Polygon2D {
private:
    std::vector<Point2D> vertices;
public:
    Polygon2D(): vertices()
    {}

    Polygon2D(const std::vector<Point2D>& v): vertices(v)
    {}

    Polygon2D(
        const std::vector<glm::dvec3>& v,
        const std::vector<glm::dvec3>& v_coord,
        const std::vector<glm::dvec2>& text_coord,
        glm::dvec3 norm,
        uint32_t w,
        uint32_t h)
    {
        vertices.reserve(v.size());
        for (uint32_t i = 0; i < v.size(); ++i) {
            vertices.emplace_back(
                std::min((uint32_t)(v[i].x * h / 2), h - 1), 
                std::min((uint32_t)(v[i].y * w / 2), w - 1), 
                (double)v[i].z, 
                v_coord[i], 
                text_coord[i].x,
                text_coord[i].y,
                norm);
        }
    }

    Polygon2D(
        const std::vector<glm::dvec3>& v,
        const std::vector<glm::dvec3>& v_coord,
        const std::vector<glm::dvec2>& text_coord,
        const std::vector<glm::dvec3>& norms,
        uint32_t w,
        uint32_t h)
    {
        vertices.reserve(v.size());
        if (text_coord.size() != 0) {
            for (uint32_t i = 0; i < v.size(); ++i) {
                vertices.emplace_back(
                    std::min((uint32_t)(v[i].x * h / 2), h - 1), 
                    std::min((uint32_t)(v[i].y * w / 2), w - 1), 
                    (double)v[i].z,
                    v_coord[i], 
                    text_coord[i].x,
                    text_coord[i].y,
                    norms[i]);
            }
        }
        else {
            for (uint32_t i = 0; i < v.size(); ++i) {
                vertices.emplace_back(
                    std::min((uint32_t)(v[i].x * h / 2), h - 1), 
                    std::min((uint32_t)(v[i].y * w / 2), w - 1), 
                    (double)v[i].z,
                    v_coord[i], 
                    0,
                    0,
                    norms[i]);
            }
        }
    }

    void draw(FrameBuffer& fb, ZBuffer& zb, const std::vector<LightSource*>& lights, const std::vector<glm::dvec3>& lights_pos, const Material& m, const Interpolator& intp) {
        if (vertices.size() == 0) {
            return;
        }
        for (int i = 1; i < vertices.size() - 1; ++i) {
            auto t = Triangle2D(vertices[0], vertices[i], vertices[i + 1]);
            t.draw(fb, zb, lights, lights_pos, m, intp);
        }
    }
};