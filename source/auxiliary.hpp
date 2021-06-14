#pragma once

#include "glm/glm.hpp"

#include <cmath>
#include <iostream>
#include <string>

inline std::ostream& operator<<(std::ostream& os, glm::dvec3 v) {
    return os << v.x << " " << v.y << " " << v.z;
}

inline std::ostream& operator<<(std::ostream& os, glm::dvec4 v) {
    return os << v.x << " " << v.y << " " << v.z << " " << v.w;
}

inline std::ostream& operator<<(std::ostream& os, glm::dvec2 v) {
    return os << v.x << " " << v.y;
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    for (const T& el : v) {
        os << "(" << el << ")" << " ";
    }
    return os;
}

inline double sign(double x) {
    return (x > 0) - (x < 0);
}

inline std::ostream& operator<<(std::ostream& out, const glm::dmat4& m) {
    out << m[0][0] << " " << m[1][0] << " " << m[2][0] << " " << m[3][0] << "\n";
    out << m[0][1] << " " << m[1][1] << " " << m[2][1] << " " << m[3][1] << "\n";
    out << m[0][2] << " " << m[1][2] << " " << m[2][2] << " " << m[3][2] << "\n";
    out << m[0][3] << " " << m[1][3] << " " << m[2][3] << " " << m[3][3];
}

inline std::ostream& operator<<(std::ostream& out, const glm::dmat3& m) {
    out << m[0][0] << " " << m[1][0] << " " << m[2][0] << "\n";
    out << m[0][1] << " " << m[1][1] << " " << m[2][1] << "\n";
    out << m[0][2] << " " << m[1][2] << " " << m[2][2];
}

// bool equal(const glm::dvec3& a, const glm::dvec3& b) {
//     return a == b;
// }

// bool cmp(const glm::dvec3& a, const glm::dvec3& b) {
//     if (a.x != b.x) {
//         return a.x < b.x;
//     }
//     if (a.y != b.y) {
//         return a.y < b.y;
//     }
//     if (a.z != b.z) {
//         return a.z < b.z;
//     }
//     return false;
// }


