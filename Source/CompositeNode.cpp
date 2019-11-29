#include "CompositeNode.h"

#include "ResourceBehaviourTree.h"

#include "HashString.h"

CompositeNode::CompositeNode()
{
}

CompositeNode::CompositeNode(HashString name, NodeType type) : BehaviourNode(name, NodeType::Composite)
{
}

CompositeNode::~CompositeNode()
{
}

void CompositeNode::TickNode()
{
}

void CompositeNode::CleanNode()
{
	for (auto child : nodeChildren)
	{
		child->CleanNode();
	}

	nodeChildren.clear();
}
