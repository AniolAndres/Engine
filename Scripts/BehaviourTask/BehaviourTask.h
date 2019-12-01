#ifndef  __BehaviourTask_h__
#define  __BehaviourTask_h__

#include "BaseScript.h"

#ifdef BehaviourTask_EXPORTS
#define BehaviourTask_API __declspec(dllexport)
#else
#define BehaviourTask_API __declspec(dllimport)
#endif

class BehaviourTask_API BehaviourTask : public Script
{
	void BehaviourTick() override;
};

#endif __BehaviourTask_h__
