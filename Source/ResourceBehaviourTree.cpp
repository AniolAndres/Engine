#include "ResourceBehaviourTree.h"

#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"

#include "BehaviourTransition.h"
#include "BehaviourNode.h"
#include "CompositeNode.h"
#include "DecoratorNode.h"
#include "RootBehaviourNode.h"
#include "LeafNode.h"

#include "JSON.h"

ResourceBehaviourTree::ResourceBehaviourTree(unsigned uid) : Resource(uid, TYPE::BEHAVIOURTREE)
{
}

ResourceBehaviourTree::ResourceBehaviourTree(const ResourceBehaviourTree & resource) : Resource(resource)
{
	name = resource.name;
	for (const auto& node : resource.nodes)
	{
		CreateNode(node->name, node->type, 0);
	}

	for (const auto& transition : resource.transitions)
	{
		transitions.emplace_back(transition.originName, transition.destinyName);
	}
}

ResourceBehaviourTree::~ResourceBehaviourTree()
{
	DeleteFromMemory();
}

bool ResourceBehaviourTree::LoadInMemory()
{
	App->resManager->DeleteResourceFromUnusedList(UID);

	char* data = nullptr;

	unsigned ok = App->fsystem->Load(exportedFile.c_str(), &data);

	// Load BT file
	if (ok != 0)
	{
		SetBehaviourTree(data); //Deallocates data
		++loaded;
	}
	return true;
}

void ResourceBehaviourTree::DeleteFromMemory()
{
	Resource::DeleteFromMemory();
	nodes.clear();
	transitions.clear();
}

void ResourceBehaviourTree::Delete()
{
	// Delete Resource from ResourceManager
	App->resManager->DeleteResourceFromList(UID);

	// Delete file in Library
	std::string fileInLibrary(IMPORTED_BEHAVIOURTREES);
	fileInLibrary += exportedFile;
	fileInLibrary += BEHAVIOURTREEEXTENSION;
	App->fsystem->Delete(fileInLibrary.c_str());
	DeleteFromMemory();
}

void ResourceBehaviourTree::SaveMetafile(const char * file) const
{
	std::string filepath;
	filepath.append(file);
	JSON* json = new JSON();
	JSON_value* meta = json->CreateValue();
	struct stat statFile;
	stat(filepath.c_str(), &statFile);
	meta->AddUint("metaVersion", META_VERSION);
	meta->AddUint("timeCreated", statFile.st_ctime);

	// Resource info
	meta->AddUint("GUID", UID);
	meta->AddString("Name", name.c_str());
	meta->AddString("File", file);
	meta->AddString("ExportedFile", exportedFile.c_str());

	json->AddValue("BehaviourTree", *meta);
	filepath += METAEXT;

	App->fsystem->Save(filepath.c_str(), json->ToString().c_str(), json->Size());

	// Save meta in Library
	std::string libraryPath(exportedFile + METAEXT);
	App->fsystem->Save(libraryPath.c_str(), json->ToString().c_str(), json->Size());
	RELEASE(json);
}

void ResourceBehaviourTree::LoadConfigFromMeta()
{
	Resource::LoadConfigFromMeta();

	std::string metaFile(file);
	metaFile += ".meta";

	// Check if meta file exists
	if (!App->fsystem->Exists(metaFile.c_str()))
		return;

	unsigned oldUID = GetUID();
	char* data = nullptr;

	if (App->fsystem->Load(metaFile.c_str(), &data) == 0)
	{
		LOG("Warning: %s couldn't be loaded", metaFile.c_str());
		RELEASE_ARRAY(data);
		return;
	}

	JSON* json = new JSON(data);
	JSON_value* value = json->GetValue("BehaviourTree");
	name = value->GetString("Name", "");

	if (name.empty())	// Try old meta version
		name = value->GetString("name");

	// Make sure the UID from meta is the same
	unsigned checkUID = value->GetUint("GUID");
	if (oldUID != checkUID)
	{
		UID = checkUID;
		// Update resource UID on resource list
		App->resManager->ReplaceResource(oldUID, this);
		exportedFile = IMPORTED_BEHAVIOURTREES + std::to_string(UID) + BEHAVIOURTREEEXTENSION;
	}

	// Check the meta file version
	if (value->GetUint("metaVersion", 0u) < META_VERSION)
		SaveMetafile(file.c_str());

	if (!App->fsystem->Exists((exportedFile + METAEXT).c_str()))
		SaveMetafile(file.c_str());

	RELEASE_ARRAY(data);
	RELEASE(json);
}

