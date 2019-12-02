#include "CompositeNode.h"

#include "ResourceBehaviourTree.h"

#include "HashString.h"

#include <vector>
#include <algorithm>

CompositeNode::CompositeNode()
{
}

CompositeNode::CompositeNode(HashString name, NodeType type) : BehaviourNode(name, NodeType::Composite)
{
	//We'll throw Sequence by default, adjustable from the panel
	cType = CompositeType::Sequence;
}

CompositeNode::~CompositeNode()
{
}

TickStatus CompositeNode::TickNode()
{
	TickStatus status;
	switch (cType)
	{
	case CompositeType::Selector:

		for (auto child : nodeChildren)
		{
			if (child->TickNode() == TickStatus::SUCCESS)
			{
				status = TickStatus::RUNNING;
				break;
			}
		}

		break;
	case CompositeType::Sequence:

		for (auto child : nodeChildren)
		{
			if (child->TickNode() == TickStatus::FAILURE)
			{
				status = TickStatus::RUNNING;
				break;
			}
		}

		break;
	}

	return status;
}

void CompositeNode::CleanNode()
{
	for (auto child : nodeChildren)
	{
		child->CleanNode();
	}

	nodeChildren.clear();
}

void CompositeNode::OrderChildren()
{
	std::sort(nodeChildren.begin(), nodeChildren.end(), ComparePointers());

	for (auto child : nodeChildren)
	{
		child->OrderChildren();
	}
}
