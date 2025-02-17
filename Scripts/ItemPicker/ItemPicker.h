#ifndef  __ItemPicker_h__
#define  __ItemPicker_h__

#include "BaseScript.h"
#include "Item.h"
#include "Geometry/AABB.h"
#include <vector>
#include <list>
#include "Math\float4.h"

#ifdef ItemPicker_EXPORTS
#define ItemPicker_API __declspec(dllexport)
#else
#define ItemPicker_API __declspec(dllimport)
#endif

class InventoryScript;
class AABB;
class JSON_value;
class ComponentAudioSource;
class ComponentRenderer;
class ItemNameController;
class PlayerMovement;
class ResourceMesh;
class ResourceMaterial;

enum class ItemPicker_API ItemRarity
{
	BASIC = 0,
	RARE,
	EPIC,
	LEGENDARY
};

class ItemPicker_API ItemPicker : public Script
{
public:
	ItemPicker() : Script() {};
	ItemPicker(const ItemPicker& itemPicker);
	ItemPicker& operator=(const ItemPicker& itemPicker);

	inline virtual ItemPicker* Clone() const
	{
		return new ItemPicker(*this);
	}

	void SetItem(ItemType type, std::string name, std::string sprite);
	void PickupItem() const;

private:
	void Expose(ImGuiContext* context) override;

	void Start() override;
	void Update() override;

	math::float4 GetRarityColor();

	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;

	bool ItemIntersects();

public:
	std::string name;
	std::string description;
	std::string sprite;
	ItemType type = ItemType::NONE;
	Item* item = nullptr;
	bool pickedUpViaPlayer = false;
	unsigned amount = 1;

private:

	PlayerMovement* playerMovementScript = nullptr;
	InventoryScript* inventoryScript = nullptr;
	math::AABB* playerBbox = nullptr;

	std::vector<std::string> textureFiles;
	const char* selectedTexture = "basicPostion";

	ComponentAudioSource* itemPickedAudio = nullptr;
	std::string myBboxName;

	ComponentRenderer* myRender;
	ItemRarity rarity = ItemRarity::BASIC;
	int rare = 0;

	ItemNameController* itemName = nullptr;

	std::string itemCursor = "Pick.cur";
	bool changeItemCursorIcon = true;
	bool changeStandarCursorIcon = true;

	// Mesh to change to Player item (only weapons)
	ResourceMesh* itemMesh = nullptr;
	std::vector<std::string> meshesList;		// List of ResourceMeshes

	// Material to change to Player item (only weapons)
	ResourceMaterial* itemMaterial = nullptr;
	std::vector<std::string> materialsList;		// List of ResourceMeshes

	//Colors for highlights and item name
	math::float4 white = math::float4(255.0f, 255.0f, 255.0f, 255.0f);
	math::float4 green = math::float4(0.0f, 255.0f, 0.0f, 255.0f);
	math::float4 orange = math::float4(255.0f, 165.0f, 0.0f, 255.0f);
	math::float4 purple = math::float4(255.0f, 0.0f, 255.0f, 255.0f);

};

extern "C" ItemPicker_API Script* CreateScript();

#endif __ItemPicker_h__
