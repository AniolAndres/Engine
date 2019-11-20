#include "PanelBehaviourTree.h"

#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"

#include "BaseScript.h"

#include "../BehaviourTreeScript/BehaviourTreeScript.h"

#include "imgui.h"

PanelBehaviourTree::PanelBehaviourTree()
{
}


PanelBehaviourTree::~PanelBehaviourTree()
{
}

void PanelBehaviourTree::Draw()
{
	if (!ImGui::Begin("BehaviourTree", &enabled))
	{
		ImGui::End();
		return;
	}

	if (App->scene->selected != nullptr && App->scene->selected->GetComponent<BehaviourTreeScript>())
	{
		BehaviourTreeScript* btScript = App->scene->selected->GetComponent<BehaviourTreeScript>();
		if (btScript != nullptr)
		{
			LOG("BTWorking");
		}
	}

	ImGui::End();
}
