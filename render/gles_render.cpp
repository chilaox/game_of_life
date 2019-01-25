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
    //init context
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);
    attrs.majorVersion = 2;
    attrs.minorVersion = 0;

    mglcontex = emscripten_webgl_create_context(0, &attrs);
    assert(mglcontex);
    auto res = emscripten_webgl_make_context_current(mglcontex);
    assert(res == EMSCRIPTEN_RESULT_SUCCESS);

    //link program
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

    //init vbos
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

    glGenBuffers(1, &mliveibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mliveibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, msidenum * msidenum * sizeof(int) * 5, nullptr, GL_DYNAMIC_DRAW);

    glClearColor(0, 0, 0, 0);

    //init mvp
    update_model();
    update_perspective();

    cout << "glrender init" << endl;
}

void gles_render::zoom(bool out)
{
    auto oldz = moffsetz;
    auto diff = moffsetz + mnear;

    if (out) {
        diff *= 1.1f;
    } else {
        diff /= 1.1f;
    }

    moffsetz = min(max(diff - mnear, mzmin), mzmax);

    cout << "z:" << moffsetz << endl;

    update_model();
}

void gles_render::update_view()
{
    emscripten_get_canvas_element_size("canvas", &mview_width, &mview_height);
    cout << "view size:" << mview_width << " " << mview_height << endl;

    update_perspective();

    glViewport(0, 0, mview_width, mview_height);
}

void gles_render::update_model()
{
    esMatrixLoadIdentity(&mmodel);
    esTranslate(&mmodel, 0, 0, moffsetz);
    glUniformMatrix4fv(mmvpos, 1, GL_FALSE, (GLfloat*)mmodel.m);
}

void gles_render::update_perspective()
{
    // Generate a perspective matrix with a 60 degree FOV
    esMatrixLoadIdentity(&mperspective);

    auto aspect = (float)mview_width / (float)mview_height;
    esPerspective(&mperspective, 60.0f, aspect, mnear, mfar);

    glUniformMatrix4fv(mpepos, 1, GL_FALSE, (GLfloat*)mperspective.m);
}

void gles_render::update_data(const vector<int>& liveidx)
{
    mlivenum = liveidx.size();

    // mlivenum = 2;
    // mcells[0] = 0;
    // mcells[1] = 1;
    // mcells[2] = 12;
    // mcells[3] = 11;
    // mcells[4] = 0xFFFFFFFF;

    // mcells[5] = 12;
    // mcells[6] = 13;
    // mcells[7] = 24;
    // mcells[8] = 23;
    // mcells[9] = 0xFFFFFFFF;

    int count = 0;
    for (auto idx : liveidx) {
        auto i = count * 5;
        auto sidx = idx + idx / msidenum;

        mcells[i] = sidx;
        mcells[i + 1] = sidx + 1;
        mcells[i + 2] = sidx + msidenum + 2;
        mcells[i + 3] = sidx + msidenum + 1;
        mcells[i + 4] = 0xFFFFFFFF;
        count++;
    }
}

void gles_render::draw()
{
    glClear(GL_COLOR_BUFFER_BIT);

    //draw cells
    if (mlivenum != 0) {
        glVertexAttrib4f(1, 1, 1, 1, 1);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mliveibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mlivenum * 5 * sizeof(int), mcells, GL_DYNAMIC_DRAW);
        glDrawElements(GL_TRIANGLE_FAN, mlivenum * 5, GL_UNSIGNED_INT, 0);
    }

    //draw lines
    glVertexAttrib4f(1, 0.3, 0.3, 0.3, 1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mlineibo);
    glDrawElements(GL_LINES, sizeof(mlines) / sizeof(int), GL_UNSIGNED_INT, 0);
}
