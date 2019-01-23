#include "gles_render.h"
#include <GLES3/gl3.h>
#include <assert.h>
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

void gles_render::zoom(bool out)
{
    auto diff = moffsetz + mnear;

    if (out) {
        diff *= 1.1f;
    } else {
        diff /= 1.1f;
    }

    moffsetz = min(max(diff - mnear, mzmin), mzmax);

    cout << "z: " << moffsetz << endl;
}

void gles_render::update()
{
    emscripten_get_canvas_element_size("canvas", &mview_width, &mview_height);

    // Generate a model view matrix to rotate/translate the cube
    esMatrixLoadIdentity(&mmodelview);
    // Translate away from the viewer

    esTranslate(&mmodelview, 0, 0, moffsetz);

    // Generate a perspective matrix with a 60 degree FOV
    esMatrixLoadIdentity(&mperspective);

    auto aspect = (float)mview_width / (float)mview_height;
    esPerspective(&mperspective, 60.0f, aspect, mnear, mfar);
}

void gles_render::draw()
{
    glViewport(0, 0, mview_width, mview_height);
    glClear(GL_COLOR_BUFFER_BIT);

    glUniformMatrix4fv(mmvpos, 1, GL_FALSE, (GLfloat*)mmodelview.m);
    glUniformMatrix4fv(mpepos, 1, GL_FALSE, (GLfloat*)mperspective.m);

    glDrawArrays(GL_LINES, 0, (msidenum + 1) * 4);
    glDrawArrays(GL_TRIANGLE_FAN,  (msidenum + 1) * 4, mcells.size() / 2);
}

void update_frame()
{
    gles_render::instance().update();
    gles_render::instance().draw();
}

void gles_render::start()
{
    static const char vertex_shader[] = "#version 300 es\n"
                                        "uniform mat4 modelview;"
                                        "uniform mat4 perspective;"
                                        "layout (location=0) in vec4 pos;"
                                        "void main() {"
                                        "gl_Position = perspective * modelview * pos;"
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

    mmvpos = glGetUniformLocation(program, "modelview");
    mpepos = glGetUniformLocation(program, "perspective");

    float x0 = -msidenum * msidelen / 2;

    for (int i = 0; i <= msidenum; i++) {
        mlines[midxc * i] = mlines[midxc * i + 5] = x0;
        mlines[midxc * i + 1] = mlines[midxc * i + 3] = mlines[midxc * i + 4] = mlines[midxc * i + 6] = x0 + i * msidelen;
        mlines[midxc * i + 2] = mlines[midxc * i + 7] = -x0;
    }

    mcells = { 0.0f, 0.0f, 0.0f, 0.1f, 0.1f, 0.1f, 0.1f, 0.0f};

    GLuint vbol;
    glGenBuffers(1, &vbol);
    glBindBuffer(GL_ARRAY_BUFFER, vbol);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mlines) + mcells.size() * sizeof(float), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(mlines), mlines);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(mlines), mcells.size() * sizeof(float), mcells.data());
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    glEnableVertexAttribArray(0);

    glClearColor(0, 0, 0, 0);

    cout << "render start" << endl;

    emscripten_set_main_loop(update_frame, 0, true);
}

void gles_render::pause()
{
    cout << "render pause" << endl;
}
