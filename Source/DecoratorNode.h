#ifndef _DECORATORNODE_H_
#define _DECORATORNODE_H_

#include "BehaviourNode.h"

#include "HashString.h"

class DecoratorNode :
	public BehaviourNode
{
public:
	DecoratorNode();
	DecoratorNode(HashString name, NodeType type);
	~DecoratorNode();
	
	TickStatus TickNode() override;
	void CleanNode() override;

	BehaviourNode* child = nullptr;
};

#endif