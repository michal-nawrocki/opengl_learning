#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <time.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GL_LOG_FILE "gl.log"

bool restart_gl_log(){
    // Handle IO 
    FILE* log_file = fopen(GL_LOG_FILE, "w");
    if(!log_file){
        fprintf(
            stderr,
            "ERROR: could not open GL_LOG_FILE log file %s for writing\n",
            GL_LOG_FILE
        );
        return false;
    }

    // Obtain current time
    time_t now = time(NULL);
    char* date = ctime(&now);

    // Initialise logfile with date
    fprintf(
        log_file,
        "[GL_LOG_FILE log. local time %s\n]",
        date
    );

    // IO cleanup
    fclose(log_file);

    return true;
}

bool gl_log(std::string message, ...){
    // Initialise the variable arguments
    va_list arguments;

    // Handle IO
    FILE* file = fopen(GL_LOG_FILE, "a");
    if(!file){
        fprintf(
            stderr,
            "ERROR: could not open GL_LOG_FILE %s file for appending\n",
            GL_LOG_FILE
        );
        return false;
    }
    
    // Handle variable arguments
    va_start(arguments, message);
    vfprintf(
        file,
        message.c_str(),
        arguments
    );
    va_end(arguments);
    
    // Cleanup IO
    fclose(file);

    return true;

}

bool gl_log_error(std::string message, ...){
    // Initialise variable arguments
    va_list arguments;

    // Handle IO
    FILE* file = fopen(GL_LOG_FILE, "a");
    if(!file){
        fprintf(
            stderr,
            "ERROR: could not open GL_LOG_FILE %s file for appending\n",
            GL_LOG_FILE
        );
        return false;
    }

    // Write to logfile
    va_start(arguments, message);
    vfprintf(
        file,
        message.c_str(),
        arguments
    ); 

    // Write to stderr
    vfprintf(
        stderr,
        message.c_str(),
        arguments
    );

    // IO cleanup
    va_end(arguments);
    fclose(file);

    return true;
}

void glfw_error_callback(int error, const char* description){
    gl_log_error("GLFW Error: code %i msg: %s\n", error, description);
}

int main(){
    // Assert logging works and initialise log file
    assert(restart_gl_log());
    gl_log("Starting GLFW\n%s\n", glfwGetVersionString());

    // Set error call-back function
    glfwSetErrorCallback(glfw_error_callback);

    // Initialise GLFW
    if(!glfwInit()){
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }

    // Something nice for MacOS that will be explained later
    #ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #endif

    GLFWwindow* window  = glfwCreateWindow(640, 480, "Hello Triangle", NULL, NULL);
    if(!window){
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }else{
        glfwMakeContextCurrent(window);
    }

    // GLEW extension handling
    glewExperimental = GL_TRUE;
    glewInit();

    // Get version info
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported: %s\n", version);

    // Draw something
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Draw loop
    while(!glfwWindowShouldClose(window)){
        // Clear draw surface
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Set background color to grey
        glClearColor(0.5, 0.5, 0.5, 1.0);

        // Fetch input events
        glfwPollEvents();

        // Display the surface
        glfwSwapBuffers(window);

    }

    // Cleanup and exit
    glfwTerminate();
    return 0;

}
