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
	CONTINUE
};

#define MAX_BT_NAME 64

class ResourceBehaviourTree : public Resource
{
public:
	ResourceBehaviourTree(unsigned uid);
	ResourceBehaviourTree(const ResourceBehaviourTree& resource);
	virtual ~ResourceBehaviourTree();

	bool LoadInMemory() override;		// Will load in memory all Resourceanims stored
	void DeleteFromMemory() override;	// Will delete all ResourceAnims stored in memory
	void Delete() override;
	void SaveMetafile(const char* file) const override;
	void LoadConfigFromMeta() override;
	void LoadConfigFromLibraryMeta() override;
	void SetBehaviourTree(const char* data);
	void Save();
	void SaveBehaviourTreeData(char* data);
	unsigned GetBTSize();
public:

	void CreateNode(HashString name, NodeType type);
	void CreateTransition(HashString origin, HashString destiny);

	void RemoveNodeTransitions(HashString nodeName);
	void RemoveTransition(unsigned UID);
	void RemoveNode(unsigned UID);

	unsigned FindNode(const HashString name);
	unsigned FindTransition(const HashString origin, const HashString destiny);
	HashString GetNodeName(unsigned index);
	unsigned GetNodeType(unsigned index);

	HashString GetTransitionOrigin(unsigned index);
	HashString GetTransitionDestiny(unsigned index);

	unsigned GetNodesSize();
	unsigned GetTransitionsSize();

	void BuildTree();
	void CleanTree();
private:

	std::vector<BehaviourNode> nodes;
	std::vector<BehaviourTransition> transitions;
	RootBehaviourNode* rootNode = nullptr;	
};

#endif