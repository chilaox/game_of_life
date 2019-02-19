#include "simulator.h"
#include "../core/universe.h"
#include "../render/gles_render.h"
#include <emscripten/bind.h>
#include <emscripten/emscripten.h>
#include <iostream>

using namespace std;

void update_view();
void update_frame();
EM_BOOL on_wnd_resize(int eventType, const EmscriptenUiEvent* uiEvent, void*);
EM_BOOL on_canvas_wheel(int eventType, const EmscriptenWheelEvent* wheelEvent, void*);
EM_BOOL on_canvas_mousedown(int eventType, const EmscriptenMouseEvent* mouseEvent, void*);
EM_BOOL on_canvas_mouseup(int eventType, const EmscriptenMouseEvent* mouseEvent, void*);
EM_BOOL on_canvas_mousemove(int eventType, const EmscriptenMouseEvent* mouseEvent, void*);

auto canvas_name = "canvas";

void simulator::run()
{
    universe::instance().init();
    universe::instance().random(1000);

    gles_render::instance().init();
    update_view();

    emscripten_set_resize_callback(nullptr, nullptr, false, on_wnd_resize);
    emscripten_set_wheel_callback(canvas_name, nullptr, false, on_canvas_wheel);
    emscripten_set_mousedown_callback(canvas_name, nullptr, false, on_canvas_mousedown);
    emscripten_set_mouseup_callback(canvas_name, nullptr, false, on_canvas_mouseup);
    emscripten_set_mousemove_callback(canvas_name, nullptr, false, on_canvas_mousemove);

    emscripten_set_main_loop(update_frame, 0, true);
}

void update_frame()
{
    gles_render::instance().update_data(universe::instance().generation());
    gles_render::instance().draw();
}

void update_view()
{
    int width, height;
    emscripten_get_canvas_element_size(canvas_name, &width, &height);
    gles_render::instance().update_view(width, height);
}

EM_BOOL on_canvas_wheel(int eventType, const EmscriptenWheelEvent* wheelEvent, void*)
{
    cout << "mouse wheel: " << wheelEvent->deltaX << '\t' << wheelEvent->deltaY << '\t' << wheelEvent->deltaZ << '\t' << endl;

    gles_render::instance().zoom(wheelEvent->deltaY > 0);

    return false;
}

EM_BOOL on_wnd_resize(int eventType, const EmscriptenUiEvent* uiEvent, void*)
{
    update_view();
    return false;
}

static bool mouse_down = false;

EM_BOOL on_canvas_mousedown(int eventType, const EmscriptenMouseEvent* mouseEvent, void*)
{
    cout << "mouse down: " << mouseEvent->targetX << '\t' << mouseEvent->targetY << endl;
    mouse_down = true;

    return false;
}

EM_BOOL on_canvas_mouseup(int eventType, const EmscriptenMouseEvent* mouseEvent, void*)
{
    cout << "mouse up: " << mouseEvent->targetX << '\t' << mouseEvent->targetY << endl;
    mouse_down = false;

    return false;
}

EM_BOOL on_canvas_mousemove(int eventType, const EmscriptenMouseEvent* mouseEvent, void*)
{
    if (mouse_down) {
        auto x = mouseEvent->movementX;
        auto y = mouseEvent->movementY;
        cout << "mouse move: " << x << '\t' << y << endl;
        gles_render::instance().move(x, -y);
    }

    return false;
}
