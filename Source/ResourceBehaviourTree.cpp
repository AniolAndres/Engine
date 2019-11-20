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


