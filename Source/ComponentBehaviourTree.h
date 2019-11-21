#ifndef __COMPONENTBEHAVIOURTREE_H_
#define __COMPONENTBEHAVIOURTREE_H_

#include "Component.h"

#include "NodeEditor.h"

class GameObject;
class ResourceBehaviourTree;

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

	ResourceBehaviourTree* bTree = nullptr;
	EditorContext* GetEditorBTContext();

private:
	EditorContext* context = nullptr;

};

#endif // 