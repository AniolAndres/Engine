#ifndef _PANELBEHAVIOURTREE_H_
#define _PANELBEHAVIOURTREE_H_

#include "Panel.h"
class PanelBehaviourTree : public Panel
{
public:
	PanelBehaviourTree();
	~PanelBehaviourTree();

	void Draw() override;
};

#endif // _PANELBEHAVIOURTREE_H_