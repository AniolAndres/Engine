#ifndef _BEHAVIOURNODE_H_
#define _BEHAVIOURNODE_H_

#include "HashString.h"

enum class NodeType;

class BehaviourNode
{
public:
	BehaviourNode();
	BehaviourNode(NodeType type);
	virtual ~BehaviourNode();

public:
	virtual void TickNode() {}

public:
	HashString name = HashString("default Node name");

protected:
	NodeType type;
};

#endif //