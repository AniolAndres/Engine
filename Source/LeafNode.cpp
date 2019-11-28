#include "LeafNode.h"

#include "ResourceBehaviourTree.h"

#include "HashString.h"

LeafNode::LeafNode()
{
}

LeafNode::LeafNode(HashString name, NodeType type) : BehaviourNode(name ,NodeType::Leaf)
{

}

LeafNode::~LeafNode()
{
}

void LeafNode::TickNode()
{
}
