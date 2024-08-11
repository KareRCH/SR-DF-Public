#pragma once

#include "PhysicsPrecision.h"

BEGIN(Engine)

//extern Real fSleepEpsilon;		// 슬립조건 오차범위

void Set_SleepEpsilon(Real value);
Real Get_SleepEpsilon();


/// <summary>
/// 3차원 벡터
/// </summary>
class ENGINE_DLL FVector3
{
public:
    FVector3() : x(0), y(0), z(0) {}
    FVector3(const Real x, const Real y, const Real z)
        : x(x), y(y), z(z) {}
    FVector3(const FVector3& rhs)
        : x(rhs.x), y(rhs.y), z(rhs.z)
    {}

public:
    union {
        struct {
            Real x, y, z;
        };
        Real data[3];
    };

private:
    //Real pad = 0;       // padding용 변수, 안씀. 일부 아키텍처에 최적화용도

public:
    // 미리 정의된 벡터값들
    const static FVector3 GRAVITY;
    const static FVector3 HIGH_GRAVITY;
    const static FVector3 UP;
    const static FVector3 RIGHT;
    const static FVector3 OUT_OF_SCREEN;
    const static FVector3 X;
    const static FVector3 Y;
    const static FVector3 Z;


public:
    Real operator[](unsigned i) const
    {
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }

    Real& operator[](unsigned i)
    {
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }

    FVector3& operator=(const FVector3& v)
    {
        if (this != &v)
        {
            x = v.x;
            y = v.y;
            z = v.z;
        }
        return *this;
    }

    /** Adds the given vector to this. */
    void operator+=(const FVector3& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
    }

    /**
     * Returns the value of the given vector added to this.
     */
    FVector3 operator+(const FVector3& v) const
    {
        return FVector3(x + v.x, y + v.y, z + v.z);
    }

    /** Subtracts the given vector from this. */
    void operator-=(const FVector3& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
    }

    /**
     * Returns the value of the given vector subtracted from this.
     */
    FVector3 operator-(const FVector3& v) const
    {
        return FVector3(x - v.x, y - v.y, z - v.z);
    }

    FVector3 operator-() const
    {
        return FVector3(-x, -y, -z);
    }

    /** Multiplies this vector by the given scalar. */
    void operator*=(const Real value)
    {
        x *= value;
        y *= value;
        z *= value;
    }

    /** Returns a copy of this vector scaled the given value. */
    FVector3 operator*(const Real value) const
    {
        return FVector3(x * value, y * value, z * value);
    }

    /**
     * Calculates and returns a component-wise product of this
     * vector with the given vector.
     */
    FVector3 ComponentProduct(const FVector3& vector) const
    {
        return FVector3(x * vector.x, y * vector.y, z * vector.z);
    }

    /**
     * Performs a component-wise product with the given vector and
     * sets this vector to its result.
     */
    void ComponentProductUpdate(const FVector3& vector)
    {
        x *= vector.x;
        y *= vector.y;
        z *= vector.z;
    }

    /**
     * Calculates and returns the vector product of this vector
     * with the given vector.
     */
    FVector3 CrossProduct(const FVector3& vector) const
    {
        return FVector3(y * vector.z - z * vector.y,
                        z * vector.x - x * vector.z,
                        x * vector.y - y * vector.x);
    }

    /**
     * Updates this vector to be the vector product of its current
     * value and the given vector.
     */
    void operator %=(const FVector3& vector)
    {
        *this = CrossProduct(vector);
    }

    
    FVector3 operator%(const FVector3& vector) const
    {
        return FVector3(y * vector.z - z * vector.y,
            z * vector.x - x * vector.z,
            x * vector.y - y * vector.x);
    }

    // 점곱
    Real DotProduct(const FVector3& vector) const
    {
        return x * vector.x + y * vector.y + z * vector.z;
    }

    /**
     * Calculates and returns the scalar product of this vector
     * with the given vector.
     */
    Real operator *(const FVector3& vector) const
    {
        return x * vector.x + y * vector.y + z * vector.z;
    }

    /**
     * Adds the given vector to this, scaled by the given amount.
     */
    void Add_ScaledVector(const FVector3& vector, Real scale)
    {
        x += vector.x * scale;
        y += vector.y * scale;
        z += vector.z * scale;
    }

    FVector3 operator / (const Real& devide) const
    {
        FVector3 vector;
        vector.x /= devide;
        vector.y /= devide;
        vector.z /= devide;
        return vector;
    }

