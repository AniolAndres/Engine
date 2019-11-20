#ifndef __NODE_H__
#define __NODE_H__

enum class NodeType
{
	Composite,
	Decorator,
	Leaf,
	Root
};

enum class NodeStatus
{
	Success,
	Failure,
	Running
};

class Node
{
public:
	Node();
	virtual ~Node();

	NodeType nodeType;

	virtual NodeStatus TickNode() { return NodeStatus::Failure; }
	virtual void CreateNode() {}
	virtual void DestroyNode() {}
};

#endif