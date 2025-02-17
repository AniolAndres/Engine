#include "EnemyControllerScript.h"

#include <algorithm>

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleScript.h"
#include "ModuleTime.h"
#include "ModuleNavigation.h"
#include "ModuleResourceManager.h"
#include "MouseController.h"

#include "GameObject.h"
#include "ComponentRenderer.h"
#include "ComponentAnimation.h"
#include "ComponentTransform.h"
#include "ComponentBoxTrigger.h"
#include "ComponentAudioSource.h"

#include "PlayerMovement.h"
#include "ResourceMaterial.h"
#include "DamageController.h"
#include "EnemyLifeBarController.h"
#include "LootDropScript.h"
#include "WorldControllerScript.h"
#include "ExperienceSphereScript.h"

#include "imgui.h"
#include "JSON.h"

#define MINIMUM_PATH_DISTANCE 400.0f
#define MOVE_REFRESH_TIME 0.3f
#define CRITICAL_DAMAGE 1.2f

EnemyControllerScript_API Script* CreateScript()
{
	EnemyControllerScript* instance = new EnemyControllerScript;
	return instance;
}

void EnemyControllerScript::Start()
{
	//add the enemy to the world controller script
	//this should be called everytime levels are switched
	
	if (gameobject->tag.c_str() != "Boss" && gameobject->tag != "Boss")
	{
		currentWorldControllerScript = App->scene->FindGameObjectByName("WorldController")->GetComponent<WorldControllerScript>();
		currentWorldControllerScript->addEnemy(gameobject);
	}
}

void EnemyControllerScript::Awake()
{
	std::vector<Component*> renders = gameobject->GetComponentsInChildren(ComponentType::Renderer);
	for (std::vector<Component*>::iterator it = renders.begin(); it != renders.end(); ++it)
		myRenders.push_back((ComponentRenderer*)(*it));

	if (myRenders.size() > 0u)
	{
		// Look for enemy BBox (Will take first found)
		myBbox = &myRenders.at(0)->gameobject->bbox;
		if (myBbox == nullptr)
		{
			LOG("The enemy %s has no bbox \n", gameobject->name);
		}

		// Get playerMesh
		myMesh = myRenders.at(0)->gameobject;
	}
	else
	{
		LOG("Error: The enemy mesh couldn't be found.");
	}

	// Get hit material
	std::vector<std::string> materials = App->resManager->GetResourceNamesList(TYPE::MATERIAL, true);
	for (std::string matName : materials)
	{
		if (matName == hitMaterialName)
		{
			hitMaterial = (ResourceMaterial*)App->resManager->GetByName(matName.c_str(), TYPE::MATERIAL);
		}
	}
	// Will only change first renderer material on hit
	for (ComponentRenderer* renderer : myRenders)
	{
		defaultMaterials.push_back(renderer->material);
	}

	if (!hitMaterial)
	{
		hitMaterial = (ResourceMaterial*)App->resManager->GetByName(DEFAULTMAT, TYPE::MATERIAL);
	}

	// Look for player and his BBox
	player = App->scene->FindGameObjectByTag(playerTag.c_str());
	if (player == nullptr)
	{
		LOG("The Player GO with tag %s couldn't be found \n", playerTag.c_str());
	}
	else
	{
		playerHitBox = player->GetComponent<ComponentBoxTrigger>();
		if (playerHitBox == nullptr)
		{
			LOG("The GameObject %s has no ComponentTrigger attached \n", player->name.c_str());
		}

		playerMovement = (PlayerMovement*)player->GetComponentInChildren(ComponentType::Script);
	}

	// Look for Component Animation of the enemy
	anim = (ComponentAnimation*)gameobject->GetComponentInChildren(ComponentType::Animation);
	if (anim == nullptr)
	{
		LOG("No child of the GameObject %s has an Animation component attached \n", gameobject->name.c_str());
	}

	GameObject* damageGO = App->scene->FindGameObjectByName("Damage");
	if (damageGO == nullptr)
	{
		LOG("Damage controller GO couldn't be found \n");
	}
	else
	{
		damageController = damageGO->GetComponent<DamageController>();
		if (damageController == nullptr)
		{
			LOG("Damage controller couldn't be found \n");
		}
	}

	GameObject* enemyLifeGO = App->scene->FindGameObjectByName("EnemyLife");
	if (enemyLifeGO == nullptr)
	{
		LOG("Enemy controller GO couldn't be found \n");
	}
	else
	{
		enemyLifeBar = enemyLifeGO->GetComponent<EnemyLifeBarController>();
		if (enemyLifeBar == nullptr)
		{
			LOG("Damage controller couldn't be found \n");
		}
	}

	hpBoxTrigger = (ComponentBoxTrigger*)gameobject->GetComponentInChildren(ComponentType::BoxTrigger);
	if (hpBoxTrigger == nullptr)
	{
		LOG("No child of the GameObject %s has a boxTrigger component attached \n", gameobject->name.c_str());
	}

	GameObject* attackGameObject = App->scene->FindGameObjectByName("HitBoxAttack", gameobject);
	//assert(attackGameObject != nullptr);
	if(attackGameObject != nullptr)
	{
		attackBoxTrigger = (ComponentBoxTrigger*)attackGameObject->GetComponentInChildren(ComponentType::BoxTrigger);
		if (attackBoxTrigger == nullptr)
		{
			LOG("No child of the GameObject %s has a boxTrigger component attached \n", attackGameObject->name.c_str());
		}
		else
		{
			attackBoxTrigger->Enable(false);
		}
	}

	// Hit sound
	GameObject* GO = App->scene->FindGameObjectByName("enemy_got_hit");
	if (GO != nullptr)
	{
		enemy_got_hit = GO->GetComponent<ComponentAudioSource>();
		assert(enemy_got_hit != nullptr);
	}
	else
	{
		LOG("Warning: enemy_got_hit game object not found");
	}

	// Look for LootDropScript
	lootDrop = gameobject->GetComponent<LootDropScript>();
}

