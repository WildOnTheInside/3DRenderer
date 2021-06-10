#pragma once

#include "glm/glm.hpp"

#include "Scene.hpp"
#include "Object3D.hpp"
#include "TexturedObject3D.hpp"
#include "Texture.hpp"
#include "FrameBuffer.hpp"
#include "Line2D.hpp"
#include "ZBuffer.hpp"
#include "Triangle2D.hpp"
#include "Polygon3D.hpp"
#include "Polygon2D.hpp"
#include "auxiliary.hpp"
#include "Interpolator.hpp"
#include "LightSource.hpp"
#include "Application.hpp"

#include <vector>
#include <iostream>
#include <map>
#include <unordered_map>

class Renderer
{
private:
    uint32_t wh, ww;
    FrameBuffer fb;
    ZBuffer zb;


    void renderObject(
        const TexturedObject3D& object,     // Object to be rendered
        const glm::dmat4& transform,        // Local-to-world transform matrix for vertices
        const glm::dmat3& normal_transform, // Local-to-world transform matrix for normals
        const glm::dmat4& camera,           // World-to-camera transform matrix for vertices
        const glm::dmat3& normal_camera,    // World-to-camera transform matrix for normals
        const std::vector<glm::dvec3>& frustrum_normals, // Frustrum planes' normals
        const std::vector<double>& frustrum_offsets, // Frustrum planes' offsets
        const glm::dmat4& projection,        // Projection matrix
        const std::vector<LightSource*>& lights,
        const std::vector<glm::dvec3>& lights_pos,
        const Interpolator& intp
    )
    {
        glm::dmat4 m = {{0., -1., 0., 0.}, {-1., 0., 0., 0.}, {0., 0., 1., 0.}, {1., 1., 0., 1.}};
        glm::dmat4 mvp = m * projection;// * camera * transform;
        glm::dmat4 trans = camera * transform;
        glm::dmat3 norm_trans = normal_camera * normal_transform;
        std::vector<glm::dvec3> proj_vert; // Projected vertices
        std::vector<glm::dvec3> trans_norm, trans_vert; // Transformed to camera space normals and vertices
        std::vector<std::vector<glm::dvec3>> trans_v_norm;

        proj_vert.reserve(object.vertices().size());
        trans_norm.reserve(object.vertices().size());
        trans_vert.reserve(object.vertices().size());
        trans_v_norm.reserve(object.faces().size());

        for (auto v : object.vertices()) {
            glm::dvec4 t(v, 1.);
            glm::dvec4 t_trans = trans * t;
            trans_vert.push_back(t_trans);
        }
        for (auto n : object.f_normals()) {
            trans_norm.push_back(glm::normalize(norm_trans * n));
        }
        for (auto& f : object.v_normals()) {
            std::vector<glm::dvec3> tmp;
            for (auto& v_n : f) {
                tmp.push_back(glm::normalize(norm_trans * v_n));
            }
            trans_v_norm.push_back(tmp);
        }
        std::map<glm::dvec3, int, decltype(cmp)*> new_vert(cmp);
        for (int i = 0; i != object.vertices().size(); ++i) {
            new_vert[trans_vert[i]] = i;
        }
        std::vector<std::vector<uint32_t>> faces = object.faces();
        std::vector<std::pair<std::vector<int>, std::vector<glm::dvec2>>> polygons(faces.size());
        std::vector<std::vector<glm::dvec3>> new_v_norm;
        new_v_norm.reserve(faces.size());
        for (int i = 0; i != faces.size(); ++i) {
            // Backface culling:
            // Check triangle for visibility
            std::vector<uint32_t> face_ind = faces[i];
            std::vector<glm::dvec3> face_vert;
            face_vert.reserve(face_ind.size());
            for (auto ind : face_ind) {
                face_vert.push_back(trans_vert[ind]);
            }
            glm::dvec3 norm = trans_norm[i];
            if (glm::dot(norm, face_vert[0]) > 0.0001) {
                continue;
            }
            // Clip triangle
            Polygon3D face(face_vert);
            auto [poly, textCoord, norms] = face.clip(frustrum_normals, frustrum_offsets, object.triangleTextureCoordinates(i), trans_v_norm[i]);
            for (auto new_v : poly.getVertices()) {
                if (new_vert.find(new_v) == new_vert.end()) {
                    new_vert[new_v] = new_vert.size();
                }
                polygons[i].first.push_back(new_vert[new_v]);
            }
            polygons[i].second = textCoord;
            new_v_norm.push_back(norms);
        }

        // Project vertices
        std::vector<glm::dvec3> vert_coord(new_vert.size());
        proj_vert.resize(new_vert.size());
        for (auto [v, ind] : new_vert) {
            glm::dvec4 t(v, 1.);
            glm::dvec4 t_proj = mvp * t;
            t_proj /= t_proj.w;
            proj_vert[ind] = t_proj;
            vert_coord[ind] = v;
        }
        uint32_t j = 0;
        for (uint32_t i = 0; i < polygons.size(); ++i) {
            // ++i;
            // Draw polygon
            auto [poly_index, tc] = polygons[i];
            if (poly_index.size() == 0) {
                continue;
            }
            std::vector<glm::dvec3> vert, vert_c;
            vert.reserve(poly_index.size());
            vert_c.reserve(poly_index.size());
            for (int ind : poly_index) {
                vert.push_back(proj_vert[ind]);
                vert_c.push_back(vert_coord[ind]);
            }
            Polygon2D poly(vert, vert_c, tc, new_v_norm[j], ww, wh);
            poly.draw(fb, zb, object.getTexture(), lights, lights_pos, intp);
            ++j;
        }
    }
public:
    Renderer(uint32_t h, uint32_t w):
    wh(h), ww(w), fb(h, w), zb(h, w)
    {}

    const sf::Uint8* render(const Scene& scene) {
        fb.flush();
        zb.flush();
        glm::dmat4 cameraMat = scene.cameraMatrix(), projMat = scene.projectionMatrix();
        glm::dmat3 norm_camera = scene.cameraMatrixNormal();
        std::vector<glm::dvec3> normals = scene.getFrustrumNormals();
        std::vector<double> offsets = scene.getFrustrumOffsets();
        auto lights = scene.getLights();
        auto l_pos = scene.getLightsPosition();
        for (LightSource* l : lights) {
            l->transform(norm_camera);
        }
        for (glm::dvec3& pos : l_pos) {
            pos = cameraMat * glm::dvec4(pos, 1.);
        }
        Interpolator intp = scene.getInterpolator();
        for (uint32_t i = 0; i < scene.n_objects; ++i) {
            renderObject(scene.getObject(i), scene.modelMatrix(i), scene.modelMatrixNormals(i), 
            cameraMat, norm_camera, normals, offsets, projMat, lights, l_pos, intp);
        }
        return fb.get_buffer();
    }

    void setViewportSize(uint32_t w, uint32_t h) {
        ww = w;
        wh = h;
        zb.resize(w, h);
        fb.resize(w, h);
    }
    friend class Application;
};
