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
    glDrawArrays(GL_LINES, 0, (mside + 1) * msn / 2);
}

void cdraw()
{
    gles_render::instance().draw();
}

void gles_render::start()
{
    static const char vertex_shader[] = "#version 300 es\n"
                                        "layout (location=0) in vec4 apos;"
                                        "void main() {"
                                        "gl_Position = apos;"
                                        "}";

    GLuint vs = compile_shader(GL_VERTEX_SHADER, vertex_shader);

    static const char fragment_shader[] = "#version 300 es\n"
                                          "precision highp float;"
                                          "out vec4 color;"
                                          "void main() {"
                                          "color = vec4(0.5, 0.5, 0.5, 1.0);"
                                          "}";

    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fragment_shader);

    GLuint program = create_program(vs, fs);
    glUseProgram(program);

    const float step = (1 - mmargin) * 2 / mside;
    static float pos[(mside + 1) * msn];

    for (int i = 0; i <= mside; i++) {
        pos[msn * i] = pos[msn * i + 5] = -1 + mmargin;
        pos[msn * i + 1] = pos[msn * i + 3] = pos[msn * i + 4] = pos[msn * i + 6] = -1 + mmargin + i * step;
        pos[msn * i + 2] = pos[msn * i + 7] = 1 - mmargin;
    }

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    glEnableVertexAttribArray(0);
    glClearColor(0.1f, 0.1f, 0.1f, 1);

    emscripten_set_main_loop(cdraw, 0, true);

    cout << "render start" << endl;
}

void gles_render::pause()
{
    cout << "render pause" << endl;
}
