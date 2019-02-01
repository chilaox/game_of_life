#include "../utility/singleton.h"
#include "cell_squre.h"

class universe : public singleton<universe> {
public:
    void init();

    void random(int);

    const std::vector<int>& generation();

private:
    std::vector<int> mliveidx;
    std::vector<cell_coord> mlivecoord;
    cell_squre msqure;
};
