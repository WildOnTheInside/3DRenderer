#pragma once

#include "Object3D.hpp"
#include "Texture.hpp"
#include "Material.hpp"

#include <vector>
#include <fstream>
#include <sstream>
#include <ios>
#include <cstdio>

#include "glm/glm.hpp"

class TexturedObject3D
{
private:
    std::string name;
    Object3D object;
    Texture texture;
    std::vector<std::vector<glm::dvec2>> textCoord;
    std::unordered_map<std::string, Material> materials;
    std::vector<std::string> face_materials;
public:
    TexturedObject3D(Object3D obj, std::string name, Texture text, std::vector<std::vector<glm::dvec2>>& coord, std::unordered_map<std::string, Material>& mats, std::vector<std::string>& f_mats):
    object(obj), texture(text), textCoord(coord), name(name), materials(mats), face_materials(f_mats)
    {}

    TexturedObject3D(Object3D obj, std::string name, Texture text, std::vector<std::vector<glm::dvec2>>& coord):
    object(obj), texture(text), textCoord(coord), name(name), face_materials(obj.faces.size(), "default")
    {
        materials["default"] = Material();
    }

    bool has_vertex_normals() const {
        return object.has_vertex_normals;
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

    const std::vector<std::vector<glm::dvec2>>& textureCoordinates() const {
        return textCoord;
    }

    std::vector<glm::dvec2> triangleTextureCoordinates(uint32_t i) const {
        if (i < 0 || i >= textCoord.size()) {
            throw std::out_of_range("");
        }
        return textCoord[i];
    }

    const std::unordered_map<std::string, Material>& mats() const {
        return materials;
    }

    const std::vector<std::string>& f_mats() const {
        return face_materials;
    }

    const Texture& getTexture() const {
        return texture;
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
        bool use_texture = false, use_v_normals = false, o_flag = false;
        Texture tex(sf::Color::White);
        while (!file.eof()) {
            std::getline(file, line, file.widen('\n'));
            if (line.size() == 0) {
                continue;
            }
            std::stringstream ss(line);
            ss >> kw;
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
                vert.push_back(v);
            }
            // Read texture vertex
            else if (kw == "vt") {
                use_texture = true;
                glm::dvec2 uv;
                ss >> uv.x >> uv.y;
                text_vert.push_back(uv);
            }
            // Read vertex normal
            else if (kw == "vn") {
                use_v_normals = true;
                glm::dvec3 n;
                ss >> n.x >> n.y >> n.z;
                vert_normals.push_back(glm::normalize(n));
            }
            if (kw == "o") {
                std::cout << "o" << std::endl;
                o_flag = true;
                ss >> name;
                Material mat;
                do {
                    std::getline(file, line, file.widen('\n'));
                    if (line.size() == 0) {
                        continue;
                    }
                    ss.clear();
                    ss.str(line);
                    ss >> kw;
                    // Read vertex
                    if (kw == "v") {
                        glm::dvec3 v;
                        ss >> v.x >> v.y >> v.z;
                        vert.push_back(v);
                    }
                    // Read texture vertex
                    else if (kw == "vt") {
                        use_texture = true;
                        glm::dvec2 uv;
                        ss >> uv.x >> uv.y;
                        text_vert.push_back(uv);
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
                        if (use_v_normals && use_texture) {
                            int v, t, n;
                            std::string s;
                            f_mats.push_back(mat_name);
                            std::vector<uint32_t> face_vert;
                            std::vector<glm::dvec2> face_tc;
                            std::vector<glm::dvec3> face_norm;
                            while (!ss.eof()) {
                                ss >> s;
                                std::sscanf(s.c_str(), "%d/%d/%d", &v, &t, &n);
                                v += (v < 0) * vert.size() - (v > 0);
                                t += (t < 0) * vert.size() - (t > 0);
                                n += (n < 0) * vert.size() - (n > 0);
                                face_vert.push_back(v);
                                face_tc.push_back(text_vert[t]);
                                face_norm.push_back(vert_normals[n]);
                            }
                            faces.push_back(face_vert);
                            tc.push_back(face_tc);
                            faces_norm.push_back(face_norm);
                        }
                        else if (use_texture) {
                            int v, t;
                            std::string s;
                            std::vector<uint32_t> face_vert;
                            std::vector<glm::dvec2> face_tc;
                            while (!ss.eof()) {
                                ss >> s;
                                std::sscanf(s.c_str(), "%d/%d", &v, &t);
                                v += (v < 0) * vert.size() - (v > 0);
                                t += (t < 0) * vert.size() - (t > 0);
                                face_vert.push_back(v);
                                face_tc.push_back(text_vert[t]);
                            }
                            faces.push_back(face_vert);
                            tc.push_back(face_tc);
                        }
                        else if (use_v_normals) {
                            int v, n;
                            std::string s;
                            std::vector<uint32_t> face_vert;
                            std::vector<glm::dvec3> face_norm;
                            while (!ss.eof()) {
                                ss >> s;
                                std::sscanf(s.c_str(), "%d//%d", &v, &n);
                                v += (v < 0) * vert.size() - (v > 0);
                                n += (n < 0) * vert.size() - (n > 0);
                                face_vert.push_back(v);
                                face_norm.push_back(vert_normals[n]);
                            }
                            faces.push_back(face_vert);
                            faces_norm.push_back(face_norm);
                        }
                        else {
                            int v;
                            std::vector<uint32_t> face_vert;
                            while (!ss.eof()) {
                                ss >> v;
                                v += (v < 0) * vert.size() - (v > 0);
                                face_vert.push_back(v);
                            }
                            faces.push_back(face_vert);
                        }
                    }
                } while (kw != "o" && !file.eof());
                if (use_v_normals) {
                    objects.emplace_back(Object3D(vert, faces, faces_norm), name, tex, tc, materials, f_mats);
                }
                else {
                    objects.emplace_back(Object3D(vert, faces), name, tex, tc, materials, f_mats);
                }
            }
        }
        if (!o_flag) {
            if (use_v_normals) {
                objects.emplace_back(Object3D(vert, faces, faces_norm), name, tex, tc, materials, f_mats);
            }
            else {
                objects.emplace_back(Object3D(vert, faces), name, tex, tc, materials, f_mats);
            }
        }
        file.close();
        return objects;
    }
};

