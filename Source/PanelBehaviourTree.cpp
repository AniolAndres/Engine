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

	DrawNodes(btree);
	DrawTransitions(btree);
	ManageCreate(btree);
	ShowContextMenus(btree);

	ed::Suspend();
	ShowNodeMenu(btree);
	ShowTransitionMenu(btree);
	ShowCreateNewNode(btree);
	ed::Resume();


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
			switch ((CompositeType)btree->GetCompositeType(i))
			{
			case CompositeType::Selector:
				ImGui::Text("Comp: Selector");
				break;
			case CompositeType::Sequence:
				ImGui::Text("Comp: Sequence");
				break;
			}
			break;
		case NodeType::Decorator:
			ImGui::Text("Decorator Node");
			break;
		case NodeType::Leaf:
			ImGui::Text("Leaf Node");
			break;
		case NodeType::Root:
			ImGui::Text("Root Node");
			break;
		}

		ImGui::Text("Priority: %i", btree->GetNodePriority(i));

		drawList->AddLine(
			ImGui::GetCursorScreenPos(),
			ImGui::GetCursorScreenPos() + ImVec2(size.x - 16.0f, 0.0),
			IM_COL32(255, 255, 255, 255), 1.0f);

		ImGui::Dummy(ImVec2(64.0, 8.0));

		// In Pin
		ed::PushStyleVar(ed::StyleVar_PinArrowSize, 8.0f);
		ed::PushStyleVar(ed::StyleVar_PinArrowWidth, 8.0f);
		ed::PushStyleVar(ed::StyleVar_PinRadius, 10.0f);
		ed::PushStyleVar(ed::StyleVar_TargetDirection, ImVec2(0.0f, 0.0f));
		ed::BeginPin(i * 3 + 2, ed::PinKind::Input);
		ImGui::Text("In");
		ed::EndPin();
		ed::PopStyleVar(4);

		// Out Pin
	
		ImGui::SameLine(size.x - 40);
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

void PanelBehaviourTree::ShowNodeMenu(ResourceBehaviourTree * btree)
{
	if (ImGui::BeginPopup("Node Context BT Menu"))
	{
		ImGui::TextUnformatted("Node Context Menu");
		ImGui::Separator();

		//Node Name
		char* nodeName = new char[MAX_BT_NAME];
		strcpy(nodeName, btree->GetNodeName(contextNode).C_str());
		ImGui::InputText("Node name", nodeName, MAX_BT_NAME);
		btree->SetNodeName(contextNode, HashString(nodeName));

		//WARNING, if the node name changes the transitions containing it must also change!

		//We will manage the order of children with the int priority
		int nodePriority = btree->GetNodePriority(contextNode);
		if (ImGui::InputInt("Priority", &nodePriority))
		{
			btree->SetNodePriority(contextNode, nodePriority);
			btree->Save();
		}

		switch ((NodeType)btree->GetNodeType(contextNode))
		{
			case NodeType::Composite:
			{
				HashString cTypeName;

				CompositeType nodeCType = btree->GetCompositeType(contextNode);
				switch (nodeCType)
				{
				case CompositeType::Selector:
					cTypeName = HashString("Selector");
					break;
				case CompositeType::Sequence:
					cTypeName = HashString("Sequence");
					break;
				}

				if (ImGui::BeginCombo("Comp type", cTypeName.C_str()))
				{
					bool isSelected = btree->GetCompositeType(contextNode) == CompositeType::Sequence;
					if (ImGui::Selectable("Sequence", isSelected))
					{
						btree->SetCompositeType(contextNode, CompositeType::Sequence);
					}
					isSelected = btree->GetCompositeType(contextNode) == CompositeType::Selector;
					if (ImGui::Selectable("Selector", isSelected))
					{
						btree->SetCompositeType(contextNode, CompositeType::Selector);
					}

					ImGui::EndCombo();
				}
			}
				break;
			case NodeType::Decorator:

				break;
			case NodeType::Leaf:
			//A selectable for the script to use yay
				break;
		}

		//if (ImGui::BeginCombo("Clip", stateMachine->GetNodeClip(contextNode).C_str()))
		//{
		//	for (unsigned i = 0u; i < stateMachine->GetClipsSize(); i++)
		//	{
		//		bool isSelected = stateMachine->GetNodeClip(contextNode) == stateMachine->GetClipName(i);
		//		if (ImGui::Selectable(stateMachine->GetClipName(i).C_str(), isSelected))
		//		{
		//			stateMachine->SetNodeClip(contextNode, stateMachine->GetClipName(i));
		//			stateMachine->Save();
		//		}
		//		if (isSelected)
		//		{
		//			ImGui::SetItemDefaultFocus();
		//		}
		//	}
		//	ImGui::EndCombo();
		//}

		//ImGui::Separator();

		if (ImGui::MenuItem("Delete"))
		{
			ed::DeleteNode(ed::NodeId((contextNode + 1) * 3));
			btree->RemoveNode(contextNode);
			btree->Save();
		}
		ImGui::EndPopup();
	}
}

void PanelBehaviourTree::ShowTransitionMenu(ResourceBehaviourTree * btree)
{
	if (ImGui::BeginPopup("Link Context BT Menu"))
	{
		ImGui::TextUnformatted("Link Context Menu");
		ImGui::Separator();

		//Origin
		char* oName = new char[MAX_BT_NAME];
		strcpy(oName, btree->GetTransitionOrigin(contextLink).C_str());
		ImGui::InputText("Origin", oName, MAX_BT_NAME);
		btree->SetTransitionOrigin(HashString(oName), contextLink);

		//Destiny
		char* dName = new char[MAX_BT_NAME];
		strcpy(dName, btree->GetTransitionDestiny(contextLink).C_str());
		ImGui::InputText("Destiny", dName, MAX_BT_NAME);
		btree->SetTransitionDestiny(HashString(dName), contextLink);

		ImGui::EndPopup();
	}
}