void EnemyControllerScript::Update()
{
	math::float3 closestPoint;
	fPoint mouse_point = App->input->GetMousePosition();
	math::float2 mouse = { mouse_point.x, mouse_point.y };
	std::list<GameObject*> intersects = App->scene->SceneRaycastHit(mouse);
	GameObject* objectMesh = this->myMesh;

	if (playerMovement->isPlayerDead) return;

	if (bossFightStarted)
	{
		if (gameobject->tag == "Boss")
		{
			playerMovement->ThirdStageBoss = ThirdStageBoss;
		}

		enemyLifeBar->SetLifeBar(maxHealth, actualHealth, EnemyLifeBarType(enemyLevel), "Santa Muerte");
		//in case boss third stage, highlighting works differently
		if (ThirdStageBoss)
		{
			objectMesh = App->scene->FindGameObjectByName("mesh", gameobject);
		}
	}
	
	auto mesh = std::find(intersects.begin(), intersects.end(), objectMesh);
	if(mesh != std::end(intersects) && *mesh == objectMesh)
	{
		// Show enemy lifebar
		if (enemyLifeBar != nullptr)
		{
			switch (enemyType)
			{
			case EnemyType::SKELETON:	enemyLifeBar->SetLifeBar(maxHealth, actualHealth, EnemyLifeBarType(enemyLevel), "Skeleton");	break;
			case EnemyType::MINER:		enemyLifeBar->SetLifeBar(maxHealth, actualHealth, EnemyLifeBarType(enemyLevel), "Miner"); 		break;
			case EnemyType::SORCERER:	enemyLifeBar->SetLifeBar(maxHealth, actualHealth, EnemyLifeBarType(enemyLevel), "Sorcerer");	break;
			case EnemyType::SPINNER:	enemyLifeBar->SetLifeBar(maxHealth, actualHealth, EnemyLifeBarType(enemyLevel), "Spinner"); 	break;
			case EnemyType::BANDOLERO:	enemyLifeBar->SetLifeBar(maxHealth, actualHealth, EnemyLifeBarType(enemyLevel), "Bandolero");	break;
			}		
		}

		if (myRenders.size() > 0u && !isDead)
		{
			for (std::vector<ComponentRenderer*>::iterator it = myRenders.begin(); it != myRenders.end(); ++it)
				(*it)->highlighted = true;
		}

		//we need to keep track of current targeted enemy
		App->scene->enemyHovered.object = gameobject;
		App->scene->enemyHovered.health = actualHealth;
		ComponentBoxTrigger* enemyTriggerBox = gameobject->GetComponent<ComponentBoxTrigger>();
		if (enemyTriggerBox)
		{
			App->scene->enemyHovered.triggerboxMinWidth = enemyTriggerBox->getShortestDistObb();

		}
		else if(gameobject->tag == "Boss")
		{
			GameObject* hitboxGO = App->scene->FindGameObjectByName("Hitbox", gameobject);
			enemyTriggerBox = hitboxGO->GetComponent<ComponentBoxTrigger>();
			App->scene->enemyHovered.triggerboxMinWidth = enemyTriggerBox->getShortestDistObb();

			//if in phase 3, we change hitbox target
			if (ThirdStageBoss)
			{
				App->scene->enemyHovered.object = hitboxGO;
				
				//check if mouse actually intersects with the bbox
				math::LineSegment ray = App->scene->SceneRaycast(mouse);
				
				//if the ray is directly pointing to the boss, go
				const char* bossFloortag = "bossFloor";
				GameObject* firstMeshFloor = App->scene->FindGameObjectByTag(bossFloortag, App->scene->root);

				//we also check mouse not pointing to the floor
				std::list<GameObject*> intersects = App->scene->SceneRaycastHit(mouse);
				auto mesh = std::find(intersects.begin(), intersects.end(), firstMeshFloor);

				//if not intersecting with boss trigger box OR intersecting with floor, 
				//we don't want to target the boss
				math::float3 intPoint;
				GameObject* tempObject;
				if (!ray.Intersects(*enemyTriggerBox->GetOBB()) ||
					App->scene->Intersects(bossFloortag, false, intPoint, &tempObject))
				{
					App->scene->enemyHovered.object = nullptr;
					App->scene->enemyHovered.health = 0;
					App->scene->enemyHovered.triggerboxMinWidth = 0;

					//turn off highlights
					for (std::vector<ComponentRenderer*>::iterator it = myRenders.begin();
						it != myRenders.end();
						++it)
					{
						(*it)->highlighted = false;
					}
				}
			}
		}

		if (App->scene->enemyHovered.object != nullptr &&
			gameobject->UUID == App->scene->enemyHovered.object->UUID)
		{
			MouseController::ChangeCursorIcon(enemyCursor);
		}
	}
	else
	{
		if (myRenders.size() > 0u)
		{
			for (std::vector<ComponentRenderer*>::iterator it = myRenders.begin(); it != myRenders.end(); ++it)
				(*it)->highlighted = false;

			//if this is the enemy that was being targeted, we untarget it from the scene
			if (App->scene->enemyHovered.object != nullptr &&
				gameobject->UUID == App->scene->enemyHovered.object->UUID)
			{
				App->scene->enemyHovered.object = nullptr;
				App->scene->enemyHovered.health = 0;
				App->scene->enemyHovered.triggerboxMinWidth = 0;
				MouseController::ChangeCursorIcon(gameStandarCursor);
			}
		}
		//in case third stage boss
		if (ThirdStageBoss)
		{
			App->scene->enemyHovered.object = nullptr;
			App->scene->enemyHovered.health = 0;
			App->scene->enemyHovered.triggerboxMinWidth = 0;
		}
	}



	if (enemyHit && hitColorTimer > 0.f)
	{
		hitColorTimer -= App->time->gameDeltaTime;
	}
	else if (enemyHit)
	{
		// Set default material back to all meshes
		for (unsigned i = 0u; i < myRenders.size(); i++)
		{
			myRenders.at(i)->material = defaultMaterials.at(i);
		}
		enemyHit = false;
	}

	if (isDead && !lootDropped)
	{
		if (deathTimer > lootDelay)
		{
			if (lootDrop != nullptr)
			{
				// Generate a random number and if it is below the critical chance -> increase damage
				if ((rand() % 100u) < lootChance)
				{
					// If enemy has more than one item drop them in circle
					if (lootDrop->itemList.size() > 1)
						lootDrop->DropItemsInCircle(lootRadius);
					else
						lootDrop->DropItems();
				}	
			}
			lootDropped = true;
			
			//enabled = false;
		}
		else
		{
			deathTimer += App->time->gameDeltaTime;
		}
	}
  
	if (isDead && gameobject->tag.c_str() != "Boss" && currentWorldControllerScript != nullptr && !removedFromCrowd)
	{
		//remove the enemy from the crowd
		currentWorldControllerScript->RemoveEnemy(gameobject->UUID);
		removedFromCrowd = true;

		// Avoid disabling enemy before dropping loot
		//if (lootDrop == nullptr)
		//	enabled = false;
	}
}

