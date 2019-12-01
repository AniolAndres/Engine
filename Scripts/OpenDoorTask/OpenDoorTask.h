#ifndef  __OpenDoorTask_h__
#define  __OpenDoorTask_h__

#include "BaseScript.h"
#include "../Scripts/BehaviourTask/BehaviourTask.h"

#ifdef OpenDoorTask_EXPORTS
#define OpenDoorTask_API __declspec(dllexport)
#else
#define OpenDoorTask_API __declspec(dllimport)
#endif

class OpenDoorTask_API OpenDoorTask : public BehaviourTask
{
public:
	void BehaviourTick() override;
};

#endif __OpenDoorTask_h__
