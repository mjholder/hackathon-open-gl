#include "Renderer.h"
#include <iostream>

void GLClearError()
{
    // keep polling backlog of errors until we read them all
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
    // loop through errors, printing them
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << ")" 
        << function << ' ' << file << ':' << line << std::endl;
        return false;
    }

    return true;
}