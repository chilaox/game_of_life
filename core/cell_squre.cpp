#include "cell_squre.h"
#include <map>

using namespace std;

void cell_squre::set_cell_state(const cell& cell, bool alive)
{
    mcells[cell.mx][cell.my] = alive;
}

const Cells& cell_squre::cells()
{
    return mcells;
}

int cell_squre::get_side_size() const
{
    return mside_size;
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
                cell.mneighbors.push_back(&mcells[(cell.mx + side_size + dir[0]) % side_size][(cell.my + side_size + dir[1]) % side_size]);
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
    if (malive != alive) {
        malive = alive;
        auto num = alive ? 1 : -1;

        for (auto c : mneighbors) {
            c->mlivenbr += num;
        }
    }

    return *this;
}

std::ostream& operator<<(std::ostream& out, const cell& cell)
{
    out << (cell.malive ? cell::solid : cell::hollow);
    return out;
}

int cell::get_livenbr() const
{
    return mlivenbr;
}

bool cell::isalive() const
{
    return malive;
}
