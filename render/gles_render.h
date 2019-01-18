#include <assert.h>
#include <emscripten/html5.h>
#include "../singleton.h"

class gles_render : public singleton<gles_render>
{
public:
	void init();

	void start();

	void pause();

	void draw();

	void oncanvesresize();
private:
	EMSCRIPTEN_WEBGL_CONTEXT_HANDLE mglcontex;
	int mview_width = 0;
	int mview_height = 0;

	static constexpr int mside = 50;
    static constexpr int msn = 8;
    static constexpr float mmargin = 0.1;
};