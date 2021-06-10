#pragma once

#include "Point3D.hpp"
#include "glm/glm.hpp"

#include <vector>
#include <stdexcept>

class Object3D
{
public:
    std::vector<glm::dvec3> vertices;
    std::vector<std::vector<uint32_t>> faces;
    std::vector<glm::dvec3> face_normals;
    std::vector<std::vector<glm::dvec3>> vertex_normals;
    bool has_vertex_normals;
public:
    Object3D(const std::vector<glm::dvec3>& v, const std::vector<std::vector<uint32_t>>& f, const std::vector<std::vector<glm::dvec3>>& v_norm):
    vertices(v), faces(f), vertex_normals(v_norm), has_vertex_normals(true)
    {
        face_normals.reserve(faces.size());
        for (const std::vector<uint32_t>& f : faces) {
            face_normals.push_back(glm::normalize(glm::cross(vertices[f[2]] - vertices[f[1]], vertices[f[0]] - vertices[f[1]])));
        }
    }

    Object3D(std::vector<glm::dvec3>&& v, std::vector<std::vector<uint32_t>>&& f, std::vector<std::vector<glm::dvec3>>&& v_norm):
    vertices(std::move(v)), faces(std::move(f)), vertex_normals(std::move(v_norm)), has_vertex_normals(true)
    {
        face_normals.reserve(faces.size());
        for (const std::vector<uint32_t>& f : faces) {
            face_normals.push_back(glm::normalize(glm::cross(vertices[f[2]] - vertices[f[1]], vertices[f[0]] - vertices[f[1]])));
        }
    }

    Object3D(const std::vector<glm::dvec3>& v, const std::vector<std::vector<uint32_t>>& f):
    vertices(v), faces(f), has_vertex_normals(false)
    {
        face_normals.reserve(faces.size());
        vertex_normals.reserve(faces.size());
        for (const std::vector<uint32_t>& f : faces) {
            face_normals.push_back(glm::normalize(glm::cross(vertices[f[2]] - vertices[f[1]], vertices[f[0]] - vertices[f[1]])));
            vertex_normals.emplace_back(f.size(), face_normals.back());
        }
        
    }

    Object3D(std::vector<glm::dvec3>&& v, std::vector<std::vector<uint32_t>>&& f):
    vertices(std::move(v)), faces(std::move(f)), has_vertex_normals(false)
    {
        face_normals.reserve(faces.size());
        vertex_normals.reserve(faces.size());
        for (const std::vector<uint32_t>& f : faces) {
            face_normals.push_back(glm::normalize(glm::cross(vertices[f[2]] - vertices[f[1]], vertices[f[0]] - vertices[f[1]])));
            vertex_normals.emplace_back(f.size(), face_normals.back());
        }
        
    }

};
