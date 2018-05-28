#include "gl_utils/gl_error.h"

#include "glad/glad.h"

#include <utils/log.h>

void APIENTRY glDebugOutput(GLenum source,
                            GLenum type,
                            GLuint id,
                            GLenum severity,
                            GLsizei length,
                            const GLchar *message,
                            const void *userParam)
{
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    debug("\n-----------------\nDebug message (%d): %s\n", id, message);

    switch (source) {
        case GL_DEBUG_SOURCE_API:
            debug("Source: API\n"); break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            debug("Source: Window System\n"); break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            debug("Source: Shader Compiler\n"); break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            debug("Source: Third Party\n"); break;
        case GL_DEBUG_SOURCE_APPLICATION:
            debug("Source: Application\n"); break;
        case GL_DEBUG_SOURCE_OTHER:
            debug("Source: Other\n"); break;
    }
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            error("Type: Error\n"); break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            debug("Type: Deprecated Behaviour\n"); break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            error("Type: Undefined Behaviour\n"); break;
        case GL_DEBUG_TYPE_PORTABILITY:
            debug("Type: Portability\n"); break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            debug("Type: Performance\n"); break;
        case GL_DEBUG_TYPE_OTHER:
        default:
            debug("Type: Other\n"); break;
    }
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            error("Severity: high\n"); break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            warn("Severity: medium\n"); break;
        case GL_DEBUG_SEVERITY_LOW:
        default:
            debug("Severity: low\n"); break;
    }
}

