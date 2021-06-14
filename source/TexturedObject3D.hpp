#pragma once

#include "Material.hpp"
#include "Object3D.hpp"
#include "Texture.hpp"

#include <cstdio>
#include <fstream>
#include <ios>
#include <sstream>
#include <unordered_set>
#include <vector>

#include <limits>

#include "glm/glm.hpp"

class TexturedObject3D
{
private:
    std::string name;
    Object3D object;
    std::vector<std::vector<glm::dvec2>> textCoord;
    bool has_text_coord;
    std::unordered_map<std::string, Material> materials;
    std::vector<std::string> face_materials;
public:
    TexturedObject3D(Object3D obj, std::string name, std::vector<std::vector<glm::dvec2>>& coord, std::unordered_map<std::string, Material>& mats, std::vector<std::string>& f_mats):
    object(obj), textCoord(coord), name(name), materials(mats), face_materials(f_mats), has_text_coord(true)
    {}

    TexturedObject3D(Object3D obj, std::string name, std::vector<std::vector<glm::dvec2>>& coord):
    object(obj), textCoord(coord), name(name), face_materials(obj.faces.size(), "default"), has_text_coord(true)
    {
        materials["default"] = Material();
    }

    TexturedObject3D(Object3D obj, std::string name, std::unordered_map<std::string, Material>& mats, std::vector<std::string>& f_mats):
    object(obj), textCoord(), name(name), materials(mats), face_materials(f_mats), has_text_coord(false)
    {}

    TexturedObject3D(Object3D obj, std::string name):
    object(obj), textCoord(), name(name), face_materials(obj.faces.size(), "default"), has_text_coord(false)
    {
        materials["default"] = Material();
    }

    const std::vector<glm::dvec3>& vertices() const {
        return object.vertices;
    }

    const std::vector<glm::dvec3>& f_normals() const {
        return object.face_normals;
    }

    const std::vector<std::vector<glm::dvec3>>& v_normals() const {
        return object.vertex_normals;
    }

    const std::vector<std::vector<uint32_t>>& faces() const {
        return object.faces;
    }

    bool has_tc() const {
        return has_text_coord;
    }

    const std::vector<std::vector<glm::dvec2>>& textureCoordinates() const {
        return textCoord;
    }

    std::vector<glm::dvec2> triangleTextureCoordinates(uint32_t i) const {
        if (i >= textCoord.size()) {
            throw std::out_of_range("Invalid texture coordinates index");
        }
        return textCoord[i];
    }

    const std::unordered_map<std::string, Material>& mats() const {
        return materials;
    }

    const std::vector<std::string>& f_mats() const {
        return face_materials;
    }

