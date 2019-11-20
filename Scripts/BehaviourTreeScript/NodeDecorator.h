#ifndef __NODEDECORATOR_H_
#define __NODEDECORATOR_H_

#include "Node.h"

class NodeDecorator :
	public Node
{
public:
	NodeDecorator();
	~NodeDecorator();

	NodeStatus TickNode() override;
	void CreateNode() override;
	void DestroyNode() override;

	Node* decoratorChild = nullptr;
};

#endif