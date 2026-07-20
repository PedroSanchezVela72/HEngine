#ifndef QUATERNION_H
#define QUATERNION_H

#include <iostream>
#include <cmath>
#include "Vector3.h"
#include "Matrix3x3.h"

namespace H {

#ifndef H_PI
#define H_PI 3.14159265358979323846
#endif

    template <typename T>
    class Quaternion {
        void updateEuler(Vector3<T> euler) {
            pitch = euler.x;
            yaw = euler.y;
            roll = euler.z;
        }
    public:
        //Si se multiplican quaterniones esto no se actualiza. Actualizarlo vosotros o no usarlos. Es para debug y el inspector.
        T pitch, yaw, roll;
        T w, x, y, z;

        Quaternion() : w(1), x(0), y(0), z(0), pitch(0), yaw(0), roll(0) {}
        Quaternion(T w, T x, T y, T z) : w(w), x(x), y(y), z(z), pitch(0), yaw(0), roll(0) {}

        // Construye a partir de ángulos de Euler
        Quaternion(const Vector3<T>& euler, bool inRadians = false) {
            updateEuler(euler);
            *this = fromEuler(euler, inRadians);
        }

        Quaternion& operator=(const Quaternion& other) {
            if (this != &other) {
                w = other.w; x = other.x; y = other.y; z = other.z; pitch = other.pitch; yaw = other.yaw; roll = other.roll;
            }
            return *this;
        }

        // Comparación exacta
        bool operator==(const Quaternion& other) const {
            return w == other.w && x == other.x && y == other.y && z == other.z;
        }

        bool operator!=(const Quaternion& other) const {
            return !(*this == other);
        }

        // Suma
        Quaternion operator+(const Quaternion& other) const {
            return Quaternion(w + other.w, x + other.x, y + other.y, z + other.z);
        }

        // Resta
        Quaternion operator-(const Quaternion& other) const {
            return Quaternion(w - other.w, x - other.x, y - other.y, z - other.z);
        }

        // Multiplicación por escalar
        Quaternion operator*(T scalar) const {
            return Quaternion(w * scalar, x * scalar, y * scalar, z * scalar);
        }

        // Multiplicación entre quaternions (no actualiza el pitch, yaw ni roll)
        Quaternion operator*(const Quaternion& other) const {
            return Quaternion(
                w * other.w - x * other.x - y * other.y - z * other.z,
                w * other.x + x * other.w + y * other.z - z * other.y,
                w * other.y - x * other.z + y * other.w + z * other.x,
                w * other.z + x * other.y - y * other.x + z * other.w
            );
        }

        Quaternion& operator+=(const Quaternion& other) {
            w += other.w; x += other.x; y += other.y; z += other.z;
            return *this;
        }

        Quaternion& operator-=(const Quaternion& other) {
            w -= other.w; x -= other.x; y -= other.y; z -= other.z;
            return *this;
        }

        Quaternion& operator*=(T scalar) {
            w *= scalar; x *= scalar; y *= scalar; z *= scalar;
            return *this;
        }

        Quaternion& operator*=(const Quaternion& other) {
            *this = *this * other;
            return *this;
        }

        // Acceso por índice: 0=w, 1=x, 2=y, 3=z
        T operator[](int index) const {
            switch (index) {
            case 0: return w;
            case 1: return x;
            case 2: return y;
            case 3: return z;
            default: throw std::out_of_range("Quaternion index out of range");
            }
        }

        // Aplica la rotación a un vector
        Vector3<T> operator*(const Vector3<T>& v) const {
            Quaternion qv(0, v.x, v.y, v.z);
            Quaternion q_conj = conjugate();
            Quaternion result = (*this) * qv * q_conj;
            return Vector3<T>(result.x, result.y, result.z);
        }

        // Magnitud del quaternion
        T magnitude() const {
            return std::sqrt(w * w + x * x + y * y + z * z);
        }

        // Normaliza el quaternion
        Quaternion normalize() const {
            T mag = magnitude();
            return (mag != 0) ? Quaternion(w / mag, x / mag, y / mag, z / mag) : Quaternion(1, 0, 0, 0);
        }

        // Verifica si está normalizado
        bool isNormalized() const {
            return std::abs(magnitude() - 1.0) < 1e-6;
        }

        // Verifica si es la identidad
        bool isIdentity() const {
            return *this == Quaternion(1, 0, 0, 0);
        }

        // Conjugado del quaternion
        Quaternion conjugate() const {
            return Quaternion(w, -x, -y, -z);
        }

        // Inverso del quaternion
        Quaternion inverse() const {
            T magSq = magnitude() * magnitude();
            return (magSq != 0) ? conjugate() * (1 / magSq) : Quaternion(1, 0, 0, 0);
        }

        // Aplica rotación desde ángulos de Euler
        Quaternion rotate(const Vector3<T>& euler, bool inRadians = false) const {
            return fromEuler(euler, inRadians) * (*this);
        }

        // Rota un vector usando el quaternion
        Vector3<T> rotateVector(const Vector3<T>& v) const {
            Vector3<T> qVec(x, y, z);
            Vector3<T> t = qVec.cross(v) * 2.0;
            return v + t * w + qVec.cross(t);
        }

