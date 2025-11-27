#pragma once
#include "Vector4.h"
#include <array>

namespace CE::Math
{
    template<typename T>
    class Matrix4
    {
    public:
        // Column-major для Vulkan/OpenGL
        std::array<std::array<T, 4>, 4> data;

        Matrix4() { SetIdentity(); }
        Matrix4(T ) { SetIdentity(); }

        explicit Matrix4(const T* arrayData)
        {
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    data[i][j] = arrayData[i * 4 + j];
        }

        Matrix4(const Vector4<T>& col0, const Vector4<T>& col1, 
                const Vector4<T>& col2, const Vector4<T>& col3)
        {
            data[0][0] = col0.x; data[0][1] = col0.y; data[0][2] = col0.z; data[0][3] = col0.w;
            data[1][0] = col1.x; data[1][1] = col1.y; data[1][2] = col1.z; data[1][3] = col1.w;
            data[2][0] = col2.x; data[2][1] = col2.y; data[2][2] = col2.z; data[2][3] = col2.w;
            data[3][0] = col3.x; data[3][1] = col3.y; data[3][2] = col3.z; data[3][3] = col3.w;
        }

        // Единичная матрица
        void SetIdentity()
        {
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    data[i][j] = (i == j) ? T(1) : T(0);
        }

        // Доступ к элементам
        T& operator()(int row, int col) { return data[col][row]; }
        const T& operator()(int row, int col) const { return data[col][row]; }

        T& operator[](int index) { return (&data[0][0])[index]; }
        const T& operator[](int index) const { return (&data[0][0])[index]; }

        // Базовые операции
        Matrix4 operator+(const Matrix4& other) const
        {
            Matrix4 result;
            for (int i = 0; i < 16; ++i)
                result[i] = (*this)[i] + other[i];
            return result;
        }

        Matrix4 operator-(const Matrix4& other) const
        {
            Matrix4 result;
            for (int i = 0; i < 16; ++i)
                result[i] = (*this)[i] - other[i];
            return result;
        }

