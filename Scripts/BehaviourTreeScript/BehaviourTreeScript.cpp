#include "BehaviourTreeScript.h"

#include "Node.h"

#include "Application.h"
#include "ModuleEditor.h"
#include "HashString.h"

BehaviourTreeScript_API Script* CreateScript()
{
	BehaviourTreeScript* instance = new BehaviourTreeScript;
	return instance;
}

void BehaviourTreeScript::Awake()
{

}

void BehaviourTreeScript::Start()
{
}