        // Devuelve pitch
        T getPitch(bool inRadians = false) const {
            return pitch;
        }

        // Devuelve yaw
        T getYaw(bool inRadians = false) const {
            return yaw;
        }

        // Devuelve roll
        T getRoll(bool inRadians = false) const {
            return roll;
        }

        // Aplica pitch
        Quaternion setPitch(T angle, bool inRadians = false) {
            if (!inRadians) angle = angle * H_PI / 180;
            T half = angle * 0.5;

            return  Quaternion(std::cos(half), std::sin(half), 0, 0) * (*this);
        }

        // Aplica yaw
        Quaternion setYaw(T angle, bool inRadians = false) {
            if (!inRadians) angle = angle * H_PI / 180;
            T half = angle * 0.5;

            return  Quaternion(std::cos(half), 0, std::sin(half), 0) * (*this);
        }

        // Aplica roll
        Quaternion setRoll(T angle, bool inRadians = false) {
            if (!inRadians) angle = angle * H_PI / 180;
            T half = angle * 0.5;

            return Quaternion(std::cos(half), 0, 0, std::sin(half)) * (*this);
        }

        // Convierte a ángulos de Euler
        Vector3<T> toEuler(bool inRadians = false) const {
            Vector3<T> euler;
            T sinr_cosp = (T)2 * (w * x + y * z);
            T cosr_cosp = (T)1 - (T)2 * (x * x + y * y);
            euler.x = std::atan2(sinr_cosp, cosr_cosp);

            T sinp = (T)2 * (w * y - z * x);
            euler.y = std::abs(sinp) >= 1 ? std::copysign(H_PI / 2, sinp) : std::asin(sinp);

            T siny_cosp = (T)2 * (w * z + x * y);
            T cosy_cosp = (T)1 - (T)2 * (y * y + z * z);
            euler.z = std::atan2(siny_cosp, cosy_cosp);

            if (!inRadians) {
                euler.x *= 180 / H_PI;
                euler.y *= 180 / H_PI;
                euler.z *= 180 / H_PI;
            }

            return euler;
            return Vector3<T>(pitch, yaw, roll);
        }

        // Producto punto
        T dot(const Quaternion& other) const {
            return w * other.w + x * other.x + y * other.y + z * other.z;
        }

        // Interpolación lineal
        static Quaternion lerp(const Quaternion& a, const Quaternion& b, T t) {
            return (a * (1 - t) + b * t).normalize();
        }

        // Interpolación esférica
        static Quaternion slerp(const Quaternion& a, const Quaternion& b, T t) {
            T dotProd = a.dot(b);
            Quaternion b2 = b;

            if (dotProd < 0.0f) {
                dotProd = -dotProd;
                b2 = b * -1;
            }

            if (dotProd > 0.9995f) return lerp(a, b2, t);

            T theta_0 = std::acos(dotProd);
            T theta = theta_0 * t;
            T sin_theta = std::sin(theta);
            T sin_theta_0 = std::sin(theta_0);

            T s0 = std::cos(theta) - dotProd * sin_theta / sin_theta_0;
            T s1 = sin_theta / sin_theta_0;

            return (a * s0 + b2 * s1).normalize();
        }

        // Construye desde Euler
        static Quaternion fromEuler(const Vector3<T>& euler, bool inRadians = false) {
            Vector3<T> rad = inRadians ? euler : euler.toRadians();

            T cx = std::cos(rad.x * 0.5), sx = std::sin(rad.x * 0.5);
            T cy = std::cos(rad.y * 0.5), sy = std::sin(rad.y * 0.5);
            T cz = std::cos(rad.z * 0.5), sz = std::sin(rad.z * 0.5);

            Quaternion q(
                cx * cy * cz + sx * sy * sz,
                sx * cy * cz - cx * sy * sz,
                cx * sy * cz + sx * cy * sz,
                cx * cy * sz - sx * sy * cz
            );
            q.updateEuler(euler);
            return q;
        }

        void rotateLocalAxis(char axis, T angleDegrees) {
            // 2. Setear el eje que querés
            switch (axis) {
            case 'x': pitch = angleDegrees; break;
            case 'y': yaw = angleDegrees; break;
            case 'z': roll = angleDegrees; break;
            default: return; // Eje inválido
            }

            // 3. Crear nueva rotación con ese ángulo seteado
            Quaternion<T> q_pitch = Quaternion<T>::fromAngleAxis(pitch, Vector3<T>(1, 0, 0));
            Quaternion<T> q_yaw = Quaternion<T>::fromAngleAxis(yaw, Vector3<T>(0, 1, 0));
            Quaternion<T> q_roll = Quaternion<T>::fromAngleAxis(roll, Vector3<T>(0, 0, 1));

            Vector3<T> euler(pitch, yaw, roll);

            // 4. Combinar de nuevo (el orden depende de tu convención, este es XYZ)
            *this = q_roll * q_yaw * q_pitch;
            this->updateEuler(euler);
        }

