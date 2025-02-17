#include "BuriedEnemyAIScript.h"
#include "ProjectileScript.h"

#include "EnemyControllerScript.h"
#include "EnemyStateChase.h"
#include "EnemyStateAttack.h"
#include "EnemyStateCooldown.h"
#include "EnemyStateDeath.h"
#include "EnemyStateHidden.h"
#include "EnemyStateHide.h"
#include "EnemyStateShowUp.h"
#include "EnemyStateRelocate.h"
#include "EnemyStateReturnToStart.h"

#include "Application.h"
#include "ModuleScene.h"
#include "ModuleTime.h"

#include "GameObject.h"
#include "ComponentAudioSource.h"
#include "ComponentAnimation.h"
#include "ComponentRenderer.h"
#include "ComponentParticles.h"

#include "JSON.h"

#define PROJECTILE "BuriedProjectile"
#define BONE_IMPACT "BoneImpactFX"


BuriedEnemyAIScript_API Script* CreateScript()
{
	BuriedEnemyAIScript* instance = new BuriedEnemyAIScript;
	return instance;
}

void BuriedEnemyAIScript::Awake()
{
	// Look for Enemy Controller Script of the enemy
	enemyController = gameobject->GetComponent<EnemyControllerScript>();
	if (enemyController == nullptr)
	{
		LOG("The GameObject %s has no Enemy Controller Script component attached \n", gameobject->name);
	}
	boneImpactFX = App->scene->FindGameObjectByName(BONE_IMPACT);
	audioEnemy = App->scene->FindGameObjectByName("Audio", gameobject);
	GameObject* audioHitGO = App->scene->FindGameObjectByName("Hit", audioEnemy);
	if (audioHitGO != nullptr)
	{
		audioHit = audioHitGO->GetComponent<ComponentAudioSource>();
	}
	GameObject* audioFootGO = App->scene->FindGameObjectByName("FootSteps", audioEnemy);
	if (audioFootGO != nullptr)
	{
		audioFoot = audioFootGO->GetComponent<ComponentAudioSource>();
	}
	GameObject* audioBuryGO = App->scene->FindGameObjectByName("Bury", audioEnemy);
	if (audioBuryGO != nullptr)
	{
		audioBury = audioBuryGO->GetComponent<ComponentAudioSource>();
	}
	GameObject* audioBoneHitGO = App->scene->FindGameObjectByName("BoneHit", audioEnemy);
	if (audioBoneHitGO != nullptr)
	{
		audioBoneHit = audioBoneHitGO->GetComponent<ComponentAudioSource>();
	}
	GameObject* audioDeathFX1GO = App->scene->FindGameObjectByName("DeathFX1", audioEnemy);
	if (audioDeathFX1GO != nullptr)
	{
		audioDeathFX1 = audioDeathFX1GO->GetComponent<ComponentAudioSource>();
	}
	GameObject* audioDeathFX2GO = App->scene->FindGameObjectByName("DeathFX2", audioEnemy);
	if (audioDeathFX2GO != nullptr)
	{
		audioDeathFX2 = audioDeathFX2GO->GetComponent<ComponentAudioSource>();
	}
	if (enemyController == nullptr)
	{
		LOG("The GameObject %s has no Enemy Controller Script component attached \n", gameobject->name);
	}
}

void BuriedEnemyAIScript::Start()
{
	//Create states
	enemyStates.reserve(9);
	enemyStates.push_back(attack = new EnemyStateAttack(this));
	enemyStates.push_back(cooldown = new EnemyStateCooldown(this));
	enemyStates.push_back(hide = new EnemyStateHide(this));
	enemyStates.push_back(hidden = new EnemyStateHidden(this));
	enemyStates.push_back(showUp = new EnemyStateShowUp(this));
	enemyStates.push_back(returnToStart = new EnemyStateReturnToStart(this));
	enemyStates.push_back(chase = new EnemyStateChase(this));
	enemyStates.push_back(death = new EnemyStateDeath(this));
	enemyStates.push_back(relocate = new EnemyStateRelocate(this));

	startPosition = enemyController->GetPosition();

	currentState = hidden;

	enemyController->GetMainRenderer()->Enable(false);

	projectileGO = InstantiateProjectile(PROJECTILE);
	if (projectileGO == nullptr)
	{
		LOG("Enemy projectile couldn't be instantiated");
	}
	else
	{
		projectileScript = projectileGO->GetComponent<ProjectileScript>();
	}

	dustParticlesGO = App->scene->FindGameObjectByTag("DustParticles", gameobject);
	
	if (dustParticlesGO == nullptr)
	{
		LOG("DustParticles couldn't be found. \n");
	}
	else
	{
		dustParticlesGO->SetActive(false);
	}
	
	candleGO = App->scene->FindGameObjectByTag("Candle", gameobject);

	if (candleGO == nullptr)
	{
		LOG("CandleGO couldn't be found. \n");
	}
	else
	{
		candleStartZ = candleGO->transform->position.z;
		candleGO->transform->position.z -= candleOffset;
	}

	handBoneGO = App->scene->FindGameObjectByTag("buriedHandBone", gameobject);
	if (candleGO == nullptr)
	{
		LOG("Enemy hand bone couldn't be found. \n");
	}
	else
	{
		handBoneGO->SetActive(false);
	}
}

