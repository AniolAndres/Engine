#include "ResourceBehaviourTree.h"



ResourceBehaviourTree::ResourceBehaviourTree(unsigned uid) : Resource(uid, TYPE::BEHAVIOURTREE)
{
}

ResourceBehaviourTree::ResourceBehaviourTree(const ResourceBehaviourTree & resource) : Resource(resource)
{
}

ResourceBehaviourTree::~ResourceBehaviourTree()
{
}

bool ResourceBehaviourTree::LoadInMemory()
{
	return false;
}

void ResourceBehaviourTree::DeleteFromMemory()
{
}

void ResourceBehaviourTree::Delete()
{
}

void ResourceBehaviourTree::SaveMetafile(const char * file) const
{
}

void ResourceBehaviourTree::LoadConfigFromMeta()
{
}

void ResourceBehaviourTree::LoadConfigFromLibraryMeta()
{
}

void ResourceBehaviourTree::AddNode(HashString name)
{
	nodes.emplace_back(name);
}

void ResourceBehaviourTree::AddTransition(HashString origin, HashString destiny)
{
	transitions.emplace_back(origin, destiny);
}

unsigned ResourceBehaviourTree::FindNode(const HashString name)
{
	unsigned i;

	for (i = 0u; i < nodes.size(); ++i)
	{
		if (nodes[i].nodeName == name)
		{
			break;
		}
	}
	return i;
}

unsigned ResourceBehaviourTree::FindTransition(const HashString origin, const HashString destiny)
{
	unsigned i;

	for (i = 0u; i < transitions.size(); ++i)
	{
		if (transitions[i].origin == origin && transitions[i].destiny == destiny)
		{
			break;
		}
	}
	return i;
}