    FVector3 operator /= (const Real& devide)
    {
        x /= devide;
        y /= devide;
        z /= devide;
        return (*this);
    }

    /** Gets the magnitude of this vector. */
    Real Magnitude() const
    {
        return real_sqrt(x * x + y * y + z * z);
    }

    /** Gets the squared magnitude of this vector. */
    Real SquareMagnitude() const
    {
        return x * x + y * y + z * z;
    }

    /** Limits the size of the vector to the given maximum. */
    void Trim(Real size)
    {
        if (SquareMagnitude() > size * size)
        {
            Normalize();
            x *= size;
            y *= size;
            z *= size;
        }
    }

    // 정규화
    void Normalize()
    {
        Real l = Magnitude();
        if (l > 0)
        {
            (*this) *= ((Real)1) / l;
        }
    }

    // 정규화된 결과를 반환하는 함수
    FVector3 Unit() const
    {
        FVector3 result = *this;
        result.Normalize();
        return result;
    }

#pragma region 비교 연산자

    bool operator==(const FVector3& other) const
    {
        return x == other.x && 
                y == other.y &&
                z == other.z;
    }

    bool operator!=(const FVector3& other) const
    {
        return !(*this == other);
    }

    bool operator<(const FVector3& other) const
    {
        return x < other.x && y < other.y && z < other.z;
    }

    bool operator>(const FVector3& other) const
    {
        return x > other.x && y > other.y && z > other.z;
    }

    bool operator<=(const FVector3& other) const
    {
        return x <= other.x && y <= other.y && z <= other.z;
    }

    bool operator>=(const FVector3& other) const
    {
        return x >= other.x && y >= other.y && z >= other.z;
    }

#pragma endregion
    
    // 제로벡터로 만드는 함수
    void Clear()
    {
        x = y = z = 0;
    }

    // 모든 축에 대해 벡터를 반전시키는 함수
    void Invert()
    {
        x = -x;
        y = -y;
        z = -z;
    }

    D3DXVECTOR3 Convert_DX9Vec3()
    {
        D3DXVECTOR3 v3;
        v3.x = x;
        v3.y = y;
        v3.z = z;
        return v3;
    }
};


/// <summary>
/// 쿼터니온
/// </summary>
class ENGINE_DLL FQuaternion
{
public:
    FQuaternion() : r(1), i(0), j(0), k(0) {}
    FQuaternion(const Real r, const Real i, const Real j, const Real k)
        : r(r), i(i), j(j), k(k) {}

public:
    union {
        struct {
            // 쿼터니온 요소
            Real r, i, j, k;
        };

        // 배열로 쿼터니온 요소 접근
        Real data[4];
    };
    
    // 정규화
    void Normalise()
    {
        Real d = r * r + i * i + j * j + k * k;

        if (d < real_epsilon) {
            r = 1;
            return;
        }

        d = ((Real)1.0) / real_sqrt(d);
        r *= d;
        i *= d;
        j *= d;
        k *= d;
    }

    // 쿼터니온 곱
    void operator *=(const FQuaternion& multiplier)
    {
        FQuaternion q = *this;
        r = q.r * multiplier.r - q.i * multiplier.i -
            q.j * multiplier.j - q.k * multiplier.k;
        i = q.r * multiplier.i + q.i * multiplier.r +
            q.j * multiplier.k - q.k * multiplier.j;
        j = q.r * multiplier.j + q.j * multiplier.r +
            q.k * multiplier.i - q.i * multiplier.k;
        k = q.r * multiplier.k + q.k * multiplier.r +
            q.i * multiplier.j - q.j * multiplier.i;
    }

    // 크기벡터 더하기
    void Add_ScaledVector(const FVector3& vector, Real scale)
    {
        FQuaternion q(0,
            vector.x * scale,
            vector.y * scale,
            vector.z * scale);
        q *= *this;
        r += q.r * ((Real)0.5);
        i += q.i * ((Real)0.5);
        j += q.j * ((Real)0.5);
        k += q.k * ((Real)0.5);
    }

    // 회전벡터 적용
    void Rotate_ByVector(const FVector3& vector)
    {
        FQuaternion q(0, vector.x, vector.y, vector.z);
        (*this) *= q;
    }
};

