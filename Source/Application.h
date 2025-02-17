#ifndef __Application_h__
#define __Application_h__

#include<list>
#include "Globals.h"
#include "Timer.h"

#define CONFIG_FILE "config.json"

class Module;
class ModuleRender;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleCamera;
class ModuleProgram;
class ModuleEditor;
class ModuleScene;
class ModuleDebugDraw;
class ModuleFileSystem;
class ModuleResourceManager;
class ModuleTime;
class ModuleSpacePartitioning;
class ModuleUI;
class ModuleFontLoader;
class ModuleScript;
class ModuleDevelopmentBuildDebug;
class ModuleParticles;
class ModuleAudioManager;
class ModuleNavigation;
class ModuleCollisions;

class Application
{
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

public:
	ModuleRender* renderer = nullptr;
	ModuleWindow* window = nullptr;
	ModuleTextures* textures = nullptr;
	ModuleInput* input = nullptr;
    ModuleCamera* camera = nullptr;
	ModuleProgram* program = nullptr;
	ModuleEditor* editor = nullptr;
	ModuleScene* scene = nullptr;
	ModuleDebugDraw* debug = nullptr;
	ModuleFileSystem* fsystem = nullptr;
	ModuleResourceManager* resManager = nullptr;
	ModuleTime* time = nullptr;
	ModuleSpacePartitioning* spacePartitioning = nullptr;
	ModuleUI* ui = nullptr;
	ModuleFontLoader* fontLoader = nullptr;
	ModuleScript* scripting = nullptr;
	ModuleDevelopmentBuildDebug* developDebug = nullptr;
	ModuleParticles* particles = nullptr;
	ModuleAudioManager* audioManager = nullptr;
	ModuleNavigation* navigation = nullptr;
	ModuleCollisions* collisions = nullptr;

private:
	std::list<Module*> modules;

};

extern Application* App;

#endif //__Application_h__