        // Construye desde ángulo y eje
        static Quaternion fromAngleAxis(T angle, const Vector3<T>& axis, bool inRadians = false) {
            if (!inRadians) angle *= H_PI / 180.0;
            T half = angle * 0.5;
            T s = std::sin(half);
            return Quaternion(std::cos(half), axis.x * s, axis.y * s, axis.z * s);
        }

        // Convierte a ángulo y eje
        void toAngleAxis(T& angle, Vector3<T>& axis, bool inRadians = false) const {
            Quaternion q = this->normalize();
            angle = 2 * std::acos(q.w);
            T s = std::sqrt(1 - q.w * q.w);

            if (s < 1e-6)
                axis = Vector3<T>(1, 0, 0);
            else
                axis = Vector3<T>(q.x / s, q.y / s, q.z / s);

            if (!inRadians)
                angle *= 180.0 / H_PI;
        }

        // Construye una rotación desde un vector forward y uno up
        static Quaternion lookRotation(const Vector3<T>& forward, const Vector3<T>& up) {
            Vector3<T> f = forward.normalize();
            Vector3<T> r = up.cross(f).normalize();
            Vector3<T> u = f.cross(r);

            T m00 = r.x, m01 = u.x, m02 = f.x;
            T m10 = r.y, m11 = u.y, m12 = f.y;
            T m20 = r.z, m21 = u.z, m22 = f.z;

            T tr = m00 + m11 + m22;
            Quaternion q;

            if (tr > 0) {
                T S = std::sqrt(tr + 1.0) * 2;
                q.w = 0.25 * S;
                q.x = (m21 - m12) / S;
                q.y = (m02 - m20) / S;
                q.z = (m10 - m01) / S;
            }
            else if ((m00 > m11) && (m00 > m22)) {
                T S = std::sqrt(1.0 + m00 - m11 - m22) * 2;
                q.w = (m21 - m12) / S;
                q.x = 0.25 * S;
                q.y = (m01 + m10) / S;
                q.z = (m02 + m20) / S;
            }
            else if (m11 > m22) {
                T S = std::sqrt(1.0 + m11 - m00 - m22) * 2;
                q.w = (m02 - m20) / S;
                q.x = (m01 + m10) / S;
                q.y = 0.25 * S;
                q.z = (m12 + m21) / S;
            }
            else {
                T S = std::sqrt(1.0 + m22 - m00 - m11) * 2;
                q.w = (m10 - m01) / S;
                q.x = (m02 + m20) / S;
                q.y = (m12 + m21) / S;
                q.z = 0.25 * S;
            }

            return q.normalize();
        }

        static Quaternion<T> fromRotationMatrix(const Matrix3x3<T>& mat) {
            T trace = mat.cols[0].x + mat.cols[1].y + mat.cols[2].z;
            Quaternion<T> q;

            if (trace > T(0)) {
                T s = std::sqrt(trace + T(1)) * T(2); // s = 4 * qw
                q.w = T(0.25) * s;
                q.x = (mat.cols[2].y - mat.cols[1].z) / s;
                q.y = (mat.cols[0].z - mat.cols[2].x) / s;
                q.z = (mat.cols[1].x - mat.cols[0].y) / s;
            }
            else {
                if (mat.cols[0].x > mat.cols[1].y && mat.cols[0].x > mat.cols[2].z) {
                    T s = std::sqrt(T(1) + mat.cols[0].x - mat.cols[1].y - mat.cols[2].z) * T(2);
                    q.w = (mat.cols[2].y - mat.cols[1].z) / s;
                    q.x = T(0.25) * s;
                    q.y = (mat.cols[0].y + mat.cols[1].x) / s;
                    q.z = (mat.cols[0].z + mat.cols[2].x) / s;
                }
                else if (mat.cols[1].y > mat.cols[2].z) {
                    T s = std::sqrt(T(1) + mat.cols[1].y - mat.cols[0].x - mat.cols[2].z) * T(2);
                    q.w = (mat.cols[0].z - mat.cols[2].x) / s;
                    q.x = (mat.cols[0].y + mat.cols[1].x) / s;
                    q.y = T(0.25) * s;
                    q.z = (mat.cols[1].z + mat.cols[2].y) / s;
                }
                else {
                    T s = std::sqrt(T(1) + mat.cols[2].z - mat.cols[0].x - mat.cols[1].y) * T(2);
                    q.w = (mat.cols[1].x - mat.cols[0].y) / s;
                    q.x = (mat.cols[0].z + mat.cols[2].x) / s;
                    q.y = (mat.cols[1].z + mat.cols[2].y) / s;
                    q.z = T(0.25) * s;
                }
            }

            return q.normalize();
        }


        friend std::ostream& operator<<(std::ostream& os, const Quaternion& q) {
            os << "(" << q.w << ", " << q.x << "i, " << q.y << "j, " << q.z << "k)";
            return os;
        }
    };

    using QuaternionI = Quaternion<int>;
    using QuaternionF = Quaternion<float>;
    using QuaternionD = Quaternion<double>;

}

#endif // QUATERNION_H
