#ifndef _LEAFNODE_H_
#define _LEAFNODE_H_

#include "BehaviourNode.h"

class LeafNode : public BehaviourNode
{
public:
	LeafNode();
	LeafNode(NodeType type);
	~LeafNode();

	void TickNode() override;
};

#endif	