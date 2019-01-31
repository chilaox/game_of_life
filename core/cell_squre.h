#include <map>
#include <vector>

class cell_squre {
public:
    static const int node_size = 4;
    static const int chunk_size = 16;
    using node_state = unsigned short;
    using chunk_state = unsigned short;
    static const chunk_state chunk_state_max = 0xFFFF;
    using rule_set = std::vector<int>;

private:
    struct cell_chunk {
        cell_chunk(int x, int y);
        int mx = 0;
        int my = 0;
        int midx = 0;
        chunk_state mfront_cells = 0;
        chunk_state mback_cells = 0;

        static const int nbr_num = 8;
        static const int wnidx = 0;
        static const int nidx = 1;
        static const int enidx = 2;
        static const int widx = 3;
        static const int eidx = 4;
        static const int wsidx = 5;
        static const int sidx = 6;
        static const int esidx = 7;
        cell_chunk* mnbrs[nbr_num];
    };

public:
    cell_squre();
    void loadrule(const rule_set& birth, const rule_set& survival);
    void random();
    void generation();
    void get_live_cell(std::vector<int>&) const;

private:
    cell_chunk* mroot = nullptr;
    std::map<chunk_state, node_state> mrule_lookup;
    using chunk_map = std::map<int, std::unique_ptr<cell_chunk>>;
    using cmitor = chunk_map::iterator;
    using const_cmitor = chunk_map::const_iterator;
    chunk_map mchunkmap;

    template <typename FUNC>
    void for_each_chunk(FUNC f);

    template <typename FUNC>
    void for_each_chunk(FUNC f) const;
};
