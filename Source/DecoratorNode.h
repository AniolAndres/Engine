#ifndef _DECORATORNODE_H_
#define _DECORATORNODE_H_

#include "BehaviourNode.h"

class DecoratorNode :
	public BehaviourNode
{
public:
	DecoratorNode();
	DecoratorNode(NodeType type);
	~DecoratorNode();
	
	void TickNode() override;
};

#endif