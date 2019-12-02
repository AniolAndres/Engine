#include "RootBehaviourNode.h"

#include "ResourceBehaviourTree.h"

#include <algorithm>

RootBehaviourNode::RootBehaviourNode()
{
	isRoot = true;
	type = NodeType::Root;
	name = HashString("Root");
}


RootBehaviourNode::~RootBehaviourNode()
{
}

TickStatus RootBehaviourNode::TickNode()
{
	for (auto child : rootChildren)
	{
		if(child->TickNode() == TickStatus::FAILURE)
		{
			return TickStatus::FAILURE;
		}
	}

	return TickStatus::RUNNING;
}

void RootBehaviourNode::CleanNode()
{
	for (auto child : rootChildren)
	{
		child->CleanNode();
	}

	rootChildren.clear();
}

void RootBehaviourNode::OrderChildren()
{
	std::sort(rootChildren.begin(), rootChildren.end(), ComparePointers());

	for (unsigned i = 0u; i < rootChildren.size(); ++i)
	{
		LOG("Priority is %i, node Name %s", rootChildren[i]->priority, rootChildren[i]->name.C_str());
		rootChildren[i]->OrderChildren();
	}
}
