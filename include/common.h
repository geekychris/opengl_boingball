#pragma once

#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>

// Math constants
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Simple 3D vector class
struct Vec3 {
    float x, y, z;
    
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    
    Vec3 operator+(const Vec3& other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }
    
    Vec3 operator-(const Vec3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }
    
    Vec3 operator*(float scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }
    
    Vec3& operator+=(const Vec3& other) {
        x += other.x; y += other.y; z += other.z;
        return *this;
    }
    
    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }
    
    Vec3 normalize() const {
        float len = length();
        if (len > 0) return Vec3(x / len, y / len, z / len);
        return Vec3(0, 0, 0);
    }
    
    float dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    
    Vec3 cross(const Vec3& other) const {
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
};

// Simple 2D vector for texture coordinates
struct Vec2 {
    float x, y;
    Vec2() : x(0), y(0) {}
    Vec2(float x, float y) : x(x), y(y) {}
};

// Vertex structure
struct Vertex {
    Vec3 position;
    Vec3 normal;
    Vec2 texCoord;
};

// 4x4 Matrix class (simplified)
struct Mat4 {
    float data[16];
    
    Mat4() {
        // Initialize as identity matrix
        for (int i = 0; i < 16; i++) data[i] = 0;
        data[0] = data[5] = data[10] = data[15] = 1.0f;
    }
    
    static Mat4 perspective(float fov, float aspect, float near, float far) {
        Mat4 result;
        float tanHalfFov = std::tan(fov / 2.0f);
        
        result.data[0] = 1.0f / (aspect * tanHalfFov);
        result.data[5] = 1.0f / tanHalfFov;
        result.data[10] = -(far + near) / (far - near);
        result.data[11] = -1.0f;
        result.data[14] = -(2.0f * far * near) / (far - near);
        result.data[15] = 0.0f;
        
        return result;
    }
    
    static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
        Vec3 f = (center - eye).normalize();
        Vec3 s = f.cross(up).normalize();
        Vec3 u = s.cross(f);
        
        Mat4 result;
        result.data[0] = s.x;
        result.data[4] = s.y;
        result.data[8] = s.z;
        result.data[1] = u.x;
        result.data[5] = u.y;
        result.data[9] = u.z;
        result.data[2] = -f.x;
        result.data[6] = -f.y;
        result.data[10] = -f.z;
        result.data[12] = -s.dot(eye);
        result.data[13] = -u.dot(eye);
        result.data[14] = f.dot(eye);
        
        return result;
    }
    
    static Mat4 translate(const Vec3& translation) {
        Mat4 result;
        result.data[12] = translation.x;
        result.data[13] = translation.y;
        result.data[14] = translation.z;
        return result;
    }
    
    static Mat4 scale(const Vec3& scale) {
        Mat4 result;
        result.data[0] = scale.x;
        result.data[5] = scale.y;
        result.data[10] = scale.z;
        return result;
    }
    
    static Mat4 rotateX(float angle) {
        Mat4 result;
        float c = std::cos(angle);
        float s = std::sin(angle);
        result.data[5] = c;
        result.data[6] = s;
        result.data[9] = -s;
        result.data[10] = c;
        return result;
    }
    
    static Mat4 rotateY(float angle) {
        Mat4 result;
        float c = std::cos(angle);
        float s = std::sin(angle);
        result.data[0] = c;
        result.data[2] = -s;
        result.data[8] = s;
        result.data[10] = c;
        return result;
    }
    
    static Mat4 rotateZ(float angle) {
        Mat4 result;
        float c = std::cos(angle);
        float s = std::sin(angle);
        result.data[0] = c;
        result.data[1] = s;
        result.data[4] = -s;
        result.data[5] = c;
        return result;
    }
    
    Mat4 operator*(const Mat4& other) const {
        Mat4 result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result.data[i * 4 + j] = 0;
                for (int k = 0; k < 4; k++) {
                    result.data[i * 4 + j] += data[i * 4 + k] * other.data[k * 4 + j];
                }
            }
        }
        return result;
    }
};
