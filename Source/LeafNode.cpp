#include "LeafNode.h"

#include "ResourceBehaviourTree.h"

#include "HashString.h"
#include "BaseScript.h"
#include "../Scripts/BehaviourTask/BehaviourTask.h"

LeafNode::LeafNode()
{
}

LeafNode::LeafNode(HashString name, NodeType type) : BehaviourNode(name ,NodeType::Leaf)
{

}

LeafNode::~LeafNode()
{
}

TickStatus LeafNode::TickNode()
{
	task->BehaviourTick();
	return TickStatus::RUNNING;
}