/// <summary>
/// 3x4 행렬
/// 행 우선 행렬이다. 다이렉트x 행렬과 호환에 주의.
/// </summary>
class ENGINE_DLL FMatrix3x4
{
public:
    FMatrix3x4()
    {
        data[1] = data[2] = data[3] = data[4] = data[6] =
            data[7] = data[8] = data[9] = data[11] = 0;
        data[0] = data[5] = data[10] = 1;
    }

    // 대각선 요소 삽입
    void SetDiagonal(Real a, Real b, Real c)
    {
        data[0] = a;
        data[5] = b;
        data[10] = c;
    }

public:
    // 3x4 행렬
    Real data[12];

    // 행렬 곱
    FMatrix3x4 operator*(const FMatrix3x4& o) const
    {
        FMatrix3x4 result;
        result.data[0] = (o.data[0] * data[0]) + (o.data[4] * data[1]) + (o.data[8] * data[2]);
        result.data[4] = (o.data[0] * data[4]) + (o.data[4] * data[5]) + (o.data[8] * data[6]);
        result.data[8] = (o.data[0] * data[8]) + (o.data[4] * data[9]) + (o.data[8] * data[10]);

        result.data[1] = (o.data[1] * data[0]) + (o.data[5] * data[1]) + (o.data[9] * data[2]);
        result.data[5] = (o.data[1] * data[4]) + (o.data[5] * data[5]) + (o.data[9] * data[6]);
        result.data[9] = (o.data[1] * data[8]) + (o.data[5] * data[9]) + (o.data[9] * data[10]);

        result.data[2] = (o.data[2] * data[0]) + (o.data[6] * data[1]) + (o.data[10] * data[2]);
        result.data[6] = (o.data[2] * data[4]) + (o.data[6] * data[5]) + (o.data[10] * data[6]);
        result.data[10] = (o.data[2] * data[8]) + (o.data[6] * data[9]) + (o.data[10] * data[10]);

        result.data[3] = (o.data[3] * data[0]) + (o.data[7] * data[1]) + (o.data[11] * data[2]) + data[3];
        result.data[7] = (o.data[3] * data[4]) + (o.data[7] * data[5]) + (o.data[11] * data[6]) + data[7];
        result.data[11] = (o.data[3] * data[8]) + (o.data[7] * data[9]) + (o.data[11] * data[10]) + data[11];

        return result;
    }

    // 벡터로 행렬 곱, w는 1로 취급
    FVector3 operator*(const FVector3& vector) const
    {
        return FVector3(
            vector.x * data[0] +
            vector.y * data[1] +
            vector.z * data[2] + data[3],

            vector.x * data[4] +
            vector.y * data[5] +
            vector.z * data[6] + data[7],

            vector.x * data[8] +
            vector.y * data[9] +
            vector.z * data[10] + data[11]
        );
    }

    // 벡터 행렬 곱 반환 함수
    FVector3 Transform(const FVector3& vector) const
    {
        return (*this) * vector;
    }

    // 행렬식 스칼라 반환
    Real Get_Determinant() const
    {
        return -data[8] * data[5] * data[2] +
                data[4] * data[9] * data[2] +
                data[8] * data[1] * data[6] -
                data[0] * data[9] * data[6] -
                data[4] * data[1] * data[10] +
                data[0] * data[5] * data[10];
    }

