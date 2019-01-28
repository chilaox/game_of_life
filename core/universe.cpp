#include "universe.h"
#include <algorithm>
#include <iostream>

using namespace std;

void universe::init()
{
    mpfront_squre = msqures;
    mpback_squre = msqures + 1;
}

void universe::random(int side_size)
{
    *mpfront_squre = cell_squre(side_size);
    *mpback_squre = cell_squre(side_size);

    for (auto& row : mpfront_squre->cells()) {
        for (auto& c : row) {
            mpfront_squre->set_cell_state(c, rand() % 2 == 0);
        }
    }
}

const std::vector<int>& universe::generation()
{
    static const auto borns = { 3 };
    static const auto stays = { 2, 3 };

    int idx = 0;
    mliveidx.clear();

    for (auto& row : mpfront_squre->cells()) {
        for (auto& c : row) {
            auto livenbr = c.get_livenbr();
            auto wl = c.isalive();
            if (find(borns.begin(), borns.end(), livenbr) != borns.end()) {
                wl = true;
            } else if (find(stays.begin(), stays.end(), livenbr) == stays.end()) {
                wl = false;
            }

            mpback_squre->set_cell_state(c, wl);

            if (wl) {
                mliveidx.push_back(idx);
            }

            idx++;
        }
    }

    swap(mpback_squre, mpfront_squre);

    return mliveidx;
}