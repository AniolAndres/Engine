#include "WorldControllerScript.h"

#include "Application.h"

#include "GameObject.h"

#include "ModuleNavigation.h"
#include "ModuleTime.h"

#include "ComponentTransform.h"

#include "Globals.h"

const math::float3 WorldControllerScript::defaultCorrection = math::float3(200.f, 200.f, 200.f);

WorldControllerScript_API Script* CreateScript()
{
	WorldControllerScript* instance = new WorldControllerScript;
	return instance;
}

WorldControllerScript::WorldControllerScript()
{
}

WorldControllerScript::~WorldControllerScript()
{
	//no need to delete crowdToolPointer since its a smart pointer and maganes itself
	objectAgentIndexMap.clear();
	clearAgentsInfo();
}

void WorldControllerScript::resetWorldAgents()
{
	crowdToolPointer.reset();//delete smart pointer calling the destructor
	crowdToolPointer = std::make_unique<crowdTool>();//create a new instance
	//reset our map aswell
	objectAgentIndexMap.clear();
	clearAgentsInfo();
	playerAgentsPosition = -1;
}

void WorldControllerScript::clearAgentsInfo()
{
	//clear agent objects deleteing all the pointers
	for (objVel objs : agentObjectsVelocity)
	{
		delete[] objs.second;
	}
	agentObjectsVelocity.clear();
	if (playerPos != nullptr)
	{
		delete[] playerPos;
		playerPos = nullptr;
	}
}

int WorldControllerScript::addAgent(float* pos, float* vel, float speed)
{
	return crowdToolPointer.get()->AddNewAgent(pos, vel);
}

void WorldControllerScript::setPlayer(GameObject* player, float speed)
{
	playerUID = player->UUID;

	//store info about the agent
	objVel newAg;
	newAg.first = player;
	newAg.second = new float[3];
	newAg.second[0] = 0.f; newAg.second[1] = 0.f; newAg.second[2] = 0.f;
	agentObjectsVelocity.push_back(newAg);
	playerAgentsPosition = agentObjectsVelocity.size() - 1;
	//if ouru player position is null, get a new one
	if (playerPos == nullptr)
	{
		playerPos = new float[3];
	}
	playerPos[0] = player->transform->position.x; playerPos[1] = player->transform->position.y; playerPos[2] = player->transform->position.z;
	int newAgentId = addAgent(playerPos, newAg.second, speed*10);
	//once we created the agent, save the info in our map
	objectAgentIndexMap[player->UUID] = newAgentId;
}

void WorldControllerScript::addEnemy(GameObject* enemy)
{
	//store info about the agent
	objVel newAg;
	newAg.first = enemy;
	newAg.second = new float[3];
	newAg.second[0] = 0.f; newAg.second[1] = 0.f; newAg.second[2] = 0.f;
	agentObjectsVelocity.push_back(newAg);

	int newAgentId = addAgent((float*)& enemy->transform->position, newAg.second);
	//once we created the agent, save the info in our map
	objectAgentIndexMap[enemy->UUID] = newAgentId;
}

bool WorldControllerScript::PlayerMoveRequest(unsigned int goID, math::float3 posY, math::float3 correction)
{
	//since its the player we have to do the ray tracing from the mouse and stuff
	//ray tracing to get the mesh point
	math::float3 destination;
	//if (!App->navigation->HighQualityMouseDetection(&destination))
	if (!App->navigation->FindIntersectionPoint(posY, destination))
	{
		LOG("could not find point intersecting the click ray during mesh collision");
		return false;
	}
	//now we get the polygon reference closest to the point
	unsigned int targetRef = 0u;
	if (!App->navigation->NavMeshPolygonQuery(&targetRef, &destination, correction))
	{
		LOG("could not find point intersecting the click ray during nav query");
		return false;
	}
	//we make the move request
	float* endPos = new float[3];
	endPos[0] = destination.x; endPos[1] = destination.y; endPos[2] = destination.z;
	//accquire the agent id
	int agentId = objectAgentIndexMap[goID];
	crowdToolPointer.get()->MoveRequest(agentId, targetRef, endPos);
	delete[] endPos;
	return true;

}
void WorldControllerScript::EnemyMoveRequest(unsigned int agentId, math::float3 dest, math::float3 correction)
{

	unsigned int targetRef = 0u;
	if (!App->navigation->NavMeshPolygonQuery(&targetRef, &dest, correction))
	{
		LOG("could not find point intersecting the click ray during nav query");
		return;
	}
	int enemyCrowdId = objectAgentIndexMap[agentId];

	crowdToolPointer.get()->MoveRequest(enemyCrowdId, targetRef, (float*)&dest);
}

