#include "RootBehaviourNode.h"

#include "ResourceBehaviourTree.h"

RootBehaviourNode::RootBehaviourNode()
{
	isRoot = true;
	type = NodeType::Root;
	name = HashString("Root");
}


RootBehaviourNode::~RootBehaviourNode()
{
}

void RootBehaviourNode::TickNode()
{
}

void RootBehaviourNode::CleanNode()
{
	for (auto child : rootChildren)
	{
		child->CleanNode();
	}

	rootChildren.clear();
}
