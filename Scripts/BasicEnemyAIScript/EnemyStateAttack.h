#ifndef __ENEMYSTATEATTACK_H_
#define __ENEMYSTATEATTACK_H_

#include "EnemyState.h"
class ComponentTrail;
class ComponentAudioSource;

class EnemyStateAttack :
	public EnemyState
{
public:
	BasicEnemyAIScript_API EnemyStateAttack(BasicEnemyAIScript* AIScript);
	BasicEnemyAIScript_API ~EnemyStateAttack();

	void Enter() override;
	void Exit() override;
	void HandleIA() override;
	void Update() override;

	ComponentTrail* trailPunch = nullptr;
protected:
	BasicEnemyAIScript_API virtual void PunchFX(bool active);

	bool attacked = false;
	bool attackSoundMade = false;
	BasicEnemyAIScript_API virtual void Attack();
};

#endif __ENEMYSTATEATTACK_H_