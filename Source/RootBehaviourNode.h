#ifndef _ROOTBEHAVIOURNODE_H_
#define _ROOTBEHAVIOURNODE_H_

#include "BehaviourNode.h"

#include <vector>

enum class NodeType;

class RootBehaviourNode : public BehaviourNode
{
public:
	RootBehaviourNode();
	~RootBehaviourNode();

	TickStatus TickNode() override;
	void CleanNode() override;
	void OrderChildren() override;

	std::vector<BehaviourNode*> rootChildren;
};

#endif //