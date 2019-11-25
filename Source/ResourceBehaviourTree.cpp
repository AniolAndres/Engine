#include "ResourceBehaviourTree.h"

#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"

#include "BehaviourTransition.h"
#include "BehaviourNode.h"
#include "CompositeNode.h"
#include "DecoratorNode.h"
#include "LeafNode.h"

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

void ResourceBehaviourTree::Save()
{
	char* behaviourTreeData = nullptr;
	unsigned btSize = GetBTSize();
	behaviourTreeData = new char[btSize];
	SaveBehaviourTreeData(behaviourTreeData);

	App->fsystem->Save((BEHAVIOURTREES + name + BEHAVIOURTREEEXTENSION).c_str(), behaviourTreeData, btSize);
	SetFile((BEHAVIOURTREES + name + BEHAVIOURTREEEXTENSION).c_str());
	RELEASE_ARRAY(behaviourTreeData);
}

void ResourceBehaviourTree::SaveBehaviourTreeData(char * data)
{
	char* cursor = data;
}

unsigned ResourceBehaviourTree::GetBTSize()
{
	return 0;
}

void ResourceBehaviourTree::CreateNode(HashString name, NodeType type)
{
	BehaviourNode* newNode = nullptr;

	switch (type)
	{
	case NodeType::Composite:
		newNode = new CompositeNode();
		break;
	case NodeType::Decorator:
		newNode = new DecoratorNode();
		break;
	case NodeType::Leaf:
		newNode = new LeafNode();
		break;
	}

	if (newNode != nullptr)
	{
		newNode->name = name;
		nodes.push_back(newNode);
	}

}

void ResourceBehaviourTree::CreateTransition(BehaviourNode * origin, BehaviourNode * destiny)
{
	BehaviourTransition* newTransition = new BehaviourTransition(origin, destiny);

	transitions.push_back(newTransition);
}

void ResourceBehaviourTree::RemoveNodeTransitions(HashString nodeName)
{
	//std::vector<Transition>::iterator it = transitions.begin();

	//while (it != transitions.end())
	//{
	//	if (it->origin == nodeName || it->destiny == nodeName)
	//	{
	//		it = transitions.erase(it);
	//	}
	//	else
	//	{
	//		++it;
	//	}
	//}
}

void ResourceBehaviourTree::RemoveTransition(unsigned UID)
{
	/*transitions.erase(transitions.begin() + UID);*/
}

void ResourceBehaviourTree::RemoveNode(unsigned UID)
{
	//RemoveNodeTransitions(nodes[UID].nodeName);
	//nodes.erase(nodes.begin() + UID);

	//if (nodes.empty())
	//{
	//	defaultNode = 0u;
	//}
	//else
	//{
	//	if (defaultNode > nodes.size() - 1)
	//	{
	//		defaultNode = nodes.size() - 1;
	//	}

	//}
}

unsigned ResourceBehaviourTree::FindNode(const HashString name)
{
	unsigned i;

	//for (i = 0u; i < nodes.size(); ++i)
	//{
	//	if (nodes[i].nodeName == name)
	//	{
	//		break;
	//	}
	//}
	return i;
}

unsigned ResourceBehaviourTree::FindTransition(const HashString origin, const HashString destiny)
{
	unsigned i;

	//for (i = 0u; i < transitions.size(); ++i)
	//{
	//	if (transitions[i].origin == origin && transitions[i].destiny == destiny)
	//	{
	//		break;
	//	}
	//}
	return i;
}
