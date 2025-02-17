#include "Application.h"
#include "ModuleTime.h"
#include "ModuleScene.h"
#include "ModuleNavigation.h"

#include "GameObject.h"

#include "ComponentTransform.h"
#include "ComponentAnimation.h"
#include "ComponentCamera.h"
#include "ComponentBoxTrigger.h"
#include "ComponentAudioSource.h"
#include "ComponentRenderer.h"

#include "BossBehaviourScript.h"
#include "BasicEnemyAIScript/BasicEnemyAIScript.h"
#include "CameraController/CameraController.h"
#include "PlayerMovement/PlayerMovement.h"
#include "EnemyControllerScript/EnemyControllerScript.h"
#include "AOEBossScript/AOEBossScript.h"
#include "GameLoop/GameLoop.h"

#include "BossState.h"
#include "BossStateIdle.h"
#include "BossStateNotActive.h"
#include "BossStateActivated.h"
#include "BossStateDeath.h"
#include "BossStateCast.h"
#include "BossStateSummonArmy.h"
#include "BossStatePreCast.h"
#include "BossStateInterPhase.h"
#include "BossStateCutScene.h"
#include "BossStateThirdDeath.h"
#include "BossStateThirdIdle.h"
#include "BossStateThirdLeft.h"
#include "BossStateThirdRight.h"
#include "BossStateSecondInterPhase.h"

#include "imgui.h"
#include <algorithm>
#include <random>
#include "Math/MathFunc.h"
#include <stdlib.h>

#include "JSON.h"

#define FIRSTAOE "AOEFirstPhase"
#define SECONDAOE "AOESecondPhase"
#define THIRDAOE "AOEThirdPhase"

#define FIRSTSUMMON "BasicBossSummon"
#define FIRSTRANGEDSUMMON "BandoleroNormalPrefab"
#define GOLDENSUMMON "BasicGoldEnemy"
#define ELITESUMMON "BasicEliteEnemy"

#define BOSSPROJECTILE "BossProjectilePrefab"

BossBehaviourScript_API Script* CreateScript()
{
	BossBehaviourScript* instance = new BossBehaviourScript;
	return instance;
}