void BuriedEnemyAIScript::Update()
{
	ProjectileImpactFX();

	EnemyState* previous = currentState;

	UpdateTeleportCD();


	if (enemyController->GetHealth() <= 0 && currentState != death)
	{
		currentState = (EnemyState*)death;
	}

	currentState->UpdateTimer();
	currentState->HandleIA();

	CheckStates(previous);

	currentState->Update();
	

}

void BuriedEnemyAIScript::Expose(ImGuiContext * context)
{

	if (currentState == hidden)				ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: hidden");
	else if (currentState == chase)			ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Chase");
	else if (currentState == returnToStart)	ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Return");
	else if (currentState == attack)		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Attack");
	else if (currentState == cooldown)		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Cooldown");
	else if (currentState == death)			ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Dead");
	else if (currentState == relocate)		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: relocating");
	else if (currentState == hide)			ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: hide");
	else if (currentState == showUp)		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: ShowUp");


	ImGui::InputFloat("attack cooldown duration: ", &cooldownDuration);
	ImGui::InputFloat("activation distance: ", &activationDistance);
	ImGui::InputFloat("disengage distance: ", &disengageDistance);
	ImGui::InputFloat("max attack range: ", &maxAttackRange);
	ImGui::InputFloat("min attack range: ", &minAttackRange);
	ImGui::InputFloat("Projectile Delay: ", &projectileDelay);
	ImGui::InputFloat("teleport cooldown: ", &teleportCooldown);
	ImGui::InputFloat("run speed: ", &runSpeed);

}

void BuriedEnemyAIScript::Serialize(JSON_value * json) const
{
	json->AddFloat("cooldownDuration", cooldownDuration);
	json->AddFloat("activationDistance", activationDistance);
	json->AddFloat("disengageDistance", disengageDistance);
	json->AddFloat("maxAttackRange", maxAttackRange);
	json->AddFloat("minAttackRange", minAttackRange);
	json->AddFloat("teleportCooldown", teleportCooldown);
	json->AddFloat("runSpeed", runSpeed);
	json->AddFloat("projectileDelay", projectileDelay);
}

void BuriedEnemyAIScript::DeSerialize(JSON_value * json)
{
	cooldownDuration = json->GetFloat("cooldownDuration");
	activationDistance = json->GetFloat("activationDistance");
	disengageDistance = json->GetFloat("disengageDistance");
	maxAttackRange = json->GetFloat("maxAttackRange");
	minAttackRange = json->GetFloat("minAttackRange");
	teleportCooldown = json->GetFloat("teleportCooldown");
	runSpeed = json->GetFloat("runSpeed");
	projectileDelay = json->GetFloat("projectileDelay", projectileDelay);
}

void BuriedEnemyAIScript::CheckStates(EnemyState* previous)
{
	if (previous != currentState)
	{
		previous->ResetTimer();

		previous->Exit();
		currentState->Enter();

		if (enemyController->anim != nullptr)
		{
			enemyController->anim->SendTriggerToStateMachine(currentState->trigger.c_str());
		}

		currentState->duration = enemyController->anim->GetDurationFromClip();
	}
}

void BuriedEnemyAIScript::StartTeleportCD()
{
	teleportTimer = 0.0f;
	teleportAvailable = false;
}

void BuriedEnemyAIScript::UpdateTeleportCD()
{
	if (!teleportAvailable)
	{
		teleportTimer += App->time->gameDeltaTime;
		if (teleportTimer > teleportCooldown)
		{
			teleportAvailable = true;
		}
	}
}

GameObject* BuriedEnemyAIScript::InstantiateProjectile(const char* projectilePrefab)
{
	return App->scene->Spawn(projectilePrefab);
}

void BuriedEnemyAIScript::ProjectileImpactFX()
{
	if (boneImpactFX == nullptr) return;

	if (boneImpactFX->isActive())
	{
		explosionTimer += App->time->gameDeltaTime;
		if (explosionTimer > explosionDuration)
		{
			boneImpactFX->SetActive(false);
		}
	}
	else
	{
		explosionTimer = 0.0f;
	}
	if (projectileScript->exploded)
	{
		if (audioBoneHit != nullptr)
		{
			audioBoneHit->Play();
		}
		projectileScript->exploded = false;
		boneImpactFX->transform->SetGlobalPosition(projectileGO->transform->GetGlobalPosition());
		boneImpactFX->SetActive(true);
	}
}

float BuriedEnemyAIScript::randomOffset(float max)
{
	float random = rand() % (int)(max * 100);
	return (float)random / 100.f;
}