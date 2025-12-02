#include "Math/Matrix4x4.hpp"

#include "Math/Quaternion.hpp"


namespace CEMath
{
    Matrix4x4::Matrix4x4() noexcept
    {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] = (i == j) ? 1.0f : 0.0f;
    }
    
    Matrix4x4::Matrix4x4(float diagonal) noexcept
    {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] = (i == j) ? diagonal : 0.0f;
    }
    
    float* Matrix4x4::operator[](uint32_t index) noexcept
    {
        return m[index];
    }
    
    const float* Matrix4x4::operator[](uint32_t index) const noexcept
    {
        return m[index];
    }
    
    Matrix4x4 Matrix4x4::operator+(const Matrix4x4& other) const noexcept
    {
        Matrix4x4 result;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                result.m[i][j] = m[i][j] + other.m[i][j];
        return result;
    }
    
    Matrix4x4 Matrix4x4::operator-(const Matrix4x4& other) const noexcept
    {
        Matrix4x4 result;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                result.m[i][j] = m[i][j] - other.m[i][j];
        return result;
    }
    
  Matrix4x4::Matrix4x4(float m00, float m01, float m02, float m03,
                         float m10, float m11, float m12, float m13,
                         float m20, float m21, float m22, float m23,
                         float m30, float m31, float m32, float m33) noexcept
    {
        m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
        m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
        m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
        m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
    }



    Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const noexcept
    {
        Matrix4x4 result;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                result.m[i][j] = 0.0f;
                for (int k = 0; k < 4; ++k)
                {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }
    
    Matrix4x4 Matrix4x4::operator*(float scalar) const noexcept
    {
        Matrix4x4 result;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                result.m[i][j] = m[i][j] * scalar;
        return result;
    }
    
    Matrix4x4 Matrix4x4::operator/(float scalar) const noexcept
    {
        const float invScalar = 1.0f / scalar;
        return *this * invScalar;
    }
    
    Vector4D Matrix4x4::operator*(const Vector4D& vector) const noexcept
    {
        return Vector4D(
            m[0][0] * vector.x + m[0][1] * vector.y + m[0][2] * vector.z + m[0][3] * vector.w,
            m[1][0] * vector.x + m[1][1] * vector.y + m[1][2] * vector.z + m[1][3] * vector.w,
            m[2][0] * vector.x + m[2][1] * vector.y + m[2][2] * vector.z + m[2][3] * vector.w,
            m[3][0] * vector.x + m[3][1] * vector.y + m[3][2] * vector.z + m[3][3] * vector.w
        );
    }
    
    Vector3D Matrix4x4::operator*(const Vector3D& vector) const noexcept
    {
        Vector4D result = *this * Vector4D(vector, 1.0f);
        return result.ToVector3D();
    }
    
    Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& other) noexcept
    {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] += other.m[i][j];
        return *this;
    }
    
    Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& other) noexcept
    {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] -= other.m[i][j];
        return *this;
    }
    
    Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& other) noexcept
    {
        *this = *this * other;
        return *this;
    }
    
    Matrix4x4& Matrix4x4::operator*=(float scalar) noexcept
    {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] *= scalar;
        return *this;
    }
    
    Matrix4x4& Matrix4x4::operator/=(float scalar) noexcept
    {
        const float invScalar = 1.0f / scalar;
        return *this *= invScalar;
    }
    
    bool Matrix4x4::operator==(const Matrix4x4& other) const noexcept
    {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                if (std::abs(m[i][j] - other.m[i][j]) > EPSILON)
                    return false;
        return true;
    }
    
    bool Matrix4x4::operator!=(const Matrix4x4& other) const noexcept
    {
        return !(*this == other);
    }
    
    Matrix4x4 Matrix4x4::Transposed() const noexcept
    {
        Matrix4x4 result;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                result.m[i][j] = m[j][i];
        return result;
    }
    
    Matrix4x4& Matrix4x4::Transpose() noexcept
    {
        *this = Transposed();
        return *this;
    }

    bool Matrix4x4::IsOrthogonal() const
    
       {
    Matrix4x4 transposed = Transposed();
    Matrix4x4 shouldBeIdentity = *this * transposed;
    
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            float expected = (i == j) ? 1.0f : 0.0f;
            if (std::abs(shouldBeIdentity.m[i][j] - expected) > EPSILON)
                return false;
        }
    }
    return true;
}
    
    float Matrix4x4::Determinant() const noexcept
    {
        float det = 0.0f;
        
        // Using Laplace expansion
        float minor[3][3];
        
        for (int i = 0; i < 4; ++i)
        {
            // Create minor matrix
            for (int row = 1; row < 4; ++row)
            {
                int colIndex = 0;
                for (int col = 0; col < 4; ++col)
                {
                    if (col != i)
                    {
                        minor[row - 1][colIndex++] = m[row][col];
                    }
                }
            }
            
            // Calculate minor determinant
            float minorDet = 
                minor[0][0] * (minor[1][1] * minor[2][2] - minor[1][2] * minor[2][1]) -
                minor[0][1] * (minor[1][0] * minor[2][2] - minor[1][2] * minor[2][0]) +
                minor[0][2] * (minor[1][0] * minor[2][1] - minor[1][1] * minor[2][0]);
            
            det += ((i % 2 == 0) ? 1.0f : -1.0f) * m[0][i] * minorDet;
        }
        
        return det;
    }
    
    Matrix4x4 Matrix4x4::Inversed() const noexcept
    {
        Matrix4x4 inv;
        float det = Determinant();
        
        if (std::abs(det) < EPSILON)
            return inv; // Return identity if matrix is singular
            
        float invDet = 1.0f / det;
        
        inv.m[0][0] = invDet * (
            m[1][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
            m[1][2] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) +
            m[1][3] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]));
            
        inv.m[0][1] = -invDet * (
            m[0][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
            m[0][2] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) +
            m[0][3] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]));
            
        inv.m[0][2] = invDet * (
            m[0][1] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) -
            m[0][2] * (m[1][1] * m[3][3] - m[1][3] * m[3][1]) +
            m[0][3] * (m[1][1] * m[3][2] - m[1][2] * m[3][1]));
            
        inv.m[0][3] = -invDet * (
            m[0][1] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]) -
            m[0][2] * (m[1][1] * m[2][3] - m[1][3] * m[2][1]) +
            m[0][3] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]));
            
        inv.m[1][0] = -invDet * (
            m[1][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
            m[1][2] * (m[2][0] * m[3][3] - m[2][3] * m[3][0]) +
            m[1][3] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]));
            
        inv.m[1][1] = invDet * (
            m[0][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
            m[0][2] * (m[2][0] * m[3][3] - m[2][3] * m[3][0]) +
            m[0][3] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]));
            
        inv.m[1][2] = -invDet * (
            m[0][0] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) -
            m[0][2] * (m[1][0] * m[3][3] - m[1][3] * m[3][0]) +
            m[0][3] * (m[1][0] * m[3][2] - m[1][2] * m[3][0]));
            
        inv.m[1][3] = invDet * (
            m[0][0] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]) -
            m[0][2] * (m[1][0] * m[2][3] - m[1][3] * m[2][0]) +
            m[0][3] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]));
            
        inv.m[2][0] = invDet * (
            m[1][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) -
            m[1][1] * (m[2][0] * m[3][3] - m[2][3] * m[3][0]) +
            m[1][3] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
            
        inv.m[2][1] = -invDet * (
            m[0][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) -
            m[0][1] * (m[2][0] * m[3][3] - m[2][3] * m[3][0]) +
            m[0][3] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
            
        inv.m[2][2] = invDet * (
            m[0][0] * (m[1][1] * m[3][3] - m[1][3] * m[3][1]) -
            m[0][1] * (m[1][0] * m[3][3] - m[1][3] * m[3][0]) +
            m[0][3] * (m[1][0] * m[3][1] - m[1][1] * m[3][0]));
            
        inv.m[2][3] = -invDet * (
            m[0][0] * (m[1][1] * m[2][3] - m[1][3] * m[2][1]) -
            m[0][1] * (m[1][0] * m[2][3] - m[1][3] * m[2][0]) +
            m[0][3] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]));
            
        inv.m[3][0] = -invDet * (
            m[1][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) -
            m[1][1] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]) +
            m[1][2] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
            
        inv.m[3][1] = invDet * (
            m[0][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) -
            m[0][1] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]) +
            m[0][2] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
            
        inv.m[3][2] = -invDet * (
            m[0][0] * (m[1][1] * m[3][2] - m[1][2] * m[3][1]) -
            m[0][1] * (m[1][0] * m[3][2] - m[1][2] * m[3][0]) +
            m[0][2] * (m[1][0] * m[3][1] - m[1][1] * m[3][0]));
            
        inv.m[3][3] = invDet * (
            m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
            m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
            m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]));
            
        return inv;
    }
    
    Matrix4x4& Matrix4x4::Inverse() noexcept
    {
        *this = Inversed();
        return *this;
    }
    
    Matrix4x4 Matrix4x4::Translate(const Vector3D& translation) noexcept
    {
        Matrix4x4 result;
        result.m[0][3] = translation.x;
        result.m[1][3] = translation.y;
        result.m[2][3] = translation.z;
        return result;
    }
    
    Matrix4x4 Matrix4x4::Scale(const Vector3D& scale) noexcept
    {
        Matrix4x4 result;
        result.m[0][0] = scale.x;
        result.m[1][1] = scale.y;
        result.m[2][2] = scale.z;
        return result;
    }
    
    Matrix4x4 Matrix4x4::RotateX(float angle) noexcept
    {
        Matrix4x4 result;
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        
        result.m[1][1] = cosA;
        result.m[1][2] = -sinA;
        result.m[2][1] = sinA;
        result.m[2][2] = cosA;
        
        return result;
    }
    
    Matrix4x4 Matrix4x4::RotateY(float angle) noexcept
    {
        Matrix4x4 result;
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        
        result.m[0][0] = cosA;
        result.m[0][2] = sinA;
        result.m[2][0] = -sinA;
        result.m[2][2] = cosA;
        
        return result;
    }
    
    Matrix4x4 Matrix4x4::RotateZ(float angle) noexcept
    {
        Matrix4x4 result;
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        
        result.m[0][0] = cosA;
        result.m[0][1] = -sinA;
        result.m[1][0] = sinA;
        result.m[1][1] = cosA;
        
        return result;
    }
    
    Matrix4x4 Matrix4x4::Rotate(const Quaternion& rotation) noexcept
    {
        Matrix4x4 result;
        
        float xx = rotation.x * rotation.x;
        float xy = rotation.x * rotation.y;
        float xz = rotation.x * rotation.z;
        float xw = rotation.x * rotation.w;
        float yy = rotation.y * rotation.y;
        float yz = rotation.y * rotation.z;
        float yw = rotation.y * rotation.w;
        float zz = rotation.z * rotation.z;
        float zw = rotation.z * rotation.w;
        
        result.m[0][0] = 1.0f - 2.0f * (yy + zz);
        result.m[0][1] = 2.0f * (xy - zw);
        result.m[0][2] = 2.0f * (xz + yw);
        
        result.m[1][0] = 2.0f * (xy + zw);
        result.m[1][1] = 1.0f - 2.0f * (xx + zz);
        result.m[1][2] = 2.0f * (yz - xw);
        
        result.m[2][0] = 2.0f * (xz - yw);
        result.m[2][1] = 2.0f * (yz + xw);
        result.m[2][2] = 1.0f - 2.0f * (xx + yy);
        
        return result;
    }
    
    Matrix4x4 Matrix4x4::LookAt(const Vector3D& eye, const Vector3D& center, const Vector3D& up) noexcept
    {
        Vector3D f = (center - eye).Normalized();
        Vector3D s = f.Cross(up).Normalized();
        Vector3D u = s.Cross(f);
        
        Matrix4x4 result;
        result.m[0][0] = s.x;
        result.m[1][0] = s.y;
        result.m[2][0] = s.z;
        
        result.m[0][1] = u.x;
        result.m[1][1] = u.y;
        result.m[2][1] = u.z;
        
        result.m[0][2] = -f.x;
        result.m[1][2] = -f.y;
        result.m[2][2] = -f.z;
        
        result.m[0][3] = -s.Dot(eye);
        result.m[1][3] = -u.Dot(eye);
        result.m[2][3] = f.Dot(eye);
        
        return result;
    }
    
    Matrix4x4 Matrix4x4::Perspective(float fovY, float aspect, float zNear, float zFar) noexcept
    {
        Matrix4x4 result;
        
        float tanHalfFov = std::tan(fovY * 0.5f);
        float zRange = zFar - zNear;
        
        result.m[0][0] = 1.0f / (aspect * tanHalfFov);
        result.m[1][1] = 1.0f / tanHalfFov;
        result.m[2][2] = -(zFar + zNear) / zRange;
        result.m[2][3] = -2.0f * zFar * zNear / zRange;
        result.m[3][2] = -1.0f;
        result.m[3][3] = 0.0f;
        
        return result;
    }
    
    Matrix4x4 Matrix4x4::Orthographic(float left, float right, float bottom, float top, float zNear, float zFar) noexcept
    {
        Matrix4x4 result;
        
        result.m[0][0] = 2.0f / (right - left);
        result.m[1][1] = 2.0f / (top - bottom);
        result.m[2][2] = -2.0f / (zFar - zNear);
        
        result.m[0][3] = -(right + left) / (right - left);
        result.m[1][3] = -(top + bottom) / (top - bottom);
        result.m[2][3] = -(zFar + zNear) / (zFar - zNear);
        
        return result;
    }
    
    Matrix4x4 Matrix4x4::VulkanPerspective(float fovY, float aspect, float zNear, float zFar) noexcept
    {
        Matrix4x4 result = Perspective(fovY, aspect, zNear, zFar);
        
        // Vulkan uses Y down, so flip Y
        result.m[1][1] *= -1.0f;
        
        // Vulkan uses [0, 1] for Z
        result.m[2][2] = zFar / (zFar - zNear);
        result.m[2][3] = -(zFar * zNear) / (zFar - zNear);
        
        return result;
    }
    
    Matrix4x4 Matrix4x4::VulkanOrthographic(float left, float right, float bottom, float top, float zNear, float zFar) noexcept
    {
        Matrix4x4 result = Orthographic(left, right, bottom, top, zNear, zFar);
        
        // Vulkan uses Y down, so flip Y
        result.m[1][1] *= -1.0f;
        
        // Vulkan uses [0, 1] for Z
        result.m[2][2] = 1.0f / (zFar - zNear);
        result.m[2][3] = -zNear / (zFar - zNear);
        
        return result;
    }

    std::string Matrix4x4::ToString() const
    {
         std::ostringstream oss;
    oss << "Matrix4x4:\n";
    for (int i = 0; i < 4; ++i)
    {
        oss << "  [ ";
        for (int j = 0; j < 4; ++j)
        {
            oss << std::fixed << std::setprecision(3) << std::setw(8) << m[i][j];
            if (j < 3) oss << ", ";
        }
        oss << " ]\n";
    }
    return oss.str();
        
    }

std::string Matrix4x4::ToStringCompact() const 
{
    std::ostringstream oss;
    oss << "[[";
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            oss << std::fixed << std::setprecision(2) << m[i][j];
            if (j < 3) oss << " ";
        }
        if (i < 3) oss << "] [";
    }
    oss << "]]";
    return oss.str();
}

std::string Matrix4x4::ToStringGLSL() const
{
    std::ostringstream oss;
    oss << "mat4(\n";
    for (int col = 0; col < 4; ++col)
    {
        oss << "  ";
        for (int row = 0; row < 4; ++row)
        {
            oss << std::fixed << std::setprecision(4) << std::setw(8) << m[row][col];
            if (row < 3) oss << ", ";
        }
        if (col < 3) oss << ",\n";
    }
    oss << "\n)";
    return oss.str();
}

    const Matrix4x4 Matrix4x4::Identity;
    const Matrix4x4 Matrix4x4::Zero(0.0f);
}