void BossBehaviourScript::Awake()
{
	// Look for Enemy Controller Script of the enemy
	enemyController = gameobject->GetComponent<EnemyControllerScript>();
	if (enemyController == nullptr)
	{
		LOG("The GameObject %s has no Enemy Controller Script component attached \n", gameobject->name.c_str());
	}

	gameLoopGO = App->scene->FindGameObjectByName("GameController", App->scene->root);
	if(gameLoopGO == nullptr)
	{
		LOG("Gameobject with the Gameloop not found");
	}
	else
	{
		gLoop = gameLoopGO->GetComponent<GameLoop>();
	}

	//main floors for the nav Mesh
	mainFirstFloor = App->scene->FindGameObjectByTag("mainFirstFloor", App->scene->root);
	if(mainFirstFloor == nullptr)
	{
		LOG("First main floor not found");
	}
	else
	{
		floorMainGOs.push_back(mainFirstFloor);
	}

	mainSecondFloor = App->scene->FindGameObjectByTag("mainSecondFloor", App->scene->root);
	if (mainSecondFloor == nullptr)
	{
		LOG("Second main floor not found");
	}
	else
	{
		floorMainGOs.push_back(mainSecondFloor);
		floorBossGOs.push_back(mainSecondFloor);
	}

	mainThirdFloor = App->scene->FindGameObjectByTag("mainThirdFloor", App->scene->root);
	if (mainThirdFloor == nullptr)
	{
		LOG("Third main floor not found");
	}
	else
	{
		floorMainGOs.push_back(mainThirdFloor);
		floorBossGOs.push_back(mainThirdFloor);
	}

	mainFourthFloor = App->scene->FindGameObjectByTag("mainFourthFloor", App->scene->root);
	if (mainFourthFloor == nullptr)
	{
		LOG("Fourth main floor not found");
	}
	else
	{
		floorMainGOs.push_back(mainFourthFloor);
	}

	mainFifthFloor = App->scene->FindGameObjectByTag("mainFifthFloor", App->scene->root);
	if (mainFifthFloor == nullptr)
	{
		LOG("Fifth main floor not found");
	}
	else
	{
		floorMainGOs.push_back(mainFifthFloor);
	}
	mainSixthFloor = App->scene->FindGameObjectByTag("mainSixthFloor", App->scene->root);
	if (mainSixthFloor == nullptr)
	{
		LOG("Sixth main floor not found");
	}
	else
	{
		floorMainGOs.push_back(mainSixthFloor);
	}

	//stairs limitation objects
	stairsCorrector1 = App->scene->FindGameObjectByTag("stairsCorrector1", App->scene->root);
	if (stairsCorrector1 == nullptr)
	{
		LOG("Stairs corrector 1 not found");
	}
	else
	{
		floorMainGOs.push_back(stairsCorrector1);
	}

	stairsCorrector2 = App->scene->FindGameObjectByTag("stairsCorrector2", App->scene->root);
	if (stairsCorrector2 == nullptr)
	{
		LOG("Stairs corrector 2 not found");
	}
	else
	{
		floorMainGOs.push_back(stairsCorrector2);
	}

	vesselsCorrector1 = App->scene->FindGameObjectByTag("vesselCorrector1", App->scene->root);
	if (vesselsCorrector1 == nullptr)
	{ 
		LOG("Vessel corrector 1 not found");
	}
	else
	{
		floorMainGOs.push_back(vesselsCorrector1);
	}
	
	vesselsCorrector2 = App->scene->FindGameObjectByTag("vesselCorrector2", App->scene->root);
	if (vesselsCorrector2 == nullptr)
	{
		LOG("Vessel corrector 2 not found");
	}
	else
	{
		floorMainGOs.push_back(vesselsCorrector2);
	}

	vesselsCorrector3 = App->scene->FindGameObjectByTag("vesselCorrector3", App->scene->root);
	if (vesselsCorrector3 == nullptr)
	{
		LOG("Vessel corrector 3 not found");
	}
	else
	{
		floorMainGOs.push_back(vesselsCorrector3);
	}

	vesselsCorrector4 = App->scene->FindGameObjectByTag("vesselCorrector4", App->scene->root);
	if (vesselsCorrector4 == nullptr)
	{
		LOG("Vessel corrector 4 not found");
	}
	else
	{
		floorMainGOs.push_back(vesselsCorrector4);
	}


	vesselsCorrector5 = App->scene->FindGameObjectByTag("vesselCorrector5", App->scene->root);
	if (vesselsCorrector5 == nullptr)
	{
		LOG("Vessel corrector 5 not found");
	}
	else
	{
		floorMainGOs.push_back(vesselsCorrector5);
	}

	stairsObstacle1 = App->scene->FindGameObjectByTag("stairsObstacle1", App->scene->root);
	if (stairsObstacle1 == nullptr)
	{
		LOG("Sixth main floor not found");
	}
	else
	{
		floorMainGOs.push_back(stairsObstacle1);
	}

	stairsObstacle2 = App->scene->FindGameObjectByTag("stairsObstacle2", App->scene->root);
	if (stairsObstacle2 == nullptr)
	{
		LOG("Sixth main floor not found");
	}
	else
	{
		floorMainGOs.push_back(stairsObstacle2);
	}

	stairsObstacle3 = App->scene->FindGameObjectByTag("stairsObstacle3", App->scene->root);
	if (stairsObstacle3 == nullptr)
	{
		LOG("Sixth main floor not found");
	}
	else
	{
		floorMainGOs.push_back(stairsObstacle3);
	}

	stairsObstacle4 = App->scene->FindGameObjectByTag("stairsObstacle4", App->scene->root);
	if (stairsObstacle4 == nullptr)
	{
		LOG("Sixth main floor not found");
	}
	else
	{
		floorMainGOs.push_back(stairsObstacle4);
	}

	bossObstacle1 = App->scene->FindGameObjectByTag("BossObstacle1", App->scene->root);
	if (bossObstacle1 == nullptr)
	{
		LOG("Boss obstacle not found");
	}
	else
	{
		floorMainGOs.push_back(bossObstacle1);
		floorBossGOs.push_back(bossObstacle1);
	}

	bossObstacle2 = App->scene->FindGameObjectByTag("BossObstacle2", App->scene->root);
	if (bossObstacle2 == nullptr)
	{
		LOG("Boss obstacle not found");
	}
	else
	{
		floorMainGOs.push_back(bossObstacle2);
		floorBossGOs.push_back(bossObstacle2);
	}

	//Meshes for the boss navMesh
	firstMeshFloor = App->scene->FindGameObjectByTag("bossFloor", App->scene->root);
	if (firstMeshFloor == nullptr)
	{
		LOG("first mesh not found");
	}
	else
	{
		floorBossGOs.push_back(firstMeshFloor);
	}
	secondMeshFloor = App->scene->FindGameObjectByTag("holeFloor", App->scene->root);
	if (secondMeshFloor == nullptr)
	{
		LOG("second mesh not found");
	}
	else
	{
		floorBossGOs.push_back(secondMeshFloor);
	}

	//Objects for the closing door
	closingDoor = App->scene->FindGameObjectByTag("closingDoor", App->scene->root);
	if(closingDoor == nullptr)
	{
		LOG("Closing door not found");
	}
	doorParticles = App->scene->FindGameObjectByTag("doorParticles", App->scene->root);
	if(doorParticles == nullptr)
	{
		LOG("Closing door particles not found");
	}

	playerCamera = App->scene->FindGameObjectByName("PlayerCamera");
	if (playerCamera == nullptr)
	{
		LOG("Player camera not found");
	}
	else
	{
		cameraScript = playerCamera->GetComponent<CameraController>();
		compCamera = playerCamera->GetComponent<ComponentCamera>();
	}

	anim = gameobject->GetComponent<ComponentAnimation>();
	if (anim == nullptr)
	{
		LOG("Animation for the boss not found");
	}

	leftHandBall = App->scene->FindGameObjectByTag("LeftBall", gameobject);
	if (leftHandBall == nullptr)
	{
		LOG("leftHandBall for the boss not found");
	}
	else
	{
		leftHandBall->SetActive(false);
	}
	rightHandBall = App->scene->FindGameObjectByTag("RightBall", gameobject);
	if (rightHandBall == nullptr)
	{
		LOG("rightHandBall for the boss not found");
	}
	else
	{
		rightHandBall->SetActive(false);
	}
	leftHandParticles = App->scene->FindGameObjectByTag("ParticlesLeft", gameobject);
	if (leftHandParticles == nullptr)
	{
		LOG("leftHandParticles for the boss not found");
	}
	else
	{
		leftHandParticles->SetActive(false);
	}
	rightHandParticles = App->scene->FindGameObjectByTag("ParticlesRight", gameobject);
	if (rightHandParticles == nullptr)
	{
		LOG("rightHandParticles for the boss not found");
	}
	else
	{
		rightHandParticles->SetActive(false);
	}

	powerUpBall = App->scene->FindGameObjectByTag("PowerUpBall", gameobject);
	if (powerUpBall == nullptr)
	{
		LOG("powerUpBall for the boss not found");
	}

	powerUpSpread = App->scene->FindGameObjectByTag("PowerUpSpread", gameobject);
	if (powerUpSpread == nullptr)
	{
		LOG("powerUpSpread for the boss not found");
	}
	else
	{
		powerUpSpread->SetActive(false);
	}

	ringPowerUp = App->scene->FindGameObjectByTag("RingPowerUp", gameobject);
	if (ringPowerUp == nullptr)
	{
		LOG("ringPowerUp for the boss not found");
	}
	else
	{
		ringPowerUp->SetActive(false);
	}

	leftFist = App->scene->FindGameObjectByTag("LeftFist", gameobject);
	leftFist = App->scene->FindGameObjectByTag("RightFist", gameobject);

	playerGO = App->scene->FindGameObjectByTag("Player", App->scene->root);
	if (playerGO == nullptr)
	{
		LOG("player not found");
	}
	else
	{
		playerScript = playerGO->GetComponent<PlayerMovement>();
		if (playerScript == nullptr)
		{
			LOG("playerScript not found for the explosion");
		}
	}

	secondInterphaseVanishGOs = App->scene->FindGameObjectsByTag("ThirdFloor");
	secondInterphaseFlameGOs = App->scene->FindGameObjectsByTag("ThirdFlames");

	//AudioSources
	thirdPhaseHitAudio = App->scene->FindGameObjectByTag("thirdPhaseHitAudio", gameobject)->GetComponent<ComponentAudioSource>();
	if (thirdPhaseHitAudio == nullptr)
	{
		LOG("thirdPhaseHitAudio not found");
	}

	doorRisingAudio = App->scene->FindGameObjectByTag("doorRisingAudio", gameobject)->GetComponent<ComponentAudioSource>();
	if (doorRisingAudio == nullptr)
	{
		LOG("doorRisingAudio not found");
	}

	castAudio = App->scene->FindGameObjectByTag("castAudio", gameobject)->GetComponent<ComponentAudioSource>();
	if (castAudio == nullptr)
	{
		LOG("castAudio not found");
	}

	precastAudio = App->scene->FindGameObjectByTag("precastAudio", gameobject)->GetComponent<ComponentAudioSource>();
	if (precastAudio == nullptr)
	{
		LOG("precastAudio not found");
	}

	mainBGMusic = App->scene->FindGameObjectByTag("backGroundMusicDefault", App->scene->root)->GetComponent<ComponentAudioSource>();
	if (mainBGMusic == nullptr)
	{
		LOG("mainBGMusic not found");
	}
	
	bossBGMusic = App->scene->FindGameObjectByTag("backGroundMusicBoss", App->scene->root)->GetComponent<ComponentAudioSource>();
	if (bossBGMusic == nullptr)
	{
		LOG("bossBGMusic not found");
	}

	bossScream = App->scene->FindGameObjectByTag("bossScream", gameobject)->GetComponent<ComponentAudioSource>();
	if (bossScream == nullptr)
	{
		LOG("bossScream not found");
	}

	cryAudio = App->scene->FindGameObjectByTag("cryAudio", gameobject)->GetComponent<ComponentAudioSource>();
	if (cryAudio == nullptr)
	{
		LOG("cryAudio not found");
	}

	laughAudio = App->scene->FindGameObjectByTag("laughAudio", gameobject)->GetComponent<ComponentAudioSource>();
	if (laughAudio == nullptr)
	{
		LOG("laughAudio not found");
	}

	fadeEnvironmentSounds = App->scene->FindGameObjectsByTag("bossFadeSound", App->scene->root);

	enemyController->hpBoxTrigger->Enable(false);
	GenerateNewNavigability(floorMainGOs);
}

