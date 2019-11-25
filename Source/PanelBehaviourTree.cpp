#include "PanelBehaviourTree.h"

#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ComponentBehaviourTree.h"

#include "ResourceBehaviourTree.h"

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

		ShowContextMenus(btree);
		ShowCreateNewNode(btree);

		ed::End();
		ed::SetCurrentEditor(nullptr);

}

void PanelBehaviourTree::ShowContextMenus(ResourceBehaviourTree* btree)
{
	ed::Suspend();

	ed::NodeId contextNodeId = 0;
	ed::PinId contextPinId = 0;
	ed::LinkId contextLinkId = 0;
	if (ed::ShowNodeContextMenu(&contextNodeId))
	{
		contextNode = unsigned(contextNodeId.Get() - 1) / 3;
		ImGui::OpenPopup("Node Context BT Menu");
	}
	else if (ed::ShowPinContextMenu(&contextPinId))
	{
		ImGui::OpenPopup("Pin Context BT Menu");
	}
	else if (ed::ShowLinkContextMenu(&contextLinkId))
	{
		contextLink = unsigned(contextLinkId.Get()) - btree->GetNodesSize() * 3 - 1;
		ImGui::OpenPopup("Link Context BT Menu");
	}
	else if (ed::ShowBackgroundContextMenu())
	{
		newNodePosition = ImGui::GetMousePos();
		newNodePin = 0;
		ImGui::OpenPopup("Create New BT Node");
	}

	ed::Resume();
}

void PanelBehaviourTree::ShowCreateNewNode(ResourceBehaviourTree * btree)
{

	if (ImGui::BeginPopup("Create New BT Node"))
	{
		ImGui::TextUnformatted("Create Node Menu");
		ImGui::Separator();

		if (ImGui::BeginMenu("New Node"))
		{
			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}
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
