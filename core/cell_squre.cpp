#include "cell_squre.h"
#include <algorithm>
#include <iostream>
using namespace std;

cell_squre::cell_chunk::cell_chunk(int x, int y)
    : mx(x)
    , my(y)
{
    memset(mnbrs, 0, sizeof(mnbrs));
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

        for (int i = 0; i < nbr_num; i++) {
            auto& nc = nbr_coord[i];
            auto nbr = get_chunck(x + nc.first, y + nc.second);
            if (nbr != nullptr) {
                chunk->mnbrs[i] = nbr;
                nbr->mnbrs[nbr_pair[i]] = chunk;
            }
        }

        mactivelst.push_back(chunk);
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
    static const chunk_state nbrmask[cell_squre::nbr_num] = {
        0x1000,
        0x1111,
        0x0001,
        0xF000,
        0x000F,
        0x8000,
        0x8888,
        0x0008,
    };

    for (auto itor = mactivelst.begin(); itor != mactivelst.end(); itor++) {
        auto ck = *itor;
        auto cstate = ck->mfront_cells;

        //topleft node
        chunk_state tpstate = (cstate & 0x0777) << 0x5;
        //topright node
        chunk_state trstate = (cstate & 0x0EEE) << 0x3;
        //bottomleft node
        chunk_state blstate = (cstate & 0x7770) >> 0x3;
        //bottomright node
        chunk_state brstate = (cstate & 0xEEE0) >> 0x5;

        auto wsnbr = ck->mnbrs[0];
        auto wnbr = ck->mnbrs[1];
        auto wnnbr = ck->mnbrs[2];
        auto snbr = ck->mnbrs[3];
        auto nnbr = ck->mnbrs[4];
        auto esnbr = ck->mnbrs[5];
        auto enbr = ck->mnbrs[6];
        auto ennbr = ck->mnbrs[7];

        if (wsnbr) {
            blstate |= ((wsnbr->mfront_cells & 0x0008) << 0x9);
        }

        if (wnbr) {
            tpstate |= ((wnbr->mfront_cells & 0x0888) << 0x1);
            blstate |= ((wnbr->mfront_cells & 0x8880) >> 0x7);
        }

        if (wnnbr) {
            tpstate |= (wnnbr->mfront_cells >> 0xF);
        }

        if (snbr) {
            blstate |= ((snbr->mfront_cells & 0x0007) << 0xD);
            brstate |= ((snbr->mfront_cells & 0x000E) << 0xB);
        }

        if (nnbr) {
            tpstate |= ((nnbr->mfront_cells & 0x7000) >> 0xB);
            trstate |= ((nnbr->mfront_cells & 0xE000) >> 0xD);
        }

        if (esnbr) {
            brstate |= (esnbr->mfront_cells << 0xF);
        }

        if (enbr) {
            trstate |= ((enbr->mfront_cells & 0x0111) << 0x7);
            brstate |= ((enbr->mfront_cells & 0x1110) >> 0x1);
        }

        if (ennbr) {
            trstate |= ((ennbr->mfront_cells & 0x1000) >> 0x9);
        }

        tpstate = mrule_lookup[tpstate] >> 0x5;
        trstate = mrule_lookup[trstate] >> 0x3;
        blstate = mrule_lookup[blstate] << 0x3;
        brstate = mrule_lookup[brstate] << 0x5;

        auto newstate = tpstate | trstate | blstate | brstate;
        auto diffstate = newstate ^ cstate;
        if (diffstate == 0) {
            ck->mstable = true;
            itor = mactivelst.erase(itor);
        } else if (diffstate & 0xF99F) {
            for (int i = 0; i < nbr_num; i++) {
                auto np = ck->mnbrs[i];
                if (np && np->mstable && (diffstate & nbrmask[i])) {
                    np->mstable = false;
                    mactivelst.push_front(np);
                }
            }
        }

        ck->mback_cells = newstate;
    }

    for_each_chunk([](cmitor it) {
        swap(it->second->mfront_cells, it->second->mback_cells);
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