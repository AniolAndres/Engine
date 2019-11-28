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

void DecoratorNode::TickNode()
{
}
