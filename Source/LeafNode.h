#ifndef _LEAFNODE_H_
#define _LEAFNODE_H_

#include "BehaviourNode.h"

#include "HashString.h"

class Script;

class LeafNode : public BehaviourNode
{
public:
	LeafNode();
	LeafNode(HashString name, NodeType type);
	~LeafNode();

	TickStatus TickNode() override;

	//SCript whatever;
	Script* task = nullptr;
};

#endif	