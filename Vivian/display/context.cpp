
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "linmath.h"
#include <iostream>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>


namespace py = pybind11;


//static const Vertex vertices[3] =
//        {
//                { { -0.3f, -0.4f ,0.f}, { 1.f, 0.f, 0.f } },
//                { {  0.6f, -0.4f ,0.f}, { 0.f, 1.f, 0.f } },
//                { {   0.f,  0.6f ,0.f}, { 0.f, 0.f, 1.f } }
//        };

static const char* vertex_shader_text =
        "#version 330\n"
        "uniform mat4 MVP;\n"
//        "in vec3 vCol;\n"
        "in vec3 vPos;\n"
//        "out vec3 color;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = MVP * vec4(vPos, 1.0);\n"
//        "    color = vCol;\n"
        "}\n";

static const char* fragment_shader_text =
        "#version 330\n"
//        "in vec3 color;\n"
        "out vec4 fragment;\n"
        "void main()\n"
        "{\n"
        "    fragment = vec4(1.0,1.0,1.0, 1.0);\n"
        "}\n";

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void show(py::array_t<float> pointcloud,char* name)
{
    py::buffer_info pointcloud_buf = pointcloud.request();
    static float* vertices = (float *)pointcloud_buf.ptr;
    std::cout<< vertices[0]<<' '<<vertices[1]<<' '<<vertices[3]<<' '<<vertices[9]<<"      "<<pointcloud_buf.size/3<<"      ";
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);


//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(640, 480,name, NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);
    std::cout<<glGetString(GL_VERSION)<<std::endl;
    // NOTE: OpenGL error checks have been omitted for brevity

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, pointcloud_buf.size*sizeof(float), vertices, GL_STATIC_DRAW);

    const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    const GLint mvp_location = glGetUniformLocation(program, "MVP");
    const GLint vpos_location = glGetAttribLocation(program, "vPos");
//    const GLint vcol_location = glGetAttribLocation(program, "vCol");

    GLuint vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                          3*sizeof(float), (void*)0);
//    glEnableVertexAttribArray(vcol_location);
//    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
//                          6*sizeofArray(vcol_location);
//    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
//                          6*sizeof(float), (void*)((GLintptr)3*sizeof(float)));
    while (!glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        const float ratio = width / (float) height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        mat4x4 m, p, mvp;
        mat4x4_identity(m);
        mat4x4_rotate_X(m, m, (float) glfwGetTime());
        mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        mat4x4_mul(mvp, p, m);

        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) &mvp);
        glBindVertexArray(vertex_array);
        glDrawArrays(  GL_POINTS, 0, pointcloud_buf.size/3);
        glPointSize(3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    std::cout << glGetError() << std::endl;
    glfwTerminate();
    exit(EXIT_SUCCESS);
}


PYBIND11_MODULE(context, m ) {
m . def ( "show" , & show ) ;
}

