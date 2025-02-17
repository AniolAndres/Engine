#include "RangeEnemyAIScript.h"

#include "Application.h"
#include "ModuleTime.h"
#include "ModuleScene.h"

#include "GameObject.h"
#include "ComponentAudioSource.h"
#include "ComponentTransform.h"
#include "ComponentAnimation.h"
#include "ComponentBoxTrigger.h"

#include "EnemyControllerScript.h"
#include "ProjectileScript.h."

#include "EnemyState.h"
#include "EnemyStatePatrol.h"
#include "EnemyStateChase.h"
#include "EnemyStateFlee.h"
#include "EnemyStateReturnToStart.h"
#include "EnemyStateAttack.h"
#include "EnemyStateCooldown.h"
#include "EnemyStateDeath.h"

#include "imgui.h"
#include "JSON.h"
#include "debugdraw.h"

#define PROJECTILE "BandoleroProjectile"

RangeEnemyAIScript_API Script* CreateScript()
{
	RangeEnemyAIScript* instance = new RangeEnemyAIScript;
	return instance;
}

void RangeEnemyAIScript::Start()
{
	enemyStates.reserve(6);

	enemyStates.push_back(patrol = new EnemyStatePatrol(this));
	enemyStates.push_back(chase = new EnemyStateChase(this));
	enemyStates.push_back(flee = new EnemyStateFlee(this));
	enemyStates.push_back(returnToStart = new EnemyStateReturnToStart(this));
	enemyStates.push_back(attack = new EnemyStateAttack(this));
	enemyStates.push_back(cooldown = new EnemyStateCooldown(this));
	enemyStates.push_back(death = new EnemyStateDeath(this));

	currentState = patrol;

	// Look for Enemy Controller Script of the enemy
	enemyController = gameobject->GetComponent<EnemyControllerScript>();
	if (enemyController == nullptr)
	{
		LOG("The GameObject %s has no Enemy Controller Script component attached. \n", gameobject->name);
	}

	boxTrigger = (ComponentBoxTrigger*)gameobject->GetComponentInChildren(ComponentType::BoxTrigger);
	if (boxTrigger == nullptr)
	{
		LOG("No child of the GameObject %s has a boxTrigger component attached. \n", gameobject->name);
	}

	projectile1 = InstantiateProjectile(PROJECTILE);
	if (projectile1 == nullptr)
	{
		LOG("Enemy projectile couldn't be instantiated");
	}
	else
	{
		projectileScript1 = projectile1->GetComponent<ProjectileScript>();
		projectileScript1->damage = enemyController->GetDamage();
	}
	
	if (numberOfProjectiles > 1)
	{
		projectile2 = InstantiateProjectile(PROJECTILE);
		if (projectile2 == nullptr)
		{
			LOG("Enemy projectile couldn't be instantiated");
		}
		else
		{
			projectileScript2 = projectile2->GetComponent<ProjectileScript>();
			projectileScript2->damage = enemyController->GetDamage();
		}
	}

	if (numberOfProjectiles > 2)
	{
		projectile3 = InstantiateProjectile(PROJECTILE);
		if (projectile3 == nullptr)
		{
			LOG("Enemy projectile couldn't be instantiated");
		}
		else
		{
			projectileScript3 = projectile3->GetComponent<ProjectileScript>();
			projectileScript3->damage = enemyController->GetDamage();
		}
	}

	

	audioEnemy = App->scene->FindGameObjectByName("Audio", gameobject);
	GameObject* audioFootGO = App->scene->FindGameObjectByName("FootSteps", audioEnemy);
	if (audioFootGO != nullptr)
	{
		audioFoot = audioFootGO->GetComponent<ComponentAudioSource>();
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

	projectileExplosion = App->scene->FindGameObjectByName("ExplodeProjectileFX");

	startPosition = enemyController->GetPosition();
	currentState->Enter();
	LOG("Started range enemy AI script");
}

float RangeEnemyAIScript::randomOffset(float max)
{
	float random = rand() % (int)(max * 100);
	return (float)random / 100.f;
}

void RangeEnemyAIScript::Update()
{
	ProjectileImpactFX();

	EnemyState* previous = currentState;

	if (enemyController->GetHealth() <= 0 && currentState != death)
	{
		currentState = (EnemyState*)death;
	}

	if (drawDebug && currentState != death)
	{
		DrawDebug();
	}

	currentState->UpdateTimer();
	currentState->HandleIA();
	currentState->Update();

	// If previous and current are different the functions Exit() and Enter() are called
	CheckStates(previous, currentState);
}

void RangeEnemyAIScript::Expose(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ImGui::Separator();
	ImGui::Text("Enemy:");

	if (currentState == patrol)				ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Patrol");
	else if (currentState == chase)			ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Chase");
	else if (currentState == flee)			ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Flee");
	else if (currentState == returnToStart)	ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Return");
	else if (currentState == attack)		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Attack");
	else if (currentState == cooldown)		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Cooldown");
	else if (currentState == death)			ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Dead");

	ImGui::Checkbox("Draw Debug", &drawDebug);
	ImGui::Text("Patrol:");
	ImGui::InputFloat("Distance to activate", &activationDistance);

	ImGui::Text("Get In Position:");
	ImGui::InputFloat("Chase Speed", &runSpeed);
	ImGui::InputFloat("Max Attack Distance", &maxAttackDistance);
	ImGui::InputFloat("Min Attack Distance", &minAttackDistance);

	ImGui::Text("Return:");
	ImGui::InputFloat("Disengage Distance", &disengageDistance);
	ImGui::InputFloat("Return Speed", &returnSpeed);

	ImGui::Text("Attack:");
	ImGui::InputFloat("Attack Time", &attackDuration);
	ImGui::InputFloat("Attack Damage", &attackDamage);

	ImGui::SliderInt("N. of Projectiles", &numberOfProjectiles, 1, 3);

	ImGui::DragFloat("Projectile Delay1", &projectileDelay1, 0.01f, 0.0f, 3.f);

	if (numberOfProjectiles > 1)
		ImGui::DragFloat("Projectile Delay2", &projectileDelay2, 0.01f, 0.0f, 3.f);

	if (numberOfProjectiles > 2)
	ImGui::DragFloat("Projectile Delay3", &projectileDelay3, 0.01f, 0.0f, 3.f);


	ImGui::Text("Cooldown:");
	ImGui::InputFloat("Cooldown Time", &cooldownTime);
}

void RangeEnemyAIScript::Serialize(JSON_value* json) const
{
	assert(json != nullptr);

	//Wait variables
	json->AddFloat("activationDistance", activationDistance);

	// GetInPosition variables
	json->AddFloat("runSpeed", runSpeed);
	json->AddFloat("maxAttackDistance", maxAttackDistance);
	json->AddFloat("minAttackDistance", minAttackDistance);

	// Return variables
	json->AddFloat("returnSpeed", returnSpeed);
	json->AddFloat("disengageDistance", disengageDistance);

	// Attack variables
	json->AddFloat("attackDuration", attackDuration);
	json->AddFloat("attackDamage", attackDamage);
	json->AddInt("numberOfProjectiles", numberOfProjectiles);
	json->AddFloat("projectileDelay1", projectileDelay1);
	json->AddFloat("projectileDelay2", projectileDelay2);
	json->AddFloat("projectileDelay3", projectileDelay3);

	// Cooldown variables
	json->AddFloat("cooldownTime", cooldownTime);
}

void RangeEnemyAIScript::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);

	//Wait variables
	activationDistance = json->GetFloat("activationDistance");

	// GetInPosition variables
	runSpeed = json->GetFloat("runSpeed");
	maxAttackDistance = json->GetFloat("maxAttackDistance");
	minAttackDistance = json->GetFloat("minAttackDistance");

	// Return variables
	returnSpeed = json->GetFloat("returnSpeed");
	disengageDistance = json->GetFloat("disengageDistance");

	// Attack variables
	attackDuration = json->GetFloat("attackDuration");
	attackDamage = json->GetFloat("attackDamage");
	numberOfProjectiles = json->GetInt("numberOfProjectiles");
	projectileDelay1 = json->GetFloat("projectileDelay1");
	projectileDelay2 = json->GetFloat("projectileDelay2");
	projectileDelay3 = json->GetFloat("projectileDelay3");

	// Cooldown variables
	cooldownTime = json->GetFloat("cooldownTime");
}

