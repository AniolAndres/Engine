#ifndef __ROOTNODE_H_
#define __ROOTNODE_H_

#include "Node.h"

#include <vector>

class NodeRoot : public Node
{
public:
	NodeRoot();
	~NodeRoot();

	NodeStatus TickNode() override;
	void CreateNode() override;
	void DestroyNode() override;

	std::vector<Node*> rootChildren;
};

#endif