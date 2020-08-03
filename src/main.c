#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "linmath.h"
#include "utils/args.h"
#include "utils/json_parse.h"
#include "config.h"

#include "utils/log.h"
#include "utils/utils.h"
#include "context.h"

#include "gl_utils/gl_error.h"

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 640

context_t* ctx;

/**
 * A callback that resize the opengl viewport
 *
 * Should NOT be be called, because everything is handled by GLFW
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);

    context_resize(ctx, width, height);
}


/**
 * Initialize GLFW
 *
 * @return NULL on error. Otherwise, the newly created window.
 */
GLFWwindow* initializeWindow(bool debug){
    info("Initializing GLFW\n");
    GLFWwindow* window;

    if (!glfwInit()){
        error("Can't init GLFW! Bye!\n");
        return NULL;
    }

#ifndef DEBUG
    if(debug){
#endif
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#ifndef DEBUG
    }
#endif

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT,
            "Pannellum in C, more complex, less util!", NULL, NULL);
    if (!window){
        glfwTerminate();
        error("Can't create GLFW window! Bye!\n");
        exit(-1);
    }

    glfwMakeContextCurrent(window);
    info("GLFW initialized\n");

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        error("Failed to initialize GLAD! Bye!");
        exit(-1);
    }

    int major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    info("Starting with openGL %d.%d\n", major, minor);
#ifdef DEBUG
    if(major < 4)
        error("I'm waiting for openGL 3 minimum on release target, 4 on debug target");
#else
    if(major < 4)
        error("I'm waiting for openGL 3 minimum on release target, 4 on debug target");
#endif

#ifndef DEBUG
    if(debug){
#endif
        GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        warn("DEBUG MODE! GL version %s\n", glGetString(GL_VERSION));
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT){
            warn("OpenGL is in debug mode !\n");
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(glDebugOutput, NULL);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE,
                    0, NULL, GL_TRUE);
        }
#ifndef DEBUG
    }
#endif

    // Callback will be called on resize
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return window;
}

float fov = M_PI / 2,
    pitch = 0,
    yaw = 0;


/**
 * Process inputs
 */
void process_input(GLFWwindow* window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
        warn("You don't want me anymore?! That's so saaad :'-(\n");
    }
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        pitch += 0.01;
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        pitch -= 0.01;
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        yaw += 0.01;
    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        yaw -= 0.01;
    if(glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
        fov += 0.01;
    if(glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
        fov -= 0.01;
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        context_switch(ctx, 1);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        context_switch(ctx, 0);
}

/**
 * The render & event loop of the program
 */
void loop(GLFWwindow* window){
    while (!glfwWindowShouldClose(window)){
        process_input(window);

        context_render(ctx, fov, pitch, yaw);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}


int main(int argc, char** argv) {
    struct args args = {.debug=false, .json=NULL};

    parse_args(&args, argc, argv);
    info("Hi! Nice to meet you\n");

    config_t* config = json_parse_main_config(args.json);
    if(config == NULL){
        error("bye");
        return -1;
    }

    GLFWwindow* window = initializeWindow(args.debug);

    ctx = malloc(sizeof(context_t));
    context_init(ctx, SCREEN_WIDTH, SCREEN_HEIGHT);

    size_t id_start = configure_context(ctx, config);

    context_switch(ctx, id_start);

    loop(window);

    context_delete(ctx);
    free(ctx);

    free(config->valid_ids);
    free(config->config_path);
    free(config->base_path);
    free(config);

    info("Bye!\n");
    glfwTerminate();
    return 0;
}
