#include "gles_render.h"
#include <GLES3/gl3.h>
#include <emscripten.h>
#include <functional>
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
    glBindAttribLocation(program, 0, "apos");
    glBindAttribLocation(program, 1, "acolor");
    glLinkProgram(program);
    return program;
}

void gles_render::init()
{
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);

    mglcontex = emscripten_webgl_create_context(0, &attrs);
    assert(mglcontex);
    auto res = emscripten_webgl_make_context_current(mglcontex);
    assert(res == EMSCRIPTEN_RESULT_SUCCESS);

    static const char vertex_shader[] = "attribute vec4 apos;"
                                        "attribute vec4 acolor;"
                                        "varying vec4 color;"
                                        "void main() {"
                                        "color = acolor;"
                                        "gl_Position = apos;"
                                        "}";
    GLuint vs = compile_shader(GL_VERTEX_SHADER, vertex_shader);

    static const char fragment_shader[] = "precision lowp float;"
                                          "varying vec4 color;"
                                          "void main() {"
                                          "gl_FragColor = color;"
                                          "}";
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fragment_shader);

    GLuint program = create_program(vs, fs);
    oncanvesresize();
    glUseProgram(program);

    cout << "webgl contex init:" << mglcontex << endl;
}

void gles_render::oncanvesresize()
{
    emscripten_get_canvas_element_size("canvas", &mview_width, &mview_height);
    cout << mview_width << '\t' << mview_height << endl;
}

void gles_render::draw()
{
    cout << "render draw" << endl;

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
