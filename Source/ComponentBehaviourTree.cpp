#include "ComponentBehaviourTree.h"

#include "Application.h"
#include "ModuleResourceManager.h"
#include "GameObject.h"

#include "Resource.h"
#include "ResourceBehaviourTree.h"

#include "imgui.h"
#include "JSON.h"

ComponentBehaviourTree::ComponentBehaviourTree() : Component(nullptr, ComponentType::BehaviourTree)
{

}

ComponentBehaviourTree::ComponentBehaviourTree(GameObject * gameobject) : Component(gameobject, ComponentType::BehaviourTree)
{

}

ComponentBehaviourTree::ComponentBehaviourTree(const ComponentBehaviourTree& component) : Component(component)
{
	if (component.bTree != nullptr)
		bTree = (ResourceBehaviourTree*)App->resManager->Get(component.bTree->GetUID());
}

ComponentBehaviourTree::~ComponentBehaviourTree()
{

}


Component* ComponentBehaviourTree::Clone() const
{
	return new ComponentBehaviourTree(*this);
}

void ComponentBehaviourTree::Save(JSON_value * value) const
{
	Component::Save(value);
	value->AddUint("btUID", (bTree != nullptr) ? bTree->GetUID() : 0u);
}

void ComponentBehaviourTree::Load(JSON_value * value)
{
	Component::Load(value);

	unsigned bTreeUID = value->GetUint("btUID");
	if (bTreeUID != 0)
		bTree = (ResourceBehaviourTree*)App->resManager->Get(bTreeUID);
}

void ComponentBehaviourTree::DrawProperties()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("Behaviour Tree", ImGuiTreeNodeFlags_DefaultOpen))
	{

	}
	ImGui::PopID();
}

ComponentBehaviourTree::EditorContext * ComponentBehaviourTree::GetEditorBTContext()
{
	if (context == nullptr)
	{
		ax::NodeEditor::Config cfg;
		cfg.SettingsFile = "Btree.json";
		context = ax::NodeEditor::CreateEditor(&cfg);
	}
	return context;
}
