#include "NPCController.h"

#include "Application.h"
#include "ModuleScene.h"

#include "GameObject.h"
#include "ComponentTransform.h"

#include "imgui.h"
#include "JSON.h"

NPCController_API Script* CreateScript()
{
	NPCController* instance = new NPCController;
	return instance;
}

void NPCController::Start()
{
	player = App->scene->FindGameObjectByName(playerName.c_str());
	playerBbox = &App->scene->FindGameObjectByName(playerBboxName.c_str(), player)->bbox;

	myBbox = &App->scene->FindGameObjectByName(myBboxName.c_str(), gameobject)->bbox;

	dialogueGO = App->scene->FindGameObjectByName(dialogueGOName.c_str(), App->scene->canvas);
	hudGO = App->scene->FindGameObjectByName("GameHUB");
}

void NPCController::Update()
{
	if (!dialogueGO->isActive())
	{
		// Check collision with player
		if (myBbox != nullptr && myBbox->Intersects(*playerBbox))
		{
			// Look at player
			gameobject->transform->LookAt(player->transform->position);

			// Open dialogue
			dialogueGO->SetActive(true);
			hudGO->SetActive(false);
		}
	}
	else
	{
		// Check collision with player
		if (myBbox != nullptr && !myBbox->Intersects(*playerBbox))
		{
			// Close dialogue
			dialogueGO->SetActive(false);
			hudGO->SetActive(true);
		}
	}
}

void NPCController::Expose(ImGuiContext* context)
{
	char* bboxName = new char[64];
	strcpy_s(bboxName, strlen(myBboxName.c_str()) + 1, myBboxName.c_str());
	ImGui::InputText("My BBox Name", bboxName, 64);
	myBboxName = bboxName;
	delete[] bboxName;

	char* dialogueName = new char[64];
	strcpy_s(dialogueName, strlen(dialogueGOName.c_str()) + 1, dialogueGOName.c_str());
	ImGui::InputText("Dialogue GO", dialogueName, 64);
	dialogueGOName = dialogueName;
	delete[] dialogueName;

	ImGui::Separator();
	ImGui::Text("Player:");
	char* goName = new char[64];
	strcpy_s(goName, strlen(playerName.c_str()) + 1, playerName.c_str());
	ImGui::InputText("playerName", goName, 64);
	playerName = goName;
	delete[] goName;

	char* targetBboxName = new char[64];
	strcpy_s(targetBboxName, strlen(playerBboxName.c_str()) + 1, playerBboxName.c_str());
	ImGui::InputText("Player BBox Name", targetBboxName, 64);
	playerBboxName = targetBboxName;
	delete[] targetBboxName;
}

void NPCController::Serialize(JSON_value* json) const
{
	assert(json != nullptr);
	json->AddString("playerName", playerName.c_str());
	json->AddString("playerBboxName", playerBboxName.c_str());
	json->AddString("myBboxName", myBboxName.c_str());
	json->AddString("dialogueGOName", dialogueGOName.c_str());
}

void NPCController::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);
	playerName = json->GetString("playerName");
	playerBboxName = json->GetString("playerBboxName");
	myBboxName = json->GetString("myBboxName");
	dialogueGOName = json->GetString("dialogueGOName");
}