#include "tracer.h"
#include <vector>

BallTracer::BallTracer(int maxTrailPoints) 
    : maxPoints(maxTrailPoints), trailColor(Vec3(1.0f, 0.5f, 0.0f)), generated(false) {
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    generated = true;
}

BallTracer::~BallTracer() {
    if (generated) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
}

void BallTracer::addPoint(const Vec3& position) {
    trailPoints.push_back(position);
    
    // Remove oldest points if we exceed max
    while (trailPoints.size() > static_cast<size_t>(maxPoints)) {
        trailPoints.pop_front();
    }
    
    updateBuffers();
}

void BallTracer::render() {
    if (trailPoints.size() < 2) return;
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_STRIP, 0, trailPoints.size());
    glBindVertexArray(0);
}

void BallTracer::clear() {
    trailPoints.clear();
    updateBuffers();
}

void BallTracer::setTrailLength(int length) {
    maxPoints = length;
    while (trailPoints.size() > static_cast<size_t>(maxPoints)) {
        trailPoints.pop_front();
    }
    updateBuffers();
}

void BallTracer::setTrailColor(const Vec3& color) {
    trailColor = color;
}

void BallTracer::updateBuffers() {
    if (trailPoints.empty()) return;
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    // Upload trail points as vertex data - convert deque to vector for contiguous memory
    std::vector<Vec3> points(trailPoints.begin(), trailPoints.end());
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(Vec3), 
                points.data(), GL_DYNAMIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
}
