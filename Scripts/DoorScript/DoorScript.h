#ifndef  __DoorScript_h__
#define  __DoorScript_h__

#include "BaseScript.h"
#include "Application.h"

#include "Geometry/AABB.h"

#ifdef DoorScript_EXPORTS
#define DoorScript_API __declspec(dllexport)
#else
#define DoorScript_API __declspec(dllimport)
#endif

class ComponentAnimation;
class ComponentRenderer;
class ComponentAudioSource;


class DoorScript_API DoorScript : public Script
{
	void Start() override;
	void Update() override;

	void Expose(ImGuiContext* context);

	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;


	inline virtual DoorScript* Clone() const
	{
		return new DoorScript(*this);
	}

private:

	GameObject* player = nullptr;
	std::string playerName = "Player";
	std::string playerBboxName = "PlayerMesh";
	std::string myBboxName1 = "DoorMesh1";
	std::string myBboxName2 = "DoorMesh2";

	// BBoxes
	math::AABB* myBbox1 = nullptr;
	math::AABB* myBbox2 = nullptr;
	math::AABB* playerBbox = nullptr;

	ComponentAnimation* anim = nullptr;
	ComponentRenderer* renderer1 = nullptr;
	ComponentRenderer* renderer2 = nullptr;

	//Audio
	ComponentAudioSource* open_gate = nullptr;
	bool activated = false;

	bool opened = false;
};

extern "C" DoorScript_API Script* CreateScript();

#endif __DoorScript_h__