void PanelBehaviourTree::DrawTransitions(ResourceBehaviourTree * btree)
{
	ed::PushStyleVar(ed::StyleVar_LinkStrength, 4.0f);
	unsigned numNodes = btree->GetNodesSize();
	for (unsigned i = 0u, count = btree->GetTransitionsSize(); i < count; ++i)
	{
		unsigned origin = btree->FindNode(btree->GetTransitionOrigin(i));
		unsigned destiny = btree->FindNode(btree->GetTransitionDestiny(i));

		if (origin < numNodes && destiny < numNodes)
		{
			ed::Link(numNodes * 3 + i + 1, origin * 3 + 3, destiny * 3 + 2);
		}
	}
	ed::PopStyleVar(1);
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
		ImGui::TextUnformatted("Create BT Node Menu");
		ImGui::Separator();

		if (ImGui::BeginMenu("New Behaviour Node"))
		{

			
			if (ImGui::MenuItem("Composite"))
			{
				unsigned nodeIndex = btree->GetNodesSize();
				ed::SetNodePosition(nodeIndex * 3 + 1, ed::ScreenToCanvas(newNodePosition));
				btree->CreateNode(HashString("New Composite Node"), NodeType::Composite, 0);
				btree->Save();
			}
			if (ImGui::MenuItem("Decorator"))
			{
				unsigned nodeIndex = btree->GetNodesSize();
				ed::SetNodePosition(nodeIndex * 3 + 1, ed::ScreenToCanvas(newNodePosition));
				btree->CreateNode(HashString("New Decorator Node"), NodeType::Decorator, 0);
				btree->Save();
			}
			if (ImGui::MenuItem("Leaf"))
			{
				unsigned nodeIndex = btree->GetNodesSize();
				ed::SetNodePosition(nodeIndex * 3 + 1, ed::ScreenToCanvas(newNodePosition));
				btree->CreateNode(HashString("New Leaf Node"), NodeType::Leaf, 0);
				btree->Save();
			}
			if (ImGui::MenuItem("Root"))
			{
				unsigned nodeIndex = btree->GetNodesSize();
				ed::SetNodePosition(nodeIndex * 3 + 1, ed::ScreenToCanvas(newNodePosition));
				btree->CreateNode(HashString("New Root Node"), NodeType::Root, 0);
				btree->Save();
			}

			ImGui::EndMenu();
		}
		

		ImGui::EndPopup();
	}

}

void PanelBehaviourTree::ManageCreate(ResourceBehaviourTree * btree)
{
	if (ed::BeginCreate(ImColor(255, 255, 255), 2.0f))
	{
		auto showLabel = [](const char* label, ImColor color)
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
			auto size = ImGui::CalcTextSize(label);

			auto padding = ImGui::GetStyle().FramePadding;
			auto spacing = ImGui::GetStyle().ItemSpacing;

			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

			auto rectMin = ImGui::GetCursorScreenPos() - padding;
			auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

			auto drawList = ImGui::GetWindowDrawList();
			drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
			ImGui::TextUnformatted(label);
		};

		ed::PinId startPinId = 0, endPinId = 0;
		if (ed::QueryNewLink(&startPinId, &endPinId))
		{
			if (startPinId && endPinId)
			{
				bool startIsInput = unsigned(startPinId.Get() - 1) % 3 == 1;
				bool endIsInput = unsigned(endPinId.Get() - 1) % 3 == 1;
				unsigned startNode = unsigned(startPinId.Get() - 1) / 3;
				unsigned endNode = unsigned(endPinId.Get() - 1) / 3;

				if (endPinId == startPinId)
				{
					ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
				}
				else if (startIsInput == endIsInput)
				{
					showLabel("x Incompatible Pins. Must be In->Out", ImColor(45, 32, 32, 180));
					ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
				}
				else if (startNode == endNode)
				{
					showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
					ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
				}
				else
				{
					showLabel("+ Create Link", ImColor(32, 45, 32, 180));
					if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
					{
						if (startIsInput)
						{
							btree->CreateTransition(btree->GetNodeName(endNode), btree->GetNodeName(startNode));
						}
						else
						{
							btree->CreateTransition(btree->GetNodeName(startNode), btree->GetNodeName(endNode));
						}
						btree->Save();
					}
				}
			}
		}

		if (ed::QueryNewNode(&newNodePin))
		{
			bool pinIsInput = unsigned(newNodePin.Get() - 1) % 3 == 1;

			if (!pinIsInput)
			{
				if (newNodePin != ed::PinId::Invalid)
				{
					showLabel("+ Create Node", ImColor(32, 45, 32, 180));
				}

				if (ed::AcceptNewItem())
				{
					ed::Suspend();
					newNodePosition = ImGui::GetMousePos();
					ImGui::OpenPopup("Create New BT Node");
					ed::Resume();
				}
			}
		}
	}

	ed::EndCreate();
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

		if (btComp->bTree != nullptr)
		{
			DrawBT(btComp->bTree, btComp->GetEditorBTContext());
		}
	}

	ImGui::End();
}
