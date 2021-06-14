#pragma once

#include "glm/glm.hpp"

enum class LightSourceType {Ambient, Directional, Point};

class LightSource {
protected:
    LightSourceType type;
    glm::dvec3 color;
    
public:
    LightSource(LightSourceType t, glm::dvec3 c):
    type(t), color(c)
    {}

    LightSource(LightSourceType t, uint32_t c):
    type(t), color({(c >> 24) / 255., ((c >> 16) & 0xff) / 255., ((c >> 8) & 0xff) / 255.})
    {}

    virtual glm::dvec3 colorAt(glm::dvec3 pos, glm::dvec3 point) {
        return color;
    }

    virtual glm::dvec3 dirTo(glm::dvec3 pos, glm::dvec3 point) {
        return {0., 0., 0.};
    }

    LightSourceType getType() const {
        return type;
    }

    virtual void transform(glm::dmat3 m) {}
};

class PointLight : public LightSource {
private:
    double k_c, k_l, k_q;

public:
    PointLight(glm::dvec3 c, double kc, double kl, double kq):
    LightSource(LightSourceType::Point, c), k_c(kc), k_l(kl), k_q(kq)
    {}

    PointLight(uint32_t c, double kc, double kl, double kq):
    LightSource(LightSourceType::Point, c), k_c(kc), k_l(kl), k_q(kq)
    {}

    glm::dvec3 colorAt(glm::dvec3 pos, glm::dvec3 point) override {
        double d = glm::distance(pos, point);
        return color / (k_c + d * (k_l + k_q * d));
    }

    glm::dvec3 dirTo(glm::dvec3 pos, glm::dvec3 point) override {
        return glm::normalize(pos - point);
    }
};

class AmbientLight : public LightSource {
public:
    AmbientLight(glm::dvec3 c):
    LightSource(LightSourceType::Ambient, c)
    {}

    AmbientLight(uint32_t c):
    LightSource(LightSourceType::Ambient, c)
    {}
};

class DirectionalLight : public LightSource {
private:
    glm::dvec3 direction;
    glm::dvec3 trans_dir;

public:
    DirectionalLight(glm::dvec3 c, glm::dvec3 dir):
    LightSource(LightSourceType::Directional, c), direction(dir)
    {}

    DirectionalLight(uint32_t c, glm::dvec3 dir):
    LightSource(LightSourceType::Directional, c), direction(dir)
    {}

    glm::dvec3 colorAt(glm::dvec3 pos, glm::dvec3 point) override {
        return color;
    }

    glm::dvec3 dirTo(glm::dvec3 pos, glm::dvec3 point) override {
        return glm::normalize(-trans_dir);
    }

    void transform(glm::dmat3 m) override {
        trans_dir = m * direction;
    }
};