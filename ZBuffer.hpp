#pragma once

#include "Point2D.hpp"

#include <limits>
#include <memory>

class ZBuffer
{
private:
    uint32_t h, w;
    double *buffer;
public:
    ZBuffer(uint32_t wh, uint32_t ww):
    h(wh), w(ww)
    {
        buffer = new double[h * w];
        flush();
    }

    ~ZBuffer() {
        delete[] buffer;
    }

    bool fill_at(uint32_t x, uint32_t y, double z) {
        if (buffer[x * w + y] > z) {
            buffer[x * w + y] = z;
            return true;
        }
        return false;
    }

    void flush() {
        std::fill(buffer, buffer + h * w, std::numeric_limits<double>::infinity());
    }

    void resize(uint32_t new_w, uint32_t new_h) {
        w = new_w;
        h = new_h;
        delete[] buffer;
        buffer = new double[h * w];
        flush();
    }
};
