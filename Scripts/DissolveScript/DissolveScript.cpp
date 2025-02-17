#include "Application.h"
#include "ModuleTime.h"

#include "GameObject.h"
#include "ComponentRenderer.h"

#include "DissolveScript.h"

#include "imgui.h"
#include "JSON.h"	

DissolveScript_API Script* CreateScript()
{
	DissolveScript* instance = new DissolveScript;
	return instance;
}

void DissolveScript::Start()
{
	myRender = gameobject->GetComponent<ComponentRenderer>();
	assert(myRender != nullptr);

	myRender->dissolveAmount = initialDissolveAmount;
	dissolveDifference = finalDissolveAmount - initialDissolveAmount;
}

void DissolveScript::Update()
{
	if (!finished)
	{
		if (myRender->dissolveAmount >= 0.0f)
		{
			myRender->dissolveAmount -= App->time->gameDeltaTime * dissolveSpeed;
		}
		else
		{
			finished = true;
			myRender->dissolveAmount = 0.0f;
		}
	}
}

void DissolveScript::Expose(ImGuiContext * context)
{
	ImGui::DragFloat("Dissolve speed", &dissolveSpeed);
	ImGui::DragFloat("Initial dissolve amount", &initialDissolveAmount);
	ImGui::DragFloat("Final dissolve amount", &finalDissolveAmount);
}

void DissolveScript::Serialize(JSON_value * json) const
{
	json->AddFloat("dissolveSpeed", dissolveSpeed);
	json->AddFloat("initialDissolveAmount", initialDissolveAmount);
	json->AddFloat("finalDissolveAmount", finalDissolveAmount);
}

void DissolveScript::DeSerialize(JSON_value * json)
{
	dissolveSpeed = json->GetFloat("dissolveSpeed");
	initialDissolveAmount = json->GetFloat("initialDissolveAmount");
	finalDissolveAmount = json->GetFloat("finalDissolveAmount");
}
