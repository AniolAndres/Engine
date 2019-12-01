#include "DecoratorNode.h"

#include "ResourceBehaviourTree.h"

DecoratorNode::DecoratorNode()
{
}

DecoratorNode::DecoratorNode(HashString name, NodeType type) : BehaviourNode(name, NodeType::Decorator)
{

}

DecoratorNode::~DecoratorNode()
{
}

TickStatus DecoratorNode::TickNode()
{
	//This will be an inverter for now

	if (child->TickNode() == TickStatus::FAILURE)
	{
		return TickStatus::SUCCESS;
	}
	else if (child->TickNode() == TickStatus::SUCCESS)
	{
		return TickStatus::FAILURE;
	}
}

void DecoratorNode::CleanNode()
{
	child = nullptr;
}
