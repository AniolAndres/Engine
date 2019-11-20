#include "NodeComposite.h"



NodeComposite::NodeComposite()
{
}


NodeComposite::~NodeComposite()
{
}

NodeStatus NodeComposite::TickNode()
{
	return NodeStatus::Failure;
}

void NodeComposite::CreateNode()
{
}

void NodeComposite::DestroyNode()
{
}
