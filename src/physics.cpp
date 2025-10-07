#include "physics.h"
#include <random>
#include <iostream>
#include <cmath>

Physics::Physics(float ballRadius, float cubeSize) 
    : ballRadius(ballRadius), cubeSize(cubeSize), lastCollision(CollisionType::NONE) {
    
    // Set default physics parameters for proper 3D bouncing
    gravity = -9.8f;  // Realistic gravity
    bounceDamping = 0.85f;  // More realistic bounce with some energy loss
    rotationSpeed = 2.0f;  // Moderate rotation
    
    std::cout << "Physics initialized - Ball radius: " << ballRadius << ", Cube size: " << cubeSize << std::endl;
    std::cout << "Playable area: " << (-cubeSize/2 + ballRadius) << " to " << (cubeSize/2 - ballRadius) << " on each axis" << std::endl;
    
    reset();
}

void Physics::reset() {
    // Start the ball at the center-top of the cube for predictable behavior
    position = Vec3(0.0f, cubeSize * 0.3f, 0.0f);
    
    // Give it sufficient velocity to reach all walls before losing energy
    velocity = Vec3(2.5f, 3.0f, 2.0f);  // Strong enough to reach all boundaries
    
    // Initialize rotation and angular velocity
    rotation = Vec3(0, 0, 0);
    angularVelocity = Vec3(rotationSpeed, rotationSpeed * 0.7f, rotationSpeed * 0.5f);
    
    lastCollision = CollisionType::NONE;
    
    std::cout << "Ball reset - Position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
    std::cout << "Initial velocity: (" << velocity.x << ", " << velocity.y << ", " << velocity.z << ")" << std::endl;
}

void Physics::update(float deltaTime) {
    // Apply gravity
    velocity.y += gravity * deltaTime;
    
    // Apply minimal air resistance to maintain horizontal movement
    float airResistance = 0.9995f;  // Almost no air resistance
    velocity.x *= airResistance;
    velocity.z *= airResistance;
    
    // Update position based on velocity
    position += velocity * deltaTime;
    
    // Controlled rotation based on velocity for visual effect
    rotation.x += velocity.z * deltaTime * 0.5f;  // Gentle roll based on Z movement
    rotation.y += angularVelocity.y * deltaTime * 0.5f;  // Slower constant rotation
    rotation.z -= velocity.x * deltaTime * 0.5f;  // Gentle roll based on X movement
    
    // Keep rotation values reasonable (prevent infinite accumulation)
    while (rotation.x > 2 * M_PI) rotation.x -= 2 * M_PI;
    while (rotation.x < -2 * M_PI) rotation.x += 2 * M_PI;
    while (rotation.y > 2 * M_PI) rotation.y -= 2 * M_PI;
    while (rotation.y < -2 * M_PI) rotation.y += 2 * M_PI;
    while (rotation.z > 2 * M_PI) rotation.z -= 2 * M_PI;
    while (rotation.z < -2 * M_PI) rotation.z += 2 * M_PI;
    
    // Check for collisions and bounce
    checkCollisions();
}

bool Physics::checkCollisions() {
    bool collided = false;
    lastCollision = CollisionType::NONE;
    
    float maxPos = (cubeSize / 2.0f) - ballRadius;  // Maximum allowed position
    float minPos = -(cubeSize / 2.0f) + ballRadius; // Minimum allowed position
    
    // Debug output
    static int debugCounter = 0;
    debugCounter++;
    if (debugCounter % 300 == 0) {  // Every ~5 seconds at 60fps
        std::cout << "Position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
        std::cout << "Velocity: (" << velocity.x << ", " << velocity.y << ", " << velocity.z << ")" << std::endl;
    }
    
    // Proper collision detection with equal and opposite reactions
    
    // Y-axis (vertical) - Floor and ceiling for complete bouncing
    if (position.y <= minPos) {
        position.y = minPos;  // Keep ball above floor
        if (velocity.y < 0) {  // Only bounce if moving downward
            velocity.y = -velocity.y * bounceDamping;  // Reverse and dampen
            // Prevent micro-bounces by adding minimum velocity
            if (std::abs(velocity.y) < 0.3f) {
                velocity.y = velocity.y > 0 ? 0.3f : -0.3f;
            }
            lastCollision = CollisionType::FLOOR;
            collided = true;
            std::cout << "Floor bounce - new Y velocity: " << velocity.y << std::endl;
        }
    }
    if (position.y >= maxPos) {
        position.y = maxPos;  // Keep ball below ceiling
        if (velocity.y > 0) {  // Only bounce if moving upward
            velocity.y = -velocity.y * bounceDamping;  // Reverse and dampen
            lastCollision = CollisionType::CEILING;
            collided = true;
            std::cout << "Ceiling bounce - new Y velocity: " << velocity.y << std::endl;
        }
    }
    
    // X-axis (left-right walls) - Equal and opposite reaction
    if (position.x <= minPos) {
        position.x = minPos;  // Keep ball inside left wall
        if (velocity.x < 0) {  // Only bounce if moving toward wall
            velocity.x = -velocity.x * bounceDamping;  // Reverse and dampen
            lastCollision = CollisionType::LEFT_WALL;
            collided = true;
            std::cout << "Left wall bounce - new X velocity: " << velocity.x << std::endl;
        }
    }
    if (position.x >= maxPos) {
        position.x = maxPos;  // Keep ball inside right wall
        if (velocity.x > 0) {  // Only bounce if moving toward wall
            velocity.x = -velocity.x * bounceDamping;  // Reverse and dampen
            lastCollision = CollisionType::RIGHT_WALL;
            collided = true;
            std::cout << "Right wall bounce - new X velocity: " << velocity.x << std::endl;
        }
    }
    
    // Z-axis (front-back walls) - Equal and opposite reaction
    if (position.z <= minPos) {
        position.z = minPos;  // Keep ball inside front wall
        if (velocity.z < 0) {  // Only bounce if moving toward wall
            velocity.z = -velocity.z * bounceDamping;  // Reverse and dampen
            lastCollision = CollisionType::FRONT_WALL;
            collided = true;
            std::cout << "Front wall bounce - new Z velocity: " << velocity.z << std::endl;
        }
    }
    if (position.z >= maxPos) {
        position.z = maxPos;  // Keep ball inside back wall
        if (velocity.z > 0) {  // Only bounce if moving toward wall
            velocity.z = -velocity.z * bounceDamping;  // Reverse and dampen
            lastCollision = CollisionType::BACK_WALL;
            collided = true;
            std::cout << "Back wall bounce - new Z velocity: " << velocity.z << std::endl;
        }
    }
    
    return collided;
}
