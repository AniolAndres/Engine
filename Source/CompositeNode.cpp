#include "CompositeNode.h"

#include "ResourceBehaviourTree.h"

CompositeNode::CompositeNode()
{
}

CompositeNode::CompositeNode(NodeType type) : BehaviourNode(NodeType::Composite)
{
}

CompositeNode::~CompositeNode()
{
}

void CompositeNode::TickNode()
{
}
