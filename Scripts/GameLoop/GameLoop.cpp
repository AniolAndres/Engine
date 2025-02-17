#include "GameLoop.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleScript.h"
#include "ModuleRender.h"
#include "ModuleTime.h"
#include "ModuleNavigation.h"
#include "MouseController.h"

#include "GameObject.h"
#include "ComponentButton.h"
#include "ComponentText.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "ComponentTransform2D.h"
#include "ComponentImage.h"
#include "ComponentAudioSource.h"


#include "LoopState.h"
#include "LoopStateControls.h"
#include "LoopStateCredits.h"
#include "LoopStateDead.h"
#include "LoopStateIntro.h"
#include "LoopStateLoading.h"
#include "LoopStateMenu.h"
#include "LoopStateOptions.h"
#include "LoopStatePaused.h"
#include "LoopStatePlaying.h"
#include "LoopStateQuit.h"
#include "LoopStateWin.h"

#include "PlayerMovement.h"
#include "IntroScript.h"
#include "CreditsScript.h"
#include "InventoryScript.h"
#include "EquipPopupController.h"
#include "SkillTreeController.h"
#include "ExperienceController.h"
#include "WorldControllerScript.h"

#include "Globals.h"

#include "JSON.h"
#include "imgui.h"

#define MENU_SCENE "MenuScene"
#define GRAVEYARD_SCENE "Level0-TheGraveyard"

GameLoop_API Script* CreateScript()
{
	GameLoop* instance = new GameLoop;
	return instance;
}

