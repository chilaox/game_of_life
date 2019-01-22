#include <chrono>
#include <emscripten/bind.h>
#include <iostream>

#include "render/gles_render.h"

EM_BOOL on_canvas_wheel(int eventType, const EmscriptenWheelEvent* wheelEvent, void*);

using namespace std;
int main(int argc, const char** argv)
{
    srand(chrono::system_clock::now().time_since_epoch().count());

    emscripten_set_wheel_callback("canvas", nullptr, true, on_canvas_wheel);

    gles_render::instance().init();
    gles_render::instance().start();
}

EM_BOOL on_canvas_wheel(int eventType, const EmscriptenWheelEvent* wheelEvent, void*)
{
    cout << "canvas wheel: " << wheelEvent->deltaX << '\t' << wheelEvent->deltaY << '\t' << wheelEvent->deltaZ << '\t' << endl;

    gles_render::instance().zoom(wheelEvent->deltaY > 0);

    return false;
}

void oncanvasresize()
{
    gles_render::instance().oncanvesresize();
}

EMSCRIPTEN_BINDINGS(module)
{
    emscripten::function("oncanvasresize", &oncanvasresize);
}