    // Input 행렬에 대한 역행렬 삽입
    void Set_Inverse(const FMatrix3x4& m)
    {
        // 역 행렬식 만들기, 행렬식 스칼라가 0이 아니어야 합니다.
        Real det = Get_Determinant();
        if (det == 0) return;
        det = ((Real)1.0) / det;
        
        data[0] = (-m.data[9] * m.data[6] + m.data[5] * m.data[10]) * det;
        data[4] = (m.data[8] * m.data[6] - m.data[4] * m.data[10]) * det;
        data[8] = (-m.data[8] * m.data[5] + m.data[4] * m.data[9]) * det;

        data[1] = (m.data[9] * m.data[2] - m.data[1] * m.data[10]) * det;
        data[5] = (-m.data[8] * m.data[2] + m.data[0] * m.data[10]) * det;
        data[9] = (m.data[8] * m.data[1] - m.data[0] * m.data[9]) * det;

        data[2] = (-m.data[5] * m.data[2] + m.data[1] * m.data[6]) * det;
        data[6] = (+m.data[4] * m.data[2] - m.data[0] * m.data[6]) * det;
        data[10] = (-m.data[4] * m.data[1] + m.data[0] * m.data[5]) * det;

        data[3] = (m.data[9] * m.data[6] * m.data[3]
                    - m.data[5] * m.data[10] * m.data[3]
                    - m.data[9] * m.data[2] * m.data[7]
                    + m.data[1] * m.data[10] * m.data[7]
                    + m.data[5] * m.data[2] * m.data[11]
                    - m.data[1] * m.data[6] * m.data[11]) * det;
        data[7] = (-m.data[8] * m.data[6] * m.data[3]
                    + m.data[4] * m.data[10] * m.data[3]
                    + m.data[8] * m.data[2] * m.data[7]
                    - m.data[0] * m.data[10] * m.data[7]
                    - m.data[4] * m.data[2] * m.data[11]
                    + m.data[0] * m.data[6] * m.data[11]) * det;
        data[11] = (m.data[8] * m.data[5] * m.data[3]
                    - m.data[4] * m.data[9] * m.data[3]
                    - m.data[8] * m.data[1] * m.data[7]
                    + m.data[0] * m.data[9] * m.data[7]
                    + m.data[4] * m.data[1] * m.data[11]
                    - m.data[0] * m.data[5] * m.data[11]) * det;
    }

    // 역행렬 반환 함수
    FMatrix3x4 Inverse() const
    {
        FMatrix3x4 result;
        result.Set_Inverse(*this);
        return result;
    }

    // 역행렬로 만들기
    void Invert()
    {
        Set_Inverse(*this);
    }

    // 트랜스폼 방향
    FVector3 TransformDirection(const FVector3& vector) const
    {
        return FVector3(
            vector.x * data[0] +
            vector.y * data[1] +
            vector.z * data[2],

            vector.x * data[4] +
            vector.y * data[5] +
            vector.z * data[6],

            vector.x * data[8] +
            vector.y * data[9] +
            vector.z * data[10]
        );
    }

    // 트랜스폼 역방향
    FVector3 TransformInverseDirection(const FVector3& vector) const
    {
        return FVector3(
            vector.x * data[0] +
            vector.y * data[4] +
            vector.z * data[8],

            vector.x * data[1] +
            vector.y * data[5] +
            vector.z * data[9],

            vector.x * data[2] +
            vector.y * data[6] +
            vector.z * data[10]
        );
    }

    // 트랜스폼 거꾸로
    FVector3 TransformInverse(const FVector3& vector) const
    {
        FVector3 tmp = vector;
        tmp.x -= data[3];
        tmp.y -= data[7];
        tmp.z -= data[11];
        return FVector3(
            tmp.x * data[0] +
            tmp.y * data[4] +
            tmp.z * data[8],

            tmp.x * data[1] +
            tmp.y * data[5] +
            tmp.z * data[9],

            tmp.x * data[2] +
            tmp.y * data[6] +
            tmp.z * data[10]
        );
    }

    // 축벡터 구하기
    FVector3 Get_AxisVector(int i) const
    {
        return FVector3(data[i], data[i + 4], data[i + 8]);
    }

    FVector3 Get_PosVector() const
    {
        return FVector3(data[3], data[7], data[11]);
    }

    FVector3 Get_ScaleVector() const
    {
        FVector3 vScale;
        vScale.x = real_sqrt(data[0] * data[0] +
                                data[1] * data[1] +
                                data[2] * data[2]);

        vScale.y = real_sqrt(data[4] * data[4] +
                                data[5] * data[5] +
                                data[6] * data[6]);

        vScale.z = real_sqrt(data[8] * data[8] +
                                data[9] * data[9] +
                                data[10] * data[10]);
        return vScale;
    }

    // 정위와 위치를 세팅하는 함수
    void Set_OrientationAndPos(const FQuaternion& q, const FVector3& pos)
    {
        data[0] = 1 - (2 * q.j * q.j + 2 * q.k * q.k);
        data[1] = 2 * q.i * q.j + 2 * q.k * q.r;
        data[2] = 2 * q.i * q.k - 2 * q.j * q.r;
        data[3] = pos.x;

        data[4] = 2 * q.i * q.j - 2 * q.k * q.r;
        data[5] = 1 - (2 * q.i * q.i + 2 * q.k * q.k);
        data[6] = 2 * q.j * q.k + 2 * q.i * q.r;
        data[7] = pos.y;

        data[8] = 2 * q.i * q.k + 2 * q.j * q.r;
        data[9] = 2 * q.j * q.k - 2 * q.i * q.r;
        data[10] = 1 - (2 * q.i * q.i + 2 * q.j * q.j);
        data[11] = pos.z;
    }

