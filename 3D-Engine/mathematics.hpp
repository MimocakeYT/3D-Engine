#pragma once
#include <math.h>
#include <vector>

class vec3
{
public:
    float x, y, z, w;
    vec3() : x(0), y(0), z(0), w(1) {}
    vec3(float xx, float yy, float zz) : x(xx), y(yy), z(zz), w(1) {}
    friend vec3 operator + (vec3 v1, vec3 v2) { return vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z); }
    friend vec3 operator - (vec3 v1, vec3 v2) { return vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z); }
    friend vec3 operator * (vec3 v, float f) { return vec3(v.x * f, v.y * f, v.z * f); }
    friend vec3 operator / (vec3 v, float f) { return vec3(v.x / f, v.y / f, v.z / f); }
    void operator += (vec3 v) { x += v.x; y += v.y; z += v.z; }
    void operator -= (vec3 v) { x -= v.x; y -= v.y; z -= v.z; }
    void operator *= (float f) { x *= f; y *= f; z *= f; }
    void operator /= (float f) { x /= f, y /= f, z /= f; }
    friend vec3 operator - (vec3 v) { return v * -1; }
    friend float dot_prod(vec3 v1, vec3 v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }
    friend vec3 cross_prod(vec3 a, vec3 b) { return vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }
    float len() { return sqrt(x * x + y * y + z * z); }
    void norm() { *this /= sqrt(x * x + y * y + z * z); }
};

inline vec3 intersectPlane(vec3& plane_p, vec3& plane_n, vec3& lineStart, vec3& lineEnd, float& t)
{
    plane_n.norm();
    float plane_d = -dot_prod(plane_n, plane_p);
    float ad = dot_prod(lineStart, plane_n);
    float bd = dot_prod(lineEnd, plane_n);
    t = (-plane_d - ad) / (bd - ad);
    vec3 lineStartToEnd = lineEnd - lineStart;
    vec3 lineToIntersect = lineStartToEnd * t;
    return lineStart + lineToIntersect;
}

class mat4x4
{
public:
    float m[4][4] = { 0 };
    mat4x4() {}
    mat4x4(std::vector<std::vector<float>> v)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                m[i][j] = v[i][j];
            }
        }
    }
    static const mat4x4 get_proj_mat(const int width = 1920,
                              const int height = 1080,
                              const float fNear = 0.1,
                              const float fFar = 1000,
                              const float FOV = 90)
    {
        const float PI = 3.14159;
        return mat4x4(std::vector<std::vector<float>> {
            {((float)height / (float)width) / tanf(FOV / 2 / 180 * PI), 0, 0, 0},
            { 0, 1 / tanf(FOV / 2 / 180 * PI), 0, 0 },
            { 0, 0, fFar / (fFar - fNear), 1 },
            { 0, 0, -fFar * fNear / (fFar - fNear), 0 }
        });
    }
    static mat4x4 get_rot_x(float angle)
    {
        mat4x4 matrix;
        matrix.m[0][0] = 1.0f;
        matrix.m[1][1] = cosf(angle);
        matrix.m[1][2] = sinf(angle);
        matrix.m[2][1] = -sinf(angle);
        matrix.m[2][2] = cosf(angle);
        matrix.m[3][3] = 1.0f;
        return matrix;
    }
    static mat4x4 get_rot_y(float angle)
    {
        mat4x4 matrix;
        matrix.m[0][0] = cosf(angle);
        matrix.m[0][2] = sinf(angle);
        matrix.m[2][0] = -sinf(angle);
        matrix.m[1][1] = 1.0f;
        matrix.m[2][2] = cosf(angle);
        matrix.m[3][3] = 1.0f;
        return matrix;
    }
    static mat4x4 get_rot_z(float angle)
    {
        mat4x4 matrix;
        matrix.m[0][0] = cosf(angle);
        matrix.m[0][1] = sinf(angle);
        matrix.m[1][0] = -sinf(angle);
        matrix.m[1][1] = cosf(angle);
        matrix.m[2][2] = 1.0f;
        matrix.m[3][3] = 1.0f;
        return matrix;
    }
    static mat4x4 get_point_at_mat(vec3& pos, vec3& target, vec3 up)
    {
        vec3 newForward = target - pos;
        newForward.norm();

        vec3 a = newForward * dot_prod(up, newForward);
        vec3 newUp = up - a;
        newUp.norm();

        vec3 newRight = cross_prod(newUp, newForward);

        mat4x4 matrix;
        matrix.m[0][0] = newRight.x;	matrix.m[0][1] = newRight.y;	matrix.m[0][2] = newRight.z;	matrix.m[0][3] = 0.0f;
        matrix.m[1][0] = newUp.x;		matrix.m[1][1] = newUp.y;		matrix.m[1][2] = newUp.z;		matrix.m[1][3] = 0.0f;
        matrix.m[2][0] = newForward.x;	matrix.m[2][1] = newForward.y;	matrix.m[2][2] = newForward.z;	matrix.m[2][3] = 0.0f;
        matrix.m[3][0] = pos.x;			matrix.m[3][1] = pos.y;			matrix.m[3][2] = pos.z;			matrix.m[3][3] = 1.0f;
        return matrix;
    }
    void invert() // Only for Rotation/Translation Matrices
    {
        mat4x4 matrix;
        matrix.m[0][0] = this->m[0][0]; matrix.m[0][1] = this->m[1][0]; matrix.m[0][2] = this->m[2][0]; matrix.m[0][3] = 0.0f;
        matrix.m[1][0] = this->m[0][1]; matrix.m[1][1] = this->m[1][1]; matrix.m[1][2] = this->m[2][1]; matrix.m[1][3] = 0.0f;
        matrix.m[2][0] = this->m[0][2]; matrix.m[2][1] = this->m[1][2]; matrix.m[2][2] = this->m[2][2]; matrix.m[2][3] = 0.0f;
        matrix.m[3][0] = -(this->m[3][0] * matrix.m[0][0] + this->m[3][1] * matrix.m[1][0] + this->m[3][2] * matrix.m[2][0]);
        matrix.m[3][1] = -(this->m[3][0] * matrix.m[0][1] + this->m[3][1] * matrix.m[1][1] + this->m[3][2] * matrix.m[2][1]);
        matrix.m[3][2] = -(this->m[3][0] * matrix.m[0][2] + this->m[3][1] * matrix.m[1][2] + this->m[3][2] * matrix.m[2][2]);
        matrix.m[3][3] = 1.0f;
        *this = matrix;
    }
    friend vec3 operator * (vec3 i, mat4x4 m)
    {
        vec3 v;
        v.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + i.w * m.m[3][0];
        v.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + i.w * m.m[3][1];
        v.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + i.w * m.m[3][2];
        v.w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + i.w * m.m[3][3];
        if (v.w) { v.x /= v.w; v.y /= v.w; v.z /= v.w; }
        return v;
    }
};