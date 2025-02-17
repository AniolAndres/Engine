#include "BaseScript.h"

#include "Application.h"
#include "GameObject.h"
#include "imgui.h"
#include "JSON.h"

#include "ModuleScript.h"
#include "ModuleTime.h"

Script::Script(const Script & script) : Component(script)
{
	name = script.name;
	gameobject = script.gameobject;
	App = script.App;

	App->scripting->AddScriptReference(this, name);
}

Script::Script() : Component(nullptr, ComponentType::Script)
{
}

Script::~Script()
{
}

Script * Script::Clone() const
{
	return new Script(*this);
}

void Script::Enable(bool enable)
{
	Component::Enable(enable);
	if (App->time->gameState == GameState::RUN && 
		!hasBeenStarted && gameobject->isActive())
	{
		Start();
		hasBeenStarted = true;
	}
}

bool Script::CleanUp()
{
	return App->scripting->RemoveScript(this, name);
}

void Script::SetGameObject(GameObject* go)
{
	gameobject = go;
}

void Script::SetGameStandarCursor(std::string gameStandarCursor)
{
	this->gameStandarCursor = gameStandarCursor;
}

void Script::DrawProperties()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader((name + "(Script)").c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Active", &enabled); ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f / 7.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f / 7.0f, 0.8f, 0.8f));

		bool removed = ImGui::Button("Remove");
		ImGui::PopStyleColor(3);
		if (removed)
		{
			gameobject->RemoveComponent(*this);
			ImGui::PopID();
			return;
		}

		ImGuiContext * context = ImGui::GetCurrentContext();
		Expose(context); //TODO: remove expose
	}
	ImGui::PopID();
}

void Script::SetApp(Application* app)
{
	this->App = app;
}

void Script::Load(JSON_value * value) 
{
	Component::Load(value);
	const char* retrievedName = value->GetString("script");
	name = retrievedName;
	JSON_value* scriptInfo = value->GetValue("scriptInfo");
	if (scriptInfo != nullptr)
	{
		DeSerialize(scriptInfo);
	}
}

void Script::Save(JSON_value * value) const
{
	Component::Save(value);
	value->AddString("script", name.c_str());
	JSON_value *scriptInfo = value->CreateValue();
	Serialize(scriptInfo);
	value->AddValue("scriptInfo", *scriptInfo);
}

void Script::Expose(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
}