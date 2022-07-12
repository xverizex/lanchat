#include "Idle.h"

void Idle::idle()
{
	for (int i = 0; i < idle_func.size(); i++) {
		idle_func[i](params[i]);
	}
}

void Idle::add_idle(void (*a) (void*), void*b)
{
	idle_func.push_back(a);
	params.push_back(b);
}