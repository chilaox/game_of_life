#include <map>
#include "CellSqure.h"

using namespace std;

void CellSqure::generation()
{
	static const int min = 2;
	static const int max = 3;
	static const int good = 3;

	for(auto& row : mcells){
		for(auto& cell : row){
			int count = 0;
			for(auto& cp : cell.neighbors){
				count += cp->malive;
			}

			if (count == good) {
				cell = true;
			}else if (count < min or count > max) {
				cell = false;
			}
		}
	}
}

CellSqure CellSqure::MakeRandomInstance(int side_size)
{
	CellSqure squre(side_size);

	for(auto& row : squre.mcells){
		for(auto& cell : row){
			cell = rand() % 2;
		}
	}

	return squre;
}

CellSqure::CellSqure(int side_size)
	:mside_size(side_size)
{
	mcells.assign(side_size, vector<Cell>());
	for(int i = 0; i < side_size; ++i){
		auto& row = mcells[i];
		for(int j = 0; j < side_size; ++j){
			row.emplace_back(i, j);
		}
	}

	static const int dirs [][2] = {{-1,-1}, {-1,0}, {-1,1}, {0,-1}, {0,1}, {1,-1}, {1,0}, {1,1}};

	for(auto& row : mcells){
		for(auto& cell : row){
			for(auto& dir : dirs){
				cell.neighbors.push_back(&mcells[(cell.mx+side_size+dir[0])%side_size][(cell.my+side_size+dir[1])%side_size]);
			}
		}
	}
}

std::ostream& operator<<(std::ostream& out, const CellSqure& squre)
{
	for(auto& row : squre.mcells){
		for(auto& cell : row){
			out<<cell;
		}
		out<<endl;
	}
	return out;
}

const char Cell::solid[] = "■";
const char Cell::hollow[] = "□";

Cell::Cell(int x, int y)
	:mx(x), my(y)
{
}


Cell& Cell::operator=(bool alive)
{
	this->malive = alive;
	return *this;
}

std::ostream& operator<<(std::ostream& out, const Cell& cell)
{
	out<<(cell.malive?Cell::solid:Cell::hollow);
	return out;
}
