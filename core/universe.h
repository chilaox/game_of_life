#include "../utility/singleton.h"
#include "cell_squre.h"

class universe : public singleton<universe> {
public:
    void init();

    void random(int);

    const std::vector<int>& generation();

private:
    cell_squre msqures[2];
    cell_squre* mpfront_squre;
    cell_squre* mpback_squre;
    std::vector<int> mliveidx;
};
