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

private:

	struct Node
	{
		HashString nodeName;
		NodeType nodeType;
	};
	
	struct Transition
	{
		HashString origin;
		HashString destiny;
	};

	std::vector<Node*> nodes;
	std::vector<Transition*> transitions;
};

#endif