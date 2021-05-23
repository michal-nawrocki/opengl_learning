#include <assert.h>
#include <fstream>
#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <time.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GL_LOG_FILE "gl.log"

// ######## global vars ###########
int g_gl_width = 640;
int g_gl_height = 480;
// ################################

// Set Triangle coordinates
GLfloat points[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    -0.5f, 0.5f, 0.0f,

    -0.5f, 0.5f, 0.0f,
    0.5f, 0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,

};

// Util tool for reading shader programs
std::string* read_shader_program(std::string filepath){
    std::ifstream file_stream(filepath);
    return new std::string(std::istreambuf_iterator<char>(file_stream), std::istreambuf_iterator<char>());
}

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

bool gl_log_error(const char* message, ...){
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
        message,
        arguments
    ); 

    // Write to stderr
    vfprintf(
        stderr,
        message,
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

void glfw_window_size_callback(GLFWwindow* window, int width, int height){
    g_gl_width = width;
    g_gl_height = height;

    fprintf(stdout, "Width: %d\n Height: %d\n", width, height);
    /*  TODO:
    *   Update any perspective matiricies here
    */
}

void log_gl_params(){
    GLenum params[] = {
        GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
        GL_MAX_CUBE_MAP_TEXTURE_SIZE,
        GL_MAX_DRAW_BUFFERS,
        GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
        GL_MAX_TEXTURE_IMAGE_UNITS,
        GL_MAX_TEXTURE_SIZE,
        GL_MAX_VARYING_FLOATS,
        GL_MAX_VERTEX_ATTRIBS,
        GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
        GL_MAX_VERTEX_UNIFORM_COMPONENTS,
        GL_MAX_VIEWPORT_DIMS,
        GL_STEREO,
    };

    const char* names[] = {
        "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
        "GL_MAX_CUBE_MAP_TEXTURE_SIZE",
        "GL_MAX_DRAW_BUFFERS",
        "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
        "GL_MAX_TEXTURE_IMAGE_UNITS",
        "GL_MAX_TEXTURE_SIZE",
        "GL_MAX_VARYING_FLOATS",
        "GL_MAX_VERTEX_ATTRIBS",
        "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
        "GL_MAX_VERTEX_UNIFORM_COMPONENTS",
        "GL_MAX_VIEWPORT_DIMS",
        "GL_STEREO",
    };
        
    gl_log("GL Context Params:\n");
    for(int i = 0; i < 10; i++){
        int v = 0;
        glGetIntegerv(params[i], &v);
        gl_log("%s %i\n", names[i], v);
    }

    int v[2];
    v[0] = v[1] = 0;
    glGetIntegerv(params[10], v);
    gl_log("%s %i %i\n", names[10], v[0], v[1]);

    unsigned char s = 0;
    glGetBooleanv(params[11], &s);
    gl_log ("%s %u\n", names[11], (unsigned int)s);
    gl_log ("-----------------------------\n");
}

void _update_fps_counter(GLFWwindow* window){
    static double previous_seconds = glfwGetTime();
    static int frame_count;
    double current_seconds = glfwGetTime();
    double elapsed_seconds = current_seconds - previous_seconds;

    if(elapsed_seconds > 0.25){
        previous_seconds = current_seconds;
        double fps = (double) frame_count / elapsed_seconds;
        
        char tmp[128];
        sprintf(tmp, "OpenGL @ fps: %.2f", fps);
        glfwSetWindowTitle(window, tmp);
        frame_count = 0;
    }

    frame_count++;
}

void _print_shader_info_log(GLuint shader){
    int max_length = 2048;
    int actual_length = 0;
    char log[2048];

    glGetShaderInfoLog(shader, max_length, &actual_length, log);
    gl_log_error("Shader info log GL index %u:\n%s\n", &shader, &log);
}

bool check_shader_for_errors(GLuint shader){
    int params = -1;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &params);

    if(GL_TRUE != params){
        fprintf(
            stderr,
            "ERROR: Gl shader index %i did not compile\n",
            shader
        );

        _print_shader_info_log(shader);
        return false;
    }

    return true;
}

int main(){
    // Assert logging works and initialise log file
    assert(restart_gl_log());
    gl_log("Starting GLFW\n%s\n", glfwGetVersionString());

    // Setup GLFW
    glfwSetErrorCallback(glfw_error_callback);
    if(!glfwInit()){
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }

    // Use OpenGL 3.2
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Use Anti-Aliasing
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Setup fullscreen mode
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* video_mode = glfwGetVideoMode(monitor);
    GLFWwindow* window  = glfwCreateWindow(
        video_mode->width,
        video_mode->height,
        "Hello Triangle",
        NULL,
        NULL
    );
    if(!window){
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }

    // Set context to this window
    glfwMakeContextCurrent(window);

    // Setup windows size callback
    glfwSetWindowSizeCallback(window, glfw_window_size_callback);

    // GLEW extension handling
    glewExperimental = GL_TRUE;
    glewInit();

    // Get version info
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported: %s\n", version);
    log_gl_params();

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
    const char* vertex_shader = read_shader_program("shaders/test.vert")->c_str();
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);
    check_shader_for_errors(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragment_shader = read_shader_program("shaders/test.frag")->c_str();
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);
    check_shader_for_errors(fs);

    // Combine Shaders into Shader Program
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vs);
    glAttachShader(shader_program, fs);
    glLinkProgram(shader_program);
    glUseProgram(shader_program);

    // Obtain the inputColor var from the test.frag
    GLint colour_loc = glGetUniformLocation(shader_program, "inputColour" );

    // Specify the colour of our fragments
    glUniform4f(colour_loc, 1.0f, 0.0f, 0.0f, 1.0f);

    // Draw loop
    while(!glfwWindowShouldClose(window)){
        // Write FPS in window's title bar
        _update_fps_counter(window);

        // Clear draw surface
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, g_gl_width, g_gl_width);

        // Set background color to grey
        glClearColor(0.5, 0.5, 0.5, 1.0);

        // Specify the bounds and draw
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(points)/3);

        // Fetch input events
        glfwPollEvents();

        // Make Esc key close window
        if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)){
            glfwSetWindowShouldClose(window, 1);
        }

        // Display the surface
        glfwSwapBuffers(window);
    }

    // Cleanup and exit
    glfwTerminate();
    return 0;
}
