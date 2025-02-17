#include "IntroScript.h"

#include "Application.h"
#include "ModuleTime.h"
#include "ModuleScene.h"

#include "ComponentTransform.h"
#include "BaseScript.h"
#include "GameObject.h"

#include "JSON.h"
#include "imgui.h"

IntroScript_API Script* CreateScript()
{
	IntroScript* instance = new IntroScript;
	return instance;
}

void IntroScript::Expose(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ImGui::DragFloat("Speed", &speed, 1.0f, 0.0f, 100.0f);
}

void IntroScript::Start()
{
	initialPosition = gameobject->transform->GetPosition();
	finalPosition = App->scene->FindGameObjectByName("Player")->transform->GetGlobalPosition() + math::float3(0.f,800.0f,0.f);
	distanceNormalized = (finalPosition - initialPosition).Normalized();
}

void IntroScript::Update()
{
	if (!introDone)
	{
		math::float3 position = gameobject->transform->GetPosition() + (distanceNormalized * speed * App->time->gameDeltaTime);
		gameobject->transform->SetPosition(position);

		if (position.DistanceSq(finalPosition) < 200.f)
		{
			gameobject->transform->SetPosition(finalPosition);
			introDone = true;
		}
	}
}

void IntroScript::Serialize(JSON_value* json) const
{
	assert(json != nullptr);
	json->AddFloat("speed", speed);
}

void IntroScript::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);
	speed = json->GetFloat("speed");
}
