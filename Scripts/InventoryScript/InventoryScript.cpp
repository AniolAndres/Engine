#include "InventoryScript.h"
#include "JSON.h"

#include "Application.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleUI.h"

#include "ComponentImage.h"
#include "ComponentTransform2D.h"
#include "ComponentAudioSource.h"
#include "ComponentText.h"
#include "ComponentTransform.h"

#include "PlayerMovement.h"
#include "../ItemPicker/ItemPicker.h"

#include "GameObject.h"
#include "Viewport.h"
#include "PlayerPrefs.h"

InventoryScript_API Script* CreateScript()
{
	InventoryScript* instance = new InventoryScript;
	return instance;
}

InventoryScript::~InventoryScript()
{
	for (auto x : items)
	{
		delete x.first;
		x.first = nullptr;
	}
	items.clear();
}

void InventoryScript::Awake()
{

}

void InventoryScript::Start()
{
	inventory = App->scene->FindGameObjectByName("Inventory");
	GameObject* slots = App->scene->FindGameObjectByName("InventorySlots", inventory);
	assert(slots != nullptr);
	slotsTransform = slots->GetComponentsInChildren(ComponentType::Transform2D);

	slotsTransform.emplace_back(App->scene->FindGameObjectByName("HelmetSlot", inventory)->GetComponent<Transform2D>());
	slotsTransform.emplace_back(App->scene->FindGameObjectByName("BodySlot", inventory)->GetComponent<Transform2D>());
	slotsTransform.emplace_back(App->scene->FindGameObjectByName("PantsSlot", inventory)->GetComponent<Transform2D>());
	slotsTransform.emplace_back(App->scene->FindGameObjectByName("BootsSlot", inventory)->GetComponent<Transform2D>());
	slotsTransform.emplace_back(App->scene->FindGameObjectByName("WeaponSlot", inventory)->GetComponent<Transform2D>());
	slotsTransform.emplace_back(App->scene->FindGameObjectByName("AmuletSlot", inventory)->GetComponent<Transform2D>());

	std::list<GameObject*> list = App->scene->FindGameObjectByName("Items", inventory)->children;

	itemsSlots = { std::begin(list), std::end(list) }; //Pass gameObjects list to vector for better performance 

	GameObject* inventorySlotsNumbers = App->scene->FindGameObjectByName("InventorySlotsNumbers", inventory);
	if (inventorySlotsNumbers != nullptr)
	{
		std::list<GameObject*> slotsNumbersList = inventorySlotsNumbers->children;
		itemsSlotsNumbers = { std::begin(slotsNumbersList), std::end(slotsNumbersList) };
	}

	GameObject* backgroundImage = App->scene->FindGameObjectByName("BackgroundImage", inventory->parent);
	if (backgroundImage != nullptr)
	{
		menuPlayer = backgroundImage->GetComponent<Transform2D>();
		assert(menuPlayer != nullptr);
	}
	else
	{
		LOG("Warning: Background Image couldn't be found.")
	}


	GameObject* selectAudioGO = App->scene->FindGameObjectByName("SelectItemAudio");
	if (selectAudioGO != nullptr)
	{
		selectItemAudio = (ComponentAudioSource*)selectAudioGO->GetComponent<ComponentAudioSource>();
		assert(selectItemAudio != nullptr);
	}
	else
	{
		LOG("Warning: Select Item Audio couldn't be found.")
	}

	GameObject* dropAudioGO = App->scene->FindGameObjectByName("DropItemAudio");
	if (dropAudioGO != nullptr)
	{
		dropItemAudio = (ComponentAudioSource*)dropAudioGO->GetComponent<ComponentAudioSource>();
		assert(dropItemAudio != nullptr);
	}
	else
	{
		LOG("Warning: Drop Item Audio couldn't be found.")
	}

	// Player
	player = App->scene->FindGameObjectByName("Player");
	assert(player != nullptr);
	if (player != nullptr)
	{
		playerMovement = player->GetComponent<PlayerMovement>();
		assert(playerMovement != nullptr);
	}

	itemDesc = App->scene->FindGameObjectByName("ItemStats");

	LoadInventory();
}

