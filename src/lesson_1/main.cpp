#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>


// Set Triangle coordinates
GLfloat points[] = {
    0.0f, 0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f
};

// Shader programs
const char* vertex_shader = 
    "#version 400\n"
    "in vec3 vp;"
    "void main (){"
    "   gl_Position = vec4(vp, 1.0);"
    "}";

const char* fragment_shader =
    "#version 400\n"
    "out vec4 frag_colour;"
    "void main (){"
    "   frag_colour = vec4(0.5, 0.0, 0.5, 1.0);"
    "}";

std::string read_shader_program(std::string filepath){
    std::ifstream file_stream(filepath);
    std::string shader_program((std::istreambuf_iterator<char>(file_stream)), std::istreambuf_iterator<char>());

    return shader_program;
}

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

    // Setup buffer
    GLuint vbo = 0;
    glGenBuffers (1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    // Setup Vertex Attribute Object
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    // Load Shaders
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    // Combine Shaders into Shader Program
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vs);
    glAttachShader(shader_program, fs);
    glLinkProgram(shader_program);

    // Draw loop
    while(!glfwWindowShouldClose(window)){
        // Clear draw surface
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Set background color to grey
        glClearColor(0.5, 0.5, 0.5, 1.0);

        // Fillament of the triangle area bound by vertex array
        glUseProgram(shader_program);

        // Specify the bounds and draw
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Fetch input events
        glfwPollEvents();

        // Display the surface
        glfwSwapBuffers(window);

    }

    // Cleanup and exit
    glfwTerminate();
    return 0;

}
