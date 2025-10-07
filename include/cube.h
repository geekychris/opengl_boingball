#pragma once

#include "common.h"
#include "glad.h"

class Cube {
public:
    Cube(float size);
    ~Cube();
    
    void generate();
    void render();
    
    float getSize() const { return size; }

private:
    float size;
    std::vector<Vec3> vertices;
    std::vector<unsigned int> indices;
    
    unsigned int VAO, VBO, EBO;
    bool generated;
};
