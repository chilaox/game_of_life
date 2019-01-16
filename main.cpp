#include <iostream>
#include <chrono>
#include <emscripten/bind.h>

#include "render/gles_render.h"

int main(int argc, const char** argv)
{
	using namespace std;
	srand(chrono::system_clock::now().time_since_epoch().count());

	gles_render::instance().init();
	gles_render::instance().start();
}

void oncanvasresize()
{
	gles_render::instance().oncanvesresize();
}

EMSCRIPTEN_BINDINGS(module)
{
    emscripten::function("oncanvasresize", &oncanvasresize);
}