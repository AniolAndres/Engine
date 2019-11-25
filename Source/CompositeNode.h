#ifndef _COMPOSITENODE_H_
#define _COMPOSITENODE_H_

#include "BehaviourNode.h"

class CompositeNode : public BehaviourNode
{
public:
	CompositeNode();
	CompositeNode(NodeType type);
	~CompositeNode();

	void TickNode() override;
};

#endif //