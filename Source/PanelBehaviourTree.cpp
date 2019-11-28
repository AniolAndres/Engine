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

void PanelBehaviourTree::DrawNodes(ResourceBehaviourTree * btree)
{
	unsigned nodesSize = btree->GetNodesSize();
	for (unsigned i = 0u; i < nodesSize; ++i)
	{
		ed::PushStyleColor(ed::StyleColor_PinRect, ImColor(60, 180, 255, 150));
		ed::PushStyleColor(ed::StyleColor_PinRectBorder, ImColor(60, 180, 255, 150));

		ed::BeginNode(i * 3 + 1);
		ImGui::Indent(1.0);
		ImGui::Text(btree->GetNodeName(i).C_str());

		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f * ImGui::GetStyle().Alpha);

		ImVec2 size = ed::GetNodeSize(i * 3 + 1);
		ImVec2 pos = ed::GetNodePosition(i * 3 + 2);

		ImDrawList* drawList = ed::GetNodeBackgroundDrawList(i * 3 + 1);

		drawList->AddLine(
			ImGui::GetCursorScreenPos(),
			ImGui::GetCursorScreenPos() + ImVec2(size.x - 16.0f, 0.0),
			IM_COL32(255, 255, 0, 255), 1.0f);


		ImGui::PopStyleVar();

		ImGui::Dummy(ImVec2(96.0, 8.0));

		switch ((NodeType)btree->GetNodeType(i))
		{
		case NodeType::Composite:
			ImGui::Text("Composite Node");
			break;
		case NodeType::Decorator:
			ImGui::Text("Decorator Node");
			break;
		case NodeType::Leaf:
			ImGui::Text("Leaf Node");
			break;
		}

		drawList->AddLine(
			ImGui::GetCursorScreenPos(),
			ImGui::GetCursorScreenPos() + ImVec2(size.x - 16.0f, 0.0),
			IM_COL32(255, 255, 255, 255), 1.0f);

		ImGui::Dummy(ImVec2(64.0, 8.0));

		// Out Pin
		ImGui::SameLine(size.x/2);
		ed::PushStyleVar(ed::StyleVar_PinArrowSize, 0.0f);
		ed::PushStyleVar(ed::StyleVar_PinArrowWidth, 0.0f);
		ed::PushStyleVar(ed::StyleVar_TargetDirection, ImVec2(0.0f, 0.0f));
		ed::BeginPin(i * 3 + 3, ed::PinKind::Output);
		ImGui::Text("Out");
		ed::EndPin();

		ed::EndNode();

		ed::PopStyleVar(3);
		ed::PopStyleColor(2);
	}
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
