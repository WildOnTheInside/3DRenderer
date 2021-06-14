#pragma once

#include "FrameBuffer.hpp"
#include "Interpolator.hpp"
#include "Line2D.hpp"
#include "Point2D.hpp"
#include "Texture.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

class Triangle2D
{
private:
    Point2D p1, p2, p3;
    glm::dvec2 tc1, tc2, tc3;
public:

    Triangle2D(Point2D v1, Point2D v2, Point2D v3):
    p1(v1), p2(v2), p3(v3)
    {
        if (p2.x < p1.x) {
            std::swap(p1, p2);
        }
        if (p3.x < p1.x) {
            std::swap(p1, p3);
            std::swap(p2, p3);
        }
        else if (p3.x < p2.x) {
            std::swap(p2, p3);
        }
    }

    void draw(FrameBuffer& buffer, ZBuffer& zb, const std::vector<LightSource*>& lights, const std::vector<glm::dvec3>& lights_pos, const Material& m, const Interpolator& intp) const {
        std::vector<Point2D> l12 = Line2D(p1, p2, intp).draw(buffer, zb, lights, lights_pos, m);
        std::vector<Point2D> l23 = Line2D(p2, p3, intp).draw(buffer, zb, lights, lights_pos, m);
        std::vector<Point2D> l13 = Line2D(p1, p3, intp).draw(buffer, zb, lights, lights_pos, m);
        l12.reserve(l12.size() + l23.size());
        l12.insert(l12.end(), l23.begin(), l23.end());
        uint32_t i = 0, j = 0;
        bool f = false;
        int x2_pred, x3_pred;
        while (i < l12.size()) {
            if (i < l12.size() - 1 && l12[i] == l12[i + 1]) {
                i++;
                f = true;
            }
            if (f) {
                x2_pred = l12[i].x;
                while (i < l12.size() - 1 && l12[i + 1].x == x2_pred) {
                    i++;
                }
            }
            Line2D(l12[i], l13[j], intp).draw(buffer, zb, lights, lights_pos, m);
            if (!f) {
                x2_pred = l12[i].x;
                while (i < l12.size() && l12[i].x == x2_pred) {
                    i++;
                }
            }
            x3_pred = l13[j].x;
            while (j < l13.size() && l13[j].x == x3_pred) {
                j++;
            }
            i += f;
        }
    }

    friend std::ostream& operator<<(std::ostream& out, const Triangle2D& t) {
        out << t.p1 << " " << t.p2 << " " << t.p3 << std::endl;
    }
};
