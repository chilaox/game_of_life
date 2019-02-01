#include "cell_squre.h"
#include <algorithm>
#include <iostream>
using namespace std;

cell_squre::cell_chunk::cell_chunk(int x, int y)
    : mx(x)
    , my(y)
{
    for (auto i = 0; i < chunk_size; i++) {
        mcell_coords[i] = make_pair(4 * x + i % 4 - 2, 4 * y - i / 4 + 1);
    }
}

cell_squre::cell_squre()
{
    mroot = create_chunck(0, 0);
    for (int x = -125; x <= 125; x++) {
        for (int y = -125; y <= 125; y++) {
            create_chunck(x, y);
        }
    }
}

cell_squre::cell_chunk* cell_squre::create_chunck(int x, int y)
{
    auto coord = make_pair(x, y);
    auto chunk = get_chunck(coord);

    if (chunk == nullptr) {
        auto chunck_ptr = make_unique<cell_chunk>(x, y);
        chunk = mchunkmap.emplace(coord, move(chunck_ptr)).first->second.get();

        chunk->nnbr = get_chunck(x, y + 1);
        if (chunk->nnbr != nullptr) {
            chunk->nnbr->snbr = chunk;
        }

        chunk->snbr = get_chunck(x, y - 1);
        if (chunk->snbr != nullptr) {
            chunk->snbr->nnbr = chunk;
        }

        chunk->wnbr = get_chunck(x - 1, y);
        if (chunk->wnbr != nullptr) {
            chunk->wnbr->enbr = chunk;
        }

        chunk->enbr = get_chunck(x + 1, y);
        if (chunk->enbr != nullptr) {
            chunk->enbr->wnbr = chunk;
        }

        chunk->wsnbr = get_chunck(x - 1, y - 1);
        if (chunk->wsnbr != nullptr) {
            chunk->wsnbr->ennbr = chunk;
        }

        chunk->ennbr = get_chunck(x + 1, y + 1);
        if (chunk->ennbr != nullptr) {
            chunk->ennbr->wsnbr = chunk;
        }

        chunk->wnnbr = get_chunck(x - 1, y + 1);
        if (chunk->wnnbr != nullptr) {
            chunk->wnnbr->esnbr = chunk;
        }

        chunk->esnbr = get_chunck(x + 1, y - 1);
        if (chunk->esnbr != nullptr) {
            chunk->esnbr->esnbr = chunk;
        }
    }

    return chunk;
}

cell_squre::cell_chunk* cell_squre::get_chunck(int x, int y)
{
    return get_chunck(make_pair(x, y));
}

cell_squre::cell_chunk* cell_squre::get_chunck(const cell_coord& coord)
{
    auto itor = mchunkmap.find(coord);
    return (itor != mchunkmap.end()) ? itor->second.get() : nullptr;
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
        mrule_lookup[cs] = ns & 0x660;
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
        auto ck = it->second.get();
        auto cstate = ck->mfront_cells;

        //topleft node
        chunk_state tpstate = (cstate & 0x0777) << 0x5;
        //topright node
        chunk_state trstate = (cstate & 0x0EEE) << 0x3;
        //bottomleft node
        chunk_state blstate = (cstate & 0x7770) >> 0x3;
        //bottomright node
        chunk_state brstate = (cstate & 0xEEE0) >> 0x5;

        if (ck->wnbr) {
            tpstate |= ((ck->wnbr->mfront_cells & 0x0888) << 0x1);
            blstate |= ((ck->wnbr->mfront_cells & 0x8880) >> 0x7);
        }
        if (ck->nnbr) {
            tpstate |= ((ck->nnbr->mfront_cells & 0x7000) >> 0xB);
            trstate |= ((ck->nnbr->mfront_cells & 0xE000) >> 0xD);
        }
        if (ck->enbr) {
            trstate |= ((ck->enbr->mfront_cells & 0x0111) << 0x7);
            brstate |= ((ck->enbr->mfront_cells & 0x1110) >> 0x1);
        }

        if (ck->snbr) {
            blstate |= ((ck->snbr->mfront_cells & 0x0007) << 0xD);
            brstate |= ((ck->snbr->mfront_cells & 0x000E) << 0xB);
        }

        if (ck->wnnbr) {
            tpstate |= (ck->wnnbr->mfront_cells >> 0xF);
        }

        if (ck->ennbr) {
            trstate |= ((ck->ennbr->mfront_cells & 0x1000) >> 0x9);
        }
        if (ck->wsnbr) {
            blstate |= ((ck->wsnbr->mfront_cells & 0x0008) << 0x9);
        }
        if (ck->esnbr) {
            brstate |= (ck->esnbr->mfront_cells << 0xF);
        }

        tpstate = mrule_lookup[tpstate] >> 0x5;

        trstate = mrule_lookup[trstate] >> 0x3;

        blstate = mrule_lookup[blstate] << 0x3;

        brstate = mrule_lookup[brstate] << 0x5;

        it->second->mback_cells = (tpstate | trstate | blstate | brstate);
    });

    for_each_chunk([](cmitor it) {
        it->second->mfront_cells = it->second->mback_cells;
    });
}

void cell_squre::get_live_cell(vector<cell_coord>& cells) const
{
    cells.clear();
    for_each_chunk([&cells](const_cmitor it) {
        auto state = it->second->mfront_cells;
        for (int i = 0; i < chunk_size; i++) {
            if (state & (1 << i)) {
                cells.push_back(it->second->mcell_coords[i]);
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