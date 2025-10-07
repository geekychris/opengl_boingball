#pragma once

#include "common.h"
#include "glad.h"
#include <deque>

class BallTracer {
public:
    BallTracer(int maxTrailPoints = 200);
    ~BallTracer();
    
    void addPoint(const Vec3& position);
    void render();
    void clear();
    
    void setTrailLength(int length);
    void setTrailColor(const Vec3& color);

private:
    std::deque<Vec3> trailPoints;
    int maxPoints;
    Vec3 trailColor;
    
    unsigned int VAO, VBO;
    bool generated;
    
    void updateBuffers();
};