void GameLoop::Expose(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	const char * scenes[] = { "Menu","Graveyard", "Temple"};
	if (ImGui::BeginCombo("Scene", scenes[(int)gameScene]))
	{
		for (int n = 0; n < 3; n++)
		{
			bool isSelected = ((int)gameScene == n);
			if (ImGui::Selectable(scenes[n], isSelected) && (int)gameScene != n)
			{
				gameScene = (GameScene)n;
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::InputFloat("x Picking Correction", &xPickingCorrection);
	ImGui::InputFloat("y Picking Correction", &yPickingCorrection);
	ImGui::InputFloat("z Picking Correction", &zPickingCorrection);
}

void GameLoop::Start()
{
	CreateGameStates();

	if (gameScene == GameScene::MENU)
	{
		LoadMenuScene();
		currentLoopState = (LoopState*)menuState;
	}
	else if (gameScene == GameScene::CEMENTERY)
	{
		LoadHUDScene();
		LoadCementeryScene();
		App->scene->root->OnPlay();
		currentLoopState = (LoopState*)introState;
	}
	else if (gameScene == GameScene::TEMPLE)
	{
		LoadHUDScene();
		LoadTempleScene();
		App->scene->root->OnPlay();
		currentLoopState = (LoopState*)introState;
	}


	if (App->scene->actionAfterLoad)
	{
		GetStateAfterLoad();
		App->scene->actionAfterLoad = false;
	}
}

void GameLoop::Update()
{
	LoopState* previous = currentLoopState;

	currentLoopState->Update();

	CheckStates(previous);

	ManageNavigationCursorIcon();
}


void GameLoop::LoadMenuScene()
{
	LOG("Started GameLoop script");
	menu = App->scene->FindGameObjectByName("Menu");
	assert(menu != nullptr);

	options = App->scene->FindGameObjectByName("OptionsMenu");
	assert(options != nullptr);

	controls = App->scene->FindGameObjectByName("ControlsMenu");
	assert(controls != nullptr);

	GameObject* mainMenuGO = App->scene->FindGameObjectByName("MainMenu");
	assert(mainMenuGO != nullptr);
	menuButtons = mainMenuGO->GetComponentsInChildren(ComponentType::Button);
	sunHoverGO = App->scene->FindGameObjectsByTag("SunHover", mainMenuGO);

	GameObject* optionsGO = App->scene->FindGameObjectByName("ButtonOptions");
	assert(optionsGO != nullptr);
	optionButton = optionsGO->GetComponent<Button>();

	GameObject* controlsGO = App->scene->FindGameObjectByName("ButtonControls");
	assert(controlsGO != nullptr);
	controlsButton = controlsGO->GetComponent<Button>();

	introVideoGO = App->scene->FindGameObjectByName("Intro", App->scene->canvas);
	introSkipTextGO = App->scene->FindGameObjectByName("IntroSkipBackground", introVideoGO);

	//load credits stuff
	menuMusic = App->scene->FindGameObjectByName("Main Music");
	creditsAudio = App->scene->FindGameObjectByName("CreditsAudio", creditsVideoGO);
	creditsVideoGO = App->scene->FindGameObjectByName("CreditsVideo", App->scene->canvas);
	creditsVideo = creditsVideoGO->GetComponent<ComponentImage>();

	GameObject* creditsButtonGO = App->scene->FindGameObjectByName("ButtonCredits");
	assert(creditsButtonGO != nullptr);
	creditsButton = creditsButtonGO->GetComponent<Button>();

	GameObject* musicGO = App->scene->FindGameObjectByName("MusicRow");
	assert(musicGO != nullptr);
	volumeButtons = musicGO->GetComponentsInChildren(ComponentType::Button);

	GameObject* volumeGO = App->scene->FindGameObjectByName("MusicNumberText", musicGO);
	volumeText = (Text*)volumeGO->GetComponent<Text>();//MusicNumberText

	GameObject* soundGO = App->scene->FindGameObjectByName("SoundRow");
	assert(soundGO != nullptr);
	soundButtons = soundGO->GetComponentsInChildren(ComponentType::Button);

	GameObject* soundVolumeGO = App->scene->FindGameObjectByName("SoundNumberText", soundGO);
	soundText = (Text*)soundVolumeGO->GetComponent<Text>();//SoundNumberText

	vsyncGO = App->scene->FindGameObjectByName("Check", options);
	assert(vsyncGO != nullptr);

	GameObject* vsyncButtonGO = App->scene->FindGameObjectByName("CheckBackground", options);
	vsyncButton = (Button*)vsyncButtonGO->GetComponent<Button>();
	assert(vsyncButton != nullptr);

	GameObject* backOptionsGO = App->scene->FindGameObjectByName("Back", options);
	backOptionButton = (Button *)backOptionsGO->GetComponent<Button>();
	assert(backOptionButton != nullptr);

	GameObject* backControlsGO = App->scene->FindGameObjectByName("Back", controls);
	backControlsButton = (Button *)backControlsGO->GetComponent<Button>();
	assert(backControlsButton != nullptr);

	GameObject* exitButtonGO = App->scene->FindGameObjectByName("ButtonExit");
	exitButton = (Button*)exitButtonGO->GetComponent<Button>();
	assert(exitButton != nullptr);

	creditsGO = App->scene->FindGameObjectByName("Credits");
	assert(creditsGO != nullptr);

	GameObject* backCreditsGO = App->scene->FindGameObjectByName("Back", creditsGO);
	backCreditsButton = (Button *)backCreditsGO->GetComponent<Button>();
	assert(backCreditsButton != nullptr);

	/*GameObject* imageCredits = App->scene->FindGameObjectByName("ImageCredits", creditsGO);
	creditsScript = imageCredits->GetComponent<CreditsScript>();
	assert(creditsScript != nullptr);*/

	loadingGO = App->scene->FindGameObjectByName("Loading");
	assert(loadingGO != nullptr);

	GameObject* backgroundImage = App->scene->FindGameObjectByName("BackgroundVideo");
	assert(backgroundImage != nullptr);
	backgroundImage->GetComponent<ComponentImage>()->PlayVideo();
}

void GameLoop::LoadHUDScene()
{
	loseWindow = App->scene->FindGameObjectByName("LoseWindow");
	assert(loseWindow != nullptr);

	GameObject* toTheAltarGO = App->scene->FindGameObjectByName("Button", loseWindow);
	toTheAltarButton = (Button *)toTheAltarGO->GetComponent<Button>();
	assert(toTheAltarButton != nullptr);

	winWindow = App->scene->FindGameObjectByName("WinWindow");
	assert(winWindow != nullptr);

	hudGO = App->scene->FindGameObjectByName("GameHUB");
	assert(winWindow != nullptr);

	GameObject* backMenuGO = App->scene->FindGameObjectByName("MenuButton", hudGO);
	hudBackToMenuButton = (Button *)backMenuGO->GetComponent<Button>();
	assert(hudBackToMenuButton != nullptr);

	GameObject* inventoryGO = App->scene->FindGameObjectByName("InventaryButton", hudGO);
	inventoryButton = (Button *)inventoryGO->GetComponent<Button>();
	assert(inventoryButton != nullptr);

	GameObject* skillsGO = App->scene->FindGameObjectByName("SkillsButton", hudGO);
	skillsButton = (Button *)skillsGO->GetComponent<Button>();
	assert(skillsButton != nullptr);

	playerMenuGO = App->scene->FindGameObjectByName("PlayerMenu");
	assert(playerMenuGO != nullptr);

	GameObject* closeButton = App->scene->FindGameObjectByName("CloseButton", playerMenuGO);
	closePlayerMenuButton = (Button *)closeButton->GetComponent<Button>();
	assert(closePlayerMenuButton != nullptr);

	inventoryMenuGO = App->scene->FindGameObjectByName("Inventory", playerMenuGO);
	assert(inventoryMenuGO != nullptr);
	inventoryScript = inventoryMenuGO->GetComponent<InventoryScript>();

	skillsMenuGO = App->scene->FindGameObjectByName("Skills", playerMenuGO);
	assert(skillsMenuGO != nullptr);
	skillTreeScript = skillsMenuGO->GetComponent<SkillTreeController>();

	GameObject* popupButton = App->scene->FindGameObjectByName("PopUpEquip");
	if (popupButton != nullptr)
	{
		equipPopUpScript = popupButton->GetComponent<EquipPopupController>();
		assert(equipPopUpScript != nullptr);
	}
	else
	{
		LOG("Warning: PopUpBackground GO couldn't be found.");
	}


	GameObject* xp = App->scene->FindGameObjectByName("Xp");
	if (xp != nullptr)
	{
		experienceScript = xp->GetComponent<ExperienceController>();
		//assert(experienceScript != nullptr);
	}
	else
	{
		LOG("Warning: Xp GO couldn't be found.");
	}

	pauseMenuGO = App->scene->FindGameObjectByName("PauseMenu");
	GameObject* resumGO = App->scene->FindGameObjectByName("ResumeButton", pauseMenuGO);
	if (resumGO != nullptr)
	{
		pauseResume = resumGO->GetComponent<Button>();
		pauseExit = App->scene->FindGameObjectByName("ExitButton", pauseMenuGO)->GetComponent<Button>();
		pauseControls = App->scene->FindGameObjectByName("ControlsButton", pauseMenuGO)->GetComponent<Button>();
	}
	if (pauseMenuGO != nullptr)
	{
		pauseMenuGO->SetActive(false);
	}

	controls = App->scene->FindGameObjectByName("ControlsMenu");
	assert(controls != nullptr);

	GameObject* backControlsGO = App->scene->FindGameObjectByName("Back", controls);
	backControlsButton = (Button *)backControlsGO->GetComponent<Button>();
	assert(backControlsButton != nullptr);
}

void GameLoop::LoadCementeryScene()
{
	App->scene->enemyHovered.object = nullptr;
	playerGO = App->scene->FindGameObjectByName("Player");
	playerScript = playerGO->GetComponent<PlayerMovement>();
	assert(playerScript != nullptr);

	//create the world controller that will manage the enemies and their paths
	worldControllerGO = App->scene->FindGameObjectByName("WorldController");
	if (worldControllerGO != nullptr)
	{
		worldController = worldControllerGO->GetComponent<WorldControllerScript>();
		if (worldControllerGO == nullptr)
			LOG("World Controller Script couldn't be found attached to WorldController GO.");
	}
	else
		LOG("WorldController GO couldn't be found.");

	assert(worldController != nullptr);
	//reset world controllers enemies and player
	worldController->resetWorldAgents();
	//add the player to the script
	AddPlayerToWorld(playerGO);

	playerBbox = &App->scene->FindGameObjectByName("PlayerMesh", playerGO)->bbox;

	//enemyGO = App->scene->FindGameObjectByName("Enemy");
	//enemyMovementScript = (EnemyMovementScript*)enemyGO->GetScript();
	//assert(enemyMovementScript != nullptr);

	winBbox = &App->scene->FindGameObjectByName("WinZone")->bbox;
	assert(winBbox != nullptr);

	introCamera = App->scene->FindGameObjectByName("IntroCamera");
	assert(introCamera != nullptr);
	componentIntroCamera = (ComponentCamera*)introCamera->GetComponent<ComponentCamera>();
	introScript = introCamera->GetComponent<IntroScript>();
	assert(introScript != nullptr);

	loadingGO = App->scene->FindGameObjectByName("Loading");
	assert(loadingGO != nullptr);

	audioGO = App->scene->FindGameObjectByName("Audio");

	currentLoopState = (LoopState*)introState;
	//componentIntroCamera->SetAsMain();
	hudGO->SetActive(false);

	GameObject* GOtemp = App->scene->FindGameObjectByName("MenuButtonsAudio");
	if (GOtemp != nullptr)
	{
		menuButtonsSound = GOtemp->GetComponent<ComponentAudioSource>();
		assert(menuButtonsSound != nullptr);
	}
	else
	{
		LOG("The Game Object 'menuButtonsSound' couldn't be found.");
	}
}

void GameLoop::LoadTempleScene()
{
	App->scene->enemyHovered.object = nullptr;
	playerGO = App->scene->FindGameObjectByName("Player");
	playerScript = playerGO->GetComponent<PlayerMovement>();
	assert(playerScript != nullptr);

	playerScript->IsMoving();

	//create the world controller that will manage the enemies and their paths
	worldControllerGO = App->scene->FindGameObjectByName("WorldController");
	worldController = worldControllerGO->GetComponent<WorldControllerScript>();
	assert(worldController != nullptr);
	//reset world controllers enemies and player
	worldController->resetWorldAgents();
	//add the player to the script
	AddPlayerToWorld(playerGO);

	playerBbox = &App->scene->FindGameObjectByName("PlayerMesh", playerGO)->bbox;

	//enemyGO = App->scene->FindGameObjectByName("Enemy");
	//enemyMovementScript = (EnemyMovementScript*)enemyGO->GetScript();
	//assert(enemyMovementScript != nullptr);

	audioGO = App->scene->FindGameObjectByName("Audio");

	winBbox = &App->scene->FindGameObjectByName("WinZone")->bbox;
	assert(winBbox != nullptr);

	introCamera = App->scene->FindGameObjectByName("IntroCamera");
	assert(introCamera != nullptr);
	componentIntroCamera = (ComponentCamera*)introCamera->GetComponent<ComponentCamera>();
	introScript = introCamera->GetComponent<IntroScript>();
	assert(introScript != nullptr);

	loadingGO = App->scene->FindGameObjectByName("Loading");
	assert(loadingGO != nullptr);

	outroVideoGO = App->scene->FindGameObjectByName("Outro", App->scene->canvas);
	outroSkipTextGO = App->scene->FindGameObjectByName("OutroSkipBackground", outroVideoGO);

	currentLoopState = (LoopState*)introState;
	//componentIntroCamera->SetAsMain();
	hudGO->SetActive(false);

	GameObject* GOtemp = App->scene->FindGameObjectByName("MenuButtonsAudio");
	if (GOtemp != nullptr)
	{
		menuButtonsSound = GOtemp->GetComponent<ComponentAudioSource>();
		assert(menuButtonsSound != nullptr);
	}
	else
	{
		LOG("The Game Object 'menuButtonsSound' couldn't be found.");
	}
}

void GameLoop::LoadLvl(unsigned lvl)
{
	currentLoopState = deadState;
	toTheAltarButton->state = ButtonState::DOWN;
	switch (lvl)
	{
	case 1:
		gameScene = GameScene::CEMENTERY;
		break;
	case 2:
		gameScene = GameScene::TEMPLE;
		break;
	}
}

void GameLoop::CreateGameStates()
{
	loopStates.push_back(controlsState = new LoopStateControls(this));
	loopStates.push_back(creditsState = new LoopStateCredits(this));
	loopStates.push_back(deadState = new LoopStateDead(this));
	loopStates.push_back(introState = new LoopStateIntro(this));
	loopStates.push_back(loadingState = new LoopStateLoading(this));
	loopStates.push_back(menuState = new LoopStateMenu(this));
	loopStates.push_back(optionsState = new LoopStateOptions(this));
	loopStates.push_back(pausedState = new LoopStatePaused(this));
	loopStates.push_back(playingState = new LoopStatePlaying(this));
	loopStates.push_back(quitState = new LoopStateQuit(this));
	loopStates.push_back(winState = new LoopStateWin(this));
}

void GameLoop::CheckStates(LoopState* previous)
{
	if (currentLoopState != previous)
	{
		previous->Exit();
		currentLoopState->Enter();
	}
}

void GameLoop::EnableMenuButtons(bool enable)
{
	for (int i = 0; i < menuButtons.size(); i++)
	{
		menuButtons[i]->Enable(enable);
	}
}

void GameLoop::GetStateAfterLoad()
{
	std::string state = App->scene->stateAfterLoad;
	if (strcmp(state.c_str(), "Credits") == 0)
	{
		currentLoopState = (LoopState*)creditsState;
	}
	else if (strcmp(state.c_str(), "Intro") == 0)
	{
		gameScene = GameScene::CEMENTERY;
		currentLoopState = (LoopState*)introState;
	}
	else if (strcmp(state.c_str(), "Temple") == 0)
	{
		gameScene = GameScene::TEMPLE;
		currentLoopState = (LoopState*)introState;
	}
	
}

void GameLoop::VolumeManagement()
{
	if (((Button*)volumeButtons[0])->IsPressed()) //Decrease
	{
		volume = MAX(minVolume, volume - 1);
		volumeText->text = std::to_string(volume);
	}
	else if (((Button*)volumeButtons[1])->IsPressed()) //Increase
	{
		volume = MIN(maxVolume, volume + 1);
		volumeText->text = std::to_string(volume);
	}
}

void GameLoop::SoundManagement()
{
	if (((Button*)soundButtons[0])->IsPressed()) //Decrease
	{
		sound = MAX(minSound, sound - 1);
		soundText->text = std::to_string(sound);
	}
	else if (((Button*)soundButtons[1])->IsPressed()) //Increase
	{
		sound = MIN(maxSound, sound + 1);
		soundText->text = std::to_string(sound);
	}
}

void GameLoop::VsyncManagement()
{
	if (vsyncButton->IsPressed())
	{
		vsync = !vsync;
		vsyncGO->SetActive(vsync);
		App->renderer->SetVsync(vsync);
	}
}

void GameLoop::ResolutionManagement()
{
	// 1280/720 - 1600/900 - 1920/1080
}

void GameLoop::ResetPositions()
{
	//playerGO->transform->SetPosition(playerStartPosition);
	//enemyGO->transform->SetPosition(enemyStartPosition);
}

bool GameLoop::HasImageHoveredInChildren(const GameObject * go) const
{
	for (auto image : go->GetComponentsInChildren(ComponentType::Image))
	{
		if (((ComponentImage*)image)->isHovered)
		{
			return true;
		}
	}
	return false;
}

GameObject* GameLoop::DeleteAllEnemies()
{
	if (worldController != nullptr)
	{
		return worldController->DeleteEnemiesFromWorld();
	}
	return nullptr;
}

void GameLoop::AddPlayerToWorld(GameObject* Player)
{
	if (worldController != nullptr && Player != nullptr)
	{
		worldController->setPlayer(Player, playerScript->walkingSpeed);
	}
}

void GameLoop::navMeshReloaded()
{
	worldController->resetWorldAgents();
}

void GameLoop::Serialize(JSON_value* json) const
{
	assert(json != nullptr);
	json->AddInt("scene", (int)gameScene);
	json->AddFloat("xPickingCorrection", xPickingCorrection);
	json->AddFloat("yPickingCorrection", yPickingCorrection);
	json->AddFloat("zPickingCorrection", zPickingCorrection);
}

void GameLoop::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);
	gameScene = (GameScene)json->GetInt("scene");
	xPickingCorrection = json->GetFloat("xPickingCorrection", 0.0f);
	yPickingCorrection = json->GetFloat("yPickingCorrection", 0.f);
	zPickingCorrection = json->GetFloat("zPickingCorrection", 0.0f);
}

void GameLoop::ManageNavigationCursorIcon()
{
	if (gameScene == GameScene::CEMENTERY || gameScene == GameScene::TEMPLE)
	{
		if (changeNavigationCursorIcon && !App->navigation->IsCursorPointingToNavigableZone(xPickingCorrection, yPickingCorrection, zPickingCorrection))
		{
			MouseController::ChangeWindowsCursorIcon();
			changeNavigationCursorIcon = false;
			changeStandarCursorIcon = true;
		}
		else if (changeStandarCursorIcon && App->navigation->IsCursorPointingToNavigableZone(xPickingCorrection, yPickingCorrection, zPickingCorrection))
		{
			MouseController::ChangeCursorIcon(gameStandarCursor);
			changeStandarCursorIcon = false;
			changeNavigationCursorIcon = true;
		}
	}
}