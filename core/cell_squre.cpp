#include "cell_squre.h"
#include <algorithm>
#include <iostream>
using namespace std;

cell_squre::cell_chunk::cell_chunk(int x, int y)
    : mx(x)
    , my(y)
    , midx(x * chunk_size + y)
{
    memset(mnbrs, 0, sizeof(mnbrs));
}

cell_squre::cell_squre()
{
    auto root_ptr = make_unique<cell_chunk>(0, 0);
    mroot = root_ptr.get();
    mchunkmap[mroot->midx] = move(root_ptr);
}

void cell_squre::loadrule(const rule_set& birth, const rule_set& survival)
{
    static auto count_one = [](chunk_state state) {
        int count = 0;
        while (state != 0) {
            count++;
            state = state & (state - 1);
        }
        return count;
    };

    static auto is_bitn_one = [](chunk_state state, int n) {
        return (state & (1 << n)) >> n;
    };

    static const int nodeidx[] = { 0x5, 0x6, 0x9, 0xA };

    for (int cs = 0; cs <= chunk_state_max; cs++) {
        auto ca = count_one(cs);
        auto c0 = is_bitn_one(cs, 0x0);
        auto c3 = is_bitn_one(cs, 0x3);
        auto cC = is_bitn_one(cs, 0xC);
        auto cF = is_bitn_one(cs, 0xF);
        auto s0123 = count_one(cs & 0x000F);
        auto s048C = count_one(cs & 0x1111);
        auto s37BF = count_one(cs & 0x8888);
        auto sCDEF = count_one(cs & 0xF000);

        int live[node_size] = {
            ca + cF - s37BF - sCDEF - is_bitn_one(cs, nodeidx[0]),
            ca + cC - s048C - sCDEF - is_bitn_one(cs, nodeidx[1]),
            ca + c3 - s0123 - s37BF - is_bitn_one(cs, nodeidx[2]),
            ca + c0 - s0123 - s048C - is_bitn_one(cs, nodeidx[3]),
        };

        node_state ns = cs;
        for (int i = 0; i < node_size; i++) {
            auto num = live[i];
            if (find(birth.begin(), birth.end(), num) != birth.end()) {
                ns |= (1 << nodeidx[i]);
            } else if (find(survival.begin(), survival.end(), num) == survival.end()) {
                ns &= (chunk_state_max - (1 << nodeidx[i]));
            }
        }
        mrule_lookup[cs] = ns;
    }
}

void cell_squre::random()
{
    for_each_chunk([](cmitor it) {
        it->second->mfront_cells = rand();
    });
}

void cell_squre::generation()
{
    for_each_chunk([&](cmitor it) {
        it->second->mback_cells = mrule_lookup[it->second->mfront_cells];
    });

    for_each_chunk([](cmitor it) {
        it->second->mfront_cells = it->second->mback_cells;
    });
}

void cell_squre::get_live_cell(vector<int>& cells) const
{
    cells.clear();
    for_each_chunk([&cells](const_cmitor it) {
        auto idx = it->first;
        auto state = it->second->mfront_cells;
        for (int i = 0; i < chunk_size; i++) {
            if (state & (1 << i)) {
                cells.push_back(idx + i);
            }
        }
    });
}

template <typename FUNC>
void cell_squre::for_each_chunk(FUNC f)
{
    for (auto itor = mchunkmap.begin(); itor != mchunkmap.end(); itor++) {
        f(itor);
    }
}

template <typename FUNC>
void cell_squre::for_each_chunk(FUNC f) const
{
    for (auto itor = mchunkmap.cbegin(); itor != mchunkmap.cend(); itor++) {
        f(itor);
    }
}