void InventoryScript::Update()
{
	// All return paths should check for slots activation! (code as follows:)
	/* 
		for (GameObject* slot : slotsToActivate) slot->SetActive(true);
		slotsToActivate.clear();
	 */

	if (!inventory->isActive())
	{
		if (skill) App->scene->FindGameObjectByName("NewSkillPoint")->SetActive(true);
		skill = false;

		for (GameObject* slot : slotsToActivate) slot->SetActive(true);
		slotsToActivate.clear();
		return;
	}

	App->scene->FindGameObjectByName("NewItem")->SetActive(false);
	if (App->scene->FindGameObjectByName("NewSkillPoint")->isActive())
	{
		skill = true;
		App->scene->FindGameObjectByName("NewSkillPoint")->SetActive(false);
	}
	math::float2 mouse = reinterpret_cast<const float2&>(App->input->GetMousePosition());
	float screenX, screenY;

	if (App->renderer->viewGame != nullptr)
	{
		screenX = mouse.x - App->renderer->viewGame->winPos.x - (App->ui->currentWidth * .5f);
		screenY = mouse.y - App->renderer->viewGame->winPos.y - (App->ui->currentHeight * .5f);
	}
	else
	{
		screenX = mouse.x - (App->ui->currentWidth * .5f);
		screenY = mouse.y - (App->ui->currentHeight * .5f);
	}

	for (int i = 0; i < itemsSlots.size(); ++i)
	{
		if (!itemsSlots[i]->isActive()) continue;

		Transform2D* rectTransform = itemsSlots[i]->GetComponent<Transform2D>();
		ComponentImage* image = itemsSlots[i]->GetComponent<ComponentImage>();
		image->uiOrder = 5;

		if (image->isHovered && App->input->GetMouseButtonDown(1) == KEY_DOWN)
		{
			image->isPressed = true;
			initialitemPos = rectTransform->getPosition();
			if (!itemGrabbed) selectItemAudio->Play();
			itemGrabbed = true;
			HideConsumableItemText(i);
		}

		if (!image->isHovered && itemDesc->isActive() && imageHover == image)
		{
			itemDesc->SetActive(false);
			imageHover = nullptr;
		}

		if (image->isHovered && imageHover == nullptr /*&& i < INVENTARY_SLOTS*/)
		{
			for (int t = 0; t < items.size(); ++t)
			{
				if (items[t].second == i)
				{
					showDescription(t);
					break;
				}
			}
			imageHover = image;
		}
		
		if (image->isPressed)
		{
			math::float2 newPos = math::float2(screenX, -screenY);
			rectTransform->SetPositionUsingAligment(newPos);
			image->uiOrder = 6;
		}

		if (image->isPressed && App->input->GetMouseButtonDown(1) == KEY_UP)
		{
			bool moved = false;
			image->isPressed = false;
			itemGrabbed = false; //Audio

			math::float2 menuPos = menuPlayer->getPosition();
			math::float2 menuSize = menuPlayer->getSize();
			math::float2 menuMin = float2(menuPos.x - menuSize.x *.5f, -menuPos.y - menuSize.y *.5f);
			math::float2 menuMax = float2(menuPos.x + menuSize.x *.5f, -menuPos.y + menuSize.y *.5f);
			if (!(screenX > menuMin.x && screenX < menuMax.x && screenY > menuMin.y && screenY < menuMax.y))
			{
				// Drop Outside the menu
				rectTransform->SetPositionUsingAligment(initialitemPos);
				itemsSlots[i]->SetActive(false);
				itemDesc->SetActive(false);
				imageHover = nullptr;
		

				for (int j = 0; j < items.size(); ++j)
				{
					if (items[j].second == i)
					{
						GameObject* go = App->scene->FindGameObjectByUID(items[j].first->gameobjectUID);
						int amount = static_cast<int>(GetCurrentQuantity(*items[j].first));
						if (go) 
						{
							go->transform->SetGlobalPosition(player->transform->GetGlobalPosition());
							go->GetComponent<ItemPicker>()->amount = amount;
							go->SetActive(true);
						}

						if (items[j].first->isEquipped)
						{
							playerMovement->UnEquip((unsigned)items[j].first->type);
						}

						for (int h = 0; h < ASSIGNED_CONSUMABLES_SIZE; ++h)
						{
							if (items[j].first->name == assignedConsumableItem[h])
							{
								equipedConsumablesToRemove.emplace_back(h);
							}
						}

						ManageConsumableItemsQuantity(*items[j].first, -amount);
						items.erase(items.begin() + j);
						HideConsumableItemText(i);

						for (GameObject* slot : slotsToActivate) slot->SetActive(true);
						slotsToActivate.clear();
						return;
					}
				}
				for (GameObject* slot : slotsToActivate) slot->SetActive(true);
				slotsToActivate.clear();
				return;
			}

			// Get the item that was dropped in an inventory slot
			std::pair<Item*, int> pair;
			for (int t = 0; t < items.size(); ++t)
			{
				if (items[t].second == i)
				{
					pair = items[t];
					break;
				}
			}

			for (int j = 0; j < TOTAL_SLOTS; ++j)
			{
				math::float2 posSlot = ((Transform2D*)slotsTransform[j])->getPosition();
				math::float2 sizeSlot = ((Transform2D*)slotsTransform[j])->getSize();
				math::float2 slotMin = float2(posSlot.x - sizeSlot.x *.5f, -posSlot.y - sizeSlot.y *.5f);
				math::float2 slotMax = float2(posSlot.x + sizeSlot.x *.5f, -posSlot.y + sizeSlot.y *.5f);
				if (screenX > slotMin.x && screenX < slotMax.x && screenY > slotMin.y && screenY < slotMax.y)
				{
					// See if the item has been dropped inside player equip slots
					if (j >= INVENTARY_SLOTS)
					{
						bool exit = false;
						switch (j)
						{
						case 18:
							if (pair.first->type != ItemType::HELMET)
							{
								exit = true;
							}
							else
							{
								for (int l = 0; l < items.size(); ++l)
								{
									// Unequip previous item (if there was an item equipped)
									if (items[l].first != pair.first && items[l].first->isEquipped && items[l].first->type == ItemType::HELMET)
									{
										items[l].first->isEquipped = false;
										playerMovement->UnEquip((unsigned)items[l].first->type);
										break;
									}
								}
								for (int l = 0; l < items.size(); ++l)
								{
									// Equip new item
									if (items[l].first == pair.first && !items[l].first->isEquipped)
									{
										items[l].first->isEquipped = true;
										playerMovement->Equip((unsigned)pair.first->type, pair.first->meshUID, pair.first->materialUID);
										break;
									}
								}
							}
							break;
						case 19:
							if (pair.first->type != ItemType::CHEST)
							{
								exit = true;
							}
							else
							{
								for (int l = 0; l < items.size(); ++l)
								{
									// Unequip previous item (if there was an item equipped)
									if (items[l].first != pair.first && items[l].first->isEquipped && items[l].first->type == ItemType::CHEST)
									{
										items[l].first->isEquipped = false;
										playerMovement->UnEquip((unsigned)items[l].first->type);
										break;
									}
								}
								for (int l = 0; l < items.size(); ++l)
								{
									// Equip new item
									if (items[l].first == pair.first && !items[l].first->isEquipped)
									{
										items[l].first->isEquipped = true;
										playerMovement->Equip((unsigned)pair.first->type, pair.first->meshUID, pair.first->materialUID);
										break;
									}
								}
							}
							break;
						case 20:
							if (pair.first->type != ItemType::PANTS)
							{
								exit = true;
							}
							else
							{
								for (int l = 0; l < items.size(); ++l)
								{
									// Unequip previous item (if there was an item equipped)
									if (items[l].first != pair.first && items[l].first->isEquipped && items[l].first->type == ItemType::PANTS)
									{
										items[l].first->isEquipped = false;
										playerMovement->UnEquip((unsigned)items[l].first->type);
										break;
									}
								}
								for (int l = 0; l < items.size(); ++l)
								{
									// Equip new item
									if (items[l].first == pair.first && !items[l].first->isEquipped)
									{
										items[l].first->isEquipped = true;
										playerMovement->Equip((unsigned)pair.first->type, pair.first->meshUID, pair.first->materialUID);
										break;
									}
								}
							}
							break;
						case 21:
							if (pair.first->type != ItemType::BOOTS)
							{
								exit = true;
							}
							else
							{
								for (int l = 0; l < items.size(); ++l)
								{
									// Unequip previous item (if there was an item equipped)
									if (items[l].first != pair.first && items[l].first->isEquipped && items[l].first->type == ItemType::BOOTS)
									{
										items[l].first->isEquipped = false;
										playerMovement->UnEquip((unsigned)items[l].first->type);
										break;
									}
								}
								for (int l = 0; l < items.size(); ++l)
								{
									// Equip new item
									if (items[l].first == pair.first && !items[l].first->isEquipped)
									{
										items[l].first->isEquipped = true;
										playerMovement->Equip((unsigned)pair.first->type, pair.first->meshUID, pair.first->materialUID);
										break;
									}
								}
							}
							break;
						case 22:
							if (pair.first->type != ItemType::WEAPON)
							{
								exit = true;
							}
							else
							{
								for (int l = 0; l < items.size(); ++l)
								{
									// Unequip previous item (if there was an item equipped)
									if (items[l].first != pair.first && items[l].first->isEquipped && items[l].first->type == ItemType::WEAPON)
									{
										items[l].first->isEquipped = false;
										playerMovement->UnEquip((unsigned)items[l].first->type);
										break;
									}
								}
								for (int l = 0; l < items.size(); ++l)
								{
									// Equip new item
									if (items[l].first == pair.first && !items[l].first->isEquipped)
									{
										items[l].first->isEquipped = true;
										playerMovement->Equip((unsigned)pair.first->type, pair.first->meshUID, pair.first->materialUID);
										break;
									}
								}
							}
							break;
						case 23:
							if (pair.first->type != ItemType::AMULET)
							{
								exit = true;
							}
							else
							{
								for (int l = 0; l < items.size(); ++l)
								{
									// Unequip previous item (if there was an item equipped)
									if (items[l].first != pair.first && items[l].first->isEquipped && items[l].first->type == ItemType::AMULET)
									{
										items[l].first->isEquipped = false;
										playerMovement->UnEquip((unsigned)items[l].first->type);
										break;
									}
								}
								for (int l = 0; l < items.size(); ++l)
								{
									// Equip new item
									if (items[l].first == pair.first && !items[l].first->isEquipped)
									{
										items[l].first->isEquipped = true;
										playerMovement->Equip((unsigned)pair.first->type, pair.first->meshUID, pair.first->materialUID);
										break;
									}
								}
							}
							break;
						default:
							break;
						}

						if (exit)
						{
							int quantity = GetCurrentQuantity(*pair.first);
							ManageConsumableItemsQuantityText(*pair.first, quantity);
							break;
						}
					}
					else
					{
						bool slotOcupate = false;
						//dont unequip if there is some item in that slot
						for (int z = 0; z < items.size(); ++z)
						{
							if (items[z].second == j)
							{
								slotOcupate = true;
								break;
							}
						}
						if (slotOcupate)
						{
							break;
						}
						// It has been dropped inside an inventory slot
						for (int l = 0; l < items.size(); ++l)
						{
							if (items[l].first->isEqual(*pair.first) && pair.first->isEquipped)
							{
								items[l].first->isEquipped = false;
								playerMovement->UnEquip((unsigned)items[l].first->type);
								break;
							}
						}
					}

					moved = true;
					math::float2 newPos = math::float2(posSlot.x, posSlot.y);
					rectTransform->SetPositionUsingAligment(newPos);

					if (itemsSlots[j]->UUID != itemsSlots[i]->UUID)
					{
						itemsSlots[j]->GetComponent<Transform2D>()->SetPositionUsingAligment(initialitemPos);
						GameObject* tmp(std::move(itemsSlots[j]));
						itemsSlots[j] = std::move(itemsSlots[i]);
						itemsSlots[i] = std::move(tmp);
						tmp = nullptr;

						dropItemAudio->Play();

						HideConsumableItemText(i);
						HideConsumableItemText(j);

						for (int z = 0; z < items.size(); ++z)
						{
							if (items[z].second == i)
							{
								items[z].second = j;
								int quantity = GetCurrentQuantity(*items[z].first);
								ManageConsumableItemsQuantityText(*items[z].first, quantity);
								continue;
							}
							if (items[z].second == j)
							{
								items[z].second = i;
								int quantity = GetCurrentQuantity(*items[z].first);
								ManageConsumableItemsQuantityText(*items[z].first, quantity);
								continue;
							}
						}

					}
					else
					{
						for (int z = 0; z < items.size(); ++z)
						{
							if (items[z].second == i)
							{
								int quantity = GetCurrentQuantity(*items[z].first);
								ManageConsumableItemsQuantityText(*items[z].first, quantity);
								continue;
							}
						}
					}
					break;
				}
			}

			if (!moved)
			{
				rectTransform->SetPositionUsingAligment(initialitemPos);
				initialitemPos = math::float2::zero;

				for (GameObject* slot : slotsToActivate) slot->SetActive(true);
				slotsToActivate.clear();

				if (pair.first != nullptr)
				{
					int quantity = GetCurrentQuantity(*pair.first);
					ManageConsumableItemsQuantityText(*pair.first, quantity);
				}
				return;
			}

		}
	}


	for (GameObject* slot : slotsToActivate) slot->SetActive(true);
	slotsToActivate.clear();
}

