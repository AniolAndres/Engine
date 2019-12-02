#ifndef _COMPOSITENODE_H_
#define _COMPOSITENODE_H_

#include "BehaviourNode.h"

#include "HashString.h"
#include <vector>


class CompositeNode : public BehaviourNode
{
public:
	CompositeNode();
	CompositeNode(HashString name, NodeType type);
	~CompositeNode();

	TickStatus TickNode() override;
	void CleanNode() override;
	void OrderChildren() override;

	std::vector<BehaviourNode*> nodeChildren;

};

#endif //