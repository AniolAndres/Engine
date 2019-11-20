#ifndef __NODELEAF_H__
#define __NODELEAF_H__

#include "Node.h"
class NodeLeaf :
	public Node
{
public:
	NodeLeaf();
	~NodeLeaf();

	NodeStatus TickNode() override;
	void CreateNode() override;
	void DestroyNode() override;
};

#endif