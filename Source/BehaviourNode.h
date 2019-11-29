#ifndef _BEHAVIOURNODE_H_
#define _BEHAVIOURNODE_H_

#include "HashString.h"

enum class NodeType;

class BehaviourNode
{
public:
	BehaviourNode();
	BehaviourNode(HashString name, NodeType type);
	virtual ~BehaviourNode();

public:
	virtual void TickNode() {}
	virtual void CleanNode() {}

public:
	HashString name;
	NodeType type;
	bool isRoot = false;
};

#endif //