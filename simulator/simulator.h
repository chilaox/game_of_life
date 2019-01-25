#include "../utility/singleton.h"

class simulator : public singleton<simulator>
{
	public:
	void run();
};