#ifndef _BEHAVIOURTRANSITION_H_
#define _BEHAVIOURTRANSITION_H_

#include "HashString.h"

class BehaviourNode;

class BehaviourTransition
{
public:
	BehaviourTransition();
	BehaviourTransition(BehaviourNode* origin, BehaviourNode* destiny);
	~BehaviourTransition();

private:
	BehaviourNode* originNode = nullptr;
	BehaviourNode* destinyNode = nullptr;

	HashString originName = HashString("defaultOrigin");
	HashString destinyName = HashString("defaultDestiny");
};

#endif 