bool InventoryScript::AddItem(Item item, unsigned amount)
{
	for (int i = 0; i < INVENTARY_SLOTS; i++)
	{
		if (!itemsSlots[i]->activeSelf)
		{
			bool found = false;
			for (GameObject* slot : slotsToActivate)
			{
				if (itemsSlots[i] == slot)
				{
					found = true;
					break;
				}
			}
			if (found) continue;

			int quantity = ManageConsumableItemsQuantity(item, amount);
			if (quantity <= amount)
			{
				//itemsSlots[i]->SetActive(true);
				slotsToActivate.emplace_back(itemsSlots[i]);
				ComponentImage* image = itemsSlots[i]->GetComponent<ComponentImage>();
				image->UpdateTexture(item.sprite);
				items.emplace_back(std::make_pair(new Item(item), i));
				App->scene->FindGameObjectByName("NewItem")->SetActive(true);
			}

			ManageConsumableItemsQuantityText(item, quantity);

			updateConsumableItemsQuantity = true;

			return true;
		}
	}
	return false;
}

std::vector<Item> InventoryScript::GetQuickItems()
{
	std::vector<Item> itemsToReturn;
	for (int i = 0; i < items.size(); ++i)
	{
		if (items[i].first->type == ItemType::QUICK)
		{
			itemsToReturn.emplace_back(*items[i].first);
		}
	}

	return itemsToReturn;
}

