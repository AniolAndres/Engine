#include "DamageFeedbackUI.h"
#include "Application.h"
#include "ModuleTime.h"
#include "GameObject.h"
#include "imgui.h"
#include "JSON.h"
#include <assert.h>

DamageFeedbackUI_API Script* CreateScript()
{
	DamageFeedbackUI* instance = new DamageFeedbackUI;
	return instance;
}

void DamageFeedbackUI::Awake()
{
	image = gameobject->GetComponent<ComponentImage>();
	assert(image != nullptr);
	gameobject->SetActive(false);
}

void DamageFeedbackUI::Update()
{
	if (!isDamaged) return;
	timer += App->time->gameDeltaTime;

	switch (fade)
	{
	case DamageFeedbackUI::IN:
		if (timer <= fadeInTime)
		{
			image->color.w =  timer / fadeInTime;
		}
		else
		{
			timer = 0.0f;
			fade = OUT;
		}
		break;
	case DamageFeedbackUI::OUT:
		if (timer <= fadeOutTime)
		{
			image->color.w = (fadeOutTime - timer) / fadeOutTime;
		}
		else
		{
			isDamaged = false;
			gameobject->SetActive(false);
			timer = 0.0f;
		}
		break;
	}
}

void DamageFeedbackUI::Expose(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ImGui::InputFloat("FadeInTime", &fadeInTime, 0.05f, 0.1f, 2);
	ImGui::InputFloat("FadeOutTime", &fadeOutTime, 0.05f, 0.1f, 2);
}

void DamageFeedbackUI::Serialize(JSON_value * json) const
{
	json->AddFloat("FadeInTime", fadeInTime);
	json->AddFloat("FadeOutTime", fadeOutTime);
}

void DamageFeedbackUI::DeSerialize(JSON_value * json)
{
	fadeInTime = json->GetFloat("FadeInTime", fadeInTime);
	fadeOutTime = json->GetFloat("FadeOutTime", fadeOutTime);
}

void DamageFeedbackUI::ActivateDamageUI()
{
	if (isDamaged && fade == IN)
	{
		if (timer > 0.5f*fadeInTime)
		{
			timer = 0.5f*fadeInTime;
			image->color.w = timer / fadeInTime;
		}
	}
	else
	{
		isDamaged = true;
		gameobject->SetActive(true);
		image->color.w = 0.0f;
		timer = 0.0f;
		fade = IN;
	}
}
