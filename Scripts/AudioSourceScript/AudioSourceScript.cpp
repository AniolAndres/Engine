#include "AudioSourceScript.h"

#include "soloud.h"
#include "soloud_wav.h"
#include "soloud_wavstream.h"

#include "Application.h"

#include "ModuleInput.h"

#include "ComponentAudioSource.h"
#include "GameObject.h"

AudioSourceScript_API Script* CreateScript()
{
	AudioSourceScript* instance = new AudioSourceScript;
	return instance;
}

void AudioSourceScript::Start()
{
	audioSource = gameobject->GetComponent<ComponentAudioSource>();
	if (audioSource == nullptr) LOG("The GameObject %s has no Audio Source component attached \n", gameobject->name);
}

void AudioSourceScript::Update()
{
	if (audioSource != nullptr) 
	{
		//Test
		if (App->input->IsKeyPressed(SDL_SCANCODE_A)) 
		{
			audioSource->Play();
		}

		if (App->input->IsKeyPressed(SDL_SCANCODE_T)) 
		{
			audioSource->SetVolume(audioSource->volume + 0.01);
		}

		if (App->input->IsKeyPressed(SDL_SCANCODE_G)) 
		{
			audioSource->SetVolume(audioSource->volume - 0.01);
		}
	}
}