void InventoryScript::SaveInventory()
{
	JSON* inventory = new JSON();
	JSON_value *itemsJSON = inventory->CreateValue(rapidjson::kArrayType);
	for (int i = 0; i < items.size(); ++i)
	{
		JSON_value *item = itemsJSON->CreateValue();
		item->AddInt("type", (int)items[i].first->type);
		item->AddString("name", items[i].first->name.c_str());
		item->AddString("description", items[i].first->description.c_str());
		item->AddString("sprite", items[i].first->sprite.c_str());
		item->AddInt("equiped", items[i].first->isEquipped);
		item->AddUint("meshUID", items[i].first->meshUID);
		item->AddUint("materialUID", items[i].first->materialUID);
		item->AddUint("UID", items[i].first->gameobjectUID);
		item->AddFloat("dexterity", items[i].first->stats.dexterity);
		item->AddFloat("health", items[i].first->stats.health);
		item->AddFloat("hpRegen", items[i].first->stats.hpRegen);
		item->AddFloat("mana", items[i].first->stats.mana);
		item->AddFloat("manaRegen", items[i].first->stats.manaRegen);
		item->AddFloat("strength", items[i].first->stats.strength);
		item->AddInt("position", items[i].second);
		itemsJSON->AddValue("", *item);
	}
	inventory->AddValue("items", *itemsJSON);

	JSON_value *consumablesJSON = inventory->CreateValue(rapidjson::kArrayType);
	for (int i = 0; i < consumableItems.size(); ++i)
	{
		JSON_value *consumable = consumablesJSON->CreateValue();
		consumable->AddString("name", consumableItems[i].first.c_str());
		consumable->AddInt("quantity", consumableItems[i].second);
		consumablesJSON->AddValue("", *consumable);
	}
	inventory->AddValue("consumables", *consumablesJSON);

	PlayerPrefs::SaveJson(inventory, "Inventory");
}

