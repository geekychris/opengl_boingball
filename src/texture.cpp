#include "texture.h"

Texture::Texture() : textureID(0), generated(false) {
}

Texture::~Texture() {
    if (generated) {
        glDeleteTextures(1, &textureID);
    }
}

void Texture::generateBoingTexture(int width, int height, int checkerSize) {
    // Create the classic Amiga boing ball red and white checkerboard pattern
    std::vector<unsigned char> data(width * height * 3);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * 3;
            
            // Determine checker pattern
            bool isRed = ((x / checkerSize) + (y / checkerSize)) % 2 == 0;
            
            if (isRed) {
                // Red square
                data[index + 0] = 255;  // R
                data[index + 1] = 0;    // G
                data[index + 2] = 0;    // B
            } else {
                // White square
                data[index + 0] = 255;  // R
                data[index + 1] = 255;  // G
                data[index + 2] = 255;  // B
            }
        }
    }
    
    // Generate OpenGL texture
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    generated = true;
}

void Texture::bind() const {
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}
