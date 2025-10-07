#include "common.h"
#include "glad.h"
#include <GLFW/glfw3.h>

#include "shader.h"
#include "sphere.h"
#include "cube.h"
#include "texture.h"
#include "physics.h"
#include "sound.h"
#include "tracer.h"

// Window dimensions
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 900;
const char* WINDOW_TITLE = "OpenGL Boing Ball Demo";

// Global objects
GLFWwindow* window = nullptr;
Shader ballShader, wireframeShader;
Sphere* boingBall = nullptr;
Cube* boundingCube = nullptr;
Texture* ballTexture = nullptr;
Physics* ballPhysics = nullptr;
SoundSystem* soundSystem = nullptr;
BallTracer* ballTracer = nullptr;

// Camera settings
Vec3 cameraPos = Vec3(0, 0, 8);
Vec3 cameraTarget = Vec3(0, 0, 0);
Vec3 cameraUp = Vec3(0, 1, 0);
float cameraDistance = 8.0f;
float cameraAngleX = 0.0f;
float cameraAngleY = 0.0f;

// Lighting
Vec3 lightPos = Vec3(2, 4, 3);
Vec3 lightColor = Vec3(1.0f, 1.0f, 1.0f);

// Time tracking
float lastFrame = 0.0f;
float deltaTime = 0.0f;

// Input handling
bool keys[1024] = {false};
bool firstMouse = true;
float lastMouseX = WINDOW_WIDTH / 2.0f;
float lastMouseY = WINDOW_HEIGHT / 2.0f;

// Function declarations
bool initializeOpenGL();
bool initializeShaders();
bool initializeGeometry();
void processInput();
void updateCamera();
void render();
void cleanup();

// Callback functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

int main() {
    std::cout << "Starting OpenGL Boing Ball Demo..." << std::endl;
    
    // Initialize OpenGL and create window
    if (!initializeOpenGL()) {
        std::cerr << "Failed to initialize OpenGL" << std::endl;
        return -1;
    }
    
    // Load shaders
    if (!initializeShaders()) {
        std::cerr << "Failed to initialize shaders" << std::endl;
        cleanup();
        return -1;
    }
    
    // Create geometry and textures
    if (!initializeGeometry()) {
        std::cerr << "Failed to initialize geometry" << std::endl;
        cleanup();
        return -1;
    }
    
    // Initialize physics
    ballPhysics = new Physics(0.5f, 6.0f); // Ball radius 0.5, cube size 6
    
    // Initialize ball tracer
    ballTracer = new BallTracer(500);  // Track up to 500 points
    ballTracer->setTrailColor(Vec3(1.0f, 0.5f, 0.0f));  // Orange trail
    
    // Initialize sound system
    soundSystem = new SoundSystem();
    if (!soundSystem->initialize()) {
        std::cout << "Warning: Sound system failed to initialize" << std::endl;
    }
    
    // Enable depth testing and blending
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    std::cout << "Initialization complete. Starting main loop..." << std::endl;
    
    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        // Calculate delta time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Process input
        processInput();
        
        // Update physics
        ballPhysics->update(deltaTime);
        
        // Add current ball position to trail (sample every few frames to avoid too dense trail)
        static int trailSampleCounter = 0;
        if (++trailSampleCounter >= 3) {  // Sample every 3rd frame
            ballTracer->addPoint(ballPhysics->getPosition());
            trailSampleCounter = 0;
        }
        
        // Play sound if there was a collision
        CollisionType collision = ballPhysics->getLastCollision();
        if (collision != CollisionType::NONE && soundSystem) {
            soundSystem->playBounceSound(collision);
            ballPhysics->clearLastCollision();
        }
        
        // Update camera
        updateCamera();
        
        // Render everything
        render();
        
        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    cleanup();
    return 0;
}

bool initializeOpenGL() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA
    
    // Create window
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    // Make context current
    glfwMakeContextCurrent(window);
    
    // Set callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    
    // Initialize GLAD
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    
    // Print OpenGL version
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    
    return true;
}

bool initializeShaders() {
    // Load ball shader
    if (!ballShader.loadFromFiles("shaders/vertex.glsl", "shaders/fragment.glsl")) {
        std::cerr << "Failed to load ball shader" << std::endl;
        return false;
    }
    
    // Load wireframe shader (using same vertex shader, different fragment)
    if (!wireframeShader.loadFromFiles("shaders/vertex.glsl", "shaders/wireframe.glsl")) {
        std::cerr << "Failed to load wireframe shader" << std::endl;
        return false;
    }
    
    return true;
}

bool initializeGeometry() {
    // Create boing ball (sphere)
    boingBall = new Sphere(0.5f, 32, 32);
    boingBall->generate();
    
    // Create bounding cube
    boundingCube = new Cube(6.0f);
    boundingCube->generate();
    
    // Create boing ball texture
    ballTexture = new Texture();
    ballTexture->generateBoingTexture(256, 256, 32); // 256x256 texture with 32x32 pixel checkers
    
    return true;
}