void BossBehaviourScript::Start()
{
	startingPoint = enemyController->GetPosition();
	bossPhase = BossPhase::None;

	bossStates.reserve(14);
	bossStates.push_back(notActive = new BossStateNotActive(this));
	bossStates.push_back(activated = new BossStateActivated(this));
	bossStates.push_back(summonArmy = new BossStateSummonArmy(this));
	bossStates.push_back(idle = new BossStateIdle(this));
	bossStates.push_back(cast = new BossStateCast(this));
	bossStates.push_back(precast = new BossStatePreCast(this));
	bossStates.push_back(interPhase = new BossStateInterPhase(this));
	bossStates.push_back(secondInterphase = new BossStateSecondInterPhase(this));
	bossStates.push_back(death = new BossStateDeath(this));
	bossStates.push_back(cutscene = new BossStateCutScene(this));
	bossStates.push_back(thirdDeath = new BossStateThirdDeath(this));
	bossStates.push_back(thirdIdle = new BossStateThirdIdle(this));
	bossStates.push_back(thirdLeft = new BossStateThirdLeft(this));
	bossStates.push_back(thirdRight = new BossStateThirdRight(this));

	currentState = notActive;

	positionsSkullsSecond.reserve(8);
	positionsSkullsSecond.push_back(northSecondSkull);
	positionsSkullsSecond.push_back(northEastSecondSkull);
	positionsSkullsSecond.push_back(eastSecondSkull);
	positionsSkullsSecond.push_back(southEastSecondSkull);
	positionsSkullsSecond.push_back(southSecondSkull);
	positionsSkullsSecond.push_back(southWestSecondSkull);
	positionsSkullsSecond.push_back(westSecondSkull);
	positionsSkullsSecond.push_back(northWestSecondSkull);

	auto rng = std::default_random_engine{};
	std::shuffle(std::begin(positionsSkullsSecond), std::end(positionsSkullsSecond), rng);

	//allocate the spawn locations in a vector
	spawns.resize(6);
	spawns[0] = &firstSpawnLocation; spawns[1] = &secondSpawnLocation;
	spawns[2] = &thirdSpawnLocation; spawns[3] = &fourthSpawnLocation;
	spawns[4] = &fifthSpawnLocation; spawns[5] = &sixthSpawnLocation;
}

void BossBehaviourScript::Update()
{
	//With this we avoid having to 
	GetPositionVariables();

	BossState* previous = currentState;
	
	//floating movement
	FloatInSpace();

	//Here we use every skill the boss has
	HandleSkills();
	
	//these two will handle any state changes, health has priority
	currentState->HandleIA();
	CheckHealth();

	CheckStates(previous);
	currentState->UpdateTimer();
	currentState->Update();
}