void InventoryScript::LoadInventory()
{
	JSON* inventory = PlayerPrefs::LoadJson("Inventory");
	if (inventory != nullptr) {
		JSON_value* itemsJSON = inventory->GetValue("items");
		items.clear();
		for (unsigned i = 0; i < itemsJSON->Size(); i++)
		{
			JSON_value* itemJSON = itemsJSON->GetValue(i);
			int position = itemJSON->GetInt("position");
			Item* item = new Item;
			item->type = (ItemType)itemJSON->GetInt("type");
			item->name = itemJSON->GetString("name");
			item->description = itemJSON->GetString("description");
			item->sprite = itemJSON->GetString("sprite");
			item->isEquipped = itemJSON->GetInt("equiped");
			item->meshUID = itemJSON->GetUint("meshUID");
			item->materialUID = itemJSON->GetUint("materialUID");
			item->gameobjectUID = itemJSON->GetUint("UID");
			item->stats.dexterity = itemJSON->GetFloat("dexterity");
			item->stats.health = itemJSON->GetFloat("health");
			item->stats.hpRegen = itemJSON->GetFloat("hpRegen");
			item->stats.mana = itemJSON->GetFloat("mana");
			item->stats.manaRegen = itemJSON->GetFloat("manaRegen");
			item->stats.strength = itemJSON->GetFloat("strength");

			// Equip item mesh (if any)
			if (item->isEquipped && item->meshUID > 0)
				playerMovement->EquipMesh((unsigned)item->type, item->meshUID, item->materialUID);

			itemsSlots[position]->SetActive(true);
			ComponentImage* image = itemsSlots[position]->GetComponent<ComponentImage>();
			image->UpdateTexture(item->sprite);
			items.emplace_back(std::make_pair(item, position));
		}

		JSON_value* consumablesJSON = inventory->GetValue("consumables");
		consumableItems.clear();
		for (unsigned i = 0; i < consumablesJSON->Size(); i++)
		{
			JSON_value* consumableJSON = consumablesJSON->GetValue(i);
			consumableItems.emplace_back(std::make_pair(consumableJSON->GetString("name"), consumableJSON->GetInt("quantity")));
		}

		for (int i = 0; i < consumableItems.size(); ++i)
		{
			for (int j = 0; j < items.size(); ++j)
			{
				if (items[j].first->name == consumableItems[i].first)
				{
					itemsSlotsNumbers[items[j].second]->SetActive(true);
					Text* itemsSlotNumber = itemsSlotsNumbers[items[j].second]->GetComponent<Text>();
					itemsSlotNumber->text = std::to_string(consumableItems[i].second);
					itemsSlotNumber->uiOrder = 6;
					break;
				}
			}
		}
	}
}

