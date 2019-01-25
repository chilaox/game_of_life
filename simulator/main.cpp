#include "simulator.h"
#include <chrono>

using namespace std;

int main(int argc, const char** argv)
{
    srand(chrono::system_clock::now().time_since_epoch().count());

    simulator::instance().run();
}