void ResourceBehaviourTree::LoadConfigFromLibraryMeta()
{
	Resource::LoadConfigFromMeta();

	std::string metaFile(exportedFile);
	metaFile += ".meta";

	// Check if meta file exists
	if (!App->fsystem->Exists(metaFile.c_str()))
		return;

	unsigned oldUID = GetUID();
	char* data = nullptr;

	if (App->fsystem->Load(metaFile.c_str(), &data) == 0)
	{
		LOG("Warning: %s couldn't be loaded", metaFile.c_str());
		RELEASE_ARRAY(data);
		return;
	}
	JSON* json = new JSON(data);
	JSON_value* value = json->GetValue("BehaviourTree");

	// Get resource variables
	name = value->GetString("Name");
	file = value->GetString("File");

	RELEASE_ARRAY(data);
	RELEASE(json);
}

void ResourceBehaviourTree::SetBehaviourTree(const char * data)
{
	nodes.clear();
	transitions.clear();

	//import nodes
	unsigned nodesSize = 0u;
	memcpy(&nodesSize, data, sizeof(unsigned));
	data += sizeof(unsigned);

	for (unsigned i = 0u; i < nodesSize; ++i)
	{
		char nodeName[MAX_BONE_NAME_LENGTH];
		memcpy(nodeName, data, sizeof(char) * MAX_BONE_NAME_LENGTH);
		data += sizeof(char)* MAX_BONE_NAME_LENGTH;
		HashString newNodeName(nodeName);

		unsigned nodeType = 0u;
		memcpy(&nodeType, data, sizeof(unsigned));
		data += sizeof(unsigned);
		
		unsigned compositeType = 0u;
		memcpy(&compositeType, data, sizeof(unsigned));
		data += sizeof(unsigned);

		unsigned priority = 0;
		memcpy(&priority, data, sizeof(int));
		data += sizeof(int);

		BehaviourNode* newNode = CreateNode(newNodeName, (NodeType)nodeType, priority);
		newNode->cType = (CompositeType)compositeType;
	}

	//import transitions
	unsigned transistionSize = 0u;
	memcpy(&transistionSize, data, sizeof(unsigned));
	data += sizeof(unsigned);

	for (unsigned j = 0u; j < transistionSize; ++j)
	{
		char originName[MAX_BONE_NAME_LENGTH];
		memcpy(originName, data, sizeof(char) * MAX_BONE_NAME_LENGTH);
		data += sizeof(char)* MAX_BONE_NAME_LENGTH;
		HashString newOrigin(originName);

		char destinyName[MAX_BONE_NAME_LENGTH];
		memcpy(destinyName, data, sizeof(char) * MAX_BONE_NAME_LENGTH);
		data += sizeof(char)* MAX_BONE_NAME_LENGTH;
		HashString newDestiny(destinyName);

		CreateTransition(newOrigin, newDestiny);
	}

	BuildTree();
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

	unsigned nodeSize = nodes.size();
	memcpy(cursor, &nodeSize, sizeof(unsigned));
	cursor += sizeof(unsigned);

	for (const auto& node : nodes)
	{
		memcpy(cursor, node->name.C_str(), sizeof(char) * MAX_BONE_NAME_LENGTH);
		cursor += sizeof(char) * MAX_BONE_NAME_LENGTH;

		unsigned nType = (unsigned)node->type;
		memcpy(cursor, &nType, sizeof(unsigned));
		cursor += sizeof(unsigned);

		unsigned cType = (unsigned)node->cType;
		memcpy(cursor, &cType, sizeof(unsigned));
		cursor += sizeof(unsigned);

		memcpy(cursor, &node->priority, sizeof(int));
		cursor += sizeof(int);
	}

	unsigned transitionSize = transitions.size();
	memcpy(cursor, &transitionSize, sizeof(unsigned));
	cursor += sizeof(unsigned);

	for (const auto& transition : transitions)
	{
		memcpy(cursor, transition.originName.C_str(), sizeof(char) * MAX_BONE_NAME_LENGTH);
		cursor += sizeof(char) * MAX_BONE_NAME_LENGTH;

		memcpy(cursor, transition.destinyName.C_str(), sizeof(char) * MAX_BONE_NAME_LENGTH);
		cursor += sizeof(char) * MAX_BONE_NAME_LENGTH;
	}

}

unsigned ResourceBehaviourTree::GetBTSize()
{
	unsigned size = 0u;
	size += sizeof(unsigned) * 2u;

	for (const auto& node : nodes)
	{
		size += sizeof(char) * MAX_BONE_NAME_LENGTH;
		size += sizeof(unsigned) * 2;
		size += sizeof(int);
	}

	for (const auto& transition : transitions)
	{
		size += sizeof(char) * MAX_BONE_NAME_LENGTH * 2;
	}

	return size;
}

void ResourceBehaviourTree::Tick()
{
	rootNode->TickNode();
}