void BossBehaviourScript::Expose(ImGuiContext * context)
{
	//Boss state check
	if (currentState == notActive)				ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Not active");
	else if (currentState == activated)			ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Activated");
	else if (currentState == death)				ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Dead");
	else if (currentState == summonArmy)		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Summon Army");
	else if (currentState == interPhase)		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: interPhase");
	else if (currentState == secondInterphase)	ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: secondInterphase");
	else if (currentState == precast)			ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Precast");
	else if (currentState == thirdIdle)			ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Third Idle");
	else if (currentState == thirdLeft)			ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Third Left");
	else if (currentState == thirdRight)		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Third Right");
	else if (currentState == thirdDeath)		ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Third Death");
	else if (currentState == cast)				ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: Cast");
	else if (currentState == idle)				ImGui::TextColored(ImVec4(1, 1, 0, 1), "State: idle");

	if (bossPhase == BossPhase::First)			ImGui::TextColored(ImVec4(0, 1, 0, 1), "First phase");
	else if(bossPhase == BossPhase::Second)		ImGui::TextColored(ImVec4(0, 1, 0, 1), "Second phase");
	else if (bossPhase == BossPhase::Third)		ImGui::TextColored(ImVec4(0, 1, 0, 1), "Third phase");

	if (lastUsedSkill == BossSkill::Aoe)			ImGui::TextColored(ImVec4(0, 1, 1, 1), "AOE skill");
	else if(lastUsedSkill == BossSkill::Summon)		ImGui::TextColored(ImVec4(0, 1, 1, 1), "Summon skill");
	else if (lastUsedSkill == BossSkill::Bombs)		ImGui::TextColored(ImVec4(0, 1, 1, 1), "Bombs skill");
	else if (lastUsedSkill == BossSkill::Teleport)	ImGui::TextColored(ImVec4(0, 1, 1, 1), "Teleport skill");


	ImGui::Text("Floating parameters:");
	ImGui::DragFloat("floating constant:", &floatConstant, 0.1f,0.0f,10.0f);
	ImGui::DragFloat("angle constant:", &angleConstant, 0.1f, 0.0f, 10.0f);

	ImGui::Separator();
	ImGui::PushID(0);
	ImGui::Text("Positions camera pointing door");
	ImGui::InputFloat3("Door Position", (float*)&cameraPositionDoorCS);
	ImGui::InputFloat3("Door Rotation", (float*)&cameraRotationDoorEulerCS);
	cameraRotationDoorCS = cameraRotationDoorCS.FromEulerXYZ(math::DegToRad(cameraRotationDoorEulerCS.x),
		math::DegToRad(cameraRotationDoorEulerCS.y), math::DegToRad(cameraRotationDoorEulerCS.z));
	ImGui::PopID();
	ImGui::Separator();
	ImGui::Text("Position camera pointing boss");
	ImGui::InputFloat3("Boss Position", (float*)&cameraPositionBossCS);
	ImGui::InputFloat3("Boss Rotation", (float*)&cameraRotationBossEulerCS);
	cameraRotationBossCS = cameraRotationBossCS.FromEulerXYZ(math::DegToRad(cameraRotationBossEulerCS.x),
		math::DegToRad(cameraRotationBossEulerCS.y), math::DegToRad(cameraRotationBossEulerCS.z));
	ImGui::Separator();
	ImGui::Text("Durations for the cutscene");
	ImGui::DragFloat("Door lerping", &cutsceneDoorDuration);
	ImGui::DragFloat("Door rising", &cutsceneDoorRisingDuration);
	ImGui::DragFloat("Boss lerping", &cutsceneBossDuration);
	ImGui::DragFloat("Player lerping", &cutsceneBackToPlayerDuration);
	ImGui::DragFloat("final FOV", &finalFOV);
	ImGui::Separator();
	ImGui::InputFloat3("first Spawn Point", (float*)&firstSpawnLocation);
	ImGui::InputFloat3("second Spawn Point", (float*)& secondSpawnLocation);
	ImGui::InputFloat3("third Spawn Point", (float*)& thirdSpawnLocation);
	ImGui::InputFloat3("fourth Spawn Point", (float*)& fourthSpawnLocation);
	ImGui::InputFloat3("fifth Spawn Point", (float*)& fifthSpawnLocation);
	ImGui::InputFloat3("sixth Spawn Point", (float*)& sixthSpawnLocation);
	ImGui::InputFloat3("HighPoint position", (float*)&highPointFirstCS);
	ImGui::InputFloat3("Start Fight position", (float*)&pointStartFight);
	ImGui::InputFloat("Door final height", &finalDoorHeight);

	ImGui::Separator();
	ImGui::Text("Fight parameters");
	ImGui::DragFloat("Activation Distance" , &activationDistance, 10.0f,500.0f,10000.0f);
	ImGui::DragFloat("Door closing distance" , &doorClosingDistance, 10.0f, 500.0f, 10000.0f);
	ImGui::DragFloat("Activation time", &activationTime, 0.1f, 0.5f, 20.0f);
	ImGui::Separator();
	ImGui::Text("Summon parameters");
	ImGui::InputInt("Skeletons to summon", &summonSkeletonsNumber);
	ImGui::DragFloat("Time between summons", &timerBetweenSummonsSummonPhase);
	ImGui::DragFloat("Time betweenSummons fight", &timerBetweenSummonsFight);
	ImGui::DragFloat("Fog lerp duration", &fogLerpDuration);
	ImGui::ColorEdit3("Fog final color", &fogFinalColor[0]);
	ImGui::Separator();
	ImGui::DragFloat("Circle noise", &circleNoise, 1.0f, 1.0f, 500.0f);
	ImGui::DragFloat("Orbs appear", &percOrbs, 0.01f, 0.01f, 1.0f);
	ImGui::DragFloat("Orbs disappear", &percOrbsDisappear, 0.01f, 0.01f, 1.0f);
	ImGui::Separator();
	ImGui::InputInt("Skulls first phase", &skullsToShootFirst);
	ImGui::InputFloat3("top TP", (float*)&topTP);
	ImGui::InputFloat3("bottom TP", (float*)&bottomTP);
	ImGui::InputFloat3("left TP", (float*)&leftTP);
	ImGui::InputFloat3("right TP", (float*)&rightTP);
	ImGui::Separator();
	ImGui::Text("First interphase");
	ImGui::DragFloat("duration 1IP", &firstInterphaseDuration, 0.1f, 0.1f, 20.0f);
	ImGui::InputFloat3("Throne position", (float*)&firstInterphasePosition);
	ImGui::InputFloat3("PowerUpPosition", (float*)&firstInterphasePowerUpPosition);
	ImGui::InputFloat3("Point to look", (float*)&pointToLookAtFirstInterphase);
	ImGui::DragFloat("Relocate time", &relocateInterPhaseTime);
	ImGui::DragFloat("Vibration Ball speed", &vibrationSpeed);
	ImGui::DragFloat("Vibration Ball Amplitude", &vibrationAmplitude);
	ImGui::DragFloat("Appear PU ball", &ballAppearTime);
	ImGui::DragFloat("Explode PU ball", &ballExplodeTime);
	ImGui::DragFloat("PU finish FX", &ballFxStopTime);
	ImGui::DragFloat("Hands Orbs appear", &handsAppearTime);
	ImGui::Separator();
	ImGui::Text("Second Interphase");
	ImGui::DragFloat("Kneel time", (float*)&secondInterphaseKneelTime, 0.1f,0.1f,5.0f);
	ImGui::InputFloat("Kneel final height", (float*)&endingHeightKneel);
	ImGui::DragFloat("Cry time", (float*)&secondInterphaseCryTime, 0.1f, 0.1f, 5.0f);
	ImGui::DragFloat("FadeIn time", (float*)&secondInterphaseFadeInTime, 0.1f, 0.1f, 5.0f);
	ImGui::DragFloat("FadeOff time", (float*)&secondInterphaseFadeOffTime, 0.1f, .1f, 5.0f);
	ImGui::DragFloat("Floor vanish time", (float*)&secondInterphaseFloorVanishTime, 0.1f, 0.1f, 5.0f);
	ImGui::DragFloat("Final scale", (float*)&secondInterphaseFinalScale, 0.1f, 1.5f, 50.0f);
	ImGui::InputFloat3("Position Third", (float*)&secondInterphaseFinalPosition);
	ImGui::Separator();
	ImGui::Text("Skulls position in second phase");
	ImGui::InputFloat3("North", (float*)&northSecondSkull);
	ImGui::InputFloat3("NorthEast", (float*)&northEastSecondSkull);
	ImGui::InputFloat3("East", (float*)&eastSecondSkull);
	ImGui::InputFloat3("SouthEast", (float*)&southEastSecondSkull);
	ImGui::InputFloat3("South", (float*)&southSecondSkull);
	ImGui::InputFloat3("SouthWest", (float*)&southWestSecondSkull);
	ImGui::InputFloat3("West", (float*)&westSecondSkull);
	ImGui::InputFloat3("NorthWest", (float*)&northWestSecondSkull);
	ImGui::Separator();
	ImGui::Text("Third phase");
	ImGui::DragFloat("Dissolve death speed", &deathDissolveSpeed);
	ImGui::DragFloat("Audio hit delay", &audioDelayThirdPhaseHit);
}

