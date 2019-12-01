#ifndef _RESOURCEBEHAVIOURTREE_H_
#define _RESOURCEBEHAVIOURTREE_H_

#include "Resource.h"

#include "HashString.h"
#include <vector>

class BehaviourNode;
class CompositeNode;
class RootBehaviourNode;
class LeafNode;
class DecoratorNode;
class BehaviourTransition;

enum class NodeType
{
	Composite = 0,
	Decorator,
	Leaf,
	Root
};

enum class TickStatus
{
	RUNNING,
	FAILURE,
	SUCCESS
};

enum class CompositeType
{
	//Sequence requires all children to succeed, Selector only requires one
	Sequence,
	Selector
};

enum class DecoratorType
{
	Inverter,
	Unused //I'll keep this here if anytime soon I want to add more types
};

#define MAX_BT_NAME 64

class ResourceBehaviourTree : public Resource
{
public:
	ResourceBehaviourTree(unsigned uid);
	ResourceBehaviourTree(const ResourceBehaviourTree& resource);
	virtual ~ResourceBehaviourTree();

	bool LoadInMemory() override;		// Will load in memory all ResourceBT stored
	void DeleteFromMemory() override;	// Will delete all ResourceBT stored in memory
	void Delete() override;
	void SaveMetafile(const char* file) const override;
	void LoadConfigFromMeta() override;
	void LoadConfigFromLibraryMeta() override;
	void SetBehaviourTree(const char* data);
	void Save();
	void SaveBehaviourTreeData(char* data);
	unsigned GetBTSize();
public:

	void Tick();

	BehaviourNode* CreateNode(HashString name, NodeType type, int priority);
	void CreateTransition(HashString origin, HashString destiny);

	void RemoveNodeTransitions(HashString nodeName);
	void RemoveTransition(unsigned UID);
	void RemoveNode(unsigned UID);

	unsigned FindNode(const HashString name);
	unsigned FindTransition(const HashString origin, const HashString destiny);
	HashString GetNodeName(unsigned index);
	unsigned GetNodeType(unsigned index);

	void SetNodeName(unsigned index, HashString name);
	void SetNodePriority(unsigned index, int priority);
	int GetNodePriority(unsigned index);
	CompositeType GetCompositeType(unsigned index);
	void SetCompositeType(unsigned index, CompositeType cType);


	HashString GetTransitionOrigin(unsigned index);
	HashString GetTransitionDestiny(unsigned index);
	void SetTransitionOrigin(HashString name, unsigned index);
	void SetTransitionDestiny(HashString name, unsigned index);

	unsigned GetNodesSize();
	unsigned GetTransitionsSize();

	void BuildTree();
	void CleanTree();
	void PriorizeNodes();

private:

	std::vector<BehaviourNode*> nodes;
	std::vector<BehaviourTransition> transitions;
	RootBehaviourNode* rootNode = nullptr;	
};

#endif