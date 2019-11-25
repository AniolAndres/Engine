#include "DecoratorNode.h"

#include "ResourceBehaviourTree.h"

DecoratorNode::DecoratorNode()
{
}

DecoratorNode::DecoratorNode(NodeType type) : BehaviourNode(NodeType::Decorator)
{

}

DecoratorNode::~DecoratorNode()
{
}

void DecoratorNode::TickNode()
{
}