void BossBehaviourScript::Serialize(JSON_value * json) const
{
	json->AddFloat("floatConstant", floatConstant);
	json->AddFloat("angleConstant", angleConstant);
	json->AddFloat("activationDistance", activationDistance);
	json->AddFloat("doorClosingDistance", doorClosingDistance);
	json->AddFloat("activationTime", activationTime);
	
	json->AddFloat3("firstSpawnLocation", firstSpawnLocation);
	json->AddFloat3("secondSpawnLocation", secondSpawnLocation);
	json->AddFloat3("thirdSpawnLocation", thirdSpawnLocation);
	json->AddFloat3("fourthSpawnLocation", fourthSpawnLocation);
	json->AddFloat3("fifthSpawnLocation", fifthSpawnLocation);
	json->AddFloat3("sixthSpawnLocation", sixthSpawnLocation);
	json->AddFloat3("highPointFirstCS", highPointFirstCS);
	json->AddFloat3("pointStartFight", pointStartFight);
	json->AddFloat("finalDoorHeight", finalDoorHeight);

	json->AddInt("skullsToShootFirst", skullsToShootFirst);
	json->AddFloat3("topTP",topTP);
	json->AddFloat3("bottomTP",bottomTP);
	json->AddFloat3("leftTP",leftTP);
	json->AddFloat3("rightTP",rightTP);

	json->AddFloat("timerBetweenSummonsFight", timerBetweenSummonsFight);
	json->AddFloat("timerBetweenSummonsSummonPhase", timerBetweenSummonsSummonPhase);
	json->AddInt("summonSkeletonsNumber", summonSkeletonsNumber);
	json->AddFloat3("fogFinalColor", fogFinalColor);
	json->AddFloat("Fog lerp duration", fogLerpDuration);

	json->AddFloat("firstInterphaseDuration", firstInterphaseDuration);
	json->AddFloat("vibrationAmplitude", vibrationAmplitude);
	json->AddFloat("vibrationSpeed", vibrationSpeed);
	json->AddFloat("ballAppearTime", ballAppearTime);
	json->AddFloat("ballExplodeTime", ballExplodeTime);
	json->AddFloat("ballFxStopTime", ballFxStopTime);
	json->AddFloat("handsAppearTime", handsAppearTime);

	json->AddFloat3("northSecondSkull", northSecondSkull);
	json->AddFloat3("northEastSecondSkull", northEastSecondSkull);
	json->AddFloat3("eastSecondSkull", eastSecondSkull);
	json->AddFloat3("southEastSecondSkull", southEastSecondSkull);
	json->AddFloat3("southSecondSkull", southSecondSkull);
	json->AddFloat3("southWestSecondSkull", southWestSecondSkull);
	json->AddFloat3("westSecondSkull", westSecondSkull);
	json->AddFloat3("northWestSecondSkull", northWestSecondSkull);

	json->AddFloat("circleNoise", circleNoise);

	json->AddQuat("cameraRotationDoorCS", cameraRotationDoorCS);
	json->AddFloat3("cameraRotationDoorEulerCS", cameraRotationDoorEulerCS);
	json->AddFloat3("cameraPositionDoorCS", cameraPositionDoorCS);
	json->AddQuat("cameraRotationBossCS", cameraRotationBossCS);
	json->AddFloat3("cameraRotationBossEulerCS", cameraRotationBossEulerCS);
	json->AddFloat3("cameraPositionBossCS", cameraPositionBossCS);

	json->AddFloat("firstInterphaseDuration", firstInterphaseDuration);
	json->AddFloat3("firstInterphasePosition", firstInterphasePosition);
	json->AddFloat3("pointToLookAtFirstInterphase", pointToLookAtFirstInterphase);

	json->AddFloat("cutsceneDoorDuration", cutsceneDoorDuration);
	json->AddFloat("cutsceneBossDuration", cutsceneBossDuration);
	json->AddFloat("cutsceneBackToPlayerDuration", cutsceneBackToPlayerDuration);
	json->AddFloat("cutsceneDoorRisingDuration", cutsceneDoorRisingDuration);
	json->AddFloat("finalFOV", finalFOV);

	json->AddFloat("relocateInterPhaseTime", relocateInterPhaseTime);

	json->AddFloat("percOrbs", percOrbs);
	json->AddFloat("percOrbsDisappear", percOrbsDisappear);

	json->AddFloat("deathDissolveSpeed", deathDissolveSpeed);
	json->AddFloat("audioDelayThirdPhaseHit", audioDelayThirdPhaseHit);

	json->AddFloat("secondInterphaseKneelTime", secondInterphaseKneelTime);
	json->AddFloat("endingHeightKneel", endingHeightKneel);
	json->AddFloat("secondInterphaseCryTime", secondInterphaseCryTime);
	json->AddFloat("secondInterphaseFloorVanishTime", secondInterphaseFloorVanishTime);
	json->AddFloat("secondInterphaseFadeOffTime", secondInterphaseFadeOffTime);
	json->AddFloat("secondInterphaseFadeInTime", secondInterphaseFadeInTime);
	json->AddFloat3("secondInterphaseFinalPosition", secondInterphaseFinalPosition);
	json->AddFloat("secondInterphaseFinalScale", secondInterphaseFinalScale);
	json->AddFloat3("firstInterphasePowerUpPosition", firstInterphasePowerUpPosition);
}

