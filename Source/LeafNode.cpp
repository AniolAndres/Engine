#include "LeafNode.h"

#include "ResourceBehaviourTree.h"

LeafNode::LeafNode()
{
}

LeafNode::LeafNode(NodeType type) : BehaviourNode(NodeType::Leaf)
{

}

LeafNode::~LeafNode()
{
}

void LeafNode::TickNode()
{
}
