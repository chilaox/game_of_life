#include <iostream>
#include <vector>

class cell_squre;

class cell {
public:
    cell(int x, int y);
    friend std::ostream& operator<<(std::ostream&, const cell&);

    static const char solid[];
    static const char hollow[];

private:
    cell& operator=(bool alive);

    int mx;
    int my;
    bool malive;
    bool mwillalive;
    std::vector<cell*> neighbors;

    friend cell_squre;
};

class cell_squre {
public:
    static cell_squre random_instance(int side_size);

    friend std::ostream& operator<<(std::ostream&, const cell_squre&);

    const std::vector<int>& generation();

    cell_squre(){};

private:
    cell_squre(int side_size);

private:
    std::vector<std::vector<cell>> mcells;
    int mside_size;
    std::vector<int> mliveidx;
};