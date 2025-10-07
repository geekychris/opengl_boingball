#pragma once

#include "common.h"

enum class CollisionType {
    NONE = 0,
    FLOOR = 1,
    CEILING = 2,
    LEFT_WALL = 4,
    RIGHT_WALL = 8,
    FRONT_WALL = 16,
    BACK_WALL = 32
};

class Physics {
public:
    Physics(float ballRadius, float cubeSize);
    
    void update(float deltaTime);
    void reset();
    
    Vec3 getPosition() const { return position; }
    Vec3 getVelocity() const { return velocity; }
    Vec3 getRotation() const { return rotation; }
    
    CollisionType getLastCollision() const { return lastCollision; }
    void clearLastCollision() { lastCollision = CollisionType::NONE; }
    
    // Physics parameters
    void setGravity(float g) { gravity = g; }
    void setBounceDamping(float damping) { bounceDamping = damping; }
    void setRotationSpeed(float speed) { rotationSpeed = speed; }

private:
    Vec3 position;
    Vec3 velocity;
    Vec3 rotation;
    Vec3 angularVelocity;
    
    float ballRadius;
    float cubeSize;
    float gravity;
    float bounceDamping;
    float rotationSpeed;
    
    CollisionType lastCollision;
    
    bool checkCollisions();
};
