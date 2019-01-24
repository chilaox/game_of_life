#include "../singleton.h"
#include "esUtil.h"
#include <emscripten/html5.h>
#include <vector>

class gles_render : public singleton<gles_render> {
public:
	void init();

	void start();

	void pause();

	void update();

	void draw();

	void oncanvesresize();

	void zoom(bool isout);

private:
	EMSCRIPTEN_WEBGL_CONTEXT_HANDLE mglcontex;
	int mview_width = 0;
	int mview_height = 0;

	float mnear = 1.0f;
	float mfar = 100.0f;
	float mzmax = -mnear - 0.1f;
	float mzmin = -mfar;
	float moffsetz = mzmax;

	ESMatrix mperspective;
	ESMatrix mmodelview;

	GLint mmvpos;
	GLint mpepos;
	GLuint mlineibo;
	GLuint mposvbo;

	static constexpr float msidelen = 0.1f;
	static constexpr int msidenum = 1000;
	static constexpr int mvpl = 4;
	static constexpr int mversize = 2;
	static constexpr int mstride = sizeof(float) * mversize;
	static constexpr int mposnum = (msidenum + 1) * (msidenum + 1);
	float mpos[mposnum * mversize];
    int mlines[mversize * (msidenum + 1) * 2];
	std::vector<int> mcells;
};