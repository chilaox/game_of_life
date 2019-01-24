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

	//mvp matrix
    glUniformMatrix4fv(mmvpos, 1, GL_FALSE, (GLfloat*)mmodelview.m);
    glUniformMatrix4fv(mpepos, 1, GL_FALSE, (GLfloat*)mperspective.m);

	//draw lines
    glVertexAttrib4f(1, 0.3, 0.3, 0.3, 1);
    glDrawElements(GL_LINES, sizeof(mlines) / sizeof(int), GL_UNSIGNED_INT, 0);
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
                                        "layout (location=0) in vec4 apos;"
                                        "layout (location=1) in vec3 acolor;"
                                        "out vec3 vcolor;"
                                        "void main() {"
                                        "gl_Position = perspective * modelview * apos;"
                                        "vcolor = acolor;"
                                        "}";

    GLuint vs = compile_shader(GL_VERTEX_SHADER, vertex_shader);

    static const char fragment_shader[] = "#version 300 es\n"
                                          "precision highp float;"
                                          "in vec3 vcolor;"
                                          "out vec4 color;"
                                          "void main() {"
                                          "color = vec4(vcolor, 1.0);"
                                          "}";

    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fragment_shader);

    GLuint program = create_program(vs, fs);
    glUseProgram(program);

    mmvpos = glGetUniformLocation(program, "modelview");
    mpepos = glGetUniformLocation(program, "perspective");

    //init pos
    auto idx = mpos;
    for (int x = 0; x <= msidenum; x++) {
        auto xp = msidelen * ((float)x - (float)msidenum / 2);
        auto yp = -msidelen * msidenum / 2;
        for (int y = 0; y <= msidenum; y++) {
            *idx++ = xp;
            *idx++ = yp;
            yp += msidelen;
        }
    }

    //line indices
    for (int i = 0; i <= msidenum; i++) {
        auto idx = i * 4;
        mlines[idx] = i;
        mlines[idx + 1] = msidenum * (msidenum + 1) + i;
        mlines[idx + 2] = i * msidenum + i;
        mlines[idx + 3] = i * msidenum + i + msidenum;
    }

    auto possize = sizeof(mpos);
    auto linesize = sizeof(mlines);

    glGenBuffers(1, &mposvbo);
    glBindBuffer(GL_ARRAY_BUFFER, mposvbo);
    glBufferData(GL_ARRAY_BUFFER, possize, mpos, GL_STATIC_DRAW);
    glVertexAttribPointer(0, mversize, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &mlineibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mlineibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, linesize, mlines, GL_STATIC_DRAW);

    glClearColor(0, 0, 0, 0);

    cout << "render start" << endl;

    emscripten_set_main_loop(update_frame, 0, true);
}

void gles_render::pause()
{
    cout << "render pause" << endl;
}
