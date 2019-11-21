#include "PanelBehaviourTree.h"

#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ComponentBehaviourTree.h"

#include "BaseScript.h"


#include "HashString.h"
#include "NodeEditor.h"
#include "Globals.h"

#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui.h"
#include "imgui_internal.h"

PanelBehaviourTree::PanelBehaviourTree()
{
}


PanelBehaviourTree::~PanelBehaviourTree()
{
}

void PanelBehaviourTree::DrawBT(ResourceBehaviourTree * btree, ax::NodeEditor::EditorContext * context)
{
	auto& io = ImGui::GetIO();

	ed::SetCurrentEditor(context);
	ed::Begin("Behaviour Tree Editor", ImVec2(0.0, 0.0f));

	ed::End();
	ed::SetCurrentEditor(nullptr);
}

void PanelBehaviourTree::Draw()
{
	if (!ImGui::Begin("BehaviourTree", &enabled))
	{
		ImGui::End();
		return;
	}

	if (App->scene->selected != nullptr && App->scene->selected->GetComponent<ComponentBehaviourTree>())
	{
		ComponentBehaviourTree* btComp = App->scene->selected->GetComponent<ComponentBehaviourTree>();
		if (btComp != nullptr)
		{
			DrawBT(btComp->bTree, btComp->GetEditorBTContext());
		}
	}

	ImGui::End();
}
