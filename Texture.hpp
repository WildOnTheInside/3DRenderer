#pragma once

#include "SFML/Graphics.hpp"

class Texture
{
private:
    sf::Image image;
    uint32_t w, h;
public:
    Texture(const std::string& path) {
        image.loadFromFile(path);
        auto size = image.getSize();
        w = size.x;
        h = size.y;
    }

    Texture(const sf::Color& color):
    w(1), h(1)
    {
        image.create(w, h, color);
    }

    Texture(const uint32_t color):
    Texture(sf::Color(color))
    {}

    Texture():
    Texture(sf::Color::White)
    {}

    uint32_t getPixelUV(double u, double v) const {
        return image.getPixel(std::min((uint32_t)(v * w), w - 1), std::min((uint32_t)(u * h), h - 1)).toInteger();
    }

    glm::dvec3 getPixelUV_d(double u, double v) const {
        uint32_t c = getPixelUV(u, v);
        return {(c >> 24) / 255., ((c >> 16) & 0xff) / 255., ((c >> 8) & 0xff) / 255.};
    }
};
