#pragma once
#include "Vector.h"

template <typename T>
struct Matrix2 {
    Matrix2()
    {
        x.x = 1; x.y = 0;
        y.x = 0; y.y = 1;
    }
    Matrix2(const T* m)
    {
        x.x = m[0]; x.y = m[1];
        y.x = m[2]; y.y = m[3];
    }
    vec2 x;
    vec2 y;
};

template <typename T>
struct Matrix3 {
    Matrix3()
    {
        x.x = 1; x.y = 0; x.z = 0;
        y.x = 0; y.y = 1; y.z = 0;
        z.x = 0; z.y = 0; z.z = 1;
    }
    Matrix3(const T* m)
    {
        x.x = m[0]; x.y = m[1]; x.z = m[2];
        y.x = m[3]; y.y = m[4]; y.z = m[5];
        z.x = m[6]; z.y = m[7]; z.z = m[8];
    }
    Matrix3 Transposed() const
    {
        Matrix3 m;
        m.x.x = x.x; m.x.y = y.x; m.x.z = z.x;
        m.y.x = x.y; m.y.y = y.y; m.y.z = z.y;
        m.z.x = x.z; m.z.y = y.z; m.z.z = z.z;
        return m;
    }
    Vector3<T> operator * (const Vector3<T>& b) const
    {
        Vector3<T> v;
        v.x = x.x * b.x + x.y * b.y + x.z * b.z;
        v.y = y.x * b.x + y.y * b.y + y.z * b.z;
        v.z = z.x * b.x + z.y * b.y + z.z * b.z;
        return v;
    }
    const T* Pointer() const
    {
        return &x.x;
    }
    vec3 x;
    vec3 y;
    vec3 z;
};

