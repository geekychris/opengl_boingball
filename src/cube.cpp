#include "cube.h"

Cube::Cube(float size) : size(size), generated(false) {
}

Cube::~Cube() {
    if (generated) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
}

void Cube::generate() {
    vertices.clear();
    indices.clear();
    
    float halfSize = size / 2.0f;
    
    // Define the 8 vertices of a cube
    vertices = {
        // Bottom face
        Vec3(-halfSize, -halfSize, -halfSize), // 0
        Vec3( halfSize, -halfSize, -halfSize), // 1
        Vec3( halfSize, -halfSize,  halfSize), // 2
        Vec3(-halfSize, -halfSize,  halfSize), // 3
        
        // Top face
        Vec3(-halfSize,  halfSize, -halfSize), // 4
        Vec3( halfSize,  halfSize, -halfSize), // 5
        Vec3( halfSize,  halfSize,  halfSize), // 6
        Vec3(-halfSize,  halfSize,  halfSize)  // 7
    };
    
    // Define the edges of the cube (wireframe)
    indices = {
        // Bottom face edges
        0, 1,  1, 2,  2, 3,  3, 0,
        
        // Top face edges
        4, 5,  5, 6,  6, 7,  7, 4,
        
        // Vertical edges connecting bottom to top
        0, 4,  1, 5,  2, 6,  3, 7
    };
    
    // Generate OpenGL buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vec3), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
    generated = true;
}

void Cube::render() {
    if (!generated) {
        generate();
    }
    
    glBindVertexArray(VAO);
    glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
