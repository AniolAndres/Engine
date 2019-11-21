#ifndef _PANELBEHAVIOURTREE_H_
#define _PANELBEHAVIOURTREE_H_

#include "Panel.h"

#include "NodeEditor.h"
#include "HashString.h"

namespace ed = ax::NodeEditor;

class ResourceBehaviourTree;
class BehaviourTreeScript;

class PanelBehaviourTree : public Panel
{
public:
	PanelBehaviourTree();
	~PanelBehaviourTree();

	void DrawBT(ResourceBehaviourTree* btree, ax::NodeEditor::EditorContext* context);
	void DrawNodes(ResourceBehaviourTree* btree);
	void DrawTransitions(ResourceBehaviourTree* btree);


	void Draw() override;
};

#endif // _PANELBEHAVIOURTREE_H_