void InventoryScript::showDescription(int i)
{
	PlayerStats previousStats = playerMovement->GetTotalPlayerStats();
	PlayerStats newStats = GetPlayerStatsOnItemEquip(i);

	// Show item stats
	ItemType type = items[i].first->type;
	int health = round(items[i].first->stats.health);
	int dex = round(items[i].first->stats.dexterity);
	int str = round(items[i].first->stats.strength);
	int mana = round(items[i].first->stats.mana);
	std::string name = items[i].first->name;
	App->scene->FindGameObjectByName("ItemStatsName", itemDesc)->GetComponent<Text>()->text = name;
	App->scene->FindGameObjectByName("ItemStatsHealthNum", itemDesc)->GetComponent<Text>()->text = std::to_string(health);
	App->scene->FindGameObjectByName("ItemStatsDexNum", itemDesc)->GetComponent<Text>()->text = std::to_string(dex);
	App->scene->FindGameObjectByName("ItemStatsStrNum", itemDesc)->GetComponent<Text>()->text = std::to_string(str);
	App->scene->FindGameObjectByName("ItemStatsSoulNum", itemDesc)->GetComponent<Text>()->text = std::to_string(mana);

	// Show total player stats on item equip
	Text* txt = App->scene->FindGameObjectByName("ItemStatsHealthDiff", itemDesc)->GetComponent<Text>();
	std::string healthDif = std::to_string(round(newStats.health));
	txt->text = healthDif.substr(0, healthDif.size() - 7);
	if (newStats.health > previousStats.health)
		txt->color = math::float4(0, 255, 0, 255);
	else if (newStats.health < previousStats.health)
		txt->color = math::float4(255, 0, 0, 255);
	else
		txt->color = math::float4(183, 153, 41, 255);
	txt = App->scene->FindGameObjectByName("ItemStatsDexDiff", itemDesc)->GetComponent<Text>();
	txt->text = std::to_string(newStats.dexterity);
	if (newStats.dexterity > previousStats.dexterity)
		txt->color = math::float4(0, 255, 0, 255);
	else if (newStats.dexterity < previousStats.dexterity)
		txt->color = math::float4(255, 0, 0, 255);
	else
		txt->color = math::float4(183, 153, 41, 255);
	txt = App->scene->FindGameObjectByName("ItemStatsStrDiff", itemDesc)->GetComponent<Text>();
	txt->text = std::to_string(newStats.strength);
	if (newStats.strength > previousStats.strength)
		txt->color = math::float4(0, 255, 0, 255);
	else if (newStats.strength < previousStats.strength)
		txt->color = math::float4(255, 0, 0, 255);
	else
		txt->color = math::float4(183, 153, 41, 255);
	txt = App->scene->FindGameObjectByName("ItemStatsSoulDiff", itemDesc)->GetComponent<Text>();
	std::string manaDif = std::to_string(round(newStats.mana));
	txt->text = manaDif.substr(0, manaDif.size() - 7);
	if (newStats.mana > previousStats.mana)
		txt->color = math::float4(0, 255, 0, 255);
	else if (newStats.mana < previousStats.mana)
		txt->color = math::float4(255, 0, 0, 255);
	else
		txt->color = math::float4(183, 153, 41, 255);
	itemDesc->SetActive(true);
}

