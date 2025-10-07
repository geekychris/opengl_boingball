#pragma once

#include "common.h"
#include "glad.h"

class Texture {
public:
    Texture();
    ~Texture();
    
    void generateBoingTexture(int width, int height, int checkerSize);
    void bind() const;
    void unbind() const;
    
    unsigned int getID() const { return textureID; }

private:
    unsigned int textureID;
    bool generated;
};
