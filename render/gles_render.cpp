#include "gles_render.h"
#include <GLES3/gl3.h>
#include <emscripten.h>
#include <iostream>

using namespace std;

GLuint compile_shader(GLenum shaderType, const char* src)
{
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (!isCompiled) {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
        char* buf = (char*)malloc(maxLength + 1);
        glGetShaderInfoLog(shader, maxLength, &maxLength, buf);
        printf("compile shader error : %s\n", buf);
        free(buf);
        return 0;
    }

    return shader;
}

GLuint create_program(GLuint vertexShader, GLuint fragmentShader)
{
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    return program;
}

void gles_render::init()
{
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);
    attrs.majorVersion = 2;
    attrs.minorVersion = 0;

    mglcontex = emscripten_webgl_create_context(0, &attrs);
    assert(mglcontex);
    auto res = emscripten_webgl_make_context_current(mglcontex);
    assert(res == EMSCRIPTEN_RESULT_SUCCESS);

    static const char vertex_shader[] = "#version 300 es\n"
                                        "layout (location=0) in vec4 apos;"
                                        "layout (location=1) in vec4 acolor;"
                                        "out vec4 color;"
                                        "void main() {"
                                        "color = acolor;"
                                        "gl_Position = apos;"
                                        "}";

    GLuint vs = compile_shader(GL_VERTEX_SHADER, vertex_shader);

    static const char fragment_shader[] = "#version 300 es\n"
                                          "precision lowp float;"
                                          "in vec4 color;"
                                          "out vec4 frcolor;"
                                          "void main() {"
                                          "frcolor = color;"
                                          "}";

    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fragment_shader);

    GLuint program = create_program(vs, fs);
    glUseProgram(program);

    cout << "webgl contex init:" << mglcontex << endl;
}

void gles_render::oncanvesresize()
{
}

void gles_render::draw()
{
    emscripten_get_canvas_element_size("canvas", &mview_width, &mview_height);
    glViewport(0, 0, mview_width, mview_height);
    
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void cdraw()
{
    gles_render::instance().draw();
}

void gles_render::start()
{
    cout << "render start" << endl;

    static const float pos_and_color[] = {
        //     x,     y, r, g, b
        -0.6f,
        -0.6f,
        1,
        0,
        0,
        0.6f,
        -0.6f,
        0,
        1,
        0,
        0.f,
        0.6f,
        0,
        0,
        1,
    };

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pos_and_color), pos_and_color, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 20, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 20, (void*)8);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glClearColor(0.0f, 0.0f, 0.0f, 1);

    emscripten_set_main_loop(cdraw, 0, true);
}

void gles_render::pause()
{
    cout << "render pause" << endl;
}