    static std::vector<TexturedObject3D> fromFile(std::string filename) {
        std::ifstream file(filename);
        size_t slash_occ = filename.rfind('/');
        if (slash_occ == std::string::npos) {
            slash_occ = 0;
        }
        std::string dirname = filename.substr(0, slash_occ) + "/";
        if (!file.good()) {
            throw std::ios_base::failure("Invalid object file");
        }

        std::string kw, line;
        std::vector<glm::dvec3> vert, vert_normals;
        std::vector<glm::dvec2> text_vert;
        std::vector<std::vector<uint32_t>> faces;
        std::vector<std::vector<glm::dvec2>> tc;
        std::vector<std::vector<glm::dvec3>> faces_norm;
        std::string name, mat_name;
        std::unordered_map<std::string, Material> materials;
        std::vector<TexturedObject3D> objects;
        std::vector<std::string> f_mats;
        bool use_texture = false, use_v_normals = false, o_flag = false, kw_flag = false;
        // double x_max = std::numeric_limits<double>::min(), 
        //         x_min = std::numeric_limits<double>::max(), 
        //         y_max = std::numeric_limits<double>::min(), 
        //         y_min = std::numeric_limits<double>::max(), 
        //         z_max = std::numeric_limits<double>::min(), 
        //         z_min = std::numeric_limits<double>::max();
        std::stringstream ss;
        while (!file.eof()) {
            if (!kw_flag) {
                std::getline(file, line, file.widen('\n'));
                ss.clear();
                ss.str(line);
                ss >> kw;
            }
            if (line.size() == 0) {
                continue;
            }
            kw_flag = false;
            if (kw[0] == '#') {
                continue;
            }
            if (kw == "mtllib") {
                std::string mtl;
                ss >> mtl;
                materials.merge(Material::fromFile(dirname + mtl));
            }
            // Read vertex
            if (kw == "v") {
                glm::dvec3 v;
                ss >> v.x >> v.y >> v.z;
                // x_max = std::max(x_max, v.x);
                // x_min = std::min(x_min, v.x);
                // y_max = std::max(y_max, v.y);
                // y_min = std::min(y_min, v.y);
                // z_max = std::max(z_max, v.z);
                // z_min = std::min(z_min, v.z);
                vert.push_back(v);
            }
            // Read texture vertex
            else if (kw == "vt") {
                use_texture = true;
                double u, v;
                ss >> u >> v;
                text_vert.push_back({1. - v, u});
            }
            // Read vertex normal
            else if (kw == "vn") {
                use_v_normals = true;
                glm::dvec3 n;
                ss >> n.x >> n.y >> n.z;
                vert_normals.push_back(glm::normalize(n));
            }
            if (kw == "o") {
                kw_flag = true;
                std::unordered_map<uint32_t, uint32_t> obj_vert_map, obj_v_norm_map;
                std::vector<glm::dvec3> obj_vert, obj_v_norm;
                faces.clear();
                tc.clear();
                faces_norm.clear();
                f_mats.clear();
                o_flag = true;
                ss >> name;
                Material mat;
                do {
                    std::getline(file, line, file.widen('\n'));
                    if (line.size() == 0) {
                        kw = "";
                        continue;
                    }
                    ss.clear();
                    ss.str(line);
                    ss >> kw;
                    // Read vertex
                    if (kw == "v") {
                        glm::dvec3 v;
                        ss >> v.x >> v.y >> v.z;
                        // x_max = std::max(x_max, v.x);
                        // x_min = std::min(x_min, v.x);
                        // y_max = std::max(y_max, v.y);
                        // y_min = std::min(y_min, v.y);
                        // z_max = std::max(z_max, v.z);
                        // z_min = std::min(z_min, v.z);
                        vert.push_back(v);
                    }
                    // Read texture vertex
                    else if (kw == "vt") {
                        use_texture = true;
                        double u, v;
                        ss >> u >> v;
                        text_vert.push_back({1. - v, u});
                    }
                    // Read vertex normal
                    else if (kw == "vn") {
                        use_v_normals = true;
                        glm::dvec3 n;
                        ss >> n.x >> n.y >> n.z;
                        vert_normals.push_back(glm::normalize(n));
                    }
                    // Read material
                    if (kw == "usemtl") {
                        ss >> mat_name;
                        mat = materials[mat_name];
                    }
                    // Read face
                    else if (kw == "f") {
                        int v = 0, t = 0, n = 0;
                        std::string s;
                        f_mats.push_back(mat_name);
                        std::vector<uint32_t> face_vert;
                        std::vector<glm::dvec2> face_tc;
                        std::vector<glm::dvec3> face_norm;
                        while (!ss.eof()) {
                            ss >> s;
                            if (use_v_normals && use_texture) {
                                std::sscanf(s.c_str(), "%d/%d/%d", &v, &t, &n);
                            }
                            else if (use_v_normals) {
                                std::sscanf(s.c_str(), "%d//%d", &v, &n);
                            }
                            else if (use_texture) {
                                std::sscanf(s.c_str(), "%d/%d", &v, &t);
                            }
                            else {
                                std::sscanf(s.c_str(), "%d", &v);
                            }
                            v += (v < 0) * vert.size() - (v > 0);
                            t += (t < 0) * vert.size() - (t > 0);
                            n += (n < 0) * vert.size() - (n > 0);
                            if (obj_vert_map.find(v) == obj_vert_map.end()) {
                                obj_vert_map[v] = obj_vert_map.size();
                                obj_vert.push_back(vert[v]);
                            }
                            face_vert.push_back(obj_vert_map[v]);
                            if (use_texture) {
                                face_tc.push_back(text_vert[t]);
                            }
                            if (use_v_normals) {
                                face_norm.push_back(vert_normals[n]);
                                if (obj_v_norm_map.find(n) == obj_v_norm_map.end()) {
                                    obj_v_norm_map[n] = obj_v_norm_map.size();
                                    obj_v_norm.push_back(vert_normals[n]);
                                }
                            }
                        }
                        faces.push_back(face_vert);
                        if (use_texture) {
                            tc.push_back(face_tc);
                        }
                        if (use_v_normals) {
                            faces_norm.push_back(face_norm);
                        }

                    }
                } while (kw != "o" && !file.eof());
                if (use_v_normals && use_texture) {
                    objects.emplace_back(Object3D(obj_vert, faces, faces_norm), name, tc, materials, f_mats);
                }
                else if (use_texture) {
                    objects.emplace_back(Object3D(obj_vert, faces), name, tc, materials, f_mats);
                }
                else if (use_v_normals) {
                    objects.emplace_back(Object3D(obj_vert, faces, faces_norm), name, materials, f_mats);
                }
                else {
                    objects.emplace_back(Object3D(obj_vert, faces), name, materials, f_mats);
                }
            }
        }
        if (!o_flag) {
            if (use_v_normals) {
                objects.emplace_back(Object3D(vert, faces, faces_norm), name, tc, materials, f_mats);
            }
            else {
                objects.emplace_back(Object3D(vert, faces), name, tc, materials, f_mats);
            }
        }
        file.close();
        // std::cout << x_min << " " << x_max << std::endl;
        // std::cout << y_min << " " << y_max << std::endl;
        // std::cout << z_min << " " << z_max << std::endl;
        return objects;
    }
};

