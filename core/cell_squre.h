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

        cell_chunk* enbr = nullptr;
        cell_chunk* snbr = nullptr;
        cell_chunk* wnbr = nullptr;
        cell_chunk* nnbr = nullptr;
        cell_chunk* esnbr = nullptr;
        cell_chunk* wsnbr = nullptr;
        cell_chunk* wnnbr = nullptr;
        cell_chunk* ennbr = nullptr;
    };

public:
    cell_squre();
    void loadrule(const rule_set& birth, const rule_set& survival);
    void random();
    void generation();
    void get_live_cell(std::vector<int>&) const;

private:
    cell_chunk* mroot = nullptr;
    node_state mrule_lookup[chunk_state_max + 1];
    using chunk_map = std::map<int, std::unique_ptr<cell_chunk>>;
    using cmitor = chunk_map::iterator;
    using const_cmitor = chunk_map::const_iterator;
    chunk_map mchunkmap;

    template <typename FUNC>
    void for_each_chunk(FUNC f);

    template <typename FUNC>
    void for_each_chunk(FUNC f) const;
};
