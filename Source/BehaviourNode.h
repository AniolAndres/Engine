#ifndef _BEHAVIOURNODE_H_
#define _BEHAVIOURNODE_H_

#include "HashString.h"

enum class NodeType;
enum class CompositeType;
enum class TickStatus;

class BehaviourNode
{
protected:
	struct ComparePointers
	{
		bool operator()(const BehaviourNode* l, const BehaviourNode* r) 
		{
			return *l < *r;
		}
	};

	bool operator < (const BehaviourNode &node) const
	{
		return (this->priority < node.priority);
	}

public:
	BehaviourNode();
	BehaviourNode(HashString name, NodeType type);
	virtual ~BehaviourNode();

public:
	virtual TickStatus TickNode();
	virtual void CleanNode() {}
	virtual void OrderChildren() {}



public:
	HashString name;
	NodeType type;
	CompositeType cType;
	bool isRoot = false;
	int priority = 0;
};

#endif //