void BossBehaviourScript::DeSerialize(JSON_value * json)
{
	floatConstant = json->GetFloat("floatConstant", 0.0f);
	angleConstant = json->GetFloat("angleConstant", 0.0f);
	activationDistance = json->GetFloat("activationDistance", 0.0f);
	doorClosingDistance = json->GetFloat("doorClosingDistance", 0.0f);
	firstSpawnLocation = json->GetFloat3("firstSpawnLocation");
	firstInterphasePowerUpPosition = json->GetFloat3("firstInterphasePowerUpPosition");

	vibrationSpeed = json->GetFloat("vibrationSpeed", 1.0f);
	vibrationAmplitude = json->GetFloat("vibrationAmplitude", 1.0f);
	ballAppearTime = json->GetFloat("ballAppearTime", 0.0f);
	ballExplodeTime = json->GetFloat("ballExplodeTime", 0.0f);
	ballFxStopTime = json->GetFloat("ballFxStopTime", 0.0f);
	handsAppearTime = json->GetFloat("handsAppearTime", 0.0f);

	timerBetweenSummonsSummonPhase = json->GetFloat("timerBetweenSummonsSummonPhase", 1.0f);
	timerBetweenSummonsFight = json->GetFloat("timerBetweenSummonsFight", 4.0f);
	summonSkeletonsNumber = json->GetInt("summonSkeletonsNumber", 10);
	fogFinalColor = json->GetFloat3("fogFinalColor");
	fogLerpDuration = json->GetFloat("fogLerpDuration", 3.0f);

	/*if we don't have the info we want and are given 0, 0, 0 for the spawn positions,
	we just keep the default values. We just need that for spawns from 2 to 6
	*/
	loadSpawningPositions(json);

	activationTime = json->GetFloat("activationTime", 0.0f);
	highPointFirstCS = json->GetFloat3("highPointFirstCS");
	pointStartFight = json->GetFloat3("pointStartFight");
	finalDoorHeight = json->GetFloat("finalDoorHeight", 500.0f);

	skullsToShootFirst = json->GetInt("skullsToShootFirst", 5);
	topTP = json->GetFloat3("topTP");
	bottomTP = json->GetFloat3("bottomTP");
	leftTP = json->GetFloat3("leftTP");
	rightTP = json->GetFloat3("rightTP");	

	firstInterphaseDuration = json->GetFloat("firstInterphaseDuration");

	northSecondSkull = json->GetFloat3("northSecondSkull");
	northEastSecondSkull = json->GetFloat3("northEastSecondSkull");
	eastSecondSkull = json->GetFloat3("eastSecondSkull");
	southEastSecondSkull = json->GetFloat3("southEastSecondSkull");
	southSecondSkull = json->GetFloat3("southSecondSkull");
	southWestSecondSkull = json->GetFloat3("southWestSecondSkull");
	westSecondSkull = json->GetFloat3("westSecondSkull");
	northWestSecondSkull = json->GetFloat3("northWestSecondSkull");

	circleNoise = json->GetFloat("circleNoise",0.0f);

	cameraPositionDoorCS = json->GetFloat3("cameraPositionDoorCS");
	cameraRotationDoorCS = json->GetQuat("cameraRotationDoorCS");
	cameraRotationDoorEulerCS = json->GetFloat3("cameraRotationDoorEulerCS");

	firstInterphaseDuration = json->GetFloat("firstInterphaseDuration", 5.0f);
	firstInterphasePosition = json->GetFloat3("firstInterphasePosition");
	pointToLookAtFirstInterphase = json->GetFloat3("pointToLookAtFirstInterphase");

	cameraPositionBossCS = json->GetFloat3("cameraPositionBossCS");
	cameraRotationBossCS = json->GetQuat("cameraRotationBossCS");
	cameraRotationBossEulerCS = json->GetFloat3("cameraRotationBossEulerCS");

	cutsceneDoorDuration = json->GetFloat("cutsceneDoorDuration", 10.0f);
	cutsceneBossDuration = json->GetFloat("cutsceneBossDuration", 10.0f);
	cutsceneBackToPlayerDuration = json->GetFloat("cutsceneBackToPlayerDuration", 10.0f);
	cutsceneDoorRisingDuration = json->GetFloat("cutsceneDoorRisingDuration", 10.0f);
	relocateInterPhaseTime = json->GetFloat("relocateInterPhaseTime", 4.0f);
	finalFOV = json->GetFloat("finalFOV", 60.0f);

	percOrbs = json->GetFloat("percOrbs", 0.25f);
	percOrbsDisappear = json->GetFloat("percOrbsDisappear", 0.5f);

	deathDissolveSpeed = json->GetFloat("deathDissolveSpeed", 1.0f);
	audioDelayThirdPhaseHit = json->GetFloat("audioDelayThirdPhaseHit", 1.0f);

	secondInterphaseFinalPosition = json->GetFloat3("secondInterphaseFinalPosition");
	endingHeightKneel = json->GetFloat("endingHeightKneel", 0.0f);
	secondInterphaseKneelTime = json->GetFloat("secondInterphaseKneelTime", 1.0f);
	secondInterphaseCryTime = json->GetFloat("secondInterphaseCryTime", 1.0f);
	secondInterphaseFloorVanishTime = json->GetFloat("secondInterphaseFloorVanishTime", 1.0f);
	secondInterphaseFadeOffTime = json->GetFloat("secondInterphaseFadeOffTime", 1.0f);
	secondInterphaseFadeInTime = json->GetFloat("secondInterphaseFadeInTime", 1.0f);
	secondInterphaseFinalScale = json->GetFloat("secondInterphaseFinalScale", 2.0f);
}

void BossBehaviourScript::StartThirdPhase()
{
	gameobject->SetBBEqualToBoxTrigger();
}

void BossBehaviourScript::EndThirdPhase()
{
	gameobject->shrinkBB();
}

void BossBehaviourScript::OnAnimationEvent(std::string nameEvent)
{
	if (nameEvent == "castAudio")
	{
		castAudio->Play();
	}
	else if (nameEvent == "precastAudio")
	{
		precastAudio->Play();
	}
}

void BossBehaviourScript::GenerateNewNavigability(std::vector<GameObject*>& vectorGOs)
{
	App->navigation->GenerateNavigabilityFromGOs(vectorGOs);
}

