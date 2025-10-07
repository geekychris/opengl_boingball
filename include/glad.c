#include "glad.h"

int gladLoadGL(void) {
    // On macOS with OpenGL 3.3 core profile, functions are already loaded
    // This is a dummy implementation for compatibility
    return 1;
}