    // OpenGL전용 배열 변환 대입
    void FillGLArray(float array[16]) const
    {
        array[0] = (float)data[0];
        array[1] = (float)data[4];
        array[2] = (float)data[8];
        array[3] = (float)0;

        array[4] = (float)data[1];
        array[5] = (float)data[5];
        array[6] = (float)data[9];
        array[7] = (float)0;

        array[8] = (float)data[2];
        array[9] = (float)data[6];
        array[10] = (float)data[10];
        array[11] = (float)0;

        array[12] = (float)data[3];
        array[13] = (float)data[7];
        array[14] = (float)data[11];
        array[15] = (float)1;
    }

    // DirectX전용 배열 변환 대입
    void FillDXArray(float array[16]) const
    {
        array[0] = (float)data[0];
        array[1] = (float)data[1];
        array[2] = (float)data[2];
        array[3] = (float)0;

        array[4] = (float)data[4];
        array[5] = (float)data[5];
        array[6] = (float)data[6];
        array[7] = (float)0;

        array[8] = (float)data[8];
        array[9] = (float)data[9];
        array[10] = (float)data[10];
        array[11] = (float)0;

        array[12] = (float)data[3];
        array[13] = (float)data[7];
        array[14] = (float)data[11];
        array[15] = (float)1;
    }

    void RecieveDXArray(const float array[16])
    {
        data[0] = (Real)array[0];
        data[1] = (Real)array[1];
        data[2] = (Real)array[2];

        data[4] = (Real)array[4];
        data[5] = (Real)array[5];
        data[6] = (Real)array[6];

        data[8] = (Real)array[8];
        data[9] = (Real)array[9];
        data[10] = (Real)array[10];

        data[3] = (Real)array[12];
        data[7] = (Real)array[13];
        data[11] = (Real)array[14];
    }
};

/// <summary>
/// 3x3 행렬
/// </summary>
class ENGINE_DLL FMatrix3
{
public:
    FMatrix3()
    {
        data[0] = data[1] = data[2] = data[3] = data[4] = data[5] =
            data[6] = data[7] = data[8] = 0;
    }
    FMatrix3(const FVector3& compOne, const FVector3& compTwo,
        const FVector3& compThree)
    {
        Set_Components(compOne, compTwo, compThree);
    }
    FMatrix3(Real c0, Real c1, Real c2, Real c3, Real c4, Real c5,
        Real c6, Real c7, Real c8)
    {
        data[0] = c0; data[1] = c1; data[2] = c2;
        data[3] = c3; data[4] = c4; data[5] = c5;
        data[6] = c6; data[7] = c7; data[8] = c8;
    }
public:
    Real data[9];

public:
    // 대각선 요소 세팅
    void Set_Diagonal(Real a, Real b, Real c)
    {
        Set_InertiaTensorCoeffs(a, b, c);
    }

    // 관성 텐서계수 세팅
    void Set_InertiaTensorCoeffs(Real ix, Real iy, Real iz,
        Real ixy = 0, Real ixz = 0, Real iyz = 0)
    {
        data[0] = ix;
        data[1] = data[3] = -ixy;
        data[2] = data[6] = -ixz;
        data[4] = iy;
        data[5] = data[7] = -iyz;
        data[8] = iz;
    }

    // 막힘 관성 텐서 세팅
    void Set_BlockInertiaTensor(const FVector3& halfSizes, Real mass)
    {
        FVector3 squares = halfSizes.ComponentProduct(halfSizes);
        Set_InertiaTensorCoeffs(0.3f * mass * (squares.y + squares.z),
            0.3f * mass * (squares.x + squares.z),
            0.3f * mass * (squares.x + squares.y));
    }

    // 비스듬한 대칭 세팅
    void Set_SkewSymmetric(const FVector3 vector)
    {
        data[0] = data[4] = data[8] = 0;
        data[1] = -vector.z;
        data[2] = vector.y;
        data[3] = vector.z;
        data[5] = -vector.x;
        data[6] = -vector.y;
        data[7] = vector.x;
    }

