#include "SceneEffects.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleTime.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentLight.h"
#include "ComponentAnimation.h"
#include "ComponentAudioSource.h"
#include "JSON.h"
#include "imgui.h"

SceneEffects_API Script* CreateScript()
{
	SceneEffects* instance = new SceneEffects;
	return instance;
}

void SceneEffects::Expose(ImGuiContext * context)
{
	if (ImGui::BeginCombo("Behaviours", item_current, 0)) 
	{
		for (int n = 0; n < IM_ARRAYSIZE(behaviours); n++)
		{
			bool is_selected = (item_current == behaviours[n]);
			if (ImGui::Selectable(behaviours[n], is_selected))
			{
				item_current = behaviours[n];
				behaviour = static_cast<Behaviours>((int)Behaviours::NONE + n);
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();   
		}
		ImGui::EndCombo();
	}
	switch (behaviour)
	{
	case Behaviours::BATS:
		ImGui::InputInt("Bat amount ", &batAmount);		
		break;
	case Behaviours::STORM:
		break;
	}
}

void SceneEffects::Start()
{
	thunderAudioObj = App->scene->FindGameObjectByTag("Thunder");
	switch (behaviour)
	{
	case Behaviours::BATS:
		for (GameObject* wayPoint : gameobject->children)
		{
			waypoints.emplace_back(wayPoint->transform->position);
		}
		for (int i = 0; i < batAmount; ++i)
		{
			Bat newBat;
			newBat.destination = waypoints[rand() % waypoints.size()];
			newBat.gameobject = App->scene->Spawn("BatPrefab", gameobject);
			assert(newBat.gameobject);
			bats.push_back(newBat);
		}

		break;
	case Behaviours::STORM:
		{
			for (GameObject* lightings : gameobject->children)
			{
				ComponentLight* newLight = (ComponentLight*)lightings->CreateComponent(ComponentType::Light, nullptr, true);
				assert(newLight);
				newLight->intensity = 2.f;
				newLight->color = math::float3(.5f, .5f, 1.f);
				newLight->lightType = LightType::DIRECTIONAL;
				lightingLights.emplace_back(newLight);
			}
			ComponentLight* directionLight = App->renderer->directionalLight;
			assert(directionLight);
			originalDirectionalLight = directionLight;
			break;
		}
	case Behaviours::SPIDER:
		for (GameObject* child : gameobject->children)
		{
			if (child->name != "Spider")
				spiderWaypoints.emplace_back(child->transform->position);
			else
				spider = child;
		}		
		spiderDestination = spiderWaypoints[rand() % spiderWaypoints.size()];
		assert(spider && spider->GetComponent<ComponentAnimation>());
		break;
	}

	
}

void SceneEffects::Update()
{
	switch (behaviour)
	{
	case Behaviours::BATS:
		for (Bat& bat : bats)
		{
			if (bat.gameobject->transform->GetGlobalPosition().Distance(bat.destination) < BAT_STOP_DISTANCE)
				bat.destination = waypoints[rand() % waypoints.size()];			

			math::float3 direction = (bat.destination - bat.gameobject->transform->position).Normalized();
			bat.gameobject->transform->position +=  direction * BAT_SPEED * App->time->gameDeltaTime;
			bat.gameobject->transform->SetRotation(Quat::LookAt(-math::float3::unitZ, direction, math::float3::unitY, math::float3::unitY));
		}
		break;
	case Behaviours::STORM:
		if (stormTimer > 0.f)
		{
			stormTimer -= App->time->gameDeltaTime;
			if (rand() % 100 < 30)
			{
				App->renderer->directionalLight = lightingLights[rand() % lightingLights.size()];
			}
			if (stormTimer <= 0.f)
			{
				App->renderer->directionalLight = originalDirectionalLight;
			}
		}
		else if (stormCooldownTimer > 0.f)
		{

			stormCooldownTimer -= App->time->gameDeltaTime;
		}
		else
		{
			//play audio
			if (thunderAudioObj)
			{
				ComponentAudioSource* aud = (ComponentAudioSource*)thunderAudioObj->GetComponent<ComponentAudioSource>();
				aud->Play();
			}
			stormTimer = float(rand() % ((int)stormDuration * 1000)) / 1000.f;
			stormCooldownTimer = float(rand() % ((int)stormCooldown * 1000)) / 1000.f;
			//in case random gave a value that is too small
			if (stormCooldownTimer < minStormCD)
			{
				stormCooldownTimer = minStormCD;
			}
		}
		break;
	case Behaviours::SPIDER:
		if (spiderTimer > 0.f)
		{
			spiderTimer -= App->time->gameDeltaTime;
			if (spiderTimer < 0.f)
			{
				spider->GetComponent<ComponentAnimation>()->SendTriggerToStateMachine("Walking");
			}
			break;
		}
		if (spider->transform->position.Distance(spiderDestination) < SPIDER_STOP_DISTANCE)
		{
			spiderTimer = float(rand() % 20) / 10.f;
			spiderDestination = spiderWaypoints[rand() % spiderWaypoints.size()];
			spider->GetComponent<ComponentAnimation>()->SendTriggerToStateMachine("IdleSpider");
		}

		math::float3 direction = (spiderDestination - spider->transform->position).Normalized();
		spider->transform->position += direction * SPIDER_SPEED * App->time->gameDeltaTime;		
		break;
	}
}

void SceneEffects::Serialize(JSON_value* json) const
{
	json->AddInt("behaviour", (int)behaviour);
	json->AddInt("batAmount", batAmount);
}

void SceneEffects::DeSerialize(JSON_value* json)
{
	int index = json->GetInt("behaviour", (int)behaviour);
	item_current = behaviours[index];
	behaviour = (Behaviours)index;

	batAmount = json->GetInt("batAmount", batAmount);
}