int InventoryScript::ManageConsumableItemsQuantity(const Item& item, int value)
{
	if (item.type == ItemType::QUICK)
	{
		for (std::vector<std::pair<std::string, int>>::iterator pair = consumableItems.begin(); pair != consumableItems.end(); ++pair)
		{
			if (pair._Ptr->first == item.name)
			{
				pair._Ptr->second += value;
				return pair._Ptr->second;
			}
		}
		
		if (value > 0)
		{
			consumableItems.emplace_back(std::make_pair(item.name, value));
			return value;
		}
	}

	return 0;
}

int InventoryScript::GetItemIndexPosition(const Item& item)
{
	for (int i = 0; i < items.size(); ++i)
	{
		if (items[i].first->name == item.name)
		{
			return items[i].second;
		}
	}

	return -1;
}

int InventoryScript::GetCurrentQuantity(std::string itemName)
{
	for (int i = 0; i < consumableItems.size(); ++i)
	{
		if (consumableItems[i].first == itemName)
		{
			return consumableItems[i].second;
		}
	}

	return -1;
}

void InventoryScript::UseItemConsumableOnPlayer(int itemPosition)
{

	for (int i = 0; i < consumableItems.size(); ++i)
	{
		if (consumableItems[i].first == assignedConsumableItem[itemPosition] && consumableItems[i].second > 0)
		{
			for (int j = 0; j < items.size(); ++j)
			{
				if (items[j].first->name == consumableItems[i].first)
				{
					playerMovement->ConsumeItem(items[j].first->stats);
					consumableItems[i].second -= 1;
					if (consumableItems[i].second < 0)
					{
						consumableItems[i].second = 0;
					}
					ManageConsumableItemsQuantityText(*items[j].first, consumableItems[i].second);
					if (consumableItems[i].second == 0)
					{
						for (int x = j; x < INVENTARY_SLOTS; x++) 
						{
							if (itemsSlots[x]->activeSelf) 
							{
								itemsSlots[x]->SetActive(false);
								itemsSlotsNumbers[x]->SetActive(false);
								break;
							}
						}						
						items.erase(items.begin() + j);
						if(items.size() > 1)j--;
					}
				}
			}
		}
	}
}

int InventoryScript::GetCurrentQuantity(const Item& item)
{
	for (int i = 0; i < consumableItems.size(); ++i)
	{
		if (consumableItems[i].first == item.name)
		{
			int ret = consumableItems[i].second;
			if (ret == 0)
			{
				consumableItems.erase(consumableItems.begin() + i);
			}
			return ret;
		}
	}

	return -1;
}

void InventoryScript::AssignConsumableItem(const Item& item, int position)
{
	assignedConsumableItem[position] = item.name;
}

void InventoryScript::UnAssignConsumableItem(int position)
{
	assignedConsumableItem[position] = "";
}

void InventoryScript::HideConsumableItemText(int position)
{
	if (position < 18 && itemsSlotsNumbers[position] != nullptr)
	{
		itemsSlotsNumbers[position]->SetActive(false);
		Text* itemsSlotNumber = itemsSlotsNumbers[position]->GetComponent<Text>();
		if (itemsSlotNumber != nullptr)
		{
			itemsSlotNumber->text = std::to_string(1);
		}
	}
}

void InventoryScript::ManageConsumableItemsQuantityText(const Item& item, int quantity)
{
	if (item.type == ItemType::QUICK)
	{
		int itemPosition = GetItemIndexPosition(item);

		if (itemPosition != -1 && itemPosition <= INVENTARY_SLOTS)
		{
			itemsSlotsNumbers[itemPosition]->SetActive(true);
			Text* itemsSlotNumber = itemsSlotsNumbers[itemPosition]->GetComponent<Text>();
			itemsSlotNumber->text = std::to_string(quantity);
			itemsSlotNumber->uiOrder = 6;
		}
	}

}


