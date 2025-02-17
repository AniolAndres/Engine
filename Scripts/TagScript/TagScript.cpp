#include "TagScript.h"

#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"

TagScript_API Script* CreateScript()
{
	TagScript* instance = new TagScript;
	return instance;
}

void TagScript::Start()
{
	GameObject* enemy = App->scene->FindGameObjectByTag("Enemy");
	std::vector<GameObject*> items = App->scene->FindGameObjectsByTag("Item");
}

void TagScript::Update()
{
	math::float3 point;
	std::string test = "TESTING";
	GameObject* gameobject = nullptr;
	if (App->scene->Intersects("Enemy", false, point, &gameobject))
	{
		LOG("intersected with %s",gameobject->name.c_str());
		LOG("intersected with %s", test.c_str())
	}
}