void WorldControllerScript::Update()
{
	//if we got a player as agent, we 
	if (playerAgentsPosition != -1)
	{
		math::float3 pos = agentObjectsVelocity[playerAgentsPosition].first->transform->position;
		playerPos[0] = pos.x;
		playerPos[1] = pos.y;
		playerPos[2] = pos.z;
	}
	crowdToolPointer.get()->UpdateCrowd(App->time->gameDeltaTime);
	for (objVel agent : agentObjectsVelocity)
	{
		//only if not the player since it does not behave like an agent
		if (agent.first->UUID != playerUID)
		{
			//set moved
			agent.first->movedFlag = true;
			//orientate agent
			math::float3 tmpVel(agent.second[0], agent.second[1], agent.second[2]);
			//tell the agent that its moving towards that position, now if it wants it can look at the position
			//agent.first->transform->movingOrientation = agent.first->transform->position + tmpVel;

			//math::float3 tmpVel(agent.second[0], agent.second[1], agent.second[2]);
			if (!tmpVel.IsZero())
			{
				agent.first->transform->UpdateLocalTransform();
				agent.first->transform->UpdateGlobalTransform();
				agent.first->transform->LookAt(agent.first->transform->GetGlobalPosition() + tmpVel);
			}
		}
	}
}

void WorldControllerScript::changeVelocity(unsigned int goID, float newVel)
{
	int agentId = objectAgentIndexMap[goID];
	crowdToolPointer.get()->ChangeVelocity(agentId, newVel);
}

void WorldControllerScript::StopAgent(unsigned goID)
{
	int agentId = objectAgentIndexMap[goID];
	crowdToolPointer.get()->StopAgent(agentId);
}

bool WorldControllerScript::IsAgentRequesting(unsigned goID)
{
	int agentId = objectAgentIndexMap[goID];
	return crowdToolPointer.get()->IsAgentRequesting(agentId);
}

bool WorldControllerScript::IsAgentIdle(unsigned goID)
{
	int agentId = objectAgentIndexMap[goID];
	return crowdToolPointer.get()->IsAgentIdle(agentId);
}

bool WorldControllerScript::IsAgentStuck(unsigned goID)
{
	int agentId = objectAgentIndexMap[goID];
	return crowdToolPointer.get()->IsAgentStuck(agentId);
}

GameObject* WorldControllerScript::DeleteEnemiesFromWorld()
{
	//first we get the gameobject of the player
	GameObject* playerObj = nullptr;
	for (objVel obj : agentObjectsVelocity)
	{
		//in case player
		if (obj.first->UUID == playerUID)
		{
			playerObj = obj.first;
		}
		else
		{
			//in case enemy we delete the object
			obj.first->deleteFlag = true;
		}
	}
	//then we proceed to clear crowd
	resetWorldAgents();
	return playerObj;
}

void WorldControllerScript::RemoveEnemy(unsigned enemyID)
{
	for (int i = 0; i < agentObjectsVelocity.size(); ++i)
	{
		if (agentObjectsVelocity[i].first->UUID == enemyID)
		{
			//enemy found, lets free it

			//need to stop it first
			StopAgent(enemyID);

			//first we free its velocity vector
			delete[]agentObjectsVelocity[i].second;

			//now we delete it from the crowd
			crowdToolPointer.get()->DeleteAgent(objectAgentIndexMap[enemyID]);

			//delete the mapping of the enemy if we have it
			if (objectAgentIndexMap.find(enemyID) != objectAgentIndexMap.end())
			{
				objectAgentIndexMap.erase(enemyID);
			}

			//finally, lets delete it
			agentObjectsVelocity.erase(agentObjectsVelocity.begin() + i);
			return;
		}
	}
}
