#ifndef _RESOURCEBEHAVIOURTREE_H_
#define _RESOURCEBEHAVIOURTREE_H_

#include "Resource.h"

#include "HashString.h"
#include <vector>

enum class NodeType
{
	Composite,
	Decorator,
	Leaf
};

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

public:

	void AddNode(HashString name);
	void AddTransition(HashString origin, HashString destiny);

	unsigned FindNode(const HashString name);
	unsigned FindTransition(const HashString origin, const HashString destiny);

private:

	struct Node
	{
		HashString nodeName;
		NodeType nodeType;

		Node() { ; }
		Node(HashString n) : nodeName(n) { LOG("Node created"); }
	};

	struct LeafNode : Node
	{
		HashString taskName;

		LeafNode() { ; }
		LeafNode(HashString t) : taskName(t) { LOG("LeafNode created"); }
	};

	struct CompositeNode : Node
	{
		CompositeNode() { LOG("CompositeNode created"); }
	};

	struct DecoratorNode : Node
	{
		DecoratorNode() { LOG("DecoratorNode Created"); }
	};

	struct Transition
	{
		HashString origin;
		HashString destiny;

		Transition() { ; }
		Transition(HashString o, HashString d) : origin(o), destiny(d) {;}
	};

	std::vector<Node> nodes;
	std::vector<Transition> transitions;
};

#endif