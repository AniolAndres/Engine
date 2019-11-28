#ifndef _BEHAVIOURTRANSITION_H_
#define _BEHAVIOURTRANSITION_H_

#include "HashString.h"

class BehaviourTransition
{
public:
	BehaviourTransition();
	BehaviourTransition(HashString origin, HashString destiny);
	~BehaviourTransition();

	HashString originName;
	HashString destinyName;
};

#endif 