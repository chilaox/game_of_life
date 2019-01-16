#include <iostream>
#include <chrono>
#include <thread>
#include <stdlib.h>

#include "CellSqure.h"

using namespace std;

int main(int argc, const char** argv)
{
	srand(chrono::system_clock::now().time_since_epoch().count());

	int sidesize = 10;
	int msec = 200;
	
	if (argc > 1) {
		sidesize = atoi(argv[1]);
		if (argc > 2) {
			msec = atoi(argv[2]);
		}
	}
	
	auto squre = CellSqure::MakeRandomInstance(sidesize);

	while(true){
		cout<<squre;
		this_thread::sleep_for(chrono::milliseconds(msec));
		auto x = system("clear");
		squre.generation();
	}
}