    // 컴포넌트 세팅
    void Set_Components(const FVector3& compOne, const FVector3& compTwo,
        const FVector3& compThree)
    {
        data[0] = compOne.x;
        data[1] = compTwo.x;
        data[2] = compThree.x;
        data[3] = compOne.y;
        data[4] = compTwo.y;
        data[5] = compThree.y;
        data[6] = compOne.z;
        data[7] = compTwo.z;
        data[8] = compThree.z;

    }

    // 벡터와 행렬을 곱한값 반환
    FVector3 operator*(const FVector3& vector) const
    {
        return FVector3(
            vector.x * data[0] + vector.y * data[1] + vector.z * data[2],
            vector.x * data[3] + vector.y * data[4] + vector.z * data[5],
            vector.x * data[6] + vector.y * data[7] + vector.z * data[8]
        );
    }

    // 벡터와 행렬 곱 변환
    FVector3 Transform(const FVector3& vector) const
    {
        return (*this) * vector;
    }

    // 변환
    FVector3 TransformTranspose(const FVector3& vector) const
    {
        return FVector3(
            vector.x * data[0] + vector.y * data[3] + vector.z * data[6],
            vector.x * data[1] + vector.y * data[4] + vector.z * data[7],
            vector.x * data[2] + vector.y * data[5] + vector.z * data[8]
        );
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="i"></param>
    /// <returns></returns>
    FVector3 Get_RowVector(int i) const
    {
        return FVector3(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="i"></param>
    /// <returns></returns>
    FVector3 Set_AxisVector(int i) const
    {
        return FVector3(data[i], data[i + 3], data[i + 6]);
    }
    
    /// <summary>
    /// Input으로 들어온 행렬을 기준으로 역행렬 구하기
    /// </summary>
    /// <param name="m">Input 행렬</param>
    void Set_Inverse(const FMatrix3& m)
    {
        Real t4 = m.data[0] * m.data[4];
        Real t6 = m.data[0] * m.data[5];
        Real t8 = m.data[1] * m.data[3];
        Real t10 = m.data[2] * m.data[3];
        Real t12 = m.data[1] * m.data[6];
        Real t14 = m.data[2] * m.data[6];

        // Calculate the determinant
        Real t16 = (t4 * m.data[8] - t6 * m.data[7] - t8 * m.data[8] +
            t10 * m.data[7] + t12 * m.data[5] - t14 * m.data[4]);

        // Make sure the determinant is non-zero.
        if (t16 == (Real)0.0f) return;
        Real t17 = 1 / t16;

        data[0] = (m.data[4] * m.data[8] - m.data[5] * m.data[7]) * t17;
        data[1] = -(m.data[1] * m.data[8] - m.data[2] * m.data[7]) * t17;
        data[2] = (m.data[1] * m.data[5] - m.data[2] * m.data[4]) * t17;
        data[3] = -(m.data[3] * m.data[8] - m.data[5] * m.data[6]) * t17;
        data[4] = (m.data[0] * m.data[8] - t14) * t17;
        data[5] = -(t6 - t10) * t17;
        data[6] = (m.data[3] * m.data[7] - m.data[4] * m.data[6]) * t17;
        data[7] = -(m.data[0] * m.data[7] - t12) * t17;
        data[8] = (t4 - t8) * t17;
    }

    /// <summary>
    /// 역행렬
    /// </summary>
    /// <returns>역행렬</returns>
    FMatrix3 Inverse() const
    {
        FMatrix3 result;
        result.Set_Inverse(*this);
        return result;
    }

    /// <summary>
    /// 역행렬
    /// </summary>
    void Invert()
    {
        Set_Inverse(*this);
    }

    /// <summary>
    /// 행렬 반전
    /// </summary>
    /// <param name="m"></param>
    void Set_Transpose(const FMatrix3& m)
    {
        data[0] = m.data[0];
        data[1] = m.data[3];
        data[2] = m.data[6];
        data[3] = m.data[1];
        data[4] = m.data[4];
        data[5] = m.data[7];
        data[6] = m.data[2];
        data[7] = m.data[5];
        data[8] = m.data[8];
    }

    /// <summary>
    /// 행렬 변환
    /// </summary>
    /// <returns>변환된 행렬</returns>
    FMatrix3 Transpose() const
    {
        FMatrix3 result;
        result.Set_Transpose(*this);
        return result;
    }

    /// <summary>
    /// 행렬 곱
    /// </summary>
    /// <param name="o"></param>
    /// <returns></returns>
    FMatrix3 operator*(const FMatrix3& o) const
    {
        return FMatrix3(
            data[0] * o.data[0] + data[1] * o.data[3] + data[2] * o.data[6],
            data[0] * o.data[1] + data[1] * o.data[4] + data[2] * o.data[7],
            data[0] * o.data[2] + data[1] * o.data[5] + data[2] * o.data[8],

            data[3] * o.data[0] + data[4] * o.data[3] + data[5] * o.data[6],
            data[3] * o.data[1] + data[4] * o.data[4] + data[5] * o.data[7],
            data[3] * o.data[2] + data[4] * o.data[5] + data[5] * o.data[8],

            data[6] * o.data[0] + data[7] * o.data[3] + data[8] * o.data[6],
            data[6] * o.data[1] + data[7] * o.data[4] + data[8] * o.data[7],
            data[6] * o.data[2] + data[7] * o.data[5] + data[8] * o.data[8]
        );
    }

    /// <summary>
    /// 행렬곱
    /// </summary>
    /// <param name="o"></param>
    void operator*=(const FMatrix3& o)
    {
        Real t1;
        Real t2;
        Real t3;

        t1 = data[0] * o.data[0] + data[1] * o.data[3] + data[2] * o.data[6];
        t2 = data[0] * o.data[1] + data[1] * o.data[4] + data[2] * o.data[7];
        t3 = data[0] * o.data[2] + data[1] * o.data[5] + data[2] * o.data[8];
        data[0] = t1;
        data[1] = t2;
        data[2] = t3;

        t1 = data[3] * o.data[0] + data[4] * o.data[3] + data[5] * o.data[6];
        t2 = data[3] * o.data[1] + data[4] * o.data[4] + data[5] * o.data[7];
        t3 = data[3] * o.data[2] + data[4] * o.data[5] + data[5] * o.data[8];
        data[3] = t1;
        data[4] = t2;
        data[5] = t3;

        t1 = data[6] * o.data[0] + data[7] * o.data[3] + data[8] * o.data[6];
        t2 = data[6] * o.data[1] + data[7] * o.data[4] + data[8] * o.data[7];
        t3 = data[6] * o.data[2] + data[7] * o.data[5] + data[8] * o.data[8];
        data[6] = t1;
        data[7] = t2;
        data[8] = t3;
    }

    /// <summary>
    /// 실수 곱
    /// </summary>
    /// <param name="scalar"></param>
    void operator*=(const Real scalar)
    {
        data[0] *= scalar; data[1] *= scalar; data[2] *= scalar;
        data[3] *= scalar; data[4] *= scalar; data[5] *= scalar;
        data[6] *= scalar; data[7] *= scalar; data[8] *= scalar;
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="o"></param>
    void operator+=(const FMatrix3& o)
    {
        data[0] += o.data[0]; data[1] += o.data[1]; data[2] += o.data[2];
        data[3] += o.data[3]; data[4] += o.data[4]; data[5] += o.data[5];
        data[6] += o.data[6]; data[7] += o.data[7]; data[8] += o.data[8];
    }

    /// <summary>
    /// 회전 세팅
    /// </summary>
    /// <param name="q"></param>
    void Set_Orientation(const FQuaternion& q)
    {
        data[0] = 1 - (2 * q.j * q.j + 2 * q.k * q.k);
        data[1] = 2 * q.i * q.j + 2 * q.k * q.r;
        data[2] = 2 * q.i * q.k - 2 * q.j * q.r;
        data[3] = 2 * q.i * q.j - 2 * q.k * q.r;
        data[4] = 1 - (2 * q.i * q.i + 2 * q.k * q.k);
        data[5] = 2 * q.j * q.k + 2 * q.i * q.r;
        data[6] = 2 * q.i * q.k + 2 * q.j * q.r;
        data[7] = 2 * q.j * q.k - 2 * q.i * q.r;
        data[8] = 1 - (2 * q.i * q.i + 2 * q.j * q.j);
    }

    /// <summary>
    /// 행렬 보간
    /// </summary>
    /// <param name="a"></param>
    /// <param name="b"></param>
    /// <param name="prop"></param>
    /// <returns></returns>
    static FMatrix3 LinearInterpolate(const FMatrix3& a, const FMatrix3& b, Real prop)
    {
        FMatrix3 result;
        for (unsigned i = 0; i < 9; i++) {
            result.data[i] = a.data[i] * (1 - prop) + b.data[i] * prop;
        }
        return result;
    }
};

END
