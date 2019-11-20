#ifndef  __BehaviourTreeScript_h__
#define  __BehaviourTreeScript_h__

#include "BaseScript.h"

#include <vector>

#ifdef BehaviourTreeScript_EXPORTS
#define BehaviourTreeScript_API __declspec(dllexport)
#else
#define BehaviourTreeScript_API __declspec(dllimport)
#endif

class Node;

class BehaviourTreeScript_API BehaviourTreeScript : public Script
{
	void Awake() override;
	void Start() override;

	Node* RootNode = nullptr;
};

extern "C" BehaviourTreeScript_API Script* CreateScript();

#endif __BehaviourTreeScript_h__

// I need a resource and at the start I'll take it and create all the nodes and stuff