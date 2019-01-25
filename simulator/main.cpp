#include "simulator.h"
#include <chrono>
#include <iostream>
#include <emscripten/bind.h>

using namespace std;

int main(int argc, const char** argv)
{
    srand(chrono::system_clock::now().time_since_epoch().count());

    simulator::instance().run();
}

// void start_simulator()
// {
//     srand(chrono::system_clock::now().time_since_epoch().count());

//     simulator::instance().run();
// }

// EMSCRIPTEN_BINDINGS(module)
// {
//     emscripten::function("start_simulator", &start_simulator);
// }