void RangeEnemyAIScript::ProjectileImpactFX()
{
	if (projectileExplosion == nullptr) return;
	if (projectileExplosion->isActive())
	{
		explosionTimer += App->time->gameDeltaTime;
		if (explosionTimer > explosionDuration)
		{
			projectileExplosion->SetActive(false);
		}
	}
	else
	{
		explosionTimer = 0.0f;
	}

	if (projectileScript1->exploded)
	{
		projectileScript1->exploded = false;
		projectileExplosion->transform->SetGlobalPosition(projectile1->transform->GetGlobalPosition());
		projectileExplosion->SetActive(true);
	}
	else if (projectileScript2 != nullptr && projectileScript2->exploded)
	{
		projectileScript2->exploded = false;
		projectileExplosion->transform->SetGlobalPosition(projectile2->transform->GetGlobalPosition());
		projectileExplosion->SetActive(true);
	}
	else if (projectileScript3 != nullptr && projectileScript3->exploded)
	{
		projectileScript3->exploded = false;
		projectileExplosion->transform->SetGlobalPosition(projectile3->transform->GetGlobalPosition());
		projectileExplosion->SetActive(true);
	}
}

void RangeEnemyAIScript::CheckStates(EnemyState* previous, EnemyState* current)
{
	if (previous != current)
	{
		previous->ResetTimer();

		previous->Exit();

		if (enemyController->anim != nullptr)
		{
			enemyController->anim->SendTriggerToStateMachine(current->trigger.c_str());
			current->duration = enemyController->anim->GetDurationFromClip();
		}

		current->Enter();
	}
}

GameObject* RangeEnemyAIScript::InstantiateProjectile(const char* projectilePrefab)
{
	return App->scene->Spawn(projectilePrefab);
}

void RangeEnemyAIScript::DrawDebug() const
{
	if (currentState == patrol || currentState == returnToStart)
	{
		dd::circle(enemyController->GetPosition(), gameobject->transform->up, dd::colors::Red, activationDistance, 20.0f);
	}
	else
	{
		dd::circle(enemyController->GetPosition(), gameobject->transform->up, dd::colors::Purple, disengageDistance, 20.0f);
		dd::cone(enemyController->GetPlayerPosition(), enemyController->player->transform->up, dd::colors::Red, maxAttackDistance, minAttackDistance);
	}

	if (currentState != patrol)
	{
		dd::point(startPosition, dd::colors::Green, 10.0f);
		dd::arrow(startPosition, enemyController->GetPosition(), dd::colors::Green, 20.0f);
	}
}

void RangeEnemyAIScript::OnTriggerEnter(GameObject* go)
{
	if ((currentState == patrol || currentState == returnToStart) && go->tag.c_str() == "PlayerHitBoxAttack")
	{

		// Change state to getInPosition and update states
		EnemyState* previous = currentState;
		currentState = chase;
		CheckStates(previous, currentState);
	}
}