template <typename T>
struct Matrix4 {
    Matrix4()
    {
        x.x = 1; x.y = 0; x.z = 0; x.w = 0;
        y.x = 0; y.y = 1; y.z = 0; y.w = 0;
        z.x = 0; z.y = 0; z.z = 1; z.w = 0;
        w.x = 0; w.y = 0; w.z = 0; w.w = 1;
    }
    Matrix4(const Matrix3<T>& m)
    {
        x.x = m.x.x; x.y = m.x.y; x.z = m.x.z; x.w = 0;
        y.x = m.y.x; y.y = m.y.y; y.z = m.y.z; y.w = 0;
        z.x = m.z.x; z.y = m.z.y; z.z = m.z.z; z.w = 0;
        w.x = 0; w.y = 0; w.z = 0; w.w = 1;
    }
    Matrix4(const T* m)
    {
        x.x = m[0];  x.y = m[1];  x.z = m[2];  x.w = m[3];
        y.x = m[4];  y.y = m[5];  y.z = m[6];  y.w = m[7];
        z.x = m[8];  z.y = m[9];  z.z = m[10]; z.w = m[11];
        w.x = m[12]; w.y = m[13]; w.z = m[14]; w.w = m[15];
    }
    Matrix4 operator * (const Matrix4& b) const
    {
        Matrix4 m;
        m.x.x = x.x * b.x.x + x.y * b.y.x + x.z * b.z.x + x.w * b.w.x;
        m.x.y = x.x * b.x.y + x.y * b.y.y + x.z * b.z.y + x.w * b.w.y;
        m.x.z = x.x * b.x.z + x.y * b.y.z + x.z * b.z.z + x.w * b.w.z;
        m.x.w = x.x * b.x.w + x.y * b.y.w + x.z * b.z.w + x.w * b.w.w;
        m.y.x = y.x * b.x.x + y.y * b.y.x + y.z * b.z.x + y.w * b.w.x;
        m.y.y = y.x * b.x.y + y.y * b.y.y + y.z * b.z.y + y.w * b.w.y;
        m.y.z = y.x * b.x.z + y.y * b.y.z + y.z * b.z.z + y.w * b.w.z;
        m.y.w = y.x * b.x.w + y.y * b.y.w + y.z * b.z.w + y.w * b.w.w;
        m.z.x = z.x * b.x.x + z.y * b.y.x + z.z * b.z.x + z.w * b.w.x;
        m.z.y = z.x * b.x.y + z.y * b.y.y + z.z * b.z.y + z.w * b.w.y;
        m.z.z = z.x * b.x.z + z.y * b.y.z + z.z * b.z.z + z.w * b.w.z;
        m.z.w = z.x * b.x.w + z.y * b.y.w + z.z * b.z.w + z.w * b.w.w;
        m.w.x = w.x * b.x.x + w.y * b.y.x + w.z * b.z.x + w.w * b.w.x;
        m.w.y = w.x * b.x.y + w.y * b.y.y + w.z * b.z.y + w.w * b.w.y;
        m.w.z = w.x * b.x.z + w.y * b.y.z + w.z * b.z.z + w.w * b.w.z;
        m.w.w = w.x * b.x.w + w.y * b.y.w + w.z * b.z.w + w.w * b.w.w;
        return m;
    }
    Matrix4 operator * (const T b) const
    {
        Matrix4 m;
        m.x.x = x.x * b;
        m.x.y = x.y * b;
        m.x.z = x.z * b;
        m.x.w = x.w * b;
        m.y.x = y.x * b;
        m.y.y = y.y * b;
        m.y.z = y.z * b;
        m.y.w = y.w * b;
        m.z.x = z.x * b;
        m.z.y = z.y * b;
        m.z.z = z.z * b;
        m.z.w = z.w * b;
        m.w.x = w.x * b;
        m.w.y = w.y * b;
        m.w.z = w.z * b;
        m.w.w = w.w * b;
        return m;
    }
    Vector4<T> operator * (const Vector4<T>& b) const
    {
        Vector4<T> v;
        v.x = x.x * b.x + x.y * b.y + x.z * b.z + x.w * b.w;
        v.y = y.x * b.x + y.y * b.y + y.z * b.z + y.w * b.w;
        v.z = z.x * b.x + z.y * b.y + z.z * b.z + z.w * b.w;
        v.w = w.x * b.x + w.y * b.y + w.z * b.z + w.w * b.w;
        return v;
    }
    Matrix4& operator *= (const Matrix4& b)
    {
        Matrix4 m = *this * b;
        return (*this = m);
    }
    Matrix4 Transposed() const
    {
        Matrix4 m;
        m.x.x = x.x; m.x.y = y.x; m.x.z = z.x; m.x.w = w.x;
        m.y.x = x.y; m.y.y = y.y; m.y.z = z.y; m.y.w = w.y;
        m.z.x = x.z; m.z.y = y.z; m.z.z = z.z; m.z.w = w.z;
        m.w.x = x.w; m.w.y = y.w; m.w.z = z.w; m.w.w = w.w;
        return m;
    }
    /*
    ** Invert 4x4 matrix.
    ** Contributed by David Moore (See Mesa bug #6748)
    */
    Matrix4 Invert(T& det)
	{
    	Matrix4 inv;
    	T* m = &x.x;

    	//inv.x.x = y.y*z.z*w.w - y.y*z.w*w.z - z.y*y.z*w.w + z.y*y.w*w.z + w.y*y.z*z.w - w.y*y.w*z.z;
    	inv.x.x =  m[5]*m[10]*m[15] - m[5]*m[11]*m[14] - m[9]*m[6]*m[15] + m[9]*m[7]*m[14] + m[13]*m[6]*m[11] - m[13]*m[7]*m[10];
    	inv.x.y = -m[1]*m[10]*m[15] + m[1]*m[11]*m[14] + m[9]*m[2]*m[15] - m[9]*m[3]*m[14] - m[13]*m[2]*m[11] + m[13]*m[3]*m[10];
    	inv.x.z =  m[1]*m[6]*m[15] - m[1]*m[7]*m[14] - m[5]*m[2]*m[15] + m[5]*m[3]*m[14] + m[13]*m[2]*m[7] - m[13]*m[3]*m[6];
    	inv.x.w = -m[1]*m[6]*m[11] + m[1]*m[7]*m[10] + m[5]*m[2]*m[11] - m[5]*m[3]*m[10] - m[9]*m[2]*m[7] + m[9]*m[3]*m[6];

    	inv.y.x = -m[4]*m[10]*m[15] + m[4]*m[11]*m[14] + m[8]*m[6]*m[15] - m[8]*m[7]*m[14] - m[12]*m[6]*m[11] + m[12]*m[7]*m[10];
    	inv.y.y =  m[0]*m[10]*m[15] - m[0]*m[11]*m[14] - m[8]*m[2]*m[15] + m[8]*m[3]*m[14] + m[12]*m[2]*m[11] - m[12]*m[3]*m[10];
    	inv.y.z = -m[0]*m[6]*m[15] + m[0]*m[7]*m[14] + m[4]*m[2]*m[15] - m[4]*m[3]*m[14] - m[12]*m[2]*m[7] + m[12]*m[3]*m[6];
    	inv.y.w =  m[0]*m[6]*m[11] - m[0]*m[7]*m[10] - m[4]*m[2]*m[11] + m[4]*m[3]*m[10] + m[8]*m[2]*m[7] - m[8]*m[3]*m[6];

    	inv.z.x =  m[4]*m[9]*m[15] - m[4]*m[11]*m[13] - m[8]*m[5]*m[15] + m[8]*m[7]*m[13] + m[12]*m[5]*m[11] - m[12]*m[7]*m[9];
    	inv.z.y = -m[0]*m[9]*m[15] + m[0]*m[11]*m[13] + m[8]*m[1]*m[15] - m[8]*m[3]*m[13] - m[12]*m[1]*m[11] + m[12]*m[3]*m[9];
    	inv.z.z =  m[0]*m[5]*m[15] - m[0]*m[7]*m[13] - m[4]*m[1]*m[15] + m[4]*m[3]*m[13] + m[12]*m[1]*m[7] - m[12]*m[3]*m[5];
    	inv.z.w = -m[0]*m[5]*m[11] + m[0]*m[7]*m[9] + m[4]*m[1]*m[11] - m[4]*m[3]*m[9] - m[8]*m[1]*m[7] + m[8]*m[3]*m[5];

    	inv.w.x = -m[4]*m[9]*m[14] + m[4]*m[10]*m[13] + m[8]*m[5]*m[14] - m[8]*m[6]*m[13] - m[12]*m[5]*m[10] + m[12]*m[6]*m[9];
    	inv.w.y = m[0]*m[9]*m[14] - m[0]*m[10]*m[13] - m[8]*m[1]*m[14] + m[8]*m[2]*m[13] + m[12]*m[1]*m[10] - m[12]*m[2]*m[9];
    	inv.w.z = -m[0]*m[5]*m[14] + m[0]*m[6]*m[13] + m[4]*m[1]*m[14] - m[4]*m[2]*m[13] - m[12]*m[1]*m[6] + m[12]*m[2]*m[5];
    	inv.w.w = m[0]*m[5]*m[10] - m[0]*m[6]*m[9] - m[4]*m[1]*m[10] + m[4]*m[2]*m[9] + m[8]*m[1]*m[6] - m[8]*m[2]*m[5];

    	det = m[0]*inv.x.x + m[1]*inv.y.x + m[2]*inv.z.x +m[3]*inv.w.x;
    	if (det == 0) {
    		return Matrix4();
    	}

    	det = 1.0f/det;

    	inv = inv * det;

    	return inv;
	}
    Matrix3<T> ToMat3() const
    {
        Matrix3<T> m;
        m.x.x = x.x; m.y.x = y.x; m.z.x = z.x;
        m.x.y = x.y; m.y.y = y.y; m.z.y = z.y;
        m.x.z = x.z; m.y.z = y.z; m.z.z = z.z;
        return m;
    }
    const T* Pointer() const
    {
        return &x.x;
    }
    static Matrix4<T> Identity()
    {
        return Matrix4();
    }
    static Matrix4<T> Translate(T x, T y, T z)
    {
        Matrix4 m;
        m.x.x = 1; m.x.y = 0; m.x.z = 0; m.x.w = 0;
        m.y.x = 0; m.y.y = 1; m.y.z = 0; m.y.w = 0;
        m.z.x = 0; m.z.y = 0; m.z.z = 1; m.z.w = 0;
        m.w.x = x; m.w.y = y; m.w.z = z; m.w.w = 1;
        return m;
    }
    static Matrix4<T> Scale(T s)
    {
        Matrix4 m;
        m.x.x = s; m.x.y = 0; m.x.z = 0; m.x.w = 0;
        m.y.x = 0; m.y.y = s; m.y.z = 0; m.y.w = 0;
        m.z.x = 0; m.z.y = 0; m.z.z = s; m.z.w = 0;
        m.w.x = 0; m.w.y = 0; m.w.z = 0; m.w.w = 1;
        return m;
    }
    static Matrix4<T> Scale(T x, T y, T z)
    {
        Matrix4 m;
        m.x.x = x; m.x.y = 0; m.x.z = 0; m.x.w = 0;
        m.y.x = 0; m.y.y = y; m.y.z = 0; m.y.w = 0;
        m.z.x = 0; m.z.y = 0; m.z.z = z; m.z.w = 0;
        m.w.x = 0; m.w.y = 0; m.w.z = 0; m.w.w = 1;
        return m;
    }
    static Matrix4<T> RotateZ(T degrees)
    {
        T radians = degrees * 3.14159f / 180.0f;
        T s = std::sin(radians);
		T c; // 1.5707이 되면 0과 근사하기 때문에 예외처리
		if(radians < 1.5708 && radians > 1.5706)
			c = std::cos(std::floor(std::floor(radians * 1E2)) / 1E2);
		else 
			c = std::cos(radians);
        
        Matrix4 m;
        m.x.x =  c; m.x.y = -s; m.x.z = 0; m.x.w = 0;
        m.y.x =  s; m.y.y = c; m.y.z = 0; m.y.w = 0;
        m.z.x =  0; m.z.y = 0; m.z.z = 1; m.z.w = 0;
        m.w.x =  0; m.w.y = 0; m.w.z = 0; m.w.w = 1;
        return m;
    }
    static Matrix4<T> RotateY(T degrees)
    {
        T radians = degrees * 3.14159f / 180.0f;
        T s = std::sin(radians);
		T c; // 1.5707이 되면 0과 근사하기 때문에 예외처리
		if(radians < 1.5708 && radians > 1.5706)
			c = std::cos(std::floor(std::floor(radians * 1E2)) / 1E2);
		else 
			c = std::cos(radians);
        
        Matrix4 m;
        m.x.x =  c; m.x.y = 0; m.x.z = s; m.x.w = 0;
        m.y.x =  0; m.y.y = 1; m.y.z = 0; m.y.w = 0;
        m.z.x = -s; m.z.y = 0; m.z.z = c; m.z.w = 0;
        m.w.x =  0; m.w.y = 0; m.w.z = 0; m.w.w = 1;
        return m;
    }
    static Matrix4<T> RotateX(T degrees)
    {
        T radians = degrees * 3.14159f / 180.0f;
		T s = std::sin(radians);
		T c; // 1.5707이 되면 0과 근사하기 때문에 예외처리
		if(radians < 1.5708 && radians > 1.5706)
			c = std::cos(std::floor(std::floor(radians * 1E2)) / 1E2);
		else 
			c = std::cos(radians);

        Matrix4 m;
        m.x.x =  1; m.x.y = 0; m.x.z = 0; m.x.w = 0;
        m.y.x =  0; m.y.y = c; m.y.z =-s; m.y.w = 0;
        m.z.x =  0; m.z.y = s; m.z.z = c; m.z.w = 0;
        m.w.x =  0; m.w.y = 0; m.w.z = 0; m.w.w = 1;
        return m;
    }
    static Matrix4<T> Frustum(T tLeft, T tRight, T tBottom, T tTop, T tNear, T tFar)
    {
        T a = 2 * tNear / (tRight - tLeft);
        T b = 2 * tNear / (tTop - tBottom);
        T c = (tRight + tLeft) / (tRight - tLeft);
        T d = (tTop + tBottom) / (tTop - tBottom);
        T e = - (tFar + tNear) / (tFar - tNear);
        T f = -2 * tFar * tNear / (tFar - tNear);
        Matrix4 m;
        m.x.x = a; m.x.y = 0; m.x.z = 0; m.x.w = 0;
        m.y.x = 0; m.y.y = b; m.y.z = 0; m.y.w = 0;
        m.z.x = c; m.z.y = d; m.z.z = e; m.z.w = -1;
        m.w.x = 0; m.w.y = 0; m.w.z = f; m.w.w = 0;
        return m;
    }
	static Matrix4<T> Perspective(T fovy, T aspect, T tNear, T tFar)
	{
		Matrix4 m;
		T sine, cosine, cotangent, deltaZ;
		T radians = (T) (fovy/2.0f * Pi/180.0f);
		
		deltaZ = (T)(tFar - tNear);
		sine = (T)sin(radians);
		cosine = (T)cos(radians);
		
		if ((deltaZ == 0.0f) || (sine == 0.0f) || (aspect == 0.0f)) {
			return m;
		}
		
		cotangent = (T)(cosine / sine);
		
		m.x.x = cotangent / aspect;
		m.y.y = cotangent;
		m.z.z = -(tFar + tNear) / deltaZ;
		m.z.w = -1.0f;
		m.w.z = -2.0f * tNear * tFar / deltaZ;
		m.w.w = 0.0f;
		
		return m;
	}
	static Matrix4<T> Ortho(T xMin, T xMax, T yMin, T yMax, T zMin, T zMax)
	{
		Matrix4 m;
		
		T xDiff = xMax - xMin;
		T yDiff = yMax - yMin;
		T zDiff = zMax - zMin;

		m.x.x = 2.0f / xDiff;
		m.y.y = 2.0f / yDiff;
		m.z.z = 2.0f / zDiff;

		m.w.x = -(xMax + xMin) / xDiff;
		m.w.y = -(yMax + yMin) / yDiff;
		m.w.z = -(zMax + zMin) / zDiff;
		m.w.w = 1.0f;
		
		return m;
	}
    static Matrix4<T> LookAt(const Vector3<T>& eye,
                             const Vector3<T>& target,
                             const Vector3<T>& up)
    {
        Vector3<T> z = (eye - target).Normalized();
        Vector3<T> x = up.Cross(z).Normalized();
        Vector3<T> y = z.Cross(x).Normalized();
        
        Matrix4<T> m;
        m.x = Vector4<T>(x, 0);
        m.y = Vector4<T>(y, 0);
        m.z = Vector4<T>(z, 0);
        m.w = Vector4<T>(0, 0, 0, 1);
        
        Vector4<T> eyePrime = m * Vector4<T>(-eye, 1);
        m = m.Transposed();
        m.w = eyePrime;
        
        return m;
    }
    vec4 x;
    vec4 y;
    vec4 z;
    vec4 w;
};

typedef Matrix2<float> mat2;
typedef Matrix3<float> mat3;
typedef Matrix4<float> mat4;

