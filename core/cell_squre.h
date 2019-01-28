#include <iostream>
#include <vector>

class cell_squre;

class cell {
public:
    cell(int x, int y);
    friend std::ostream& operator<<(std::ostream&, const cell&);

    int get_livenbr() const;
    bool isalive() const;

    static const char solid[];
    static const char hollow[];

private:
    cell& operator=(bool alive);

    int mx;
    int my;
    bool malive = false;
    int mlivenbr = 0;
    std::vector<cell*> mneighbors;

    friend cell_squre;
};

using Cells = std::vector<std::vector<cell>>;
class cell_squre {
public:
    friend std::ostream& operator<<(std::ostream&, const cell_squre&);

    cell_squre(){};
    cell_squre(int side_size);

    int get_side_size() const;
    const Cells& cells();
    void set_cell_state(const cell& cell, bool alive);

private:
    Cells mcells;
    int mside_size;
};