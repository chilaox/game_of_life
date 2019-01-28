#include "simulator.h"
#include "../core/universe.h"
#include "../render/gles_render.h"
#include <emscripten/bind.h>
#include <emscripten/emscripten.h>
#include <iostream>

using namespace std;

void update_frame();
EM_BOOL on_canvas_wheel(int eventType, const EmscriptenWheelEvent* wheelEvent, void*);
EM_BOOL on_wnd_resize(int eventType, const EmscriptenUiEvent* uiEvent, void*);

auto canvas_name = "canvas";

void simulator::run()
{
    universe::instance().init();
    universe::instance().random(1000);

    gles_render::instance().init();
    gles_render::instance().update_view();

    emscripten_set_resize_callback(nullptr, nullptr, false, on_wnd_resize);
    emscripten_set_wheel_callback(canvas_name, nullptr, false, on_canvas_wheel);

    emscripten_set_main_loop(update_frame, 0, true);
}

void update_frame()
{
    gles_render::instance().update_data(universe::instance().generation());
    gles_render::instance().draw();
}

EM_BOOL on_canvas_wheel(int eventType, const EmscriptenWheelEvent* wheelEvent, void*)
{
    cout << "canvas wheel: " << wheelEvent->deltaX << '\t' << wheelEvent->deltaY << '\t' << wheelEvent->deltaZ << '\t' << endl;

    gles_render::instance().zoom(wheelEvent->deltaY > 0);

    return false;
}

EM_BOOL on_wnd_resize(int eventType, const EmscriptenUiEvent* uiEvent, void*)
{
    gles_render::instance().update_view();

    return false;
}