#include "Application.h"

#include "ModuleNavigation.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleTime.h"
#include "ModuleEditor.h"

#include "GameObject.h"
#include "ComponentCamera.h"
#include "ComponentTransform.h"

#include "ResourceTexture.h"

#include "Viewport.h"
#include "GL/glew.h"
#include "SDL_mouse.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "ImGuizmo.h"
#include "Geometry/LineSegment.h"
#include "Math/float4x4.h"
#include "Brofiler.h"

Viewport::Viewport(std::string name) : name(name) {}

Viewport::~Viewport()
{
	if (FBO != 0)
	{
		glDeleteFramebuffers(1, &FBO);
	}
	if (RBO != 0)
	{
		glDeleteRenderbuffers(1, &RBO);
	}
	if (texture != 0)
	{
		glDeleteTextures(1, &texture);
	}
	if (MSAAFBO != 0)
	{
		glDeleteFramebuffers(1, &MSAAFBO);
	}
	if (MSAADEPTH != 0)
	{
		glDeleteRenderbuffers(1, &MSAADEPTH);
	}
	if (MSAACOLOR != 0)
	{
		glDeleteRenderbuffers(1, &MSAACOLOR);
	}
}

void Viewport::Draw(ComponentCamera * cam, bool isEditor)
{
	PROFILE;
	if (enabled) 
	{
		ImGui::Begin(name.c_str(), &enabled, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus);

		//if the viewport is hidden, it is not rendered
		hidden = ImGui::GetCurrentWindow()->Hidden;
		if (hidden)
		{
			focus = false;
			hover = false;
			ImGui::End();
			return;
		}
		
		if (ImGui::IsAnyMouseDown() && ImGui::IsWindowHovered() || ImGui::IsWindowAppearing())
		{
			ImGui::SetWindowFocus();
		}
		focus = ImGui::IsWindowFocused();
		hover = ImGui::IsWindowHovered();

		if (cam == nullptr  || !isEditor && (!cam->enabled || !cam->gameobject->isActive()))
		{
			ImVec2 size = ImGui::GetWindowSize();
			size.x = MAX(size.x, 400);
			size.y = MAX(size.y, 400);
			
			if (App->scene->camera_notfound_texture != nullptr)
			{
				ImGui::Image((ImTextureID)App->scene->camera_notfound_texture->gpuID,
					size, { 0,1 }, { 1,0 });
				ImGui::End();
				return;
			}
			else
			{
				ImGui::End();
				return;
			}

		}

		ImVec2 size = ImGui::GetWindowSize();

		cam->SetAspect(size.x / size.y);

		if (cam->aspectDirty)
		{
			App->renderer->OnResize();
			CreateFrameBuffer(size.x, size.y);
		}

		current_width = size.x;
		current_height = size.y;

		glBindFramebuffer(GL_FRAMEBUFFER, FBO);

		App->renderer->Draw(*cam, current_width, current_height, isEditor);


		if (isEditor)
		{
			for (GameObject* go : App->scene->selection)
			{
				go->DrawBBox();
				if (go->light != nullptr)
				{
					go->light->DrawDebug();
				}
			}
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		ImGui::SetCursorPos({ 0,0 });
		ImVec2 pos = ImGui::GetWindowPos();
		winPos = reinterpret_cast<math::float2&>(pos);

		if (isEditor)
		{
			ImGui::Image((ImTextureID)texture, size, ImVec2(0, 1), ImVec2(1, 0));
			DrawImGuizmo(*cam);
			if (!ImGuizmo::IsUsing() && !ImGui::IsAnyItemHovered())
			{
				Pick();
				DebugNavigate();
			}
		}
		else
		{
			ImGui::Image((ImTextureID)App->renderer->renderedSceneGame, size, ImVec2(0, 1), ImVec2(1, 0));
		}
		ImGui::End();
	}
}

void Viewport::DrawGuizmoButtons()
{

	if ((ImGui::Button("Translate") || (App->input->IsKeyPressed(SDL_SCANCODE_W) && !App->editor->wantKeyboard)) && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_IDLE)
	{
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		mCurrentGizmoMode = mCurrentModeAux;
	}
	ImGui::SameLine();
	if (ImGui::Button("Rotate") || (App->input->IsKeyPressed(SDL_SCANCODE_E) && !App->editor->wantKeyboard))
	{
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
		mCurrentGizmoMode = mCurrentModeAux;

	}
	ImGui::SameLine();
	if (ImGui::Button("Scale") || (App->input->IsKeyPressed(SDL_SCANCODE_R) && !App->editor->wantKeyboard))
	{
		mCurrentGizmoOperation = ImGuizmo::SCALE;
		mCurrentGizmoMode = ImGuizmo::LOCAL;
	}
	if (mCurrentGizmoOperation == ImGuizmo::SCALE)
	{
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	ImGui::SameLine();
	if (mCurrentGizmoMode == ImGuizmo::WORLD)
	{
		if (ImGui::Button("Local"))
		{
			mCurrentGizmoMode = ImGuizmo::LOCAL;
			mCurrentModeAux = ImGuizmo::LOCAL;
		}
	}
	else
	{
		if (ImGui::Button("World"))
		{
			mCurrentGizmoMode = ImGuizmo::WORLD;
			mCurrentModeAux = ImGuizmo::WORLD;

		}
	}

	if (mCurrentGizmoOperation == ImGuizmo::SCALE)
	{
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	ImGui::SameLine();
	if (ImGui::Button("Snap"))
	{
		useSnap = !useSnap;
	}
	if (useSnap)
	{
		ImGui::Begin("SnapSettings");
		if (mCurrentGizmoOperation == ImGuizmo::TRANSLATE)
		{
			ImGui::InputFloat3("Snap Translation", (float*)&snapSettings);
		}
		else if (mCurrentGizmoOperation == ImGuizmo::ROTATE)
		{
			ImGui::InputFloat("Snap Angle", (float*)&snapSettings);
		}
		else
		{
			ImGui::InputFloat("Snap Scale", (float*)&snapSettings);
		}
		ImGui::End();
	}
}

void Viewport::CreateFrameBuffer(int width, int height)
{
	BROFILER_CATEGORY("Create FrameBuffer", Profiler::Color::Azure);
	
	if (width != current_width || height != current_height)
	{
		if (FBO == 0)
		{
			glGenFramebuffers(1, &FBO);
		}

		if (texture == 0)
		{
			glGenTextures(1, &texture);
		}

		glBindTexture(GL_TEXTURE_2D, texture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		if (depthTexture == 0)
		{
			glGenTextures(1, &depthTexture);
		}

		glBindTexture(GL_TEXTURE_2D, depthTexture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindFramebuffer(GL_FRAMEBUFFER, FBO);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

		glBindTexture(GL_TEXTURE_2D, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			LOG("Framebuffer ERROR");

	}
}

void Viewport::CreateMSAABuffers(int width, int height)  //deprecated
{
	if (MSAAFBO == 0)
	{
		glGenFramebuffers(1, &MSAAFBO);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, MSAAFBO);

	if (MSAADEPTH == 0)
	{
		glGenRenderbuffers(1, &MSAADEPTH);
	}
	glBindRenderbuffer(GL_RENDERBUFFER, MSAADEPTH);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, App->renderer->msaa_level, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, MSAADEPTH);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	if (MSAACOLOR == 0)
	{
		glGenRenderbuffers(1, &MSAACOLOR);
	}
	glBindRenderbuffer(GL_RENDERBUFFER, MSAACOLOR);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, App->renderer->msaa_level, GL_RGBA, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, MSAACOLOR);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG("Framebuffer ERROR");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Viewport::DrawImGuizmo(const ComponentCamera& cam)
{
	PROFILE;
	ImVec2 pos = ImGui::GetWindowPos();
	ImGuizmo::SetRect(pos.x, pos.y, current_width, current_height);
	ImGuizmo::SetDrawlist();

	ImGui::SetCursorPos({ 20,30 });
	DrawGuizmoButtons();

	if (App->scene->selected != nullptr && App->scene->selected != App->scene->root && ((ComponentTransform*)App->scene->selected->GetComponentOld(ComponentType::Transform)))
	{
		ImGuizmo::Enable(!App->scene->selected->isStatic || App->time->gameState == GameState::RUN);

		math::float4x4 model = App->scene->selected->GetGlobalTransform();
		math::float4x4 originalModel = model;
		math::float4x4 difference = math::float4x4::zero;

		math::float4x4 view = cam.GetViewMatrix();
		math::float4x4 proj = cam.GetProjectionMatrix();

		ImGuizmo::SetOrthographic(false);

		model.Transpose();

		ImGuizmo::Manipulate((float*)&view, (float*)&proj,
			(ImGuizmo::OPERATION)mCurrentGizmoOperation, (ImGuizmo::MODE)mCurrentGizmoMode,
			(float*)&model, NULL, useSnap ? (float*)&snapSettings : NULL, NULL, NULL);

		if (ImGuizmo::IsUsing())
		{
			if (!startImguizmoUse)
			{
				App->scene->TakePhoto();
				startImguizmoUse = true;
			}
			model.Transpose();
			App->scene->selected->SetGlobalTransform(model);
			difference = model - originalModel;
			App->scene->selected->transform->MultiSelectionTransform(difference); //checks if multi transform is required & do it
		}
		else
		{
			startImguizmoUse = false;
		}
	}
}

void Viewport::Pick()
{
	PROFILE;
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN && ImGui::IsWindowFocused() && ImGui::IsMouseHoveringWindow())
	{
		ImVec2 pos = ImGui::GetWindowPos();		
		float2 mouse((float*)&App->input->GetMousePosition());
		float normalized_x = ((mouse.x - pos.x) / current_width) * 2 - 1; //0 to 1 -> -1 to 1
		float normalized_y = (1 - (mouse.y - pos.y) / current_height) * 2 - 1; //0 to 1 -> -1 to 1
		App->scene->Pick(normalized_x, normalized_y);
	}
}

void Viewport::DebugNavigate() const
{
	if (App->input->GetMouseButtonDown(SDL_BUTTON_MIDDLE) == KEY_DOWN && ImGui::IsWindowFocused() && ImGui::IsMouseHoveringWindow())
	{
		math::float3 intersectionPoint = math::float3::inf;
		if (App->scene->Intersects(intersectionPoint, "floor", true))
		{
			App->navigation->RecalcPath(intersectionPoint);
		}
	}
}

void Viewport::ToggleEnabled() 
{
	enabled = !enabled;
}