#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

int main(){
    // Initialise GLFW
    if(!glfwInit()){
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }

    // Something nice for MacOS that will be explained later
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

    // Cleanup and exit
    glfwTerminate();
    return 0;

}
