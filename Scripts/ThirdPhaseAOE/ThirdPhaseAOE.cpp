#include "Application.h"
#include "ModuleScene.h"
#include "ModuleTime.h"

#include "GameObject.h"
#include "ComponentRenderer.h"
#include "ComponentBoxTrigger.h"
#include "ComponentTransform.h"

#include "PlayerMovement/PlayerMovement.h"
#include "CameraController/CameraController.h"
#include "BossBehaviourScript/BossBehaviourScript.h"

#include "ThirdPhaseAOE.h"

#include "Math/Quat.h"
#include "Math/float3.h"
#include "imgui.h"
#include "JSON.h"

#define THIRDAOE "ThirdPhaseAOEPrefab"

ThirdPhaseAOE_API Script* CreateScript()
{
	ThirdPhaseAOE* instance = new ThirdPhaseAOE;
	return instance;
}

void ThirdPhaseAOE::Awake()
{
	playerGO = App->scene->FindGameObjectByTag("Player", gameobject->parent);
	if (playerGO == nullptr)
	{
		LOG("The AOE didnt find the playerGO!");
	}
	else
	{
		playerScript = playerGO->GetComponent<PlayerMovement>();
		if (playerScript == nullptr)
		{
			LOG("The AOE didnt find the playerScript!");
		}
	}

	bossGO = App->scene->FindGameObjectByTag("Boss", App->scene->root);
	if (bossGO == nullptr)
	{
		LOG("The AOE didnt find the bossGO!");
	}
	else
	{
		BossScript = bossGO->GetComponent<BossBehaviourScript>();
		if (BossScript == nullptr)
		{
			LOG("The AOE didnt find the BossScript!");
		}
	}

	playerCamera = App->scene->FindGameObjectByName("PlayerCamera");
	if (playerCamera == nullptr)
	{
		LOG("Player camera not found");
	}
	else
	{
		cameraScript = playerCamera->GetComponent<CameraController>();
	}
}

void ThirdPhaseAOE::Start()
{
	prepParticlesFirstAOEGO = App->scene->FindGameObjectByTag("PrepParticlesFirst", gameobject);
	prepParticlesSecondAOEGO = App->scene->FindGameObjectByTag("PrepParticlesSecond", gameobject);
	smashParticlesFirst = App->scene->FindGameObjectByTag("SmashParticlesFirst", gameobject);
	hitParticlesFirstAOEGO = App->scene->FindGameObjectByTag("hitParticlesFirst", gameobject);

	rockParticlesFirstAOEGO = App->scene->FindGameObjectByTag("rockParticlesFirst", gameobject);

	boxTriggerFirstAOEGO = App->scene->FindGameObjectByTag("boxTriggerFirst", gameobject);
	if (boxTriggerFirstAOEGO != nullptr)
	{
		boxTriggerComponentFirst = boxTriggerFirstAOEGO->GetComponent<ComponentBoxTrigger>();
	}

	boxTriggerSecondAOEGO = App->scene->FindGameObjectByTag("boxTriggerSecond", gameobject);
	if (boxTriggerSecondAOEGO != nullptr)
	{
		boxTriggerComponentSecond = boxTriggerSecondAOEGO->GetComponent<ComponentBoxTrigger>();
	}

	//deactivate evertyhing else
	hitParticlesFirstAOEGO->SetActive(false);
	boxTriggerFirstAOEGO->SetActive(false);
	boxTriggerSecondAOEGO->SetActive(false);
	boxTriggerComponentFirst->Enable(false);
	boxTriggerComponentSecond->Enable(false);
	rockParticlesFirstAOEGO->SetActive(false);
	smashParticlesFirst->SetActive(false);
}

void ThirdPhaseAOE::Update()
{


	if (!particlesSet)
	{
		if (aoeType == 0)
		{
			//Activate the warning particles for the first AOE
			prepParticlesFirstAOEGO->SetActive(true);
			prepParticlesSecondAOEGO->SetActive(false);
		}
		else
		{
			//Activate the particles for the second AOE
			prepParticlesSecondAOEGO->SetActive(true);
			prepParticlesFirstAOEGO->SetActive(false);
		}

		particlesSet = true;
	}

	switch (aoeType)
	{
	case 0:
		HandleFirstAOE();
		break;
	case 1:
		HandleReplicas();
		break;
	}
}

void ThirdPhaseAOE::Expose(ImGuiContext * context)
{
	ImGui::InputInt("Type: ", &aoeType);
	ImGui::DragFloat("First explosion delay", &timeToExplodeFirstAOE);
	ImGui::DragFloat("First explosion finish", &timeToFinishFirstAOE);
	ImGui::DragFloat("Second spawn delay", &timeToSpawnRepetition);
	ImGui::DragFloat("Second explosion finish", &timeToFinishRepetition);
}

