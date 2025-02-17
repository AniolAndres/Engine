#ifndef __ModuleEditor_h__
#define __ModuleEditor_h__

#include <list>
#include "Module.h"
#include "NodeEditor.h"
#include "HashString.h"
#include "SDL_events.h"

class Panel;
class PanelConsole;
class PanelInspector;
class PanelBrowser;
class PanelConfiguration;
class PanelAbout;
class PanelHardware;
class PanelHierarchy;
class PanelTime;
class PanelResourceManager;
class PanelState;
class PanelBehaviourTree;
class PanelAnimation;
class PanelNavigation;
class GameObject;
class MaterialEditor;
class FileExplorer;
class PanelResourceManagerUnused;

class ModuleEditor :
	public Module
{
	typedef ax::NodeEditor::EditorContext EditorContext;
public:
	ModuleEditor();
	~ModuleEditor();
	bool Init(JSON * config) override;
	update_status PreUpdate() override;
	update_status Update(float dt) override;
	bool CleanUp() override;

	void RenderGUI() const;
	void ShowInspector();

	ENGINE_API EditorContext* CreateNodeEditorContext(const char* name );

	void processInput(SDL_Event* event) const;
	void AddFpsLog(float dt) const;
	void AddLog(const char *log) const;
	void GenerateGenericPopUp(const char* title, const char* text);

private:
	void CreateDockSpace() const;
	void DrawPanels();
	void WindowsMenu();
	void ToolsMenu();
	void HelpMenu();

	void OpenGenericPopUp();

public:
	PanelConsole *console = nullptr;
	PanelConfiguration *configuration = nullptr;
	PanelInspector *inspector = nullptr;
	PanelAbout *about = nullptr;
	PanelHardware *hardware = nullptr;
	PanelHierarchy *hierarchy = nullptr;
	PanelTime *time = nullptr;
	PanelBrowser *assets = nullptr;
	PanelResourceManager *resource = nullptr;
	PanelState *states = nullptr;
	PanelAnimation *animation = nullptr;
	PanelNavigation* navigation = nullptr;
	PanelResourceManagerUnused *resourceUnused = nullptr;
	PanelBehaviourTree* behaviour = nullptr;

	MaterialEditor *materialEditor = nullptr;
	FileExplorer *fileExplorer = nullptr;
	mutable bool wantKeyboard = false;

private:
	std::list<Panel*> panels;

	// Generic Pop-Up variables
	std::string popUpTitle;
	std::string popUpText;
	bool openGenericPopUp = false;
};

#endif //__ModuleEditor_h__