std::string InventoryScript::ConsumeItemsController()
{
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_UP)
	{
		int itemQuantity = GetCurrentQuantity(assignedConsumableItem[HUD_BUTTON_1]);
		if (itemQuantity > 0)
		{
			UseItemConsumableOnPlayer(HUD_BUTTON_1);
		}

		return assignedConsumableItem[HUD_BUTTON_1];
	}
	else if (App->input->GetKey(SDL_SCANCODE_2) == KEY_UP)
	{
		int itemQuantity = GetCurrentQuantity(assignedConsumableItem[HUD_BUTTON_2]);
		if (itemQuantity > 0)
		{
			UseItemConsumableOnPlayer(HUD_BUTTON_2);
		}

		return assignedConsumableItem[HUD_BUTTON_2];
	}
	else if (App->input->GetKey(SDL_SCANCODE_3) == KEY_UP)
	{
		int itemQuantity = GetCurrentQuantity(assignedConsumableItem[HUD_BUTTON_3]);
		if (itemQuantity > 0)
		{
			UseItemConsumableOnPlayer(HUD_BUTTON_3);
		}

		return assignedConsumableItem[HUD_BUTTON_3];
	}
	else if (App->input->GetKey(SDL_SCANCODE_4) == KEY_UP)
	{
		int itemQuantity = GetCurrentQuantity(assignedConsumableItem[HUD_BUTTON_4]);
		if (itemQuantity > 0)
		{
			UseItemConsumableOnPlayer(HUD_BUTTON_4);
		}

		return assignedConsumableItem[HUD_BUTTON_4];
	}
	else if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_UP)
	{
		int itemQuantity = GetCurrentQuantity(assignedConsumableItem[HUD_BUTTON_Q]);
		if (itemQuantity > 0)
		{
			UseItemConsumableOnPlayer(HUD_BUTTON_Q);
		}

		return assignedConsumableItem[HUD_BUTTON_Q];
	}
	else if (App->input->GetKey(SDL_SCANCODE_W) == KEY_UP)
	{
		int itemQuantity = GetCurrentQuantity(assignedConsumableItem[HUD_BUTTON_W]);
		if (itemQuantity > 0)
		{
			UseItemConsumableOnPlayer(HUD_BUTTON_W);
		}

		return assignedConsumableItem[HUD_BUTTON_W];
	}
	else if (App->input->GetKey(SDL_SCANCODE_E) == KEY_UP)
	{
		int itemQuantity = GetCurrentQuantity(assignedConsumableItem[HUD_BUTTON_E]);
		if (itemQuantity > 0)
		{
			UseItemConsumableOnPlayer(HUD_BUTTON_E);
		}

		return assignedConsumableItem[HUD_BUTTON_E];
	}
	else if (App->input->GetKey(SDL_SCANCODE_R) == KEY_UP)
	{
		int itemQuantity = GetCurrentQuantity(assignedConsumableItem[HUD_BUTTON_R]);
		if (itemQuantity > 0)
		{
			UseItemConsumableOnPlayer(HUD_BUTTON_R);
		}

		return assignedConsumableItem[HUD_BUTTON_R];
	}

	return "";
}

PlayerStats InventoryScript::GetPlayerStatsOnItemEquip(int i) const
{
	PlayerStats newStats;
	newStats.health = playerMovement->baseStats.health + playerMovement->equipedStats.health;
	newStats.mana = playerMovement->baseStats.mana + playerMovement->equipedStats.mana;
	newStats.strength = playerMovement->baseStats.strength + playerMovement->equipedStats.strength;
	newStats.dexterity = playerMovement->baseStats.dexterity + playerMovement->equipedStats.dexterity;
	newStats.manaRegen = playerMovement->baseStats.manaRegen + playerMovement->equipedStats.manaRegen;
	newStats.hpRegen = playerMovement->baseStats.hpRegen + playerMovement->equipedStats.hpRegen;

	// Add new item to equip stats
	Item* item = items[i].first;
	newStats.health += item->stats.health;
	newStats.mana += item->stats.mana;
	newStats.dexterity += item->stats.dexterity;
	newStats.strength += item->stats.strength;
	newStats.hpRegen += item->stats.hpRegen;
	newStats.manaRegen += item->stats.manaRegen;

	// Substract stats of the item already equiped on the slot
	for (int j = 0; j < items.size(); ++j)
	{
		if (items[j].first->isEquipped && items[j].first->type == item->type)
		{
			Item* oldItem = items[j].first;
			newStats.health -= oldItem->stats.health;
			newStats.mana -= oldItem->stats.mana;
			newStats.dexterity -= oldItem->stats.dexterity;
			newStats.strength -= oldItem->stats.strength;
			newStats.hpRegen -= oldItem->stats.hpRegen;
			newStats.manaRegen -= oldItem->stats.manaRegen;
			return newStats;
		}
	}

	return newStats;
}