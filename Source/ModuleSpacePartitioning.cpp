#include "ModuleSpacePartitioning.h"
#include "Application.h"

bool ModuleSpacePartitioning::Init(JSON * config)
{
	kDTree.Init();
	aabbTree.Init();
	aabbTreeLighting.Init();
	return true;
}

update_status ModuleSpacePartitioning::Update(float dt)
{		
	return UPDATE_CONTINUE;
}

bool ModuleSpacePartitioning::CleanUp()
{
	aabbTree.CleanUp();
	aabbTreeLighting.CleanUp();
	kDTree.CleanUp();
	return true;
}


