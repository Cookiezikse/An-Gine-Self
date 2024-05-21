#include "Camera/CameraManager.h"

#include "Camera/BaseCamera.h"
/*#include "InputModule.hpp"
#include "StringBuilder.hpp"*/

#include <iostream>

namespace lve
{
	CameraManager::CameraManager() /*:
		m_ActionCallback(this, &CameraManager::OnActionEvent)*/
	{
	}

	CameraManager::~CameraManager()
	{
		while (!cameraStack.empty())
		{
			cameraStack.pop();
		}

		for (BaseCamera* camera : cameras)
		{
			camera->Finalize();
			delete camera;
		}
		cameras.clear();
	}

	void CameraManager::Init()
	{
		BaseCamera* camera = GetCurrentCamera();
		/*CHECK_NE(camera, nullptr);

		camera->Initialize();
		camera->OnPossess();

		g_InputModule->BindActionCallback(&m_ActionCallback, 12);*/

		bInitialized = true;
	}

	void CameraManager::Start()
	{
		Module::Start();
	}


	void CameraManager::FixedUpdate()
	{
		Module::FixedUpdate();
	}

	void CameraManager::Update()
	{
		Module::Update();
	}

	void CameraManager::PreRender()
	{
		Module::PreRender();
	}

	void CameraManager::Render()
	{
		Module::Render();
	}

	void CameraManager::RenderGui()
	{
		Module::RenderGui();
	}

	void CameraManager::PostRender()
	{
		Module::PostRender();
	}

	void CameraManager::Release()
	{
		Module::Release();
	}

	void CameraManager::Finalize()
	{
		while (!cameraStack.empty())
		{
			cameraStack.pop();
		}

		for (BaseCamera* camera : cameras)
		{
			camera->Finalize();
			delete camera;
		}
		cameras.clear();

		/*
		g_InputModule->UnbindActionCallback(&m_ActionCallback);*/

		bInitialized = false;
	}


	void CameraManager::OnPostSceneChange() const
	{
		for (BaseCamera* cam : cameras)
		{
			cam->OnPostSceneChange();
		}
	}

	BaseCamera* CameraManager::GetCurrentCamera() const
	{
		return cameraStack.top();
	}

	void CameraManager::AddCamera(BaseCamera* _camera, const bool _bSwitchTo)
	{
		/*CHECK_NE(camera, nullptr);*/

		if (const uint32_t camera_index = GetCameraIndex(_camera); camera_index == -1)
		// Only add camera if it hasn't been added before
		{
			cameras.push_back(_camera);

			if (_bSwitchTo) SetCamera(_camera, false);
		}
	}

	BaseCamera* CameraManager::SetCamera(BaseCamera* _camera, const bool _bAlignWithPrevious)
	{
		if (!cameraStack.empty()) BaseCamera* active_camera = GetCurrentCamera();
		/*active_camera->OnDepossess();
		active_camera->Destroy();*/

		while (!cameraStack.empty())
		{
			cameraStack.pop();
		}

		return PushCamera(_camera, _bAlignWithPrevious, true);
	}

	BaseCamera* CameraManager::CycleCamera(const uint32_t _deltaIndex, const bool _bAlignWithPrevious)
	{
		/*CHECK_EQ(glm::abs(deltaIndex), 1);*/

		const uint32_t num_cameras = static_cast<uint32_t>(cameras.size());

		const uint32_t desired_index = GetCameraIndex(cameraStack.top()) + _deltaIndex;
		uint32_t       new_index;
		uint32_t       offset = 0;
		do
		{
			new_index = desired_index + offset;
			offset += _deltaIndex;
			if (new_index < 0) new_index += num_cameras;
			if (new_index >= num_cameras) new_index -= num_cameras;
		}
		while (!cameras[new_index]->bDebugCyclable);

		return SetCamera(cameras[new_index], _bAlignWithPrevious);
	}

	BaseCamera* CameraManager::SetCameraByName(const std::string& _name, const bool _bAlignWithPrevious)
	{
		BaseCamera* cam = GetCameraByName(_name);
		if (cam == nullptr)
		{
			const std::string error_str = "Attempted to set camera with invalid name: " + _name + "\n";
			std::cout << error_str.c_str() << std::endl;
			return nullptr;
		}
		return SetCamera(cam, _bAlignWithPrevious);
	}

	BaseCamera* CameraManager::PushCamera(BaseCamera* _camera, const bool _bAlignWithPrevious, const bool _bInitialize)
	{
		if (_camera == nullptr)
		{
			std::cout << "Attempted to push null camera\n" << std::endl;
			return nullptr;
		}

		/*g_InputModule->ClearAllInputs();*/

		BaseCamera* pActiveCam = nullptr;
		if (!cameraStack.empty())
		{
			pActiveCam = cameraStack.top();

			pActiveCam->OnDepossess();
			pActiveCam->Finalize();
		}

		cameraStack.push(_camera);

		if (_bAlignWithPrevious && pActiveCam != nullptr) AlignCameras(pActiveCam, _camera);

		if (_bInitialize && bInitialized)
		{
			/*camera->Initialize();
			camera->OnPossess();*/
		}

		return _camera;
	}

	BaseCamera* CameraManager::PushCameraByName(const std::string& _name, const bool _bAlignWithPrevious,
	                                            const bool         _bInitialize)
	{
		BaseCamera* cam = GetCameraByName(_name);
		if (cam == nullptr)
		{
			const std::string error_str = "Attempted to push camera with invalid name: " + _name + "\n";
			std::cout << error_str.c_str() << std::endl;
			return nullptr;
		}
		return PushCamera(cam, _bAlignWithPrevious, _bInitialize);
	}

