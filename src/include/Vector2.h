#ifndef VECTOR2_H
#define VECTOR2_H

#include <iostream>
#include <cmath>

namespace H {

    template <typename T>
    class Vector2 {
    public:
        T x, y;

        Vector2() : x(0), y(0) {}

        Vector2(T x, T y) : x(x), y(y) {}

        Vector2& operator=(const Vector2& other) {
            if (this != &other) {
                x = other.x;
                y = other.y;
            }
            return *this;
        }

        bool operator==(const Vector2& other) const {
            return x == other.x && y == other.y;
        }

        bool operator!=(const Vector2& other) const {
            return !(*this == other);
        }

        Vector2 operator+(const Vector2& other) const {
            return Vector2(x + other.x, y + other.y);
        }

        Vector2 operator-(const Vector2& other) const {
            return Vector2(x - other.x, y - other.y);
        }

        Vector2 operator*(T scalar) const {
            return Vector2(x * scalar, y * scalar);
        }

        T magnitude() const {
            return std::sqrt(x * x + y * y);
        }

        Vector2 normalize() const {
            T mag = magnitude();
            return (mag != 0) ? Vector2(x / mag, y / mag) : Vector2(0, 0);
        }

        friend std::ostream& operator<<(std::ostream& os, const Vector2& vec) {
            os << "(" << vec.x << ", " << vec.y << ")";
            return os;
        }
    };

    using Vector2I = Vector2<int>;
    using Vector2F = Vector2<float>;
    using Vector2D = Vector2<double>;
}
#endif // VECTOR2_H

