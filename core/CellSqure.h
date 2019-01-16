#include <iostream>
#include <vector>

class CellSqure;

class Cell
{
	public:
	Cell(int x, int y);
	friend std::ostream& operator<<(std::ostream&, const Cell&);
	//~Cell(){std::cout<<"cell dead"<<std::endl;};

	static const char solid[];
	static const char hollow[];
	
	private:
	Cell& operator=(bool alive);

	int mx;
	int my;
	bool malive;
	std::vector<Cell*> neighbors;

	friend CellSqure;
};

class CellSqure
{
	public:
	static CellSqure MakeRandomInstance(int side_size);

	friend std::ostream& operator<<(std::ostream&, const CellSqure&);

	void generation();

	private:
	CellSqure(int side_size);

	private:
	std::vector<std::vector<Cell>> mcells;
	int mside_size;
};