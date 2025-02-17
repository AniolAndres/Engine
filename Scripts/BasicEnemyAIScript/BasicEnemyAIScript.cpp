#include "BasicEnemyAIScript.h"

#include "Application.h"
#include "ModuleTime.h"
#include "ModuleScene.h"

#include "GameObject.h"
#include "ComponentAudioSource.h"
#include "ComponentTransform.h"
#include "ComponentAnimation.h"
#include "ComponentBoxTrigger.h"

#include "EnemyControllerScript.h"
#include "EnemyState.h"
#include "EnemyStatePatrol.h"
#include "EnemyStateChase.h"
#include "EnemyStateReturnToStart.h"
#include "EnemyStateAttack.h"
#include "EnemyStateCooldown.h"
#include "EnemyStateFlee.h"
#include "EnemyStateDeath.h"

#include "imgui.h"
#include "JSON.h"
#include "debugdraw.h"

BasicEnemyAIScript_API Script* CreateScript()
{
	BasicEnemyAIScript* instance = new BasicEnemyAIScript;
	return instance;
}

void BasicEnemyAIScript::Awake()
{
	// Look for Enemy Controller Script of the enemy
	enemyController = gameobject->GetComponent<EnemyControllerScript>();
	audioEnemy = App->scene->FindGameObjectByName("Audio", gameobject);
	GameObject * audioHitGO = App->scene->FindGameObjectByName("Hit", audioEnemy);
	if (audioHitGO != nullptr)
	{
		audioHit = audioHitGO->GetComponent<ComponentAudioSource>();
	}
	GameObject* audioFootGO = App->scene->FindGameObjectByName("FootSteps", audioEnemy);
	if (audioFootGO != nullptr)
	{
		audioFoot = audioFootGO->GetComponent<ComponentAudioSource>();
	}
	GameObject* audioDeathFX1GO = App->scene->FindGameObjectByName("DeathFX1", audioEnemy);
	if (audioDeathFX1GO != nullptr)
	{
		audioDeathFX1= audioDeathFX1GO->GetComponent<ComponentAudioSource>();
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

void BasicEnemyAIScript::Start()
{
	enemyStates.reserve(6);

	enemyStates.push_back(patrol = new EnemyStatePatrol(this));
	enemyStates.push_back(chase = new EnemyStateChase(this));
	enemyStates.push_back(returnToStart = new EnemyStateReturnToStart(this));
	enemyStates.push_back(attack = new EnemyStateAttack(this));
	enemyStates.push_back(cooldown = new EnemyStateCooldown(this));
	enemyStates.push_back(death = new EnemyStateDeath(this));
	enemyStates.push_back(flee = new EnemyStateFlee(this));

	currentState = patrol;


	startPosition = enemyController->GetPosition();

	LOG("Started basic enemy AI script");
}

void BasicEnemyAIScript::Update()
{
	EnemyState* previous = currentState;

	if (enemyController->GetHealth() <= 0 && currentState != death)
	{
		enemyController->attackBoxTrigger->Enable(false);
		currentState = (EnemyState*)death;
	}
	
	if(drawDebug && currentState != death)
	{
		DrawDebug();
	}

	currentState->UpdateTimer();
	currentState->HandleIA();
	currentState->Update();

	// If previous and current are different the functions Exit() and Enter() are called
	CheckStates(previous, currentState);
}

void BasicEnemyAIScript::Expose(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ImGui::Separator();
	ImGui::Text("Enemy:");

	if (currentState == patrol)				ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Patrol");
	else if(currentState == chase)			ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Chase");
	else if (currentState == returnToStart)	ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Return");
	else if (currentState == attack)		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Attack");
	else if (currentState == cooldown)		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Cooldown");
	else if (currentState == death)			ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Dead");
	else if (currentState == flee)			ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Flee");

	ImGui::Checkbox("Draw Debug", &drawDebug);
	ImGui::Text("Patrol:");
	ImGui::InputFloat("Distance to activate", &activationDistance);
	ImGui::Text("Chase:");
	ImGui::InputFloat("Chase Speed", &chaseSpeed);
	ImGui::Text("Return:");
	ImGui::InputFloat("Return Distance", &returnDistance);
	ImGui::InputFloat("Return Speed", &returnSpeed);
	ImGui::Text("Attack:");
	ImGui::InputFloat("Attack Time", &attackDuration);
	ImGui::InputFloat("Attack Damage", &attackDamage);
	ImGui::InputFloat("Attack Range", &attackRange);
	ImGui::InputFloat("Attack Delay", &attackDelay);
	ImGui::Text("Cooldown:");
	ImGui::InputFloat("Cooldown Time", &cooldownTime);

}

void BasicEnemyAIScript::Serialize(JSON_value* json) const
{
	assert(json != nullptr);

	//Wait variables
	json->AddFloat("activationDistance", activationDistance);

	// Chase variables
	json->AddFloat("chaseSpeed", chaseSpeed);

	// Return variables
	json->AddFloat("returnSpeed", returnSpeed);
	json->AddFloat("returnDistance", returnDistance);

	// Attack variables
	json->AddFloat("attackDuration", attackDuration);
	json->AddFloat("attackDamage", attackDamage);
	json->AddFloat("attackRange", attackRange);
	json->AddFloat("attackDelay", attackDelay);

	// Cooldown variables
	json->AddFloat("cooldownTime", cooldownTime);
}

void BasicEnemyAIScript::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);

	//Wait variables
	activationDistance = json->GetFloat("activationDistance");

	// Chase variables
	chaseSpeed = json->GetFloat("chaseSpeed");

	// Return variables
	returnSpeed = json->GetFloat("returnSpeed");
	returnDistance = json->GetFloat("returnDistance");
	
	// Attack variables
	attackDuration = json->GetFloat("attackDuration");
	attackDamage = json->GetFloat("attackDamage");
	attackRange = json->GetFloat("attackRange", attackRange);
	attackDelay = json->GetFloat("attackDelay", attackDelay);


	// Cooldown variables
	cooldownTime = json->GetFloat("cooldownTime");
}

float BasicEnemyAIScript::randomOffset(float max)
{
	float random = rand() % (int)(max * 100);
	return (float)random / 100.f;
}


void BasicEnemyAIScript::CheckStates(EnemyState* previous, EnemyState* current)
{
	if (previous != current)
	{
		previous->ResetTimer();

		previous->Exit();
		current->Enter();

		if (enemyController->anim != nullptr)
		{
			enemyController->anim->SendTriggerToStateMachine(current->trigger.c_str());
		}

		current->duration = enemyController->anim->GetDurationFromClip();
	}
}

void BasicEnemyAIScript::DrawDebug() const
{
	if (currentState == patrol || currentState == returnToStart)
	{
		dd::circle(enemyController->GetPosition(), gameobject->transform->up, dd::colors::Red, activationDistance, 20.0f);
	}
	else
	{
		dd::circle(enemyController->GetPosition(), gameobject->transform->up, dd::colors::Purple, returnDistance, 20.0f);
	}

	if (currentState != patrol)
	{
		dd::point(startPosition, dd::colors::Green, 10.0f);
		dd::arrow(startPosition, enemyController->GetPosition(), dd::colors::Green, 20.0f);
	}
}

void BasicEnemyAIScript::OnTriggerEnter(GameObject* go)
{
	if ((currentState == patrol || currentState == returnToStart) && go->tag == "PlayerHitBoxAttack")
	{
		// Change state to chase and update states
		EnemyState* previous = currentState;
		currentState = chase;
		CheckStates(previous, currentState);
	}
}