bool BossBehaviourScript::IsBossDead()
{
	if (currentState == thirdDeath)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void BossBehaviourScript::GetPositionVariables()
{
	distanceToPlayer = enemyController->GetDistanceToPlayer2D();
	playerPosition = enemyController->GetPlayerPosition();
	currentPosition = enemyController->GetPosition();
	currentRotation = enemyController->GetRotation();
}

void BossBehaviourScript::CheckStates(BossState * previous)
{
	if (previous != currentState)
	{
		previous->ResetTimer();
		previous->Exit();
		currentState->Enter();
	}
}

void BossBehaviourScript::CheckHealth()
{
	float actualHealth = enemyController->GetHealth();
	float maxHealth = enemyController->GetMaxHealth();

	float healthPerc = actualHealth / maxHealth;
	switch (bossPhase)
	{
		case BossPhase::None:
			break;
		case BossPhase::First:
			if (healthPerc < firstHealthThreshold)
			{
				bossPhase = BossPhase::Second;
				currentState = interPhase;
			}
			break;
		case BossPhase::Second:
			if (healthPerc < secondHealthThreshold)
			{
				//set things up for third
				gLoop->DeleteAllEnemies();
				//set the bb to be the same as the hitbox
				StartThirdPhase();
				bossPhase = BossPhase::Third;
				//tell the controller that we are in third stage
				enemyController->ThirdStageBoss = true;
				currentState = secondInterphase;
				isFloating = false;
			}
			break;
		case BossPhase::Third:
			break;
	}

	if (actualHealth <= 0.0f)
	{
		currentState = thirdDeath;
	}
}

void BossBehaviourScript::FloatInSpace()
{
	if (isFloating)
	{
		yOffset = floatConstant * sin(angle);
		angle += angleConstant * App->time->gameDeltaTime;

		math::float3 enemyPosition = enemyController->GetPosition();
		enemyController->SetPosition({ enemyPosition.x,enemyPosition.y + yOffset,enemyPosition.z });
	}
}

void BossBehaviourScript::HandleSkills()
{
	//handling summon since it hapens consistently during the fight
	HandleSummon();
	if (circlesSpawning)
	{
		circlesTimer += App->time->gameDeltaTime;

		switch (bossPhase)
		{
			case BossPhase::First:
				HandleFirstPhaseCircles();
				break;
			case BossPhase::Second:
				HandleSecondPhaseCircles();
				break;
			case BossPhase::Third:
				HandleSecondPhaseCircles();
				/*HandleThirdPhaseCircles();*/
				break;
		}
	}
//summon is not a skill anymore
	if (bossTeleporting)
	{
		switch (bossPhase)
		{
			case BossPhase::First:
				HandleFirstTP();
				break;
			case BossPhase::Second:
				HandleSecondTP();
				break;
			case BossPhase::Third:
				HandleSecondTP();
				break;
		}
	}
}

void BossBehaviourScript::HandleSummon()
{
	//check if its time to spawn
	summonTimer += App->time->gameDeltaTime;
	if (summonTimer >= timerBetweenSummonsFight)
	{
		switch (bossPhase)
		{
		case BossPhase::First:
		{
			ActuallySummon(0);
		} break;
		case BossPhase::Second:
		{
			ActuallySummon(1);
		} break;
		//case BossPhase::Third:
		//{
		//	ActuallySummon(2);
		//} break;
		}
		summonTimer = 0.f;
	}
}

void BossBehaviourScript::HandleFirstTP()
{
	ComponentRenderer* mainRender = enemyController->GetMainRenderer();

	switch (teleportState)
	{
	case TPState::None:
		teleportState = TPState::FadeOut;
		enemyController->hpBoxTrigger->Enable(false);
		break;
	case TPState::FadeOut:
		mainRender->dissolveAmount += App->time->gameDeltaTime;
		if (mainRender->dissolveAmount >= 1.0f)
		{
			teleportState = TPState::Relocate;
		}	
		break;
	case TPState::Relocate:
		TPtoLocation(ChooseNextTP(currentLocation));
		teleportState = TPState::FadeIn;
		break;
	case TPState::FadeIn:
		mainRender->dissolveAmount -= App->time->gameDeltaTime;
		if (mainRender->dissolveAmount <= 0.0f)
		{
			teleportState = TPState::Projectiles;
			enemyController->hpBoxTrigger->Enable(true);
		}
		break;
	case TPState::Projectiles:
		skullsTimer += App->time->gameDeltaTime;
	
		if (skullsTimer > timeBetweenSkulls || numberSkullsShot == 0)
		{
			float randZ = rand() % 500 - 250.0f;
			float randY = rand() % 300 - 150.0f;

			math::float3 directionToPlayer = playerPosition - currentPosition;
			directionToPlayer.Normalize();
			math::float3 sideVector = directionToPlayer.Cross(math::float3::unitY);
			sideVector.Normalize();
			math::float3 projPosition = currentPosition - directionToPlayer * 300.0f + 
				sideVector * randZ + math::float3(0, 400.0f + randY, 0);

			GameObject* skull = App->scene->Spawn(BOSSPROJECTILE, projPosition, currentRotation);
			++numberSkullsShot;
			skullsTimer = 0.0f;
			if (numberSkullsShot >= skullsToShootFirst)
			{
				numberSkullsShot = 0;
				teleportState = TPState::Finished;
			}
		}
			break;
	case TPState::Finished:
		bossTeleporting = false;
		teleportState = TPState::None;
		break;
	}
	//We first make it disappear then we TP and then we show her again
}

void BossBehaviourScript::HandleSecondTP()
{
	ComponentRenderer* mainRender = enemyController->GetMainRenderer();

	switch (teleportState)
	{
	case TPState::None:
		{
		teleportState = TPState::FadeOut;
		auto rng = std::default_random_engine{}; //Do I need to do this every frame?
		std::shuffle(std::begin(positionsSkullsSecond), std::end(positionsSkullsSecond), rng);
		enemyController->hpBoxTrigger->Enable(false);
		}
		break;
	case TPState::FadeOut:
		mainRender->dissolveAmount += App->time->gameDeltaTime;
		if (mainRender->dissolveAmount >= 1.0f)
		{
			teleportState = TPState::Relocate;
		}
		break;
	case TPState::Relocate:
		TPtoLocation(ChooseNextTP(currentLocation));
		teleportState = TPState::FadeIn;
		break;
	case TPState::FadeIn:
		mainRender->dissolveAmount -= App->time->gameDeltaTime;
		if (mainRender->dissolveAmount <= 0.0f)
		{
			teleportState = TPState::Projectiles;
			enemyController->hpBoxTrigger->Enable(true);
		}
		break;
	case TPState::Projectiles:
		skullsTimer += App->time->gameDeltaTime;

		if (skullsTimer > timeBetweenSkulls || numberSkullsShot == 0)
		{
			math::float3 projPosition = positionsSkullsSecond[positionsIt];
			++positionsIt;

			GameObject* skull = App->scene->Spawn(BOSSPROJECTILE, projPosition, currentRotation);
			++numberSkullsShot;
			skullsTimer = 0.0f;
			if (numberSkullsShot >= numberSkullsSecondTotal)
			{
				positionsIt = 0;
				numberSkullsShot = 0;
				teleportState = TPState::Finished;
			}
		}
		break;
	case TPState::Finished:
		bossTeleporting = false;
		teleportState = TPState::None;
		break;
	}
}

TPlocations BossBehaviourScript::ChooseNextTP(TPlocations currentLoc)
{
	TPlocations nextLocation;

	if (currentLoc == TPlocations::None)
	{
		nextLocation = TPlocations::Left;
	}
	else
	{
		nextLocation = currentLoc;

		while (currentLoc == nextLocation)
		{
			int randomInt = rand() % 4;

			switch (randomInt)
			{
			case 0:
				nextLocation = TPlocations::Top;
				break;
			case 1:
				nextLocation = TPlocations::Bottom;
				break;
			case 2:
				nextLocation = TPlocations::Right;
				break;
			case 3:
				nextLocation = TPlocations::Left;
				break;
			}
		}
	}

	currentLocation = nextLocation;

	return nextLocation;
}

void BossBehaviourScript::TPtoLocation(TPlocations tpLoc)
{
	switch (tpLoc)
	{
	case TPlocations::Left:
		enemyController->SetPosition(leftTP);
		break;
	case TPlocations::Right:
		enemyController->SetPosition(rightTP);
		break;
	case TPlocations::Top:
		enemyController->SetPosition(topTP);
		break;
	case TPlocations::Bottom:
		enemyController->SetPosition(bottomTP);
		break;
	}
}

void BossBehaviourScript::HandleFirstPhaseCircles()
{
	if (!firstCircleCast)
	{
		float randX = (float(rand() % 100) - 50.f) / 100.f * circleNoise;
		float randZ = (float(rand() % 100) - 50.f) / 100.f * circleNoise;

		math::float3 newPosition = playerPosition + math::float3(randX, 50, randZ);

		App->scene->Spawn(FIRSTAOE, newPosition, math::Quat::identity, App->scene->root);
		firstCircleCast = true;
	}
	else if (circlesTimer > timeBetweenCirclesFirst && circlesCast < circlesInFirstPhase)
	{
		float randX = (float(rand() % 100) - 50.f) / 100.f * circleNoise;
		float randZ = (float(rand() % 100) - 50.f) / 100.f * circleNoise;

		math::float3 newPosition = playerPosition + math::float3(randX, 50, randZ);

		App->scene->Spawn(FIRSTAOE, newPosition, math::Quat::identity, App->scene->root);
		circlesTimer = 0.0f;
		++circlesCast;
	}
	else if (circlesCast >= circlesInFirstPhase)
	{
		circlesCast = 0;
		circlesTimer = 0.0f;
		circlesSpawning = false;
		firstCircleCast = false;
	}
	
}

void BossBehaviourScript::HandleSecondPhaseCircles()
{
	if (!firstCircleCast)
	{
		float randX = (float(rand() % 100) - 50.f) / 100.f * circleNoise;
		float randZ = (float(rand() % 100) - 50.f) / 100.f * circleNoise;

		math::float3 newPosition = playerPosition + math::float3(randX, 50, randZ);

		GameObject* circle = App->scene->Spawn(FIRSTAOE, newPosition, math::Quat::identity, App->scene->root);
		AOEBossScript* circleScript = circle->GetComponent<AOEBossScript>();
		circleScript->circleType = 2;
		firstCircleCast = true;
	}
	if (circlesTimer > timeBetweenCirclesFirst && circlesCast < circlesInFirstPhase)
	{
		float randX = (float(rand() % 100) - 50.f) / 100.f * circleNoise;
		float randZ = (float(rand() % 100) - 50.f) / 100.f * circleNoise;

		math::float3 newPosition = playerPosition + math::float3(randX, 50, randZ);

		GameObject* circle = App->scene->Spawn(FIRSTAOE, newPosition, math::Quat::identity, App->scene->root);
		AOEBossScript* circleScript = circle->GetComponent<AOEBossScript>();
		circleScript->circleType = 2;
		circlesTimer = 0.0f;
		++circlesCast;
	}
	else if (circlesCast >= circlesInFirstPhase)
	{
		circlesCast = 0;
		circlesTimer = 0.0f;
		circlesSpawning = false;
	}
}

void BossBehaviourScript::HandleThirdPhaseCircles()
{
}


void BossBehaviourScript::ActuallySummon(int type)
{
	math::Quat bossRotation;
	math::float3 spawnPos = ChooseRandomSpawn();
	GameObject* spawn;
	switch (type)
	{
		case 0:
		{
			spawn = App->scene->Spawn(FIRSTSUMMON, ChooseRandomSpawn(), bossRotation);
		}break;
		case 1:
		{
			spawn = App->scene->Spawn(FIRSTSUMMON, ChooseRandomSpawn(), bossRotation);
		}break;
		case 2:
		{
			spawn = App->scene->Spawn(FIRSTSUMMON, ChooseRandomSpawn(), bossRotation);
		}break;
		default:
		{
			spawn = App->scene->Spawn(FIRSTSUMMON, ChooseRandomSpawn(), bossRotation);
		}break;
	}
		
	
	spawn->transform->LookAtLocal(playerPosition);

	//We need this so they agro automatically
	spawn->GetComponent<BasicEnemyAIScript>()->activationDistance = 9000.0f;
	spawn->GetComponent<BasicEnemyAIScript>()->returnDistance = 9000.0f;

	bossSummoning = false;
}

void BossBehaviourScript::HandleFirstBombs()
{
}

void BossBehaviourScript::HandleSecondBombs()
{
}

void BossBehaviourScript::HandleThirdBombs()
{
}

void BossBehaviourScript::loadSpawningPositions(JSON_value * json)
{
	/*
	Basically, if the position in the json is not 0, 0, 0, we load it.
	If not, default values are good
	*/
	//spawn 2
	if (json->GetFloat3("secondSpawnLocation").y != 0)
	{
		secondSpawnLocation = json->GetFloat3("secondSpawnLocation");
	}

	//spawn 3
	if (json->GetFloat3("thirdSpawnLocation").y != 0)
	{
		thirdSpawnLocation = json->GetFloat3("thirdSpawnLocation");
	}

	//4
	if (json->GetFloat3("fourthSpawnLocation").y != 0)
	{
		fourthSpawnLocation = json->GetFloat3("fourthSpawnLocation");
	}

	//5
	if (json->GetFloat3("fifthSpawnLocation").y != 0)
	{
		fifthSpawnLocation = json->GetFloat3("fifthSpawnLocation");
	}

	//6
	if (json->GetFloat3("sixthSpawnLocation").y != 0)
	{
		sixthSpawnLocation = json->GetFloat3("sixthSpawnLocation");
	}
}

math::Quat BossBehaviourScript::InterpolateQuat(const math::Quat first, const math::Quat second, float lambda)
{
	math::Quat result;
	float dot = first.Dot(second);

	if (dot >= 0.0f) // Interpolate through the shortest path
	{
		result.x = first.x*(1.0f - lambda) + second.x*lambda;
		result.y = first.y*(1.0f - lambda) + second.y*lambda;
		result.z = first.z*(1.0f - lambda) + second.z*lambda;
		result.w = first.w*(1.0f - lambda) + second.w*lambda;
	}
	else
	{
		result.x = first.x*(1.0f - lambda) - second.x*lambda;
		result.y = first.y*(1.0f - lambda) - second.y*lambda;
		result.z = first.z*(1.0f - lambda) - second.z*lambda;
		result.w = first.w*(1.0f - lambda) - second.w*lambda;
	}

	result.Normalize();

	return result;
}

math::float3 BossBehaviourScript::InterpolateFloat3(const math::float3 first, const math::float3 second, float lambda)
{
	return first * (1.0f - lambda) + second * lambda;
}

float BossBehaviourScript::InterpolateFloat(const float first, const float second, float lambda)
{
	return first * (1.0f - lambda) + second * lambda;
}

math::float3 BossBehaviourScript::ChooseRandomSpawn()
{
	int spawnIndex = rand() % 6;
	return *spawns[spawnIndex];
}

void BossBehaviourScript::stopPlayerMovement()
{
	playerScript->stopPlayerWalking();
}

void BossBehaviourScript::ResetVariables()
{
	circlesCast = 0;
	circlesTimer = 0.0f;
	firstCircleCast = false;
	circlesSpawning = false;
	bossTeleporting = false;
	numberSkullsShot = 0;
	skullsTimer = 0.0f;
	bossSummoning = false;
	leftHandBall->SetActive(false);
	rightHandBall->SetActive(false);
	leftHandParticles->SetActive(false);
	rightHandParticles->SetActive(false);
	//summon timer reset
	summonTimer = 0.f;
}

void BossBehaviourScript::PrepareBossFight(std::vector<GameObject*>& vectorGOs)
{
	//first we delete all enemies in the level
	gLoop->DeleteAllEnemies();

	//then we generate the new navigability
	GenerateNewNavigability(vectorGOs);

	//reset the crowd again
	gLoop->navMeshReloaded();

	//now we add again the player to the new navigability
	gLoop->AddPlayerToWorld(playerGO);
}