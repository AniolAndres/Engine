#ifndef __NODECOMPOSITE_H_
#define __NODECOMPOSITE_H_

#include "Node.h"
#include <vector>

class NodeComposite :
	public Node
{
public:
	NodeComposite();
	~NodeComposite();

	NodeStatus TickNode() override;
	void CreateNode() override;
	void DestroyNode() override;

	std::vector<Node*> compositeChildren;
};

#endif 