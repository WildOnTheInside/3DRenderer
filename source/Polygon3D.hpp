#pragma once

#include "glm/glm.hpp"

#include "Interpolator.hpp"
#include "Line2D.hpp"
#include "Point2D.hpp"
#include "Texture.hpp"
#include "Triangle2D.hpp"

#include <vector>
#include <tuple>

class Polygon3D
{
private:
    std::vector<glm::dvec3> vertices;
public:
    // Polygon3D(std::vector<glm::dvec3>& v):
    // vertices(v)
    // {}

    Polygon3D(std::vector<glm::dvec3> v):
    vertices(v)
    {}

    Polygon3D():
    vertices({})
    {}

    std::tuple<Polygon3D, std::vector<glm::dvec2>, std::vector<glm::dvec3>> clip(const std::vector<glm::dvec3>& normals, const std::vector<double>& offsets, const std::vector<glm::dvec2>& textCoord, const std::vector<glm::dvec3>& v_normals) const {
        std::vector<glm::dvec3> vert(vertices);
        std::vector<glm::dvec2> text(textCoord);
        std::vector<glm::dvec3> norm(v_normals);
        bool has_tc = text.size() != 0;
        for (int i = 0; i < 6; ++i) {
            int f1 = -2, f2 = -2, start = 0, end = 0;
            for (int j = 0; j < vert.size(); ++j) {
                if (glm::dot(vert[j], normals[i]) + offsets[i] > 1e-6) {
                    if (f1 == -1) {
                        f1 = 0;
                        start = j;
                    }
                    if (f2 == -2) {
                        f2 = -1;
                    }
                }
                else {
                    if (f1 == -2) {
                        f1 = -1;
                    }
                    if (f2 == -1) {
                        f2 = 0;
                        end = j;
                    }
                }
            }
            if (f1 == -2) {
                return {Polygon3D(), {}, {}};
            }
            if (f2 == -2) {
                continue;
            }

            int prestart = (start + vert.size() - 1) % vert.size();
            int preend = (end + vert.size() - 1) % vert.size();
            glm::dvec3 a0 = (vert[prestart] + vert[start]) / 2.;
            glm::dvec3 v = vert[prestart] - vert[start];
            glm::dvec3 new_v1 = a0 - v * ((glm::dot(normals[i], a0) + offsets[i]) / (glm::dot(normals[i], v)));
            double t = glm::distance(new_v1, vert[prestart]) / glm::distance(vert[start], vert[prestart]);
            glm::dvec2 new_v1_text;
            if (has_tc) {
                new_v1_text = (1 - t) * text[prestart] + t * text[start];
            }
            glm::dvec3 new_v1_norm = (1 - t) * norm[prestart] + t * norm[start];

            a0 = (vert[end] + vert[preend]) / 2.;
            v = vert[preend] - vert[end];
            glm::dvec3 new_v2 = a0 - v * ((glm::dot(normals[i], a0) + offsets[i]) / (glm::dot(normals[i], v)));
            t = glm::distance(new_v2, vert[preend]) / glm::distance(vert[end], vert[preend]);
            glm::dvec2 new_v2_text;
            if (has_tc) {
                new_v2_text = (1 - t) * text[preend] + t * text[end];
            }
            glm::dvec3 new_v2_norm = (1 - t) * norm[preend] + t * norm[end];
            glm::dvec3 arr[] = {new_v1, new_v2};
            glm::dvec2 text_arr[] = {new_v1_text, new_v2_text};
            glm::dvec3 norm_arr[] = {new_v1_norm, new_v2_norm};
            vert.insert(vert.begin() + start, arr, arr + 2);
            norm.insert(norm.begin() + start, norm_arr, norm_arr + 2);
            if (has_tc) {
                text.insert(text.begin() + start, text_arr, text_arr + 2);
            }
            if (start <= preend) {
                vert.erase(vert.begin() + start + 2, vert.begin() + preend + 3);
                norm.erase(norm.begin() + start + 2, norm.begin() + preend + 3);
                if (has_tc) {
                    text.erase(text.begin() + start + 2, text.begin() + preend + 3);
                }
            }
            else {
                vert.erase(vert.begin() + start + 2, vert.end());
                vert.erase(vert.begin(), vert.begin() + end);
                norm.erase(norm.begin() + start + 2, norm.end());
                norm.erase(norm.begin(), norm.begin() + end);
                if (has_tc) {
                    text.erase(text.begin() + start + 2, text.end());
                    text.erase(text.begin(), text.begin() + end);
                }
            }
        }
        return {Polygon3D(vert), text, norm};
    }

    std::vector<glm::dvec3> getVertices() const {
        return vertices;
    }

    friend std::ostream& operator<<(std::ostream& out, const Polygon3D& p) {
        for (const glm::dvec3& v : p.vertices) {
            out << v << std::endl;
        }
        return out;
    }
};

