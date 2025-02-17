#ifndef __Component_BoxTrigger_H__
#define __Component_BoxTrigger_H__

#include "Component.h"
#include <unordered_map>
#include "Math/float3.h"
#include "Geometry/OBB.h"

enum class OverlapState
{
	Enter = 0,
	Idle,
	PostIdle,
	Exit
};

enum class BoxTriggerType
{
	Unknown = 0,
	PlayerHp,
	PlayerAttack,
	EnemyHp,
	EnemyAttack,
	Other
};

class ComponentBoxTrigger : public Component
{
public:
	ComponentBoxTrigger();
	ComponentBoxTrigger(GameObject* gameobject);
	ComponentBoxTrigger(const ComponentBoxTrigger& component);
	virtual ~ComponentBoxTrigger();

	virtual Component* Clone() const override;
	virtual void DrawProperties() override;

	virtual void Update() override;
	virtual void OnPlay() override;
	virtual void OnDisable() override;
	virtual void OnEnable() override;

	void DrawDebug();

	void SetBoxType(BoxTriggerType boxType);
	inline BoxTriggerType GetBoxTriggerType() const { return boxType; }

	inline const math::OBB* GetOBB() const { return boxTrigger; }

	inline math::float3 GetBoxCenter() const { return position; }
	inline void SetBoxCenter(math::float3 center) { position = center; boxTrigger->pos = center; }
	inline void SetBoxPosition(float x, float y, float z) { position = math::float3(x, y, z); boxTrigger->pos = position; }

	inline math::float3 GetBoxSize() const { return size; }
	inline void SetBoxSize(math::float3 sizes) { size = sizes;  boxTrigger->r = sizes; }
	inline void SetBoxSize(float width, float height, float depth) { size = math::float3(width, height, depth); boxTrigger->r = size; }
	ENGINE_API inline float getShortestDistObb() { return MIN(fabsf(boxTrigger->Diagonal().x), fabsf(boxTrigger->Diagonal().z)); };
	
	math::AABB getOOBBAsAABB();

	void AddOverlap(const ComponentBoxTrigger* other);
	void RemoveOverlap(const ComponentBoxTrigger* other);

	void Save(JSON_value* value) const;
	void Load(JSON_value* value);

private:
	void PropagateState(GameObject* other, OverlapState state);

public:
	std::unordered_map<const ComponentBoxTrigger*, OverlapState> overlapList;

private:
	math::OBB* boxTrigger = nullptr;

	math::float3 position = math::float3::zero;
	math::float3 size     = math::float3::one;

	BoxTriggerType boxType = BoxTriggerType::Unknown;

	bool debugDraw = false;
};

#endif // !__Component_BoxTrigger_H__

