#pragma once

#include "common.h"
#include "glad.h"

class Shader {
public:
    Shader();
    ~Shader();
    
    bool loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
    void use() const;
    
    // Uniform setting functions
    void setFloat(const std::string& name, float value) const;
    void setInt(const std::string& name, int value) const;
    void setBool(const std::string& name, bool value) const;
    void setVec3(const std::string& name, const Vec3& value) const;
    void setMat4(const std::string& name, const Mat4& mat) const;
    
    unsigned int getID() const { return programID; }

private:
    unsigned int programID;
    
    bool compileShader(const std::string& source, GLenum type, unsigned int& shaderID);
    std::string readFile(const std::string& filepath);
};
