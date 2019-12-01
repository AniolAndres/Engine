#include "ComponentBehaviourTree.h"

#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleTime.h"
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
		bool removed = Component::DrawComponentState();
		if (removed)
		{
			ImGui::PopID();
			return;
		}

		if (ImGui::Button("New Tree"))
		{
			if (bTree != nullptr)
				bTree->Save();

			CreateNewBehaviourTree();
		}

		ImGui::PushID("BTRee Combo");
		if (ImGui::BeginCombo("Btree", bTree != nullptr ? bTree->GetName() : ""))
		{
			if (guiBTrees.empty())
			{
				guiBTrees = App->resManager->GetResourceNamesList(TYPE::BEHAVIOURTREE, true);
			}
			for (unsigned i = 0u; i < guiBTrees.size(); i++)
			{
				bool is_selected = (bTree != nullptr ? bTree->GetName() == guiBTrees[i].c_str() : false);
				if (ImGui::Selectable(guiBTrees[i].c_str(), is_selected))
				{
					if (bTree != nullptr)
						bTree->Save();

					SetBehaviourTree(guiBTrees[i].c_str());
				}
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		else
		{
			guiBTrees.clear();
		}
		ImGui::PopID();
	}
	ImGui::PopID();
}

void ComponentBehaviourTree::CreateNewBehaviourTree()
{
	ResourceBehaviourTree* newBTree = new ResourceBehaviourTree(App->resManager->GenerateNewUID());

	newBTree->SetName("NewBehaviourTree");

	//Do not fear the while, accept it
	if (App->resManager->NameExists(newBTree->GetName(), TYPE::BEHAVIOURTREE))
	{
		std::string newName = App->resManager->GetAvailableName(newBTree->GetName(), TYPE::BEHAVIOURTREE);
		newBTree->Rename(newName.c_str());
	}

	newBTree->Save();
	RELEASE(newBTree);
}

void ComponentBehaviourTree::SetBehaviourTree(const char* btreeFile)
{
	// Delete previous Btree
	if (bTree != nullptr)
		App->resManager->DeleteResource(bTree->GetUID());

	if (btreeFile != nullptr)
	{
		bTree = (ResourceBehaviourTree*)App->resManager->GetByName(btreeFile, TYPE::BEHAVIOURTREE);
		strcpy(newBTname, bTree->GetName());
	}
}

void ComponentBehaviourTree::Update()
{
	if (bTree != nullptr && App->time->gameState == GameState::RUN)
	{
		bTree->Tick();
	}
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
