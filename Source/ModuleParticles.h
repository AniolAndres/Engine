#ifndef __ModuleParticles_h__
#define __ModuleParticles_h__

#include "Module.h"
#include <list>
#include "Math/float3.h"

class ComponentParticles;
class ComponentCamera;
class Shader;
class ComponentTrail;

#define MAX_TRAIL_VERTICES	3333 // 1111 tris
#define MAX_PARTICLES		2000

class ParticleModule;
class PMSizeOverTime;
class ImGradient;
class ImGradientMark;

class ModuleParticles :
	public Module
{
public:

	enum class BlendModes
	{
		ADDITIVE,
		ALPHA
	};

	//ModuleParticles();
	~ModuleParticles();

	bool Start() override;
	bool CleanUp() override;

	void AddParticleSystem(ComponentParticles* cp);
	void RemoveParticleSystem(ComponentParticles * cp);
	void AddTrailRenderer(ComponentTrail* cr);
	void RemoveTrailRenderer(ComponentTrail* cr);

	void Reset();

	void DrawParticleSystem(ComponentParticles* cp, const ComponentCamera* camera) const;
	void RenderTrail(ComponentTrail* ct, const ComponentCamera* camera) const;

	std::list<ComponentTrail*> trails;
	std::list<ComponentParticles*> particleSystems;

private:

	unsigned billBoardVAO = 0u;
	unsigned billBoardVBO = 0u;
	unsigned billBoardInstanceVBO = 0u;
	unsigned billBoardEBO = 0u;

	unsigned trailVAO = 0u;
	unsigned trailVBO = 0u;
	unsigned trailEBO = 0u;

	Shader* shader = nullptr;
	Shader* trailShader = nullptr;

	float trailData[MAX_TRAIL_VERTICES];
};


class ParticleModule
{
public:

	enum class ParticleModulesType
	{
		SIZE_OVER_TIME,
		COLOR_OVER_TIME
	};

	bool enabled = false;
	virtual void Update() {};
	virtual void InspectorDraw() = 0;

	ParticleModulesType type;
};

class PMSizeOverTime : public ParticleModule
{
public:


	PMSizeOverTime() { type = ParticleModulesType::SIZE_OVER_TIME; };
	PMSizeOverTime(const PMSizeOverTime& sizeOverTime);

	PMSizeOverTime* Clone() const;

	inline float GetSize(float percent, float total);
	void InspectorDraw() override;

	float v[5] = { .0f, .0f, 1.f, 1.f };
};

class PMColorOverTime : public ParticleModule
{
public:
	PMColorOverTime();
	~PMColorOverTime();
	PMColorOverTime(const PMColorOverTime& colorOverTime);

	PMColorOverTime* Clone() const;

	ImGradient* Imgradient;

	//float3 color1 = math::float3::one;
	//float3 color2 = math::float3::one;

	ImGradientMark* gradient1;
	ImGradientMark* gradient2;


public:
	void InspectorDraw() override;
	void UpdateGradientPointers();


};
#endif __ModuleParticles_h__