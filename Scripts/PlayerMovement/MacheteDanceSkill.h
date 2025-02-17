#ifndef  __MacheteDanceSkill_h__
#define  __MacheteDanceSkill_h__

#include "BasicSkill.h"
#include <vector>

class GameObject;

class MacheteDanceSkill :
	public BasicSkill
{
public:
	MacheteDanceSkill(PlayerMovement* PM, const char* trigger);
	~MacheteDanceSkill();

	void Start() override;
	void Prepare() override;
	void Update() override;
	void Exit() override;

	void RotateMachetes();

public:
	std::vector<GameObject*> spinMachetes;
	std::vector<GameObject*> spinTrails;
	float macheteRotationSpeed = 20.0f;

	float macheteDuration = 10.0f;
	float danceTimer = 0.0f;

	bool trailsActive = false;
};

#endif __MacheteDanceSkill_h__