	void CameraManager::PopCamera(const bool _bAlignWithCurrent /* = false */)
	{
		if (cameraStack.size() <= 1)
		{
			std::cout << "CameraManager::PopCamera - Attempted to pop final camera from stack\n" << std::endl;
			return;
		}

		/*g_InputModule->ClearAllInputs();*/

		BaseCamera* currentCamera = GetCurrentCamera();
		currentCamera->OnDepossess();
		currentCamera->Finalize();
		const BaseCamera* prev_camera = currentCamera;

		cameraStack.pop();

		currentCamera = GetCurrentCamera();
		currentCamera->OnPossess();
		currentCamera->Init();

		if (_bAlignWithCurrent) AlignCameras(prev_camera, currentCamera);
	}

	BaseCamera* CameraManager::GetCameraByName(const std::string& _name) const
	{
		for (uint32_t i = 0; i < static_cast<uint32_t>(cameras.size()); ++i)
		{
			if (cameras[i]->GetName().compare(_name) == 0) return cameras[i];
		}

		return nullptr;
	}

	void CameraManager::DrawImGuiObjects()
	{
		/*if (ImGui::TreeNode("Camera"))
		{
			BaseCamera* currentCamera = GetCurrentCamera();

			const uint32_t cameraCount = (uint32_t)cameras.size();

			if (cameraCount > 1) // Only show arrows if other cameras exist
			{
				bool cycleLeft = ImGui::Button("<");
				if (ImGui::IsItemHovered())
				{
					static StringBuilder actionBindingBuff;
					actionBindingBuff.Clear();
					if (g_InputModule->GetActionBindingName(Action::DBG_SWITCH_TO_PREV_CAM, actionBindingBuff))
					{
						ImGui::SetTooltip("Shortcut: %s", actionBindingBuff.ToCString());
					}
				}
				if (cycleLeft)
				{
					CycleCamera(-1, false);
				}

				ImGui::SameLine();

				std::string cameraNameStr = currentCamera->GetName();
				ImGui::Text("%12s", cameraNameStr.c_str());

				ImGui::SameLine();

				bool cycleRight = ImGui::Button(">");
				if (ImGui::IsItemHovered())
				{
					static StringBuilder actionBindingBuff;
					actionBindingBuff.Clear();
					if (g_InputModule->GetActionBindingName(Action::DBG_SWITCH_TO_NEXT_CAM, actionBindingBuff))
					{
						ImGui::SetTooltip("Shortcut: %s", actionBindingBuff.ToCString());
					}
				}
				if (cycleRight)
				{
					CycleCamera(1, false);
				}
			}

			ImGui::SliderFloat("Move speed", &currentCamera->moveSpeed, 1.0f, 750.0f);

			real turnSpeed = glm::degrees(currentCamera->mouseRotationSpeed);
			if (ImGui::SliderFloat("Turn speed", &turnSpeed, 0.01f, 0.3f))
			{
				currentCamera->mouseRotationSpeed = glm::radians(turnSpeed);
			}

			ImGui::DragFloat3("Position", &currentCamera->position.x, 0.1f);

			glm::vec2 camYawPitch;
			camYawPitch[0] = glm::degrees(currentCamera->yaw);
			camYawPitch[1] = glm::degrees(currentCamera->pitch);
			if (ImGui::DragFloat2("Yaw & Pitch", &camYawPitch.x, 0.05f))
			{
				currentCamera->yaw = glm::radians(camYawPitch[0]);
				currentCamera->pitch = glm::radians(camYawPitch[1]);
			}

			real camFOV = glm::degrees(currentCamera->FOV);
			if (ImGui::DragFloat("FOV", &camFOV, 0.05f, 10.0f, 150.0f))
			{
				currentCamera->FOV = glm::radians(camFOV);
			}

			ImGui::DragFloat("near", &currentCamera->zNear, 0.05f, 0.0f, 10.0f);

			ImGui::DragFloat("far", &currentCamera->zFar, 0.05f, 0.0f, 10000.0f);

			if (ImGui::Button("Reset orientation"))
			{
				currentCamera->ResetOrientation();
			}

			ImGui::SameLine();
			if (ImGui::Button("Reset position"))
			{
				currentCamera->ResetPosition();
			}

			GetCurrentCamera()->DrawImGuiObjects();

			ImGui::TreePop();
		}*/
	}

	uint32_t CameraManager::GetCameraIndex(const BaseCamera* _camera) const
	{
		for (uint32_t i = 0; i < cameras.size(); ++i)
		{
			if (cameras[i] == _camera) return i;
		}

		return -1;
	}

	void CameraManager::AlignCameras(const BaseCamera* _from, BaseCamera* _to)
	{
		_to->position = _from->position;
		_to->pitch    = _from->pitch;
		_to->yaw      = _from->yaw;
		_to->fov      = _from->fov;
	}

	/*EventReply CameraManager::OnActionEvent(Action action, ActionEvent actionEvent)
	{
		if (actionEvent == ActionEvent::ACTION_TRIGGER)
		{
			if (action == Action::DBG_SWITCH_TO_PREV_CAM)
			{
				CycleCamera(-1, false);
				return EventReply::CONSUMED;
			}
			else if (action == Action::DBG_SWITCH_TO_NEXT_CAM)
			{
				CycleCamera(1, false);
				return EventReply::CONSUMED;
			}
		}

		return EventReply::UNCONSUMED;
	}*/
} 
