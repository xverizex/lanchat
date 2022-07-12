#pragma once

#include <vector>

class Idle
{
public:
	std::vector<void (*)(void* p)> idle_func;
	std::vector<void*> params;
	void idle();
	void add_idle(void (*) (void*), void*);
};

