#include <list>
#include <map>
#include <vector>

using cell_coord = std::pair<int, int>;

class cell_squre {
public:
    static const int node_size = 4;
    static const int chunk_size = 16;
    using node_state = unsigned short;
    using chunk_state = unsigned short;
    static const chunk_state chunk_state_max = 0xFFFF;
    using rule_set = std::vector<int>;

private:
    static const int nbr_num = 8;
    static constexpr cell_coord nbr_coord[] = {
        { -1, -1 }, { -1, 0 }, { -1, 1 }, //ws,w,wn
        { 0, -1 }, { 0, 1 }, //s,n
        { 1, -1 }, { 1, 0 }, { 1, 1 } //es,e,en
    };
    static constexpr int nbr_pair[] = { 7, 6, 5, 4, 3, 2, 1, 0 };

    struct cell_chunk {
        cell_chunk(int x, int y);
        int mx = 0;
        int my = 0;
        chunk_state mfront_cells = 0;
        chunk_state mback_cells = 0;
        cell_coord mcell_coords[chunk_size];
        cell_chunk* mnbrs[nbr_num];
        bool mstable = false;
    };

public:
    cell_squre();
    void loadrule(const rule_set& birth, const rule_set& survival);
    void random();
    void generation();
    void get_live_cell(std::vector<cell_coord>&) const;

private:
    using chunk_map = std::map<cell_coord, std::unique_ptr<cell_chunk>>;
    using cmitor = chunk_map::iterator;
    using const_cmitor = chunk_map::const_iterator;
    cell_chunk* mroot = nullptr;
    node_state mrule_lookup[chunk_state_max + 1];
    chunk_map mchunkmap;

    std::list<cell_chunk*> mactivelst;

    cell_chunk* create_chunck(int x, int y);
    cell_chunk* get_chunck(int x, int y);
    cell_chunk* get_chunck(const cell_coord&);

    template <typename FUNC>
    void for_each_chunk(FUNC f);

    template <typename FUNC>
    void for_each_chunk(FUNC f) const;
};
