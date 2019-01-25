#include "cell_squre.h"
#include <map>

using namespace std;

const vector<int>& cell_squre::generation()
{
    static const int min = 2;
    static const int max = 3;
    static const int good = 3;

    for (auto& row : mcells) {
        for (auto& cell : row) {
            int count = 0;
            for (auto& cp : cell.neighbors) {
                count += cp->malive;
            }

            if (count == good) {
                cell.mwillalive = true;
            } else if (count < min or count > max) {
                cell.mwillalive = false;
            }
        }
    }

    mliveidx.clear();
    int idx = 0;
    for (auto& row : mcells) {
        for (auto& cell : row) {
            cell = cell.mwillalive;
            if (cell.mwillalive) {
                mliveidx.push_back(idx);
            }
            idx++;
        }
    }

    return mliveidx;
}

cell_squre cell_squre::random_instance(int side_size)
{
    cell_squre squre(side_size);

    for (auto& row : squre.mcells) {
        for (auto& cell : row) {
            cell = rand() % 2;
        }
    }

    return squre;
}

cell_squre::cell_squre(int side_size)
    : mside_size(side_size)
{
    mcells.assign(side_size, vector<cell>());
    for (int i = 0; i < side_size; ++i) {
        auto& row = mcells[i];
        for (int j = 0; j < side_size; ++j) {
            row.emplace_back(i, j);
        }
    }

    static const int dirs[][2] = { { -1, -1 }, { -1, 0 }, { -1, 1 }, { 0, -1 }, { 0, 1 }, { 1, -1 }, { 1, 0 }, { 1, 1 } };

    for (auto& row : mcells) {
        for (auto& cell : row) {
            for (auto& dir : dirs) {
                cell.neighbors.push_back(&mcells[(cell.mx + side_size + dir[0]) % side_size][(cell.my + side_size + dir[1]) % side_size]);
            }
        }
    }
}

std::ostream& operator<<(std::ostream& out, const cell_squre& squre)
{
    for (auto& row : squre.mcells) {
        for (auto& cell : row) {
            out << cell;
        }
        out << endl;
    }
    return out;
}

const char cell::solid[] = "■";
const char cell::hollow[] = "□";

cell::cell(int x, int y)
    : mx(x)
    , my(y)
{
}

cell& cell::operator=(bool alive)
{
    this->malive = alive;
    this->mwillalive = alive;
    return *this;
}

std::ostream& operator<<(std::ostream& out, const cell& cell)
{
    out << (cell.malive ? cell::solid : cell::hollow);
    return out;
}
