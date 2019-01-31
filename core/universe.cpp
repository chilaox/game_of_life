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

const std::vector<int>& universe::generation()
{
    msqure.generation();
    msqure.get_live_cell(mliveidx);

    return mliveidx;
}