BehaviourNode* ResourceBehaviourTree::CreateNode(HashString name, NodeType type, int priority)
{
	BehaviourNode* newNode = nullptr;

	switch (type)
	{
	case NodeType::Composite:
		newNode = new CompositeNode(name,type);
		break;
	case NodeType::Decorator:
		newNode = new DecoratorNode(name, type);
		break;
	case NodeType::Leaf:
		newNode = new LeafNode(name, type);
		break;
	case NodeType::Root:
		newNode = new RootBehaviourNode();
		break;
	}

	if (newNode != nullptr)
	{
		newNode->priority = priority;
		nodes.push_back(newNode);
	}

	return newNode;
}

void ResourceBehaviourTree::CreateTransition(HashString origin, HashString destiny)
{
	transitions.push_back(BehaviourTransition(origin, destiny));
}

void ResourceBehaviourTree::RemoveNodeTransitions(HashString nodeName)
{
	std::vector<BehaviourTransition>::iterator it = transitions.begin();

	while (it != transitions.end())
	{
		if (it->originName == nodeName || it->destinyName == nodeName)
		{
			it = transitions.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void ResourceBehaviourTree::RemoveTransition(unsigned UID)
{
	transitions.erase(transitions.begin() + UID);
}

void ResourceBehaviourTree::RemoveNode(unsigned UID)
{
	RemoveNodeTransitions(nodes[UID]->name);
	nodes.erase(nodes.begin() + UID);
}

unsigned ResourceBehaviourTree::FindNode(const HashString name)
{
	unsigned i;

	for (i = 0u; i < nodes.size(); ++i)
	{
		if (nodes[i]->name == name)
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
		if (transitions[i].originName == origin && transitions[i].destinyName == destiny)
		{
			break;
		}
	}
	return i;
}

HashString ResourceBehaviourTree::GetNodeName(unsigned index)
{
	return nodes[index]->name;
}

unsigned ResourceBehaviourTree::GetNodeType(unsigned index)
{
	return (unsigned)nodes[index]->type;
}

void ResourceBehaviourTree::SetNodeName(unsigned index, HashString name)
{
	HashString previousName = nodes[index]->name;

	nodes[index]->name = name;

	for (auto& transition : transitions)
	{
		if (transition.originName == previousName)
		{
			transition.originName = name;
		}

		if (transition.destinyName == previousName)
		{
			transition.destinyName = name;
		}
	}
}

void ResourceBehaviourTree::SetNodePriority(unsigned index, int priority)
{
	nodes[index]->priority = priority;
}

int ResourceBehaviourTree::GetNodePriority(unsigned index)
{
	return nodes[index]->priority;
}

CompositeType ResourceBehaviourTree::GetCompositeType(unsigned index)
{
	if (nodes[index]->type == NodeType::Composite)
	{
		CompositeNode* thisNode = (CompositeNode*)nodes[index];
		return thisNode->cType;
	}
}

void ResourceBehaviourTree::SetCompositeType(unsigned index, CompositeType cType)
{
	if (nodes[index]->type == NodeType::Composite)
	{
		CompositeNode* thisNode = (CompositeNode*)nodes[index];
		thisNode->cType = cType;
	}
}

HashString ResourceBehaviourTree::GetTransitionOrigin(unsigned index)
{
	return transitions[index].originName;
}

HashString ResourceBehaviourTree::GetTransitionDestiny(unsigned index)
{
	return transitions[index].destinyName;
}

void ResourceBehaviourTree::SetTransitionOrigin(HashString name, unsigned index)
{
	transitions[index].originName = name;
}

void ResourceBehaviourTree::SetTransitionDestiny(HashString name, unsigned index)
{
	transitions[index].destinyName = name;
}

unsigned ResourceBehaviourTree::GetNodesSize()
{
	return nodes.size();
}

unsigned ResourceBehaviourTree::GetTransitionsSize()
{
	return transitions.size();
}

void ResourceBehaviourTree::BuildTree()
{
	CleanTree();

	for (auto& node : nodes)
	{
		if (node->type == NodeType::Root)
		{ 
			rootNode = (RootBehaviourNode*)node;
			break;
		}
	}

	for (auto& node : nodes)
	{
		if (node->type != NodeType::Leaf)
		{
			for (auto& transition : transitions)
			{
				if (transition.originName == node->name)
				{
					for (auto& nodeBis : nodes)
					{
						if (nodeBis->name == transition.destinyName)
						{
							switch (node->type)
							{
							case NodeType::Composite:
								((CompositeNode*)node)->nodeChildren.push_back(nodeBis);
								break;
							case NodeType::Decorator:
								((DecoratorNode*)node)->child = nodeBis;
								break;
							case NodeType::Root:
								((RootBehaviourNode*)node)->rootChildren.push_back(nodeBis);
								break;
							}
							break;
						}
					}
				}
			}
		}
	}

	PriorizeNodes();
}

void ResourceBehaviourTree::CleanTree()
{
	if (rootNode != nullptr)
	{
		rootNode->CleanNode();
	}
}

void ResourceBehaviourTree::PriorizeNodes()
{
	if (rootNode != nullptr)
	{
		rootNode->OrderChildren();
	}
}
