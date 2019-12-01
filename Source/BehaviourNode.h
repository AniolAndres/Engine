#ifndef _BEHAVIOURNODE_H_
#define _BEHAVIOURNODE_H_

#include "HashString.h"

enum class NodeType;
enum class CompositeType;
enum class TickStatus;

class BehaviourNode
{
public:
	BehaviourNode();
	BehaviourNode(HashString name, NodeType type);
	virtual ~BehaviourNode();

public:
	virtual TickStatus TickNode();
	virtual void CleanNode() {}
	virtual void OrderChildren() {}

	bool operator < (const BehaviourNode* node) const
	{
		return (this->priority < node->priority);
	}

public:
	HashString name;
	NodeType type;
	CompositeType cType;
	bool isRoot = false;
	int priority = 0;
};

#endif //