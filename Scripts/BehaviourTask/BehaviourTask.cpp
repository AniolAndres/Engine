#include "BehaviourTask.h"

BehaviourTask_API Script* CreateScript()
{
	BehaviourTask* instance = new BehaviourTask;
	return instance;
}

void BehaviourTask::BehaviourTick()
{
	LOG("Tick!");
}
