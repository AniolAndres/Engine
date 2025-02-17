#ifndef __ENEMYSTATEHIDE_H_
#define __ENEMYSTATEHIDE_H_

#include "EnemyState.h"

class EnemyStateHide :
	public EnemyState
{
public:
	EnemyStateHide(HiddenEnemyAIScript* AIScript);
	~EnemyStateHide();

	void Exit() override;
	void HandleIA() override;
	void Update() override;
};

#endif __ENEMYSTATEHIDE_H_