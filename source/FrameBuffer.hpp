#pragma once

#include "SFML/System.hpp"

#include "glm/glm.hpp"

#include <cstring>
#include <memory>

class FrameBuffer
{
private:
    sf::Uint8 *buffer;
    std::unique_ptr<sf::Uint8[]> b;
    uint32_t h, w;
public:
    FrameBuffer(uint32_t bh, uint32_t bw):
    h(bh), w(bw), b(new sf::Uint8[h * w * 4])
    {
        buffer = new sf::Uint8[h * w * 4];
        flush();
    }

    ~FrameBuffer() {
        delete[] buffer;
    }

    sf::Uint8* ptr_to(uint32_t x, uint32_t y) {
        return buffer + 4 * (x * w + y);
    }

    const sf::Uint8* get_buffer() const {
        return buffer;
    }

    void fill_at(uint32_t x, uint32_t y, uint32_t value) {
        uint32_t value_rev = (value << 24) + ((value << 8) & 0x00ff0000) + ((value >> 8) & 0x0000ff00) + (value >> 24);
        std::memcpy(this->ptr_to(x, y), &value_rev, 4);
    }

    void fill_at_d(uint32_t x, uint32_t y, glm::dvec3 c) {
        uint32_t color = ((uint32_t)(c.x * 255.) << 24) + ((uint32_t)(c.y * 255.) << 16) + ((uint32_t)(c.z * 255.) << 8) + 0xff;
        fill_at(x, y, color);
    }

    void flush() {
        for (uint32_t i = 0; i < h; ++i) {
            for (uint32_t j = 0; j < w; ++j) {
                fill_at(i, j, 0xffffffff);
            }
        }
    }

    void resize(uint32_t new_w, uint32_t new_h) {
        w = new_w;
        h = new_h;
        delete[] buffer;
        buffer = new sf::Uint8[h * w * 4];
        flush();
    }

    std::pair<uint32_t, uint32_t> size() const {
        return {w, h};
    }
};