void ThirdPhaseAOE::OnTriggerEnter(GameObject * go)
{
	if (go == playerGO && !hasDamaged)
	{
		switch (aoeType)
		{
			case 0:
				playerScript->Damage(damageFirst);
				break;
			case 1:
				playerScript->Damage(damageSecond);
				break;
		}
		hasDamaged = true;
	}
}

void ThirdPhaseAOE::Serialize(JSON_value * json) const
{
	json->AddFloat("timeToExplodeFirstAOE", timeToExplodeFirstAOE);
	json->AddFloat("timeToFinishFirstAOE", timeToFinishFirstAOE);
	json->AddFloat("timeToSpawnRepetition", timeToSpawnRepetition);
	json->AddFloat("timeToFinishRepetition", timeToFinishRepetition);
	json->AddFloat("damageFirst", damageFirst);
	json->AddFloat("damageSecond", damageSecond);
}

void ThirdPhaseAOE::DeSerialize(JSON_value * json)
{
	timeToExplodeFirstAOE = json->GetFloat("timeToExplodeFirstAOE");
	timeToFinishFirstAOE = json->GetFloat("timeToFinishFirstAOE");

	timeToSpawnRepetition = json->GetFloat("timeToSpawnRepetition");
	timeToFinishRepetition = json->GetFloat("timeToFinishRepetition");

	damageSecond = json->GetFloat("damageSecond");
	damageFirst = json->GetFloat("damageFirst");
}

void ThirdPhaseAOE::HandleFirstAOE()
{
	if (BossScript->IsBossDead())
	{
		gameobject->deleteFlag = true;
	}

	if (firstAOEtimer > timeToExplodeFirstAOE && !firstExploded)
	{
		//Deactivate warning particles and activate hit ones
		prepParticlesFirstAOEGO->SetActive(false);
		smashParticlesFirst->GetComponent<ComponentRenderer>()->ResetAnimation();
		smashParticlesFirst->SetActive(true);
		rockParticlesFirstAOEGO->SetActive(true);
		hitParticlesFirstAOEGO->SetActive(true);
		boxTriggerFirstAOEGO->SetActive(true);
		boxTriggerComponentFirst->Enable(true);

		//Also spawn the five rows
		float randX = rand() % 72;
		math::float3 lastPosition = gameobject->transform->position;

		for (int i = 0; i < 5; ++i)
		{
			float alpha =  ((randX + i * 72.0f) * 2.0f * 3.1415f) / 360.0f;
			math::float3 newPosition = lastPosition + math::float3(100.f * sin(alpha),0,100.f* cos(alpha));
			math::float3 direction = newPosition - lastPosition;

			math::Quat newRotation = math::Quat::identity;
			
			newRotation = newRotation.LookAt(math::float3::unitZ, direction.Normalized(), math::float3::unitY, math::float3::unitY);

			GameObject* newSpawn = App->scene->Spawn(THIRDAOE, newPosition, newRotation);
			newSpawn->GetComponent<ThirdPhaseAOE>()->aoeType = 1;
		}

		cameraScript->Shake(0.5f, 70.0f, 0.2f, 0.2f, false);

		firstExploded = true;
	}
	
	

	if (firstAOEtimer > timeToFinishFirstAOE)
	{
		//Delete the gameobject
		gameobject->deleteFlag = true;
	}

	firstAOEtimer += App->time->gameDeltaTime;
}

void ThirdPhaseAOE::HandleReplicas()
{

	if (!particlesReplica)
	{
	//Deactivate warning particles and activate hit ones
	boxTriggerSecondAOEGO->SetActive(true);
	boxTriggerComponentSecond->Enable(true);
	particlesReplica = true;
	//Also spawn the following row
	}

	if (!replicaSpawned && secondAOEtimer > timeToSpawnRepetition)
	{
		if (repetitions > 0)
		{
			math::float3 nextPosition = gameobject->transform->position + 200.0f * gameobject->transform->front.Normalized();
			GameObject* nextSpawn = App->scene->Spawn(THIRDAOE, nextPosition, gameobject->transform->rotation);
			nextSpawn->GetComponent<ThirdPhaseAOE>()->repetitions = repetitions - 1;
			nextSpawn->GetComponent<ThirdPhaseAOE>()->aoeType = 1;
		}
		replicaSpawned = true;
	}

	if (secondAOEtimer > timeToFinishRepetition)
	{
		//Delete the gameobject
		gameobject->deleteFlag = true;
	}
	secondAOEtimer += App->time->gameDeltaTime;
}

