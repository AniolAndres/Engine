#ifndef _PANELBEHAVIOURTREE_H_
#define _PANELBEHAVIOURTREE_H_

#include "Panel.h"

#include "NodeEditor.h"
#include "HashString.h"

namespace ed = ax::NodeEditor;

class ResourceBehaviourTree;
class BehaviourTreeScript;
enum class NodeType;

class PanelBehaviourTree : public Panel
{
public:
	PanelBehaviourTree();
	~PanelBehaviourTree();

	void DrawBT(ResourceBehaviourTree* btree, ax::NodeEditor::EditorContext* context);
	void DrawNodes(ResourceBehaviourTree* btree);
	void ShowNodeMenu(ResourceBehaviourTree* btree);
	void DrawTransitions(ResourceBehaviourTree* btree);
	void ShowContextMenus(ResourceBehaviourTree* btree);
	void ShowCreateNewNode(ResourceBehaviourTree* btree);
	void ManageCreate(ResourceBehaviourTree* btree);


	void Draw() override;

	unsigned contextNode = 0u;
	unsigned contextLink = 0u;

	ed::PinId newNodePin = 0;
	ImVec2 newNodePosition;
};

#endif // _PANELBEHAVIOURTREE_H_