void processInput() {
    // Close window on ESC
    if (keys[GLFW_KEY_ESCAPE]) {
        glfwSetWindowShouldClose(window, true);
    }
    
    // Reset ball position on R
    if (keys[GLFW_KEY_R]) {
        ballPhysics->reset();
    }
    
    // Camera controls
    float cameraSpeed = 5.0f * deltaTime;
    if (keys[GLFW_KEY_W]) cameraDistance -= cameraSpeed;
    if (keys[GLFW_KEY_S]) cameraDistance += cameraSpeed;
    if (keys[GLFW_KEY_A]) cameraAngleY -= cameraSpeed;
    if (keys[GLFW_KEY_D]) cameraAngleY += cameraSpeed;
    if (keys[GLFW_KEY_Q]) cameraAngleX -= cameraSpeed;
    if (keys[GLFW_KEY_E]) cameraAngleX += cameraSpeed;
    
    // Clamp camera distance
    cameraDistance = std::max(3.0f, std::min(15.0f, cameraDistance));
}

void updateCamera() {
    // Calculate camera position based on spherical coordinates
    float x = cameraDistance * cos(cameraAngleX) * cos(cameraAngleY);
    float y = cameraDistance * sin(cameraAngleX);
    float z = cameraDistance * cos(cameraAngleX) * sin(cameraAngleY);
    
    cameraPos = Vec3(x, y, z);
}

void render() {
    // Clear buffers
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f); // Dark blue background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Create matrices
    Mat4 view = Mat4::lookAt(cameraPos, cameraTarget, cameraUp);
    Mat4 projection = Mat4::perspective(M_PI / 4.0f, 
        static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT), 
        0.1f, 100.0f);
    
    // Render the boing ball
    ballShader.use();
    ballShader.setMat4("view", view);
    ballShader.setMat4("projection", projection);
    ballShader.setVec3("lightPos", lightPos);
    ballShader.setVec3("lightColor", lightColor);
    ballShader.setVec3("viewPos", cameraPos);
    ballShader.setBool("useTexture", true);
    ballShader.setInt("texture1", 0);
    
    // Create model matrix for the ball
    Vec3 ballPos = ballPhysics->getPosition();
    Vec3 ballRot = ballPhysics->getRotation();
    
    Mat4 model = Mat4::translate(ballPos) * 
                 Mat4::rotateX(ballRot.x) * 
                 Mat4::rotateY(ballRot.y) * 
                 Mat4::rotateZ(ballRot.z);
    
    ballShader.setMat4("model", model);
    
    // Bind texture and render ball
    glActiveTexture(GL_TEXTURE0);
    ballTexture->bind();
    boingBall->render();
    ballTexture->unbind();
    
    // Render the ball trail
    wireframeShader.use();
    wireframeShader.setMat4("view", view);
    wireframeShader.setMat4("projection", projection);
    wireframeShader.setMat4("model", Mat4()); // Identity matrix
    wireframeShader.setVec3("wireframeColor", Vec3(1.0f, 0.5f, 0.0f)); // Orange trail
    
    glLineWidth(2.0f);  // Thicker lines for trail
    ballTracer->render();
    
    // Render the wireframe cube
    wireframeShader.setVec3("wireframeColor", Vec3(0.3f, 0.7f, 1.0f)); // Light blue
    
    glDisable(GL_DEPTH_TEST); // Draw wireframe on top
    boundingCube->render();
    glEnable(GL_DEPTH_TEST);
}

void cleanup() {
    std::cout << "Cleaning up resources..." << std::endl;
    
    delete boingBall;
    delete boundingCube;
    delete ballTexture;
    delete ballPhysics;
    delete soundSystem;
    delete ballTracer;
    
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

// Callback implementations
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            keys[key] = true;
        } else if (action == GLFW_RELEASE) {
            keys[key] = false;
        }
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastMouseX = xpos;
        lastMouseY = ypos;
        firstMouse = false;
    }
    
    float xoffset = xpos - lastMouseX;
    float yoffset = lastMouseY - ypos; // Reversed Y
    
    lastMouseX = xpos;
    lastMouseY = ypos;
    
    // Only update camera if mouse button is pressed
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        float sensitivity = 0.005f;
        cameraAngleY += xoffset * sensitivity;
        cameraAngleX += yoffset * sensitivity;
        
        // Constrain pitch
        if (cameraAngleX > M_PI / 2.0f - 0.1f) cameraAngleX = M_PI / 2.0f - 0.1f;
        if (cameraAngleX < -M_PI / 2.0f + 0.1f) cameraAngleX = -M_PI / 2.0f + 0.1f;
    }
}
