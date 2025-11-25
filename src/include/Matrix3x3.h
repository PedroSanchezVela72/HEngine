#pragma once
#ifndef MATRIX3X3_H
#define  MATRIX3X3_H
#include "Vector3.h"
#include <cmath>
#include <cassert>


namespace H {

    template <typename T>
    class Matrix3x3 {
    public:
        Vector3<T> cols[3]; // columnas: right (X), up (Y), forward (Z)  

        Matrix3x3() {
            cols[0] = Vector3<T>(1, 0, 0);
            cols[1] = Vector3<T>(0, 1, 0);
            cols[2] = Vector3<T>(0, 0, 1);
        }

        Matrix3x3(const Vector3<T>& col0, const Vector3<T>& col1, const Vector3<T>& col2) {
            cols[0] = col0;
            cols[1] = col1;
            cols[2] = col2;
        }

        Vector3<T>& operator[](int i) { return cols[i]; }
        const Vector3<T>& operator[](int i) const { return cols[i]; }

        // Multiplicar por un vector
        Vector3<T> operator*(const Vector3<T>& v) const {
            return cols[0] * v.x + cols[1] * v.y + cols[2] * v.z;
        }

        // Transpuesta (filas como vectores si quieres multiplicar desde la izquierda)
        Matrix3x3<T> transpose() const {
            return Matrix3x3<T>(
                Vector3<T>(cols[0].x, cols[1].x, cols[2].x),
                Vector3<T>(cols[0].y, cols[1].y, cols[2].y),
                Vector3<T>(cols[0].z, cols[1].z, cols[2].z)
            );
        }

        static Matrix3x3<T> fromBasis(const Vector3<T>& right, const Vector3<T>& up, const Vector3<T>& forward) { 
            return Matrix3x3<T>(right, up, forward); 
        }

        static Matrix3x3<T> identity() {
            return Matrix3x3<T>(
                Vector3<T>(1, 0, 0),
                Vector3<T>(0, 1, 0),
                Vector3<T>(0, 0, 1)
            );
        }
    };

    // Atajo para float
    using Matrix3x3F = Matrix3x3<float>;

}
#endif