#include "NodeDecorator.h"



NodeDecorator::NodeDecorator()
{
}


NodeDecorator::~NodeDecorator()
{
}

NodeStatus NodeDecorator::TickNode()
{
	return NodeStatus::Failure;
}

void NodeDecorator::CreateNode()
{
}

void NodeDecorator::DestroyNode()
{
}
