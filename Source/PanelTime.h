#ifndef __PanelTime_h__
#define __PanelTime_h__

#include "Panel.h"
class PanelTime :
	public Panel
{
public:
	PanelTime();
	~PanelTime();

	void Draw() override;

};

#endif //__PanelTime_h__