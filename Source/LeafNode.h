#ifndef _LEAFNODE_H_
#define _LEAFNODE_H_

#include "BehaviourNode.h"

#include "HashString.h"

class LeafNode : public BehaviourNode
{
public:
	LeafNode();
	LeafNode(HashString name, NodeType type);
	~LeafNode();

	void TickNode() override;
};

#endif	