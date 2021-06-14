#pragma once

#include "SFML/Graphics.hpp"

#include "FrameBuffer.hpp"
#include "Interpolator.hpp"
#include "Point2D.hpp"
#include "Texture.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>
#include <iostream>

class Line2D
{
private:
    Point2D p1, p2;
    Point2D start;
    Interpolator interpol;
public:
    Line2D(Point2D a, Point2D b, const Interpolator& in):
    p1(b), p2(a), start(a), interpol(in)
    {
        if (a.y < b.y) {
            p1 = a;
            p2 = b;
        }
    }

    std::vector<Point2D> getLine() const {
        std::vector<Point2D> line;
        if (p1.x == p2.x) {
            int len = p2.y - p1.y + 1;
            double z1 = p1.z, z2 = p2.z, u1 = p1.u, u2 = p2.u, v1 = p1.v, v2 = p2.v;
            double t = 0, t_inc = 1. / len;
            line.reserve(len);
            for (uint32_t i = p1.y; i <= p2.y; ++i) {
                double z = (1 - t) * z1 + t * z2;
                double u = interpol.interpolate(p1, p2, u1, u2, t);
                double v = interpol.interpolate(p1, p2, v1, v2, t);
                glm::dvec3 coord = interpol.interpolate(p1, p2, p1.coord, p2.coord, t);
                glm::dvec3 n = interpol.interpolate(p1, p2, p1.norm, p2.norm, t);
                line.emplace_back(p1.x, i, z, coord, u, v, n);
                t += t_inc;
            }
            line[line.size() - 1] = {p2.x, p2.y, z2, p2.coord, u2, v2, p2.norm};
        }
        else if (p2.y - p1.y == std::abs((int)p1.x - (int)p2.x)) {
            int len = p2.y - p1.y + 1;
            line.reserve(len);
            double z1 = p1.z, z2 = p2.z, u1 = p1.u, u2 = p2.u, v1 = p1.v, v2 = p2.v;
            double t = 0, t_inc = 1. / len;
            int d = (p1.x < p2.x) ? 1 : -1;
            for (int i = 0; i <= p2.y - p1.y; ++i) {
                double z = (1 - t) * z1 + t * z2;
                double u = interpol.interpolate(p1, p2, u1, u2, t);
                double v = interpol.interpolate(p1, p2, v1, v2, t);
                glm::dvec3 coord = interpol.interpolate(p1, p2, p1.coord, p2.coord, t);
                glm::dvec3 n = interpol.interpolate(p1, p2, p1.norm, p2.norm, t);
                line.emplace_back(p1.x + d * i, p1.y + i, z, coord, u, v, n);
                t += t_inc;
            }
            line[line.size() - 1] = {p2.x, p2.y, z2, p2.coord, u2, v2, p2.norm};
        }
        else {
            int x1 = p1.x, x2 = p2.x, y1 = p1.y, y2 = p2.y;
            double z1 = p1.z, z2 = p2.z, u1 = p1.u, u2 = p2.u, v1 = p1.v, v2 = p2.v;
            int k1 = 1, k2 = 0;
            if (std::abs(x1 - x2) < y2 - y1) {
                std::swap(x1, y1);
                std::swap(x2, y2);
                std::swap(k1, k2);
                std::swap(u1, v1);
                std::swap(u2, v2);
            }
            int dx = std::abs(x1 - x2);
            int dy = std::abs(y2 - y1);
            int len = dx + 1;
            double t = 0, t_inc = 1. / len;
            line.reserve(len);
            int err = 0;
            int derr = dy + 1;
            int dirx = (x1 > x2) ? -1 : 1;
            int diry = (y1 > y2) ? -1 : 1;
            for (int i = x1; i != x2; i += dirx) {
                double z = (1 - t) * z1 + t * z2;
                double u_t = interpol.interpolate(p1, p2, u1, u2, t);
                double v_t = interpol.interpolate(p1, p2, v1, v2, t);
                double u = k1 * u_t + k2 * v_t;
                double v = k1 * v_t + k2 * u_t;
                glm::dvec3 coord = interpol.interpolate(p1, p2, p1.coord, p2.coord, t);
                glm::dvec3 n = interpol.interpolate(p1, p2, p1.norm, p2.norm, t);
                line.emplace_back(k1 * i + k2 * y1, k1 * y1 + k2 * i, z, coord, u, v, n);
                err += derr;
                t += t_inc;
                if (err >= dx + 1) {
                    y1 += diry;
                    err -= dx + 1;
                }
            }
            line.emplace_back(k1 * x2 + k2 * y2, k1 * y2 + k2 * x2, z2, p2.coord, k1 * u2 + k2 * v2, k1 * v2 + k2 * u2, p2.norm);
        }
        if (line[0] != start) {
            std::reverse(line.begin(), line.end());
        }
        return line;
    }

    std::vector<Point2D> draw(FrameBuffer& buffer, ZBuffer& zb, const std::vector<LightSource*>& lights, const std::vector<glm::dvec3>& lights_pos, const Material& m) const {
        std::vector<Point2D> line = getLine();
        for (Point2D& p : line) {
            p.draw(buffer, zb, lights, lights_pos, m);
        }
        return line;
    }

};
