#include "../utility/singleton.h"
#include "esUtil.h"
#include <emscripten/html5.h>
#include <vector>

class gles_render : public singleton<gles_render> {
public:
    void init();

    void update_data(const std::vector<int>&);

    void update_view(int width, int height);

    void draw() const;

    void zoom(bool isout);
    void move(int x, int y);

private:
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE mglcontex;
    int mview_width = 0;
    int mview_height = 0;

    static constexpr float mmax_scale = 8;
    static constexpr float mmin_scale = 0.125;
    float mscale = 1;
    float mscreenoffset[2] = { 0, 0 };
    bool mshow_lines = true;

    GLint moffset_pos;
    GLint maspect_pos;
    GLint mscale_pos;

    GLuint mposvbo;
    GLuint mlineibo;
    GLuint mliveibo;

    static constexpr float msidelen = 0.01f;
    static constexpr int msidenum = 1004;
    static constexpr int mversize = 2;
    static constexpr int mstride = sizeof(float) * mversize;
    static constexpr int mposnum = (msidenum + 1) * (msidenum + 1);
    static constexpr int mcellidxsize = msidenum * msidenum * 5;
    float mpos[mposnum * mversize];
    int mlines[mversize * (msidenum + 1) * 2];
    int mcells[mcellidxsize];
    int mlivenum = 0;

    void update_model();
    void update_offest();
    void update_lines_visible();
};