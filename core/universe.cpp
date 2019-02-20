#include "universe.h"
#include <algorithm>
#include <iostream>

using namespace std;

void universe::init()
{
    msqure.loadrule({ 3 }, { 2, 3 });
}

void universe::random(int side_size)
{
    msqure.random();
}

const std::vector<int>& universe::generation(int num)
{
    for (int i = 0; i < num; i++) {
        msqure.generation();
    }

    msqure.get_live_cell(mlivecoord);

    const static int offest = 32 * 125 * 125 + 28 * 125 + 6;

    mliveidx.clear();
    for (auto& coord : mlivecoord) {
        mliveidx.push_back(coord.first - coord.second * 1004 + offest);
    }
    return mliveidx;
}