void EnemyControllerScript::Expose(ImGuiContext* context)
{

	// Enemy Type
	const char* types[] = { "Skeleton", "Miner", "Sorcerer", "Spinner", "Bandolero" , "Boss"};
	if (ImGui::BeginCombo("Type", types[(int)enemyType]))
	{
		for (int n = 0; n < 6; n++)
		{
			bool isSelected = ((int)enemyType == n);
			if (ImGui::Selectable(types[n], isSelected) && (int)enemyType != n)
			{
				enemyType = (EnemyType)n;
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::SliderInt("Level", &enemyLevel, 1, 5);

	if (ImGui::InputInt("Health", &maxHealth))
	{
		actualHealth = maxHealth;
	}
	
	ImGui::InputInt("Experience", &experience);
	ImGui::InputInt("damage", &damage);

	ImGui::Separator();
	ImGui::Text("Player:");
	char* goName = new char[64];
	strcpy_s(goName, strlen(playerTag.c_str()) + 1, playerTag.c_str());
	ImGui::InputText("playerTag", goName, 64);
	playerTag = goName;
	delete[] goName;

	ImGui::Separator();
	ImGui::Text("Enemy cursor:");
	char* enemyCursorAux = new char[64];
	strcpy_s(enemyCursorAux, strlen(enemyCursor.c_str()) + 1, enemyCursor.c_str());
	ImGui::InputText("enemyCursor", enemyCursorAux, 64);
	enemyCursor = enemyCursorAux;
	delete[] enemyCursorAux;

	// Draw the name of every GO that has a ComponentRenderer 
	if (myRenders.size() > 0u)
	{
		ImGui::Text("Renderers:");
		for (std::vector<ComponentRenderer*>::iterator it = myRenders.begin(); it != myRenders.end(); ++it)
			ImGui::Text(((Component*)(*it))->gameobject->name.c_str());
	}
	ImGui::Separator();
	ImGui::Text("Loot Variables:");
	ImGui::DragFloat("Loot Delay", &lootDelay);
	ImGui::DragFloat("Loot Radius", &lootRadius);
	ImGui::DragFloat("Loot Chance (%)", &lootChance, 1.0f, 0.0f, 100.f);
}

void EnemyControllerScript::Serialize(JSON_value* json) const
{
	assert(json != nullptr);
	json->AddInt("type", (int)enemyType);
	json->AddInt("level", enemyLevel);
	json->AddString("playerTag", playerTag.c_str());
	json->AddInt("health", maxHealth);
	json->AddInt("experience", experience);
	json->AddString("enemyCursor", enemyCursor.c_str());
	json->AddFloat("lootDelay", lootDelay);
	json->AddFloat("lootRadius", lootRadius);
	json->AddFloat("lootChance", lootChance);
	json->AddInt("damage", damage);
}

void EnemyControllerScript::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);
	enemyType = (EnemyType)json->GetInt("type");
	enemyLevel = json->GetInt("level");
	playerTag = json->GetString("playerTag", "Player");
	maxHealth = json->GetInt("health", maxHealth);
	experience = json->GetInt("experience", 20);
	actualHealth = maxHealth;
	enemyCursor = json->GetString("enemyCursor", "RedGlow.cur");
	lootDelay = json->GetFloat("lootDelay", 1.0f);
	lootRadius = json->GetFloat("lootRadius", 100.0f);
	lootChance = json->GetFloat("lootChance", 100.0f);
	damage = json->GetInt("damage", 5);
}

void EnemyControllerScript::TakeDamage(unsigned dmg, int type)
{
	if (!isDead)
	{
		float random = rand() % (int)(0.2 * 100);
		float offset = random / 100.f - 0.15;
		if (enemy_got_hit != nullptr)
		{
			enemy_got_hit->SetPitch(0.9 + offset);
			enemy_got_hit->Play();
		}
		enemyHit = true;
		if (actualHealth - dmg < 0 )
		{
			actualHealth = 0;
			gameobject->SetActive(false);
		}
		else
		{
			actualHealth -= dmg;
			// Set hit material to all enemy meshes
			for (unsigned i = 0u; i < myRenders.size(); i++)
			{
				myRenders.at(i)->material = hitMaterial;
			}
			hitColorTimer = hitColorDuration;
		}

		if (actualHealth <= 0)
		{
			isDead = true;

			if ((DamageType)type == DamageType::CRITICAL || playerMovement->IsExecutingSkill())
			{
				isDeadByCritOrSkill = true; //by default is false (Normal)
			}

			// Disable hit boxes
			hpBoxTrigger->Enable(false);
			
			if (attackBoxTrigger != nullptr)
			{
				attackBoxTrigger->Enable(false);
			}

			// Unhighlight
			if (myRenders.size() > 0u)
			{
				for (std::vector<ComponentRenderer*>::iterator it = myRenders.begin(); it != myRenders.end(); ++it)
					(*it)->highlighted = false;
			}

			// Spawn experience sphere
			GameObject* expSphere = App->scene->Spawn("ExpSphere", nullptr, gameobject->transform->position);
			if (expSphere != nullptr)
			{
				ExperienceSphereScript* expScript = expSphere->GetComponent<ExperienceSphereScript>();
				if (expScript != nullptr)
					expScript->experience = experience;
			}
		}
		damageController->AddDamage(gameobject->transform, dmg, (DamageType)type);
	}
}

inline math::float3 EnemyControllerScript::GetPosition() const
{
	assert(gameobject->transform != nullptr);
	return gameobject->transform->GetGlobalPosition();
}

inline math::Quat EnemyControllerScript::GetRotation() const
{
	assert(gameobject->transform != nullptr);
	return gameobject->transform->GetGlobalRotation();
}

inline math::float3 EnemyControllerScript::GetPlayerPosition() const
{
	assert(player->transform != nullptr);
	return player->transform->GetGlobalPosition();
}

inline void EnemyControllerScript::SetPosition(math::float3 newPos) const
{
	assert(gameobject->transform != nullptr);
gameobject->transform->SetGlobalPosition(newPos);
}

inline float EnemyControllerScript::GetDistanceTo(math::float3& position) const
{
	math::float3 enemyDistance = GetPosition();
	return enemyDistance.Distance(position);
}

inline float EnemyControllerScript::GetDistanceTo2D(math::float3& position) const
{
	math::float3 enemyDistance = GetPosition();
	enemyDistance.y = position.y;
	return enemyDistance.Distance(position);
}

inline float EnemyControllerScript::GetDistanceToPlayer2D() const
{
	math::float3 enemyPosition = GetPosition();
	math::float3 playerPosition = GetPlayerPosition();
	enemyPosition.y = playerPosition.y;
	return enemyPosition.Distance(playerPosition);
}

inline ComponentRenderer* EnemyControllerScript::GetMainRenderer() const
{
	if (myRenders.size() > 0u)
		return myRenders.at(0u);
	else
		return nullptr;
}

inline bool EnemyControllerScript::IsCollidingWithPlayer() const
{
	assert(myBbox != nullptr && playerHitBox != nullptr);
	return myBbox->Intersects(*playerHitBox->GetOBB());
}

void EnemyControllerScript::Move(float speed, math::float3& direction) const
{
	math::float3 movement = direction.Normalized() * speed * App->time->gameDeltaTime;
	gameobject->transform->SetPosition(gameobject->transform->GetPosition() + movement);
}

bool EnemyControllerScript::Move(float speed, float& refreshTime, math::float3 position, std::vector<float3>& path) const
{
	if (speed != currentSpeed)
	{
		//update our
		currentSpeed = speed;
		//now we change the corresponding agent's speed
		currentWorldControllerScript->changeVelocity(gameobject->UUID, currentSpeed);
	}
	currentWorldControllerScript->EnemyMoveRequest(gameobject->UUID, position);
	//gameobject->transform->LookAt(gameobject->transform->movingOrientation);
	return true;
}

bool EnemyControllerScript::IsIdle() const
{
	return currentWorldControllerScript->IsAgentIdle(gameobject->UUID);
}

bool EnemyControllerScript::IsStuck() const
{
	return currentWorldControllerScript->IsAgentStuck(gameobject->UUID);
}

void EnemyControllerScript::Stop()
{
	currentWorldControllerScript->StopAgent(gameobject->UUID);
}

void EnemyControllerScript::LookAt2D(const math::float3& position)
{
	math::float3 auxPos = position;
	auxPos.y = GetPosition().y;
	gameobject->transform->LookAtLocal(auxPos);
}

void EnemyControllerScript::OnTriggerEnter(GameObject* go)
{
	if (go == playerHitBox->gameobject)
	{
		auto overlaper = attackBoxTrigger->overlapList.find(playerHitBox);
		if (overlaper != attackBoxTrigger->overlapList.end() && overlaper->second == OverlapState::PostIdle)
		{

			playerMovement->Damage(damage);
		}
	}

	if (go->tag == "PlayerHitBoxAttack" || go->tag == "Machete" || go->tag == "SpinMacheteHitbox")
	{
		if (gameobject->tag.c_str() != "Boss")
		{
			// Get base damage
			int totalDamage = playerMovement->GetTotalPlayerStats().strength;

			// Add damage of the skill
			PlayerSkill* skill = playerMovement->GetSkillInUse();
			if (skill != nullptr)
				totalDamage *= skill->damage;

			// Generate a random number and if it is below the critical chance -> increase damage
			if ((rand() % 100u) < playerMovement->criticalChance)
			{
				TakeDamage(totalDamage * CRITICAL_DAMAGE, (int)DamageType::CRITICAL);
			}
			else
			{
				TakeDamage(totalDamage, (int)DamageType::NORMAL);
			}
		}
	}
}

void EnemyControllerScript::SetDefaultMaterialToCurrentMaterial()
{
	for (unsigned i = 0u; i < defaultMaterials.size(); i++)
	{
		defaultMaterials.at(i) = GetMainRenderer()->material;
	}
}