        Matrix4 operator*(const Matrix4& other) const
        {
            Matrix4 result;
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    result(i, j) = T(0);
                    for (int k = 0; k < 4; ++k) {
                        result(i, j) += (*this)(i, k) * other(k, j);
                    }
                }
            }
            return result;
        }

        Vector4<T> operator*(const Vector4<T>& vec) const
        {
            return Vector4<T>(
                data[0][0] * vec.x + data[1][0] * vec.y + data[2][0] * vec.z + data[3][0] * vec.w,
                data[0][1] * vec.x + data[1][1] * vec.y + data[2][1] * vec.z + data[3][1] * vec.w,
                data[0][2] * vec.x + data[1][2] * vec.y + data[2][2] * vec.z + data[3][2] * vec.w,
                data[0][3] * vec.x + data[1][3] * vec.y + data[2][3] * vec.z + data[3][3] * vec.w
            );
        }

        Matrix4 operator*(T scalar) const
        {
            Matrix4 result;
            for (int i = 0; i < 16; ++i)
                result[i] = (*this)[i] * scalar;
            return result;
        }

        Matrix4& operator*=(const Matrix4& other)
        {
            *this = *this * other;
            return *this;
        }

        bool operator==(const Matrix4& other) const
        {
            for (int i = 0; i < 16; ++i)
                if (!Approximately(static_cast<float>((*this)[i]), static_cast<float>(other[i])))
                    return false;
            return true;
        }

        bool operator!=(const Matrix4& other) const { return !(*this == other); }

        // Трансформации
        static Matrix4 Translation(const Vector3<T>& translation)
        {
            Matrix4 result;
            result(0, 3) = translation.x;
            result(1, 3) = translation.y;
            result(2, 3) = translation.z;
            return result;
        }

        static Matrix4 Scale(const Vector3<T>& scale)
        {
            Matrix4 result;
            result(0, 0) = scale.x;
            result(1, 1) = scale.y;
            result(2, 2) = scale.z;
            return result;
        }

        static Matrix4 RotationX(T angle)
        {
            Matrix4 result;
            T cosA = std::cos(angle);
            T sinA = std::sin(angle);
            result(1, 1) = cosA; result(1, 2) = -sinA;
            result(2, 1) = sinA; result(2, 2) = cosA;
            return result;
        }

        static Matrix4 RotationY(T angle)
        {
            Matrix4 result;
            T cosA = std::cos(angle);
            T sinA = std::sin(angle);
            result(0, 0) = cosA; result(0, 2) = sinA;
            result(2, 0) = -sinA; result(2, 2) = cosA;
            return result;
        }

        static Matrix4 RotationZ(T angle)
        {
            Matrix4 result;
            T cosA = std::cos(angle);
            T sinA = std::sin(angle);
            result(0, 0) = cosA; result(0, 1) = -sinA;
            result(1, 0) = sinA; result(1, 1) = cosA;
            return result;
        }

        // Матрицы проекции для Vulkan
        static Matrix4 Perspective(T fovY, T aspect, T zNear, T zFar)
        {
            Matrix4 result;
    T tanHalfFov = std::tan(fovY / T(2));
    
    result(0, 0) = T(1) / (aspect * tanHalfFov);
    result(1, 1) = T(1) / tanHalfFov;
    result(2, 2) = zFar / (zNear - zFar);        // zFar / (zNear - zFar)
    result(2, 3) = T(-1);                        // -1 для Vulkan
    result(3, 2) = (zFar * zNear) / (zNear - zFar); // (zFar * zNear) / (zNear - zFar)
    result(3, 3) = T(0);
    
    return result;
        }

        static Matrix4 Orthographic(T left, T right, T bottom, T top, T zNear, T zFar)
        {
            Matrix4 result;
            result(0, 0) = T(2) / (right - left);
            result(1, 1) = T(2) / (top - bottom);
            result(2, 2) = T(1) / (zFar - zNear);
            result(0, 3) = -(right + left) / (right - left);
            result(1, 3) = -(top + bottom) / (top - bottom);
            result(2, 3) = -zNear / (zFar - zNear);
            return result;
        }

        // LookAt матрица (Vulkan uses right-handed, Y-up)
        static Matrix4 LookAt(const Vector3<T>& eye, const Vector3<T>& target, const Vector3<T>& up)
        {
            Vector3<T> zAxis = (target - eye).Normalized();  // ИСПРАВЛЕНО: target - eye
    Vector3<T> xAxis = up.Cross(zAxis).Normalized();
    Vector3<T> yAxis = zAxis.Cross(xAxis);

    Matrix4 result;
    result(0, 0) = xAxis.x; result(0, 1) = xAxis.y; result(0, 2) = xAxis.z;
    result(1, 0) = yAxis.x; result(1, 1) = yAxis.y; result(1, 2) = yAxis.z;
    result(2, 0) = zAxis.x; result(2, 1) = zAxis.y; result(2, 2) = zAxis.z;
    
    result(0, 3) = -xAxis.Dot(eye);
    result(1, 3) = -yAxis.Dot(eye);
    result(2, 3) = -zAxis.Dot(eye);

    return result;
        }

        // Инверсия матрицы
        Matrix4 Inverse() const
        {
             Matrix4 result;
    const T* m = &data[0][0];
    T* invOut = &result.data[0][0]; // Переименуем переменную

    T inv[16], det; // Это локальный массив
    int i;

            inv[0] = m[5]  * m[10] * m[15] - 
                     m[5]  * m[11] * m[14] - 
                     m[9]  * m[6]  * m[15] + 
                     m[9]  * m[7]  * m[14] +
                     m[13] * m[6]  * m[11] - 
                     m[13] * m[7]  * m[10];

            inv[4] = -m[4]  * m[10] * m[15] + 
                      m[4]  * m[11] * m[14] + 
                      m[8]  * m[6]  * m[15] - 
                      m[8]  * m[7]  * m[14] - 
                      m[12] * m[6]  * m[11] + 
                      m[12] * m[7]  * m[10];

            inv[8] = m[4]  * m[9] * m[15] - 
                     m[4]  * m[11] * m[13] - 
                     m[8]  * m[5] * m[15] + 
                     m[8]  * m[7] * m[13] + 
                     m[12] * m[5] * m[11] - 
                     m[12] * m[7] * m[9];

            inv[12] = -m[4]  * m[9] * m[14] + 
                       m[4]  * m[10] * m[13] +
                       m[8]  * m[5] * m[14] - 
                       m[8]  * m[6] * m[13] - 
                       m[12] * m[5] * m[10] + 
                       m[12] * m[6] * m[9];

            inv[1] = -m[1]  * m[10] * m[15] + 
                      m[1]  * m[11] * m[14] + 
                      m[9]  * m[2] * m[15] - 
                      m[9]  * m[3] * m[14] - 
                      m[13] * m[2] * m[11] + 
                      m[13] * m[3] * m[10];

            inv[5] = m[0]  * m[10] * m[15] - 
                     m[0]  * m[11] * m[14] - 
                     m[8]  * m[2] * m[15] + 
                     m[8]  * m[3] * m[14] + 
                     m[12] * m[2] * m[11] - 
                     m[12] * m[3] * m[10];

            inv[9] = -m[0]  * m[9] * m[15] + 
                      m[0]  * m[11] * m[13] + 
                      m[8]  * m[1] * m[15] - 
                      m[8]  * m[3] * m[13] - 
                      m[12] * m[1] * m[11] + 
                      m[12] * m[3] * m[9];

            inv[13] = m[0]  * m[9] * m[14] - 
                      m[0]  * m[10] * m[13] - 
                      m[8]  * m[1] * m[14] + 
                      m[8]  * m[2] * m[13] + 
                      m[12] * m[1] * m[10] - 
                      m[12] * m[2] * m[9];

            inv[2] = m[1]  * m[6] * m[15] - 
                     m[1]  * m[7] * m[14] - 
                     m[5]  * m[2] * m[15] + 
                     m[5]  * m[3] * m[14] + 
                     m[13] * m[2] * m[7] - 
                     m[13] * m[3] * m[6];

            inv[6] = -m[0]  * m[6] * m[15] + 
                      m[0]  * m[7] * m[14] + 
                      m[4]  * m[2] * m[15] - 
                      m[4]  * m[3] * m[14] - 
                      m[12] * m[2] * m[7] + 
                      m[12] * m[3] * m[6];

            inv[10] = m[0]  * m[5] * m[15] - 
                      m[0]  * m[7] * m[13] - 
                      m[4]  * m[1] * m[15] + 
                      m[4]  * m[3] * m[13] + 
                      m[12] * m[1] * m[7] - 
                      m[12] * m[3] * m[5];

            inv[14] = -m[0]  * m[5] * m[14] + 
                       m[0]  * m[6] * m[13] + 
                       m[4]  * m[1] * m[14] - 
                       m[4]  * m[2] * m[13] - 
                       m[12] * m[1] * m[6] + 
                       m[12] * m[2] * m[5];

            inv[3] = -m[1] * m[6] * m[11] + 
                      m[1] * m[7] * m[10] + 
                      m[5] * m[2] * m[11] - 
                      m[5] * m[3] * m[10] - 
                      m[9] * m[2] * m[7] + 
                      m[9] * m[3] * m[6];

            inv[7] = m[0] * m[6] * m[11] - 
                     m[0] * m[7] * m[10] - 
                     m[4] * m[2] * m[11] + 
                     m[4] * m[3] * m[10] + 
                     m[8] * m[2] * m[7] - 
                     m[8] * m[3] * m[6];

            inv[11] = -m[0] * m[5] * m[11] + 
                       m[0] * m[7] * m[9] + 
                       m[4] * m[1] * m[11] - 
                       m[4] * m[3] * m[9] - 
                       m[8] * m[1] * m[7] + 
                       m[8] * m[3] * m[5];

            inv[15] = m[0] * m[5] * m[10] - 
                      m[0] * m[6] * m[9] - 
                      m[4] * m[1] * m[10] + 
                      m[4] * m[2] * m[9] + 
                      m[8] * m[1] * m[6] - 
                      m[8] * m[2] * m[5];

            det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return Matrix4();

    det = T(1) / det;

    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det; // Используем переименованную переменную

    return result;
        }

        // Транспонирование
        Matrix4 Transposed() const
        {
            Matrix4 result;
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    result(i, j) = (*this)(j, i);
            return result;
        }

        // Детерминант
        T Determinant() const
        {
            const T* m = &data[0][0];
            
            return m[0] * (m[5] * (m[10] * m[15] - m[11] * m[14]) -
                           m[6] * (m[9] * m[15] - m[11] * m[13]) +
                           m[7] * (m[9] * m[14] - m[10] * m[13])) -
                   m[1] * (m[4] * (m[10] * m[15] - m[11] * m[14]) -
                           m[6] * (m[8] * m[15] - m[11] * m[12]) +
                           m[7] * (m[8] * m[14] - m[10] * m[12])) +
                   m[2] * (m[4] * (m[9] * m[15] - m[11] * m[13]) -
                           m[5] * (m[8] * m[15] - m[11] * m[12]) +
                           m[7] * (m[8] * m[13] - m[9] * m[12])) -
                   m[3] * (m[4] * (m[9] * m[14] - m[10] * m[13]) -
                           m[5] * (m[8] * m[14] - m[10] * m[12]) +
                           m[6] * (m[8] * m[13] - m[9] * m[12]));
        }

        // Извлечение трансформаций
        Vector3<T> GetTranslation() const
        {
            return Vector3<T>(data[3][0], data[3][1], data[3][2]);
        }

        Vector3<T> GetScale() const
        {
            return Vector3<T>(
                Vector3<T>(data[0][0], data[0][1], data[0][2]).Length(),
                Vector3<T>(data[1][0], data[1][1], data[1][2]).Length(),
                Vector3<T>(data[2][0], data[2][1], data[2][2]).Length()
            );
        }

        // Получение указателя на данные для Vulkan
        const T* Data() const { return &data[0][0]; }
        T* Data() { return &data[0][0]; }

        // Статические матрицы
        static Matrix4 Identity()
        {
            Matrix4 result;
            result.SetIdentity();
            return result;
        }

        static Matrix4 Zero()
        {
            Matrix4 result;
            for (int i = 0; i < 16; ++i)
                result[i] = T(0);
            return result;
        }
    };

    using Matrix4f = Matrix4<float>;
}