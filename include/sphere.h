#pragma once

#include "common.h"
#include "glad.h"
#include <GLFW/glfw3.h>

class Sphere {
public:
    Sphere(float radius, int rings, int sectors);
    ~Sphere();
    
    void generate();
    void render();
    
    const std::vector<Vertex>& getVertices() const { return vertices; }
    const std::vector<unsigned int>& getIndices() const { return indices; }

private:
    float radius;
    int rings;
    int sectors;
    
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    unsigned int VAO, VBO, EBO;
    bool generated;
};
