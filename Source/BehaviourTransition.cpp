#include "BehaviourTransition.h"

#include "BehaviourNode.h"
#include "HashString.h"

BehaviourTransition::BehaviourTransition()
{
}

BehaviourTransition::BehaviourTransition(HashString origin, HashString destiny)
{
	originName = origin;
	destinyName = destiny;
}

BehaviourTransition::~BehaviourTransition()
{
}
