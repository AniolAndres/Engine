#include "BehaviourNode.h"

#include "HashString.h"

#include <string>

BehaviourNode::BehaviourNode()
{
}

BehaviourNode::BehaviourNode(HashString name, NodeType type) : name(name) , type(type)
{
}

BehaviourNode::~BehaviourNode()
{
}
