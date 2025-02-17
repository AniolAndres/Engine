#include "EnemyStateAttack.h"

#include "GameObject.h"

#include "ComponentAudioSource.h"
#include "ComponentTransform.h"
#include "ComponentBoxTrigger.h"

#include "BasicEnemyAIScript.h"
#include "EnemyControllerScript.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ComponentTrail.h"

EnemyStateAttack::EnemyStateAttack(BasicEnemyAIScript* AIScript)
{
	enemy = AIScript;
	trigger = "Attack";
	boxSize = math::float3(100.f, 70.f, 50.f);
	minTime = 0.7f;
	maxTime = 0.9f;
	GameObject* punchBone = enemy->App->scene->FindGameObjectByName("joint18", enemy->gameobject);
	if (punchBone != nullptr)
	{
		trailPunch = punchBone->GetComponent<ComponentTrail>();
	}
}

EnemyStateAttack::~EnemyStateAttack()
{
}

void EnemyStateAttack::Enter()
{
	enemy->enemyController->Stop();
}

void EnemyStateAttack::Exit()
{
	attackSoundMade = false;
	enemy->enemyController->attackBoxTrigger->Enable(false);
	hitboxCreated = false;
}


void EnemyStateAttack::HandleIA()
{
	//If player is in range nothing, else if player is in range chase, if enemy has attacked cooldown
	
	if (timer > duration)
	{
		float distance = enemy->enemyController->GetDistanceToPlayer2D();
		if (distance > enemy->attackRange) //if not in range chase
		{
			if (hitboxCreated)
			{
				// Disable hitbox
				enemy->enemyController->attackBoxTrigger->Enable(false);
				hitboxCreated = false;
			}
			PunchFX(false);
			enemy->currentState = (EnemyState*)enemy->chase;
		}
		else if (attacked)
		{
			if (hitboxCreated)
			{
				// Disable hitbox
				enemy->enemyController->attackBoxTrigger->Enable(false);
				hitboxCreated = false;
			}
			enemy->currentState = (EnemyState*)enemy->cooldown;
			attacked = false;
			PunchFX(false);
		}
	}

}

void EnemyStateAttack::Update()
{
	// Keep looking at player
	if (enemy->audioHit != nullptr && timer > 0.1f && !attackSoundMade)
	{
		attackSoundMade = true;
		enemy->audioHit->Play();
	}
	math::float3 playerPosition = enemy->enemyController->GetPlayerPosition();
	enemy->enemyController->LookAt2D(playerPosition);


	assert(enemy->enemyController->attackBoxTrigger != nullptr);

	if(!attacked && enemy->attackDelay < timer)
		Attack();
}

void EnemyStateAttack::Attack()
{
	// Reset timer
	timer = 0.0f;

	//Create the hitbox
	enemy->enemyController->attackBoxTrigger->Enable(true);
	enemy->enemyController->attackBoxTrigger->SetBoxSize(boxSize);
	boxPosition = enemy->gameobject->transform->up * 100.f;
	enemy->enemyController->attackBoxTrigger->SetBoxPosition(boxPosition.x, boxPosition.y, boxPosition.z + 100.f);
	hitboxCreated = true;
	attacked = true;
	PunchFX(true);
}

void EnemyStateAttack::PunchFX(bool active)
{
	if (trailPunch != nullptr)
	{
		trailPunch->Enable(active);
	}
}