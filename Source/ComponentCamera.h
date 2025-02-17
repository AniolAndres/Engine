#ifndef __ComponentCamera_h__
#define __ComponentCamera_h__

#include "Component.h"
#include "Geometry/LineSegment.h"

#define ZNEARDIST .1f
#define ZFARDIST 1000.f

class ComponentCamera :
	public Component
{
public:
	ComponentCamera();
	ComponentCamera(GameObject* gameobject);
	ComponentCamera(const ComponentCamera &copy);
	~ComponentCamera();

	void Update() override;
	void DrawProperties() override;
	ENGINE_API void SetAsMain();
	void Save(JSON_value* value) const override;
	void Load(JSON_value* value) override;

	void Paste() override;
	void Reset() override;

	ComponentCamera* Clone() const;
	void Center();
	void Move(math::float3 dir); 
	void Rotate(float dx, float dy);
	void Zoom(float mouseWheel, bool shiftPressed);
	void Orbit(float dx, float dy);
	void SetAspect(float aspect);
	ENGINE_API void SetView(float fov);
	ENGINE_API void SetFov(float fov);
	ENGINE_API float GetFOV();
	ENGINE_API void SetFogColor(math::float3 newColor);

	void ResetFrustum();

	math::float4x4 GetViewMatrix() const;
	math::float4x4 GetProjectionMatrix() const;
	math::LineSegment DrawRay(float x, float y) const;

private:
	void CenterBbox(const math::AABB& bbox);
	void InitFrustum();
	void LookAt(math::float3 target);

public:
	Frustum* frustum = nullptr; 

	float movementSpeed = 5.0f;
	float rotationSpeed = 200.0f;
	float zoomSpeed = 0.4f;

	bool isMainCamera = false;
	float oldAspect = 0.f;
	bool aspectDirty = false;


	float fogFalloff = 1.f;
	float fogQuadratic = 1.f;
	float maxFog = .8f;
	math::float3 fogColor = math::float3::one;
	bool fogEnabled = false;
};

#endif __ComponentCamera_h__
