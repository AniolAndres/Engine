#ifndef __COMPONENTBEHAVIOURTREE_H_
#define __COMPONENTBEHAVIOURTREE_H_

#include "Component.h"

#include "NodeEditor.h"
#include <string>
#include <vector>

class GameObject;
class ResourceBehaviourTree;

#define MAX_BTREE_NAME 30

class ComponentBehaviourTree : public Component
{
public:
	typedef ax::NodeEditor::EditorContext EditorContext;	

	ComponentBehaviourTree();
	ComponentBehaviourTree(GameObject* gameobject);
	ComponentBehaviourTree(const ComponentBehaviourTree &copy);
	~ComponentBehaviourTree();

	Component* Clone() const;

	void Save(JSON_value* value) const override;
	void Load(JSON_value* value) override;

	void DrawProperties() override;
	void CreateNewBehaviourTree();
	void SetBehaviourTree(const char* btreeFile);


	ResourceBehaviourTree* bTree = nullptr;
	EditorContext* GetEditorBTContext();

private:
	EditorContext* context = nullptr;

	std::vector<std::string> guiBTrees;
	std::vector<std::string> guiScripts;

	char newBTname[MAX_BTREE_NAME] = "";
};

#endif // 