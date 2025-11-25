#ifndef VECTOR3_H
#define VECTOR3_H

#include <iostream> 
#include "H_Defs.h"

namespace H {

    template <typename T>
    class Vector3 {
    public:
        T x, y, z;

        Vector3() : x(0), y(0), z(0) {}

        Vector3(T x, T y, T z) : x(x), y(y), z(z) {}

        inline Vector3& operator=(const Vector3& other) {
            if (this != &other) {
                x = other.x;
                y = other.y;
                z = other.z;
            }
            return *this;
        }

        inline bool operator==(const Vector3& other) const {
            return x == other.x && y == other.y && z == other.z;
        }

        inline bool operator!=(const Vector3& other) const {
            return !(*this == other);
        }

        inline Vector3 operator+(const Vector3& other) const {
            return Vector3(x + other.x, y + other.y, z + other.z);
        }

        inline Vector3 operator-(const Vector3& other) const {
            return Vector3(x - other.x, y - other.y, z - other.z);
        }

        inline Vector3 operator*(T scalar) const {
            return Vector3(x * scalar, y * scalar, z * scalar);
        }

        inline Vector3 operator*(const Vector3& other) {  
            return Vector3(x * other.x, y * other.y, z * other.z);     
        } 

        inline Vector3 cross(const Vector3& other) const {
            return Vector3(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
            );
        }

        inline T magnitude() const {
            return std::sqrt(x * x + y * y + z * z);
        }

        inline Vector3 normalize() const {
            T mag = magnitude();
            return (mag != 0) ? Vector3(x / mag, y / mag, z / mag) : Vector3(0, 0, 0);
        }

        inline Vector3<T> toRadians() const {
            return Vector3<T>(x * (H_PI / 180.0), y * (H_PI / 180.0), z * (H_PI / 180.0));  
        }

        friend std::ostream& operator<<(std::ostream& os, const Vector3& vec) {
            os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
            return os;
        }

        inline T dot(const Vector3& other) const {
            return x * other.x + y * other.y + z * other.z;
        }

        // Basics
        static constexpr Vector3<T> up() { 
            return Vector3<T>(0, 1, 0); 
        }
        static constexpr Vector3<T> down() { 
            return Vector3<T>(0, -1, 0); 
        }
        static constexpr Vector3<T> left() { 
            return Vector3<T>(-1, 0, 0); 
        }
        static constexpr Vector3<T> right() {
            return Vector3<T>(1, 0, 0);
        }
        static constexpr Vector3<T> forward() { 
            return Vector3<T>(0, 0, 1);
        }
        static constexpr Vector3<T> back() {
            return Vector3<T>(0, 0, -1);
        }
        static constexpr Vector3<T> zero() { 
            return Vector3<T>(0, 0, 0);
        }
        static constexpr Vector3<T> one() { 
            return Vector3<T>(1, 1, 1);
        }
    };

    using Vector3I = Vector3<int>;
    using Vector3F = Vector3<float>;
    using Vector3D = Vector3<double>;
}
#endif // VECTOR3_H
