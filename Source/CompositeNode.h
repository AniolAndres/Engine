#ifndef _COMPOSITENODE_H_
#define _COMPOSITENODE_H_

#include "BehaviourNode.h"

#include "HashString.h"

class CompositeNode : public BehaviourNode
{
public:
	CompositeNode();
	CompositeNode(HashString name, NodeType type);
	~CompositeNode();

	void TickNode() override;
};

#endif //