#include "BehaviourTransition.h"

#include "BehaviourNode.h"

BehaviourTransition::BehaviourTransition()
{
}

BehaviourTransition::BehaviourTransition(BehaviourNode * origin, BehaviourNode * destiny)
{
	originNode = origin;
	originName = origin->name;
	destinyNode = destiny;
	destinyName = destiny->name;
}

BehaviourTransition::~BehaviourTransition()
{
}
