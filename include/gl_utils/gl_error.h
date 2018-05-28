#ifndef GL_ERROR_H
#define GL_ERROR_H

#include "glad/glad.h"

void APIENTRY glDebugOutput(GLenum source,
                            GLenum type,
                            GLuint id,
                            GLenum severity,
                            GLsizei length,
                            const GLchar *message,
                            const void *userParam);

#endif /* ifndef GL_ERROR_H */
