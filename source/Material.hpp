#pragma once

#include "Texture.hpp"

#include "glm/glm.hpp"

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <ios>
#include <iostream>
#include <unordered_map>

class Material
{
private:
    glm::dvec3 Ka, Kd, Ks, Ke;
    std::string name;
    double Ns;
    uint32_t illum;
    Texture map_Ka, map_Kd, map_Ks;
public:
    Material(
        std::string name = "unnamed", 
        glm::dvec3 Ka = {1., 1., 1.}, 
        glm::dvec3 Kd = {1., 1., 1.}, 
        glm::dvec3 Ks = {1., 1., 1.}, 
        glm::dvec3 Ke = {0., 0., 0.}, 
        double Ns = 1., 
        uint8_t illum = 2,
        Texture map_Ka = Texture(),
        Texture map_Kd = Texture(),
        Texture map_Ks = Texture()):
    name(name), Ka(Ka), Kd(Kd), Ks(Ks), Ke(Ke), Ns(Ns), illum(illum), map_Ka(map_Ka), map_Kd(map_Kd), map_Ks(map_Ks)
    {}

    glm::dvec3 get_Ka() const {
        return Ka;
    }

    glm::dvec3 get_Kd() const {
        return Kd;
    }

    glm::dvec3 get_Ks() const {
        return Ks;
    }

    glm::dvec3 get_Ke() const {
        return Ke;
    }

    double get_Ns() const {
        return Ns;
    }

    glm::dvec3 get_map_Ka_at(glm::dvec2 uv) const {
        return map_Ka.getPixelUV_d(uv.x, uv.y);
    }

    glm::dvec3 get_map_Kd_at(glm::dvec2 uv) const {
        return map_Kd.getPixelUV_d(uv.x, uv.y);
    }

    glm::dvec3 get_map_Ks_at(glm::dvec2 uv) const {
        return map_Ks.getPixelUV_d(uv.x, uv.y);
    }

    static std::unordered_map<std::string, Material> fromFile(const std::string path) {
        std::ifstream file(path);
        size_t slash_occ = path.rfind('/');
        if (slash_occ == std::string::npos) {
            slash_occ = 0;
        }
        std::string dirname = path.substr(0, slash_occ) + "/";
        if (!file.good()) {
            throw std::ios_base::failure("Invalid material file");
        }

        std::unordered_map<std::string, Material> materials;
        std::string line, kw;
        glm::dvec3 Ka(1.), Kd(1.), Ks(1.), Ke(0.);
        double Ns = 10.;
        uint32_t illum = 2;
        Texture map_Ka, map_Kd, map_Ks;
        std::string name;
        std::getline(file, line, file.widen('\n'));
        std::stringstream ss(line);
        ss >> kw;
        while (!file.eof()) {
            if (kw[0] == '#') {
                std::getline(file, line, file.widen('\n'));
                // if (line.size() == 0) {
                //     continue;
                // }
                ss.clear();
                ss.str(line);
                ss >> kw;
                continue;
            }
            if (kw == "newmtl") {
                ss >> name;
                do {
                    std::getline(file, line, file.widen('\n'));
                    if (line.size() == 0) {
                        continue;
                    }
                    ss.clear();
                    ss.str(line);
                    ss >> kw;
                    if (kw[0] == '#') {
                        continue;
                    }
                    if (kw == "Ns") {
                        ss >> Ns;
                    }
                    else if (kw == "Ka") {
                        ss >> Ka.x >> Ka.y >> Ka.z;
                    }
                    else if (kw == "Kd") {
                        ss >> Kd.x >> Kd.y >> Kd.z;
                    }
                    else if (kw == "Ks") {
                        ss >> Ks.x >> Ks.y >> Ks.z;
                    }
                    else if (kw == "Ke") {
                        ss >> Ke.x >> Ke.y >> Ke.z;
                    }
                    else if (kw == "illum") {
                        ss >> illum;
                    }
                    else if (kw == "map_Ka") {
                        std::string t_file;
                        ss >> t_file;
                        map_Ka = Texture(dirname + t_file);
                    }
                    else if (kw == "map_Kd") {
                        std::string t_file;
                        ss >> t_file;
                        map_Kd = Texture(dirname + t_file);
                    }
                    else if (kw == "map_Ks") {
                        std::string t_file;
                        ss >> t_file;
                        map_Ks = Texture(dirname + t_file);
                    }
                } while (kw != "newmtl" && !file.eof());
                materials.emplace(name, Material(name, Ka, Kd, Ks, Ke, Ns, illum, map_Ka, map_Kd, map_Ks));
            }
        }
        file.close();
        return materials;
    }

    friend std::ostream& operator<<(std::ostream& out, const Material& m) {
        out << "name: " << m.name << "\n";
        out << "Ka: " << m.Ka.x << " " << m.Ka.y << " " << m.Ka.z << "\n";
        out << "Kd: " << m.Kd.x << " " << m.Kd.y << " " << m.Kd.z << "\n";
        out << "Ks: " << m.Ks.x << " " << m.Ks.y << " " << m.Ks.z << "\n";
        out << "Ke: " << m.Ke.x << " " << m.Ke.y << " " << m.Ke.z << "\n";
        out << "Ns: " << m.Ns << "\n";
        out << "map_Kd: " << m.map_Kd << "\n";
        out << "illum: " << m.illum;
        return out;
    }
};
