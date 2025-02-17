#include "ProjectileScript.h"

#include "Application.h"
#include "ModuleTime.h"
#include "ModuleScene.h"

#include "GameObject.h"
#include "ComponentAudioSource.h"
#include "ComponentTransform.h"
#include "ComponentBoxTrigger.h"
#include "ComponentRenderer.h"
#include "ComponentTrail.h"

#include "soloud.h"
#include "soloud_wav.h"
#include "soloud_wavstream.h"

#include "PlayerMovement/PlayerMovement.h"

#include "imgui.h"
#include "JSON.h"

ProjectileScript_API Script* CreateScript()
{
	ProjectileScript* instance = new ProjectileScript;
	return instance;
}

void ProjectileScript::Start()
{
	transform = gameobject->transform;
	if (transform == nullptr)
	{
		LOG("The transform of the GameObject %s couldn't be found. \n", gameobject->name.c_str());
	}

	boxTrigger = gameobject->GetComponent<ComponentBoxTrigger>();
	if (boxTrigger == nullptr)
	{
		LOG("The GameObject %s doesn't have a boxTrigger component attached. \n", gameobject->name.c_str());
	}

	//We need this if we want the projectile to spin
	meshGO = App->scene->FindGameObjectByTag("ProjectileMesh");
	if(meshGO!= nullptr)
	{
		LOG("No mesh found for the projectile");
	}
	
	trail = (ComponentTrail*)gameobject->GetComponentInChildren(ComponentType::Trail);

	// Player
	player = App->scene->FindGameObjectByName(playerName.c_str());
	if (player == nullptr)
	{
		LOG("NO PLAYER FOUND");
	}
	else
	{
		playerScript = player->GetComponent<PlayerMovement>();
		if (playerScript == nullptr)
		{
			LOG("The GameObject %s doesn't have a Player Movement component attached. \n", gameobject->name);
		}
	}
	impactSound = gameobject->GetComponent<ComponentAudioSource>();
}

void ProjectileScript::Update()
{
	if (shooted)
	{
		float deltaTime = App->time->gameDeltaTime;
		activeTime += deltaTime;

		math::float3 movement = direction * -speed * deltaTime;
		transform->SetPosition(transform->GetPosition() + movement);

		if (rotate)
		{
			//make it spin boi, we'll choose the axis and make it spin
			math::Quat rotationDelta = math::Quat::identity;
			if (rotationX)
			{
				rotationDelta.x +=  rotationSpeed * deltaTime;
			}
			if (rotationY)
			{
				rotationDelta.y += rotationSpeed * deltaTime;
			}
			if (rotationZ)
			{
				rotationDelta.z += rotationSpeed * deltaTime;
			}
			rotationDelta.Normalize();

			meshGO->transform->SetRotation(meshGO->transform->GetRotation()*rotationDelta);
		}

		if (activeTime > lifeTime)
		{
			gameobject->SetActive(false);
			shooted = false;
			activeTime = 0.0f;
		}
	}

}

void ProjectileScript::CleanTrailFX()
{
	if (trail != nullptr)
	{
		trail->CleanTrail();
	}
}

void ProjectileScript::OnTriggerEnter(GameObject* go)
{
	if (player != nullptr && go == player)
	{
		if(playerScript != nullptr)
			playerScript->Damage(damage);

		gameobject->SetActive(false);

		exploded = true;
		shooted = false;
		if (impactSound != nullptr)
		{
			impactSound->Play();
		}
		activeTime = 0.0f;
	}
}

void ProjectileScript::Expose(ImGuiContext* context)
{
	ImGui::Checkbox("Rotation", &rotate);

	if (rotate)
	{
		ImGui::InputFloat("rotation Speed", &rotationSpeed);
		ImGui::Checkbox("Axis X", &rotationX); ImGui::SameLine();
		ImGui::Checkbox("Axis Y", &rotationY); ImGui::SameLine();
		ImGui::Checkbox("Axis Z", &rotationZ);
	}

	ImGui::InputFloat("Projectile speed", &speed);
	ImGui::InputFloat("Life time", &lifeTime);

	char* targetName = new char[64];
	strcpy_s(targetName, strlen(playerName.c_str()) + 1, playerName.c_str());
	ImGui::InputText("Player Name", targetName, 64);
	playerName = targetName;
	delete[] targetName;
}

void ProjectileScript::Serialize(JSON_value* json) const
{
	json->AddFloat("speed", speed);
	json->AddFloat("lifeTime", lifeTime);
	json->AddString("playerName", playerName.c_str());
	json->AddUint("rotate", rotate);
	json->AddUint("rotateX", rotationX);
	json->AddUint("rotateY", rotationY);
	json->AddUint("rotateZ", rotationZ);
	json->AddFloat("rotatingSpeed", rotationSpeed);
}

void ProjectileScript::DeSerialize(JSON_value* json)
{
	speed = json->GetFloat("speed");
	lifeTime = json->GetFloat("lifeTime");
	playerName = json->GetString("playerName");
	rotate = json->GetUint("rotate", 0);
	rotationX = json->GetUint("rotateX", 0);
	rotationY = json->GetUint("rotateY", 0);
	rotationZ = json->GetUint("rotateZ", 0);
	rotationSpeed = json->GetFloat("rotatingSpeed", 0.07f);
}

