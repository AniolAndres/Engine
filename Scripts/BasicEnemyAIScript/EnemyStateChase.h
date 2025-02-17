#ifndef __ENEMYSTATECHASE_H_
#define __ENEMYSTATECHASE_H_

#include "EnemyState.h"

class ComponentAudioSource;
class EnemyStateChase :
	public EnemyState
{
public:
	BasicEnemyAIScript_API EnemyStateChase(BasicEnemyAIScript* AIScript);
	BasicEnemyAIScript_API ~EnemyStateChase();

	void HandleIA() override;
	void Enter() override;
	void Exit() override;
	void Update() override;
private:
	math::float3 positionGoingTowards = math::float3(0.f,0.f,0.f);
	ComponentAudioSource* inRangeMoan = nullptr;
	float timer = 1.0f;
	float walkTimer = 0.55f;
};

#endif __ENEMYSTATECHASE_H_