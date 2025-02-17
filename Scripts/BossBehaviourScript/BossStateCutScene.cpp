#include "Application.h"

#include "ModuleTime.h"

#include "BossStateCutScene.h"

#include "BossBehaviourScript.h"
#include "PlayerMovement/PlayerMovement.h"
#include "CameraController/CameraController.h"
#include "EnemyControllerScript/EnemyControllerScript.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "ComponentAnimation.h"
#include "ComponentAudioSource.h"

#include "Math/MathFunc.h"

BossStateCutScene::BossStateCutScene(BossBehaviourScript* AIBoss)
{
	boss = AIBoss;
	trigger = "Idle";
}

BossStateCutScene::~BossStateCutScene()
{
}

void BossStateCutScene::HandleIA()
{
	//if finished everything change to summon army
	if (finished)
	{
		boss->currentState = (BossState*)boss->summonArmy;
	}
}

void BossStateCutScene::Update()
{
	if (!musicFadeFinished)
	{
		musicTimer += boss->App->time->gameDeltaTime;

		float musicLambda = musicTimer / boss->cutsceneDoorDuration;

		if (musicLambda > 1.0f)
		{
			musicFadeFinished = true;
			boss->mainBGMusic->Stop();
		}
		else
		{
			boss->mainBGMusic->SetVolume(initialVolume *(1.0f - musicLambda));
		}
	}


	switch (csState)
	{
		case CutsceneState::None:
			csState = CutsceneState::DoorLerping;
			break;
		case CutsceneState::DoorLerping:

			if (doorLerpTimer >= boss->cutsceneDoorDuration)
			{
					csState = CutsceneState::DoorClosing;
					boss->doorParticles->SetActive(true);
					boss->doorRisingAudio->Play();
					wallSpeed = (boss->finalDoorHeight - boss->closingDoor->transform->GetPosition().y) / boss->cutsceneDoorRisingDuration;
			}
			else
			{
				doorLerpTimer += boss->App->time->gameDeltaTime;

				firstLambda = CalculateDoorLambda();
				SetPlayerCameraPosition(boss->InterpolateFloat3(cameraResetPosition, boss->cameraPositionDoorCS, firstLambda)); 
				SetPlayerCameraRotation(boss->InterpolateQuat(cameraResetRotation, boss->cameraRotationDoorCS, firstLambda));
			}
			break;

		case CutsceneState::DoorClosing:
		{
			doorClosingTimer += boss->App->time->gameDeltaTime;

			math::float3 newDoorPosition = boss->closingDoor->transform->GetPosition() + wallSpeed * math::float3::unitY * boss->App->time->gameDeltaTime;
			boss->closingDoor->transform->SetPosition(newDoorPosition);

			if (doorClosingTimer >= boss->cutsceneDoorRisingDuration)
			{
				boss->doorParticles->SetActive(false);
				csState = CutsceneState::BossLerping;
			}
			break;
		}
		case CutsceneState::BossLerping:

			if (bossLerpTimer >= boss->cutsceneBossDuration)
			{
				csState = CutsceneState::BossWatching;
				boss->laughAudio->Play();
			}
			else
			{
				bossLerpTimer += boss->App->time->gameDeltaTime;

				secondLambda = CalculateBossLambda();
				SetPlayerCameraPosition(boss->InterpolateFloat3(boss->cameraPositionDoorCS, boss->cameraPositionBossCS, secondLambda));
				SetPlayerCameraRotation(boss->InterpolateQuat(boss->cameraRotationDoorCS, boss->cameraRotationBossCS, secondLambda));
			}
			break;
		case CutsceneState::BossWatching:

			if (bossWatchingTimer >= 3.0f)
			{
				csState = CutsceneState::PlayerLerping;
				boss->bossBGMusic->Play();
			}
			else
			{
				bossWatchingTimer += boss->App->time->gameDeltaTime;
			}
			break;
		case CutsceneState::PlayerLerping:

			if (playerLerpTimer >= boss->cutsceneBackToPlayerDuration)
			{
				csState = CutsceneState::Finished;
			}
			else
			{
				playerLerpTimer += boss->App->time->gameDeltaTime;

				thirdLambda = CalculatePlayerLambda();
				SetPlayerCameraPosition(boss->InterpolateFloat3(boss->cameraPositionBossCS, cameraResetPosition, thirdLambda));
				SetPlayerCameraRotation(boss->InterpolateQuat(boss->cameraRotationBossCS, cameraResetRotation, thirdLambda));
				float newFOV = math::DegToRad(boss->InterpolateFloat(initalFOV, boss->finalFOV, thirdLambda));
				boss->compCamera->SetFov(newFOV);

			}
			break;

		case CutsceneState::Finished:
			finished = true;
			break;
	}
}

void BossStateCutScene::Enter()
{
	initalFOV = boss->playerCamera->GetComponent<ComponentCamera>()->GetFOV();

	cameraResetPosition = GetPlayerCameraPosition();
	cameraResetRotation = GetPlayerCameraRotation();

	firstCameraDirection = boss->cameraPositionDoorCS - cameraResetPosition;
	doorDistance = firstCameraDirection.Length();
	firstCameraSpeed = doorDistance / boss->cutsceneDoorDuration;
	firstCameraDirection.Normalize();

	secondCameraDirection = boss->cameraPositionBossCS - boss->cameraPositionDoorCS;
	bossDistance = secondCameraDirection.Length();
	secondCameraSpeed = bossDistance / boss->cutsceneBossDuration;
	secondCameraDirection.Normalize();

	initialVolume = boss->mainBGMusic->GetVolume();

	for (auto go : boss->fadeEnvironmentSounds)
	{
		go->GetComponent<ComponentAudioSource>()->SetVolume(0.1f);
	}

	//Deactivate player script
	boss->playerScript->anim->SendTriggerToStateMachine("Idle");
	boss->playerScript->Enable(false);
	//Deactivate camera script
	boss->playerCamera->GetComponent<CameraController>()->Enable(false);
}

void BossStateCutScene::Exit()
{
	//Activate player script
	boss->playerScript->Enable(true);
	//Activate camera script
	boss->playerCamera->GetComponent<CameraController>()->Enable(true);

	boss->enemyController->bossFightStarted = true;

}

float BossStateCutScene::CalculateDoorLambda()
{
	return (doorLerpTimer + boss->App->time->gameDeltaTime) / boss->cutsceneDoorDuration;
}

float BossStateCutScene::CalculateBossLambda()
{
	return (bossLerpTimer + boss->App->time->gameDeltaTime) / boss->cutsceneBossDuration;
}

float BossStateCutScene::CalculatePlayerLambda()
{
	return (playerLerpTimer + boss->App->time->gameDeltaTime) / boss->cutsceneBackToPlayerDuration;
}

math::float3 BossStateCutScene::GetPlayerCameraPosition()
{
	return boss->playerCamera->transform->GetPosition();
}

math::Quat BossStateCutScene::GetPlayerCameraRotation()
{
	return boss->playerCamera->transform->GetRotation();
}

void BossStateCutScene::SetPlayerCameraPosition(math::float3 newPosition)
{
	boss->playerCamera->transform->SetPosition(newPosition);
}

void BossStateCutScene::SetPlayerCameraRotation(math::Quat newRotation)
{
	boss->playerCamera->transform->SetRotation(newRotation);
}