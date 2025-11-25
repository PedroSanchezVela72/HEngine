#pragma once

#ifndef COLOR_H
#define COLOR_H

#include <iostream>
#include <cmath>

namespace H {

    class Color {
    public:
        // Componentes RGBA [0, 1]
        float r, g, b, a;

        Color() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}
        Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}

        // Métodos de utilidad
        inline void print() const {
            std::cout << "Color(R: " << r << ", G: " << g << ", B: " << b << ", A: " << a << ")" << std::endl;
        }

        // Mezclar dos colores 
        static inline Color lerp(const Color& start, const Color& end, float t) {
            t = (t < 0.0f) ? 0.0f : (t > 1.0f) ? 1.0f : t;
            return Color(
                start.r + (end.r - start.r) * t,
                start.g + (end.g - start.g) * t,
                start.b + (end.b - start.b) * t,
                start.a + (end.a - start.a) * t
            );
        }

        // Normalización para valores fuera de 0-1
        inline void normalize() {
            r = (r < 0.0f) ? 0.0f : (r > 1.0f) ? 1.0f : r;
            g = (g < 0.0f) ? 0.0f : (g > 1.0f) ? 1.0f : g;
            b = (b < 0.0f) ? 0.0f : (b > 1.0f) ? 1.0f : b;
            a = (a < 0.0f) ? 0.0f : (a > 1.0f) ? 1.0f : a;
        }

        // Operadores para operaciones con colores 
        inline Color operator+(const Color& other) const {
            return Color(r + other.r, g + other.g, b + other.b, a + other.a);
        }

        inline Color operator*(float scalar) const {
            return Color(r * scalar, g * scalar, b * scalar, a * scalar);
        }

        // Método para obtener la distancia entre dos colores 
        /*inline float DistanceTo(const Color& other) const {
            float dr = r - other.r;
            float dg = g - other.g;
            float db = b - other.b;
            float da = a - other.a;
            return std::sqrt(dr * dr + dg * dg + db * db + da * da);
        }*/

#pragma region Color Defaults

        static const Color white() {
            return Color(1.0f, 1.0f, 1.0f, 1.0f);
        }

        static const Color red() {
            return Color(1.0f, 0.0f, 0.0f, 1.0f);
        }
        static const Color green() {
            return Color(0.0f, 1.0f, 0.0f, 1.0f);
        }
        static const Color blue() {
            return Color(0.0f, 0.0f, 1.0f, 1.0f);
        }
        static const Color black() {
            return Color(0.0f, 0.0f, 0.0f, 1.0f);
        }
        static const Color transparent() {
            return Color(0.0f, 0.0f, 0.0f, 0.0f);
        }
        static  const Color yellow() {
            return Color(1.0f, 1.0f, 0.0f, 1.0f);
        }
        static  const Color cyan() {
            return Color(0.0f, 1.0f, 1.0f, 1.0f);
        }
        static const Color magenta() {
            return Color(1.0f, 0.0f, 1.0f, 1.0f);
        }
        static const Color gray() {
            return Color(0.5f, 0.5f, 0.5f, 1.0f);
        }
        static const Color lightGray() {
            return Color(0.75f, 0.75f, 0.75f, 1.0f);
        }
        static const Color darkGray() {
            return Color(0.25f, 0.25f, 0.25f, 1.0f);
        }
        static const Color orange() {
            return Color(1.0f, 0.647f, 0.0f, 1.0f);
        }
        static const Color purple() {
            return Color(0.5f, 0.0f, 0.5f, 1.0f);
        }
        static const Color brown() {
            return Color(0.6f, 0.3f, 0.1f, 1.0f);
        }
#pragma endregion

    };
}
#endif // COLOR_H
