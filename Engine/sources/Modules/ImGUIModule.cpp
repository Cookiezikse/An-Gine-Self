#include "Modules/ImGUIModule.h"
#include "Modules/WindowModule.h"
#include "Modules/ImGUIModule.h"

#include "FilesDirs.h"

#include "ImGUIInterface.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include "ImGUIInterface.h"
#include "GameObject/Components/Light.h"
#include "GameObject/Components/Transform.h"
#include "LveEngine/lve_renderer.h"
#include "Modules/ModuleManager.h"
#include "Modules/rhi.h"
#include "Modules/WindowModule.h"
#include "Scene/SceneManager.h"
#include "TCP/Errors.h"

#include <fstream>
#include <cstring>
#include <vector>
#include <imgui_internal.h>
#include <random>
#include <windows.h>
#include <locale>
#include <codecvt>
#include <algorithm>
#include <Engine/CoreEngine.h>
#include <memory>

class BaseScene;
// ----------========== IMGUI SETTINGS ==========---------- //

void ImGuiModule::Init()
{
	Module::Init();

	windowModule = moduleManager->GetModule<WindowModule>();
	rhiModule = moduleManager->GetModule<RHIModule>();
	sceneManager = moduleManager->GetModule<SceneManager>();
	device = rhiModule->GetDevice()->Device();
	graphicsQueue = rhiModule->GetDevice()->GraphicsQueue();
	const lve::QueueFamilyIndices queue_family_indices = rhiModule->GetDevice()->FindPhysicalQueueFamilies();

	// Création du pool de commandes
	const vk::CommandPoolCreateInfo command_pool_info(
		vk::CommandPoolCreateFlags(),       // Flags de création
		queue_family_indices.graphicsFamily // Indice de la famille de file d'attente de commandes
	);
	immCommandPool = device.createCommandPool(command_pool_info);

	// Allocation du tampon de commande pour les soumissions immédiates
	const vk::CommandBufferAllocateInfo cmd_alloc_info(
		immCommandPool,                   // Pool de commandes
		vk::CommandBufferLevel::ePrimary, // Niveau du tampon de commande
		1                                 // Nombre de tampons à allouer
	);
	immCommandBuffer = device.allocateCommandBuffers(cmd_alloc_info)[0];

	// Ajout de la fonction de suppression du pool de commandes à la file de suppression principale
	//_mainDeletionQueue.push_back([=]() {
	//	device.destroyCommandPool(_immCommandPool);
	//});

	soundModule = new SoundSystemModule();
	soundModule->Init();
	if (!soundModule->IsInitialized()) {
		std::cerr << "Erreur: Initialisation de SoundSystemModule a échoué." << std::endl;
		return;
	}

	imGuiAudio = new ImGUIAudio(soundModule);
}

void ImGuiModule::Start()
{
	Module::Start();
	//timeModule = moduleManager->GetModule<TimeModule>();

	//ImGui::CreateContext();

	const vk::DescriptorPoolSize pool_sizes[] = {
		{vk::DescriptorType::eSampler, 1000},
		{vk::DescriptorType::eCombinedImageSampler, 1000},
		{vk::DescriptorType::eSampledImage, 1000},
		{vk::DescriptorType::eStorageImage, 1000},
		{vk::DescriptorType::eUniformTexelBuffer, 1000},
		{vk::DescriptorType::eStorageTexelBuffer, 1000},
		{vk::DescriptorType::eUniformBuffer, 1000},
		{vk::DescriptorType::eStorageBuffer, 1000},
		{vk::DescriptorType::eUniformBufferDynamic, 1000},
		{vk::DescriptorType::eStorageBufferDynamic, 1000},
		{vk::DescriptorType::eInputAttachment, 1000}
	};

	vk::DescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = vk::StructureType::eDescriptorPoolCreateInfo;
	pool_info.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
	pool_info.maxSets = 1000;
	pool_info.poolSizeCount = static_cast<uint32_t>(std::size(pool_sizes));
	pool_info.pPoolSizes = pool_sizes;

	vk::DescriptorPool im_gui_pool;
	if (rhiModule->GetDevice()->Device().createDescriptorPool(&pool_info, nullptr, &im_gui_pool) !=
		vk::Result::eSuccess)
		throw std::runtime_error("Impossible de creer la pool de imgui!");

	// 2: initialize imgui library

	// this initializes the core structures of imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// this initializes imgui for SDL
	ImGui_ImplGlfw_InitForVulkan(windowModule->GetGlfwWindow(), true);

	// this initializes imgui for Vulkan
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = rhiModule->GetDevice()->GetInstance();
	init_info.PhysicalDevice = rhiModule->GetDevice()->GetPhysicalDevice();
	init_info.Device = device;
	init_info.Queue = graphicsQueue;
	init_info.DescriptorPool = im_gui_pool;
	init_info.MinImageCount = 3;
	init_info.ImageCount = 3;
	init_info.RenderPass = rhiModule->GetRenderer()->GetSwapChainRenderPass();
	//init_info.UseDynamicRendering = VK_TRUE;
	//init_info.ColorAttachmentFormat = _swapchainImageFormat;

	init_info.MSAASamples = static_cast<VkSampleCountFlagBits>(vk::SampleCountFlagBits::e1);

	ImGui_ImplVulkan_Init(&init_info);

	// execute a gpu command to upload imgui font textures
	//immediate_submit([&](vk::CommandBuffer cmd) { ImGui_ImplVulkan_CreateFontsTexture(); });

	// clear font textures from cpu data
	//ImGui_ImplVulkan_DestroyFontUploadObjects();

	for (int i = 0; i < moduleManager->GetModule<RHIVulkanModule>()->GetListTextures().size();i++)
	{

		VkDescriptorSet textureID = ImGui_ImplVulkan_AddTexture(
			moduleManager->GetModule<RHIVulkanModule>()->GetListTextures()[i].sampler,
			moduleManager->GetModule<RHIVulkanModule>()->GetListTextures()[i].imageView,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		);

		ListDescriptorsImGui->push_back(textureID);
	}
	//std::cout << "SIZE : " << ListDescriptorsImGui->size();
	

}

void ImGuiModule::FixedUpdate()
{
	Module::FixedUpdate();
}

void ImGuiModule::Update()
{
	Module::Update();

	HandleShortcuts();

	// Mise à jour d'ImGui
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	GetGui();
}

void ImGuiModule::PreRender()
{
	Module::PreRender();
}

void ImGuiModule::Render()
{
	Module::Render();



	// Rendu d'ImGui
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), rhiModule->GetRenderer()->GetCurrentCommandBuffer());
}

void ImGuiModule::RenderGui()
{
	Module::RenderGui();
}

void ImGuiModule::PostRender()
{
	Module::PostRender();
}

void ImGuiModule::Release()
{
	Module::Release();
}

void ImGuiModule::Finalize()
{
	Module::Finalize();


	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext();
}

void ImGuiModule::ImmediateSubmit(std::function<void(vk::CommandBuffer _cmd)>&& _function) const
{
	const vk::Device device = rhiModule->GetDevice()->Device();
	const vk::Queue  graphics_queue = rhiModule->GetDevice()->GraphicsQueue();

	device.resetFences(immFence);
	immCommandBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);

	const vk::CommandBuffer cmd = immCommandBuffer;

	vk::CommandBufferBeginInfo cmd_begin_info{};
	cmd_begin_info.sType = vk::StructureType::eCommandBufferBeginInfo;
	cmd_begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

	cmd.begin(cmd_begin_info);

	_function(cmd);

	cmd.end();

	vk::CommandBufferSubmitInfo cmd_info;
	cmd_info.sType = vk::StructureType::eCommandBufferSubmitInfo;
	cmd_info.pNext = nullptr;
	cmd_info.commandBuffer = cmd;
	cmd_info.deviceMask = 0;

	vk::SubmitInfo2 submitInfo{};
	submitInfo.commandBufferInfoCount = 1;
	submitInfo.pCommandBufferInfos = &cmd_info;
	constexpr auto dispatcher = vk::DispatchLoaderDynamic();
	// submit command buffer to the queue and execute it.
	//  _renderFence will now block until the graphic commands finish execution
	graphics_queue.submit2KHR(submitInfo, immFence, dispatcher);

	if (device.waitForFences(immFence, VK_TRUE, 9999999999) != vk::Result::eSuccess)
	{
		throw std::runtime_error("Failed GUI");
	}
}


// ----------========== IMGUI SHOWN ==========---------- //

void ImGuiModule::GetGui()
{

	


	ImVec2 mainWindowSize = ImGui::GetMainViewport()->Size;

	// Flags pour les fenêtres déplaçables et non-redimensionnables
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 10.0f));  // Augmente l'espace vertical
	ImGui::Separator();
	ImGui::PopStyleVar();

	// Dessin de la fenêtre "Hierarchy" - Gauche
	ImGui::SetNextWindowSize(ImVec2(300, mainWindowSize.y), ImGuiCond_Always); // Hauteur fixe et non-redimensionnable
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always); // Ancrage en haut à gauche
	ImGui::Begin("Hierarchy", nullptr, window_flags);
	DrawHierarchyWindow();
	ImGui::End();

	// Dessin de la fenêtre "Inspector" - Droite
	ImGui::SetNextWindowSize(ImVec2(300, mainWindowSize.y), ImGuiCond_Always); // Hauteur fixe et non-redimensionnable
	ImGui::SetNextWindowPos(ImVec2(mainWindowSize.x - 300, 0), ImGuiCond_Always); // Ancrage en haut à droite
	ImGui::Begin("Inspector", nullptr, window_flags);
	DrawInspectorWindow();
	ImGui::End();

	DrawTchatWindow();
	
	ImGui::SetNextWindowSize(ImVec2(300, 72), ImGuiCond_Always); // Hauteur fixe et non-redimensionnable
	ImGui::SetNextWindowPos(ImVec2(mainWindowSize.x / 2 - 300 / 2, 0), ImGuiCond_Always); // Ancrage en haut à droite
	ImGui::Begin("Modes", nullptr, window_flags);
	DrawModesWindow();
	ImGui::End();

	DrawConsoleWindow();

	DrawFilesExplorerWindow();

	DrawSettingsWindow();

	DrawProjectSaveWindow();
}

void ImGuiModule::AnchorWindow(const std::string& _windowName)
{
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 mainWindowPos = ImGui::GetMainViewport()->Pos;
	ImVec2 mainWindowSize = ImGui::GetMainViewport()->Size;

	// Ancrage à droite avec mise à jour dynamique lors du redimensionnement
	ImGui::SetWindowPos(_windowName.c_str(), ImVec2(mainWindowSize.x - 300, windowPos.y));

	// Ancrage à gauche ou à droite si nécessaire
	if (windowPos.x < mainWindowPos.x + 50) {
		ImGui::SetWindowPos(_windowName.c_str(), ImVec2(mainWindowPos.x, windowPos.y));
	}
	else if (windowPos.x > mainWindowPos.x + mainWindowSize.x - 350) {
		// S'assurer que la fenêtre reste collée au bord droit même après redimensionnement
		ImGui::SetWindowPos(_windowName.c_str(), ImVec2(mainWindowPos.x + mainWindowSize.x - 300, windowPos.y));
	}

	// Correction pour éviter le débordement par le haut ou par le bas
	if (windowPos.y < mainWindowPos.y) {
		ImGui::SetWindowPos(_windowName.c_str(), ImVec2(windowPos.x, mainWindowPos.y));
	}
	else if (windowPos.y + mainWindowSize.y > mainWindowPos.y + mainWindowSize.y) {
		ImGui::SetWindowPos(_windowName.c_str(), ImVec2(windowPos.x, mainWindowPos.y + mainWindowSize.y - mainWindowSize.y));
	}
}


// ----------========== DRAW WINDOWS ==========---------- //

void ImGuiModule::DrawInspectorWindow() {
	// Vérifier si un GameObject est sélectionné
	if (selectedGameObject) {
		// Affichage du nom du GameObject
		ImGui::Text("Name: ");
		ImGui::SameLine();
		ImGui::SetWindowFontScale(1.2f);
		ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), selectedGameObject->GetName().c_str());
		ImGui::SetWindowFontScale(1.0f);

		// Bouton pour afficher la popup de renommage
		ImGui::SameLine();
		if (ImGui::Button("Rename")) {
			isRenamePopupOpen = true;
			strncpy_s(renameBuffer, selectedGameObject->GetName().c_str(), sizeof(renameBuffer) - 1);
			renameBuffer[sizeof(renameBuffer) - 1] = '\0';
			ImGui::OpenPopup("Rename Entity");
		}
		ShowRenamePopup();

		bool visibility = selectedGameObject->GetVisible();
		if (ImGui::Checkbox("Visible", &visibility)) {
			selectedGameObject->SetVisible(visibility);
		}

		for (size_t j = 0; j < selectedGameObject->GetChildren().size(); ++j) {
			const auto& gameObject = selectedGameObject->GetChildren()[j];

			ImGui::PushID(j); // Identifiant unique pour chaque GameObject

			std::string name = gameObject->GetName();
			const char* namePtr = name.c_str();
			if (ImGui::Button(namePtr, ImVec2(50,50))) {
				selectedGameObject = gameObject;
			}
		}

		// Affichage des propriétés de transformation
		if (ImGui::CollapsingHeader("Transform")) {
			DisplayTransform(selectedGameObject->GetTransform());
		}

		if (ImGui::CollapsingHeader("Texture")) {
			if (selectedGameObject->GetTexture() < moduleManager->GetModule<RHIVulkanModule>()->GetListTexturesNames()->size()) 
			{
				ImGui::Text((*moduleManager->GetModule<RHIVulkanModule>()->GetListTexturesNames())[selectedGameObject->GetTexture()].c_str());
			}
			int texture = selectedGameObject->GetTexture();
			if (ImGui::InputInt("Texture", &texture)) {

				if (texture < 0) {
					texture = 0;
				}
				else if (texture > moduleManager->GetModule<RHIVulkanModule>()->GetListDescriptors().size() - 1) {
					texture -= 1;
				}
				else {
					selectedGameObject->SetTexture(texture);
				}
			}
			float textureMulti = selectedGameObject->GetTexMultiplier();
			if (ImGui::DragFloat("Texture Multiplier", &textureMulti, 1, 0.0, 100.0)) {
				std::ifstream file(selectedGameObject->GetFileModel());
				if (file.good()) {
					std::cout << "File good";
					AddLog("Model file good");
					selectedGameObject->SetTexMultiplier(textureMulti);
					std::shared_ptr<lve::LveModel> lve_model = lve::LveModel::CreateModelFromFile(*rhiModule->GetDevice(), selectedGameObject->GetFileModel(), selectedGameObject->GetTexMultiplier(), selectedGameObject->GetColor());
					selectedGameObject->SetModel(lve_model);
				}
				else {
					std::cout << "File not good :" + selectedGameObject->GetFileModel();
					AddLog("Warning : Model file not good");
				}
			}
			//ImGui::SameLine();
			if (ImGui::Button("Reset Multiplier", ImVec2(150, 20))) {
				std::ifstream file(selectedGameObject->GetFileModel());
				if (file.good()) {
					std::cout << "File good";
					AddLog("Model file good");
					selectedGameObject->SetTexMultiplier(1.0f);
					std::shared_ptr<lve::LveModel> lve_model = lve::LveModel::CreateModelFromFile(*rhiModule->GetDevice(), selectedGameObject->GetFileModel(), selectedGameObject->GetTexMultiplier(), selectedGameObject->GetColor());
					selectedGameObject->SetModel(lve_model);
				}
				else {
					std::cout << "File not good :" + selectedGameObject->GetFileModel();
					AddLog("Warning : Model file not good");
				}
			}

			if (ImGui::Checkbox("Texture Viewer (0.3v)", &textureView)) {
				!textureView;
			}

			if (textureView) {

				int sizeDescList = moduleManager->GetModule<RHIVulkanModule>()->GetListDescriptors().size();

				// Vulkan crie, parce que ce sont les descriptors d'un autre pool et d'une autre pipeline
				if (selectedGameObject->GetTexture() > sizeDescList - 1) {
					ImGui::Image((*ListDescriptorsImGui)[0], ImVec2(200, 200));
				}
				else {
					//ImGui::Image(&ListDescriptorsImGui[selectedGameObject->GetTexture()], ImVec2(200, 200));
					ImGui::Image((*ListDescriptorsImGui)[selectedGameObject->GetTexture()], ImVec2(200, 200));

				}
			}
		}

		glm::vec3 color = selectedGameObject->GetColor();
		if (ImGui::CollapsingHeader("Color")) {
			if (ImGui::ColorEdit3("Color", glm::value_ptr(color))) {
				selectedGameObject->SetColor(color);
				std::ifstream file(selectedGameObject->GetFileModel());
				if (file.good()) {
					std::cout << "File good";
					AddLog("Model file good");
					std::shared_ptr<lve::LveModel> lve_model = lve::LveModel::CreateModelFromFile(*rhiModule->GetDevice(), selectedGameObject->GetFileModel(), selectedGameObject->GetTexMultiplier(), selectedGameObject->GetColor());
					selectedGameObject->SetModel(lve_model);
				}
				else {
					std::cout << "File not good :" + selectedGameObject->GetFileModel();
					AddLog("Warning : Model file not good");
				}
			}
			if (ImGui::Button("Reset Color", ImVec2(115, 20))) {
				selectedGameObject->SetColor(glm::vec3(1,1,1));
				std::ifstream file(selectedGameObject->GetFileModel());
				if (file.good()) {
					std::cout << "File good";
					AddLog("Model file good");
					std::shared_ptr<lve::LveModel> lve_model = lve::LveModel::CreateModelFromFile(*rhiModule->GetDevice(), selectedGameObject->GetFileModel(), selectedGameObject->GetTexMultiplier(), selectedGameObject->GetColor());
					selectedGameObject->SetModel(lve_model);
				}
				else {
					std::cout << "File not good :" + selectedGameObject->GetFileModel();
					AddLog("Warning : model file not good");
				}
			}
		}



		// Detection de Light et affichage des proprietes de la lumiere
		Light* lightComponent = selectedGameObject->GetComponent<Light>();
		if (lightComponent) {
			if (ImGui::CollapsingHeader("Light")) 
			{
				// Intensite de la lumiere
				float intensity = lightComponent->lightIntensity;
				if (ImGui::DragFloat("Light Intensity", &intensity, 0.1f, 0.0f, 100.0f)) {
					lightComponent->lightIntensity = intensity;
				}
				//ImGui::SameLine();
				if (ImGui::Button("X", ImVec2(20, 20))) {
					selectedGameObject->RemoveComponent(lightComponent);
					AddLog("Removed Component Light from : " + selectedGameObject->GetName());
				}
			}
		}

		ImGui::Spacing();
		ImGui::Separator();
		// Bouton pour ajouter un composant
		if (ImGui::Button("Add Component")) {
			ImGui::OpenPopup("AddComponentPopup");
		}

		// Popup pour l'ajout de composant
		if (ImGui::BeginPopup("AddComponentPopup")) {
			if (ImGui::MenuItem("Add Light")) {
				Light* newLight = selectedGameObject->CreateComponent<Light>();
				AddLog("Created Component Light to : " + selectedGameObject->GetName());
				newLight->lightIntensity = 1.0;  // Intensit� initiale standard
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		ImGui::Spacing();
		ImGui::Separator();

		char* charBuffer = new char[300];
		strcpy_s(charBuffer, 300, selectedGameObject->GetDesc().c_str());
		ImGui::Text("Description");
		if (ImGui::InputTextMultiline("-", charBuffer, 300)) {
			selectedGameObject->SetDesc(charBuffer);
		}
		delete[] charBuffer;
	}
	else {
		ImGui::Text("No GameObject selected");
	}
}

void ImGuiModule::DrawHierarchyWindow() {
	// Bouton pour créer un nouveau GameObject
	if (ImGui::Button("Project popup")) {
		showPopupProject = true;
	}
	ImGui::Separator();

	if (ImGui::Button("New GameObject")) {
		ImGui::OpenPopup("CreateGameObjectPopup");
	}
	ImGui::SameLine();
	// Bouton pour ajouter une nouvelle scène
	if (ImGui::Button("Add New Scene")) {
		sceneManager->CreateScene("New Scene", false);
	}
	if (ImGui::BeginPopup("CreateGameObjectPopup")) {
		if (ImGui::MenuItem("Cube")) {
			CreateSpecificGameObject(GameObjectType::Cube);
			std::cout << "Added new GameObject-Cube to current scene." << std::endl;
			AddLog("Added new GameObject-Cube to current scene.");
		}
		if (ImGui::MenuItem("Colored Cube")) {
			CreateSpecificGameObject(GameObjectType::Cube,1);
			std::cout << "Added new GameObject-Cube to current scene." << std::endl;
			AddLog("Added new GameObject-Cube to current scene.");
		}
		if (ImGui::MenuItem("Light")) {
			CreateSpecificGameObject(GameObjectType::Light);
			std::cout << "Added new GameObject-Light to current scene." << std::endl;
			AddLog("Added new GameObject-Light to current scene.");

		}
		if (ImGui::MenuItem("Plane")) {
			CreateSpecificGameObject(GameObjectType::Plane);
			std::cout << "Added new GameObject-Plane to current scene." << std::endl;
			AddLog("Added new GameObject-Plane to current scene.");

		}
		if (ImGui::MenuItem("Vase Flat")) {
			CreateSpecificGameObject(GameObjectType::Vase);
			std::cout << "Added new GameObject-Vase Flat to current scene." << std::endl;
			AddLog("Added new GameObject-Vase Flat to current scene.");
		}
		if (ImGui::MenuItem("Vase Smooth")) {
			CreateSpecificGameObject(GameObjectType::Vase,1);
			std::cout << "Added new GameObject-Vase Smooth to current scene." << std::endl;
			AddLog("Added new GameObject-Vase Smooth to current scene.");
		}

		if (ImGui::MenuItem("Girl")) {
			CreateSpecificGameObject(GameObjectType::Girl);
			std::cout << "Added new GameObject-Girl to current scene." << std::endl;
			AddLog("Added new GameObject-Girl to current scene.");
		}

		if (ImGui::MenuItem("Noob")) {
			CreateSpecificGameObject(GameObjectType::Noob);
			std::cout << "Added new GameObject-Noob to current scene." << std::endl;
			AddLog("Added new GameObject-Noob to current scene.");
		}

		if (ImGui::MenuItem("Sphere")) {
			CreateSpecificGameObject(GameObjectType::Sphere);
			std::cout << "Added new GameObject-Sphere to current scene." << std::endl;
			AddLog("Added new GameObject-Sphere to current scene.");
		}

		if (ImGui::MenuItem("Cone")) {
			CreateSpecificGameObject(GameObjectType::Multiple,0);
			std::cout << "Added new GameObject-Cone to current scene." << std::endl;
			AddLog("Added new GameObject-Cone to current scene.");
		}

		if (ImGui::MenuItem("Triangle")) {
			CreateSpecificGameObject(GameObjectType::Multiple,1);
			std::cout << "Added new GameObject-Triangle to current scene." << std::endl;
			AddLog("Added new GameObject-Triangle to current scene.");
		}

		if (ImGui::MenuItem("Capsule")) {
			CreateSpecificGameObject(GameObjectType::Multiple, 2);
			std::cout << "Added new GameObject-Capsule to current scene." << std::endl;
			AddLog("Added new GameObject-Capsule to current scene.");
		}
		if (ImGui::MenuItem("Tube")) {
			CreateSpecificGameObject(GameObjectType::Multiple, 3);
			std::cout << "Added new GameObject-Tube to current scene." << std::endl;
			AddLog("Added new GameObject-Tube to current scene.");
		}
		if (ImGui::MenuItem("Anneau")) {
			CreateSpecificGameObject(GameObjectType::Multiple, 4);
			std::cout << "Added new GameObject-Anneau to current scene." << std::endl;
			AddLog("Added new GameObject-Anneau to current scene.");
		}
		if (ImGui::MenuItem("Cylindre")) {
			CreateSpecificGameObject(GameObjectType::Multiple, 5);
			std::cout << "Added new GameObject-Cylindre to current scene." << std::endl;
			AddLog("Added new GameObject-Cylindre to current scene.");
		}
		if (ImGui::MenuItem("Cylindre Coupe")) {
			CreateSpecificGameObject(GameObjectType::Multiple, 6);
			std::cout << "Added new GameObject-Cylindre Coupe to current scene." << std::endl;
			AddLog("Added new GameObject-Cylindre Coupe to current scene.");
		}


		ImGui::EndPopup();
	}

	bool showErrorPopup = false;

	// Barre de recherche
	char* searchBuffer = new char[100];
	strcpy_s(searchBuffer, 100, GetSearch().c_str());
	if (ImGui::InputText("Search", searchBuffer, 100)) { SetSearch(searchBuffer); }
	delete[] searchBuffer;


	// Affichage des scènes et de leurs GameObjects
	const auto& scenes = sceneManager->GetScenes();
	for (size_t i = 0; i < scenes.size(); ++i) {
		ImGui::Spacing();
		ImGui::Separator();

		auto& scene = scenes[i];
		bool isCurrentScene = (sceneManager->GetCurrentScene() == scene.get());

		ImGui::PushID(i); // Identifiant unique pour chaque scène

		auto SceneTree = ImGui::TreeNode(scene->GetName().c_str());

		// Menu contextuel pour chaque scène
		if (ImGui::BeginPopupContextItem("Scene Menu")) {
			if (ImGui::MenuItem("Set Active")) {
				sceneManager->SetCurrentScene(static_cast<int>(i)); // Définit la scène courante
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Rename")) {
				sceneToRename = i;
				strncpy_s(renameSceneBuffer, scene->GetName().c_str(), sizeof(renameSceneBuffer));
				renameSceneBuffer[sizeof(renameSceneBuffer) - 1] = '\0';
				ImGui::OpenPopup("Rename Scene");
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Delete")) {
				if (sceneManager->GetListScenes2().size() > 1) {
					sceneManager->DestroyScene(scene->GetName(),i);
					sceneManager->SetCurrentScene(0);
				}
				else {
					AddLog("Warning : You need at least 2 scenes to delete one");
					SetShowPopupError(true);
				}
			}
			ImGui::EndPopup();
		}

		// Affichage du nom de la scène avec un bouton "Set Active" si nécessaire
		if (SceneTree) {
			// Bouton pour définir la scène active
			if (!isCurrentScene) {
				ImGui::SameLine(ImGui::GetWindowWidth() - 100); // Décalage à droite
				std::string buttonLabel = "Set Active##" + std::to_string(i);
				if (ImGui::Button(buttonLabel.c_str())) {
					sceneManager->SetCurrentScene(static_cast<int>(i));
				}
			}

			// Affichage des GameObjects
			const auto& gameObjects = scene->GetRootObjects();

			FilesDirs filesdirs;

			for (size_t j = 0; j < gameObjects.size(); ++j) {
				const auto& gameObject = gameObjects[j];


				if (filesdirs.ContainsSubstringIns(gameObject->GetName(), GetSearch())) {
					ImGui::PushID(j); // Identifiant unique pour chaque GameObject
					if (ImGui::Selectable(gameObject->GetName().c_str(), selectedGameObject == gameObject)) {
						selectedGameObject = gameObject;
					}
					if (j == gameObjects.size() - 1) {
						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::Spacing();
					}

					// Menu contextuel pour GameObject
					if (ImGui::BeginPopupContextItem()) {
						if (ImGui::MenuItem("Rename")) {
							isRenamePopupOpen = true;
							entityToRename = j;
							strncpy_s(renameBuffer, gameObject->GetName().c_str(), sizeof(renameBuffer) - 1);
							renameBuffer[sizeof(renameBuffer) - 1] = '\0';
							ImGui::OpenPopup("Rename Entity");
							selectedGameObject = gameObject;
						}

						ImGui::Separator();
						if (ImGui::MenuItem("Delete")) { 
							DeleteGameObject(gameObject);
						}

						ImGui::Separator();

						if (ImGui::MenuItem("Duplicate")) 
						{
							DuplicateGameObject(gameObject);
						}

						ImGui::EndPopup();
					}
					ImGui::PopID();  // Restaure l'ID précédent pour les GameObjects
				}
			}
			ImGui::TreePop();
		}
		ImGui::PopID();  // Restaure l'ID précédent pour les scènes
	}
	if (GetShowPopupError()) {
		ImGui::OpenPopup("ErrorPopup");
		if (ImGui::BeginPopupModal("ErrorPopup", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("You need at least two scenes to delete one.");
			if (ImGui::Button("OK", ImVec2(120, 0))) {
				ImGui::CloseCurrentPopup();
				SetShowPopupError(false);
			}
			ImGui::EndPopup();
		}
	}
}

void ImGuiModule::DrawModesWindow() {

	//std::cout << Engine::GetInstance()->GetEngineMode() << std::endl;

	if (ImGui::Button("Play Button", ImVec2(110, 20))) {
		if (Engine::GetInstance()->GetEngineMode() == EngineMode::Editor) {
			Engine::GetInstance()->PlayEngineMode();
			std::cout << Engine::GetInstance()->GetEngineMode() << std::endl;
		}
		else {
			Engine::GetInstance()->EditorEngineMode();
			std::cout << Engine::GetInstance()->GetEngineMode() << std::endl;
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Pause Button", ImVec2(110, 20))) {
		if (Engine::GetInstance()->GetEngineMode() == EngineMode::Play) {
			Engine::GetInstance()->PauseEngineMode();
			std::cout << Engine::GetInstance()->GetEngineMode() << std::endl;
		}
		else if (Engine::GetInstance()->GetEngineMode() == EngineMode::Pause) {
			Engine::GetInstance()->PlayEngineMode();
			std::cout << Engine::GetInstance()->GetEngineMode() << std::endl;
		}
	}

	//ImGui::SameLine();

	std::string str = "Mode : ";
	switch (Engine::GetInstance()->GetEngineMode()) {
	case EngineMode::Editor:
		str += "Editor";
		break;
	case EngineMode::Play:
		str += "Play";
		break;
	case EngineMode::Pause:
		str += "Pause";
		break;
	default:
		break;
	}
	ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), str.c_str());
}


void ImGuiModule::DrawSettingsWindow() {
	if (ImGui::Begin("Settings")) {
		if (ImGui::CollapsingHeader("Interface")) {
			ImGUIInterface::EditTheme();
		}
		if (ImGui::CollapsingHeader("Input")) {
			// Input settings
		}
		if (ImGui::CollapsingHeader("Graphics")) {
			// Graphics settings
		}
		if (ImGui::CollapsingHeader("Audio")) {
			// Audio settings
			imGuiAudio->DrawAudioControls(); //Pointeur vers la fonction DrawAudioControls
		}
		if (ImGui::CollapsingHeader("Network")) {
			//Network settings
		}
	}
	ImGui::End();
}

void ImGuiModule::DrawTchatWindow() {
	StatusMessage& statusMsg = StatusMessage::GetInstance();
	if (ImGui::Begin("Tchat")) {
		ImGui::Spacing();

		ImGui::SetWindowFontScale(1.5f);  // Taille du texte modifié
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Informations:");
		ImGui::SetWindowFontScale(1.0f);  // Taille du texte par défaut

		ImGui::InputText("IP Address", ipBuffer, sizeof(ipBuffer));
		ImGui::InputText("Port", portBuffer, sizeof(portBuffer));
		if (!isConnectedTCPClient && !isConnectedTCPServer)
		{
			if (ImGui::Button("Connexion")) {
				clientTCP.SetConnectedClient(false);
				unsigned long port = std::stoul(portBuffer, nullptr, 0);
				if (std::string(ipBuffer) == "" && std::string(portBuffer) != "")
				{
					serverTCP.StartServer(port, ipBuffer);
					isConnectedTCPServer = true;
				}
				else {
					isConnectedTCPClient = true;
					clientTCP.ConnexionClientUDP(ipBuffer, port);
				}
			}
		}
		else if (isConnectedTCPClient && !isConnectedTCPServer) {
			if (ImGui::Button("Deconnexion")) {
				isConnectedTCPClient = false;
				unsigned long port = std::stoul(portBuffer, nullptr, 0);
				if (std::string(ipBuffer) != "" && std::string(portBuffer) != "")
				{
					clientTCP.SetConnectedClient(true);
				}
			}
		}
		else if (isConnectedTCPServer && !isConnectedTCPClient) {
			if (ImGui::Button("Connecte")) {
				isConnectedTCPServer = false;
			}
		}
		ImGui::Spacing();
		ImGui::Separator();

		ImGui::SetWindowFontScale(1.5f);  // Taille du texte modifié
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Logs:");
		ImGui::SetWindowFontScale(1.0f);  // Taille du texte par défaut

		// Zone pour afficher les messages
		if (ImGui::BeginChild("Logs", ImVec2(0, 200), true)) {
			const auto& receivedMessages = StatusMessage::GetInstance().GetReceivedMessages();
			if (receivedMessages.empty()) {
				ImGui::TextWrapped("Pas de messages recu");
			}
			else {
				for (const auto& msg : receivedMessages) {
					ImGui::TextWrapped("%s", msg.c_str());
				}
				if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
					ImGui::SetScrollHereY(1.0f); // Auto-scroll to the bottom
				}
			}
		}
		ImGui::EndChild();

		ImGui::Spacing();
		ImGui::Separator();

		// Champ pour entrer le message
		ImGui::InputTextMultiline("Message", messageBuffer, sizeof(messageBuffer), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 4));

		// Envoi du message
		if (ImGui::Button("Send")) {
			std::string fullMessage = "Ready to send message to " + std::string(ipBuffer) + ":" + std::string(portBuffer) + "\nMessage: " + std::string(messageBuffer);
			statusMsg.SetStatus(StatusMessage::Send);
			clientTCP.SendData(messageBuffer);
			
			memset(messageBuffer, 0, sizeof(messageBuffer));  // Effacer le buffer de message après "l'envoi"
		}

	}
	ImGui::End();
}

void ImGuiModule::DrawConsoleWindow() 
{
	ImGui::SetNextWindowSizeConstraints(ImVec2(300, 100), ImVec2(FLT_MAX, FLT_MAX));
	if (ImGui::Begin("Console")) {
		if (ImGui::Button("Clear")) {
			ClearLog();
		}

		bool filterError = GetFilterError();
		bool filterSimple = GetFilterSimple();
		bool filterWarning = GetFilterWarning();

		ImGui::SameLine();
		if (ImGui::Checkbox("Error", &filterError)) { SetFilterError(filterError); }
		ImGui::SameLine();
		if (ImGui::Checkbox("Warning", &filterWarning)) { SetFilterWarning(filterWarning); }
		ImGui::SameLine();
		if (ImGui::Checkbox("Simple", &filterSimple)) { SetFilterSimple(filterSimple); }
		ImGui::Separator();

		float scrollHeight = ImGui::GetWindowSize().y - 70;

		ImGui::BeginChild("ScrollingRegion", ImVec2(0, scrollHeight), true, ImGuiWindowFlags_HorizontalScrollbar);

		Console& console = Console::getInstance();
		const std::vector<std::string>& logsconsole = console.GetLogs();

		for (const auto& msg : logsconsole) {
			if (GetFilterError() && msg.find("Error") != std::string::npos) {
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), msg.c_str());
			}
			else if (GetFilterWarning() && msg.find("Warning") != std::string::npos) {
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), msg.c_str());
			}
			else if (GetFilterSimple()){
				ImGui::TextUnformatted(msg.c_str());
			}
		}
		ImGui::EndChild();
	}
	ImGui::End();

}

void ImGuiModule::DrawFilesExplorerWindow() {

	// File research
	// Type filtrer
	ImGui::SetNextWindowSizeConstraints(ImVec2(450, 150), ImVec2(FLT_MAX, FLT_MAX));
	if (ImGui::Begin("Files Explorer (0.7v)")) 
	{
		FilesDirs filesdirs;

		ImGui::SetNextItemWidth(250);
		char* charBuffer1 = new char[100];
		strcpy_s(charBuffer1, 100, GetCurrentDir().c_str());
		if (ImGui::InputText("Change Dir", charBuffer1, 100)) { SetCurrentDir(charBuffer1); }
		delete[] charBuffer1;
		ImGui::SameLine();
		ImGui::Checkbox("Add button", &addTexButton);

		if (ImGui::Button("Add Textures Dir")) 
		{
			AddLog("Files found in ../Textures : " + std::to_string(filesdirs.FilesInDir("../Textures")));
			std::string str;
			std::vector<std::wstring> filenames = filesdirs.GetFilesInDir(filesdirs.ConvertStringToWideString(GetCurrentDir()));
			std::vector<std::string>* listTexturesNames = moduleManager->GetModule<RHIVulkanModule>()->GetListTexturesNames();
			for (const auto& filenames_wide : filenames) {
				str += filesdirs.ConvertWideStringToString(filenames_wide) + ", ";

				std::string ext;
				std::string filename;
				filesdirs.ExtractFilenameAndExtension(GetCurrentDir() + filesdirs.ConvertWideStringToString(filenames_wide), filename, ext);
				
				if (filesdirs.IsImageExtension(ext))
				{
					if (std::find(listTexturesNames->begin(), listTexturesNames->end(), filesdirs.ConvertWideStringToString(filenames_wide)) != listTexturesNames->end())
					{
						AddLog("Texture already added : " + filesdirs.ConvertWideStringToString(filenames_wide));
					}
					else {
						AddLog("Texture not added yet : " + filesdirs.ConvertWideStringToString(filenames_wide));
						moduleManager->GetModule<RHIVulkanModule>()->AddTextureToPool(GetCurrentDir() + "/" + filesdirs.ConvertWideStringToString(filenames_wide));
						moduleManager->GetModule<RHIVulkanModule>()->AddListTexturesNames(filesdirs.ConvertWideStringToString(filenames_wide));
						AddLog("Texture has been added : " + filesdirs.ConvertWideStringToString(filenames_wide));
						VkDescriptorSet textureID = ImGui_ImplVulkan_AddTexture(
							moduleManager->GetModule<RHIVulkanModule>()->GetListTextures().back().sampler,
							moduleManager->GetModule<RHIVulkanModule>()->GetListTextures().back().imageView,
							VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
						);

						ListDescriptorsImGui->push_back(textureID);
					}
				}
			
			}
			AddLog("Find files in ../Textures :" + str);
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(200);
		char* charBuffer = new char[100];
		strcpy_s(charBuffer, 100, GetFileToLook().c_str());
		if (ImGui::InputText("-", charBuffer, 100)) { SetFileToLook(charBuffer); }
		delete[] charBuffer;

		ImGui::SameLine();

		if (ImGui::Button("Add Tex")) 
		{
			std::string ext;
			std::string filename;
			std::ifstream file(GetFileToLook());
			filesdirs.ExtractFilenameAndExtension(GetFileToLook(), filename, ext);

			if (file.good()) {
				if (filesdirs.IsImageExtension(ext))
				{
					moduleManager->GetModule<RHIVulkanModule>()->AddTextureToPool(fileToLook);
					moduleManager->GetModule<RHIVulkanModule>()->AddListTexturesNames(fileToLook);

					VkDescriptorSet textureID = ImGui_ImplVulkan_AddTexture(
						moduleManager->GetModule<RHIVulkanModule>()->GetListTextures().back().sampler,
						moduleManager->GetModule<RHIVulkanModule>()->GetListTextures().back().imageView,
						VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
					);

					ListDescriptorsImGui->push_back(textureID);

					AddLog("Texture has been added : " + fileToLook);
					SetFileToLook("");
				}
			}
			else {
				AddLog("Warning : Filepath incorrect : " + fileToLook);
			}
		}
		if (ImGui::Button("Add File (Window File Explorer)", ImVec2(100,20))) {
			FilesDirs filesDirs;
			std::string file = filesDirs.ConvertWideStringToString(filesDirs.openImageFileDialog());
			AddLog("Texture File :" + file);
			std::string filename;
			std::string ext;
			filesDirs.ExtractFilenameAndExtension(file, filename, ext);
			moduleManager->GetModule<RHIVulkanModule>()->AddTextureToPool(file);
			moduleManager->GetModule<RHIVulkanModule>()->AddListTexturesNames(filename + "." + ext);

			VkDescriptorSet textureID = ImGui_ImplVulkan_AddTexture(
				moduleManager->GetModule<RHIVulkanModule>()->GetListTextures().back().sampler,
				moduleManager->GetModule<RHIVulkanModule>()->GetListTextures().back().imageView,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			);

			ListDescriptorsImGui->push_back(textureID);

			AddLog("Texture has been added : " + file);
		}

		ImGui::Separator();

		float scrollHeight = ImGui::GetWindowSize().y - 110;

		bool filterobj = GetFilterObj();
		bool filterimg = GetFilterSupportedImages();
		bool filterother = GetFilterOther();
		bool filterdirs = GetFilterDirs();

		if(ImGui::Checkbox("obj", &filterobj)) {SetFilterObj(filterobj);}
		ImGui::SameLine();
		if(ImGui::Checkbox("image", &filterimg)) { SetFilterSupportedImages(filterimg); }
		ImGui::SameLine();
		if(ImGui::Checkbox("other", &filterother)) { SetFilterOther(filterother); }
		ImGui::SameLine();
		if(ImGui::Checkbox("dir", &filterdirs)) { SetFilterDirs(filterdirs); }
		ImGui::SameLine();

		char* charBuffer2 = new char[30];
		strcpy_s(charBuffer2, 30, GetFileSearch().c_str());
		ImGui::SetNextItemWidth(150);
		if (ImGui::InputText("Search", charBuffer2, 30)) { SetFileSearch(charBuffer2); }

		ImGui::BeginChild("ScrollingRegion", ImVec2(0, scrollHeight), true, ImGuiWindowFlags_HorizontalScrollbar);
		// A opti (baisse de perf monstrueuse quand gros dossiers
		// Fait à chaque frame ducoup énormé désastre de performance 
		
		// Refresh pas les files
		if (refreshFileExplorer) {
			
			if (GetFilterDirs()) {
				std::vector<std::string> dirNames = filesdirs.GetDirectoryNames(GetCurrentDir());

				for (const auto& dirname : dirNames)
				{
					ImGui::Text(dirname.c_str());
				}
				ImGui::Separator();
			}

			fileNames = filesdirs.GetFilesInDir(filesdirs.ConvertStringToWideString(GetCurrentDir()));
			for (const auto& filenames_wide : fileNames)
			{
				std::string filenameWideString = filesdirs.ConvertWideStringToString(filenames_wide);
				if (filesdirs.ContainsSubstring(filenameWideString, GetFileSearch())) {

					std::string ext;
					std::string filename;
					
					const char* filenameWideStringCstr = filenameWideString.c_str();
					// Opti en mettant extract juste l'ext, pas besoin de plus
					filesdirs.ExtractFilenameAndExtension(currentDir + "/" + filenameWideString, filename, ext);

					if (ext == "obj")
					{
						if (filterObj)
						{
							ImGui::Text(filenameWideStringCstr);
						}
					}
					else if (filesdirs.IsImageExtension(ext))
					{
						if (filterSupportedImages)
						{
							ImGui::Text(filenameWideStringCstr);
							if (addTexButton) {
								ImGui::SameLine();
								if (ImGui::Button("Add", ImVec2(35, 25))) {
									moduleManager->GetModule<RHIVulkanModule>()->AddTextureToPool(GetCurrentDir() + "/" + filenameWideString);
									moduleManager->GetModule<RHIVulkanModule>()->AddListTexturesNames(filenameWideString);
									AddLog("Texture has been added : " + filenameWideString);
									VkDescriptorSet textureID = ImGui_ImplVulkan_AddTexture(
										moduleManager->GetModule<RHIVulkanModule>()->GetListTextures().back().sampler,
										moduleManager->GetModule<RHIVulkanModule>()->GetListTextures().back().imageView,
										VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
									);

									ListDescriptorsImGui->push_back(textureID);
								}
							}
						}
					}
					else if (filterOther)
					{
						ImGui::Text(filenameWideStringCstr);
					}
				}
			}
			//refreshFileExplorer = false;
		}
		//else {
		//	for (const auto& filenames_wide : fileNames)
		//	{
		//		std::string ext;
		//		std::string filename;
		//		std::ifstream file(filenames_wide);

		//		filesdirs.ExtractFilenameAndExtension(currentDir + "/" + filesdirs.ConvertWideStringToString(filenames_wide), filename, ext);

		//		if (filesdirs.ContainsSubstring(filename, GetFileSearch())) {
		//			if (ext == "obj")
		//			{
		//				if (filterObj)
		//				{
		//					ImGui::Text(filesdirs.ConvertWideStringToString(filenames_wide).c_str());
		//				}
		//			}
		//			else if (ext == "png" || ext == "jpg" || ext == "gif" || ext == "tga" || ext == "bmp" || ext == "psd" || ext == "hdr" || ext == "pic")
		//			{
		//				if (filterSupportedImages)
		//				{
		//					ImGui::Text(filesdirs.ConvertWideStringToString(filenames_wide).c_str());
		//					ImGui::SameLine();
		//					if (ImGui::Button("Add", ImVec2(35, 25))) {
		//						moduleManager->GetModule<RHIVulkanModule>()->AddTextureToPool(GetCurrentDir() + "/" + filesdirs.ConvertWideStringToString(filenames_wide));
		//						// Décomposer la string pour garder ce qu'il y a après le dernier /
		//						moduleManager->GetModule<RHIVulkanModule>()->AddListTexturesNames(filesdirs.ConvertWideStringToString(filenames_wide));
		//						AddLog("Texture has been added : " + filesdirs.ConvertWideStringToString(filenames_wide));
		//					}
		//				}
		//			}
		//			else if (filterOther)
		//			{
		//				ImGui::Text(filesdirs.ConvertWideStringToString(filenames_wide).c_str());
		//			}
		//		}
		//	}
		//}

		ImGui::EndChild();

	}
	ImGui::End();
}

void ImGuiModule::DrawProjectSaveWindow() 
{

	// Bouton save
	// Bouton charger save
	// Bouton nouveau projet


	// ATTENTION : Je crois il manque la verif de si c'est un bon filepath, donc faut check au cas où
	// ATTENTION : Je check juste si c'est un json -> pas forcément le bon format de données

	if (showPopupProject) {
		ImGui::OpenPopup("Project");
		if (ImGui::BeginPopupModal("Project", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			FilesDirs filesDirs;
			if (ImGui::Button("X", ImVec2(20, 20))) {
				showPopupProject = false;
				selectedGameObject = nullptr;
			}
			ImGui::Text("Here you can save, load or start a new project !");
			ImGui::SetNextItemWidth(200);
			std::string projectName;
			char* charBuffer = new char[100];
			strcpy_s(charBuffer, 100, GetSceneToName().c_str());
			if (ImGui::InputText("Project Name", charBuffer, 100)) { projectName = charBuffer; }
			delete[] charBuffer;

			if (ImGui::Button("Save", ImVec2(120, 0))) {
				ImGui::CloseCurrentPopup();
				showPopupProject = false;
				selectedGameObject = nullptr;

				// Tu peux utiliser 
				// filesDirs.addPathToANFile(filesDirs.ConvertStringToWideString("filepath"));
				// pour save le path comme ça tu save
				// Tu créé une instance de FilesDirs et puis tu fait le truc

				//----------------------------------------------------------------------
				// TA FONCTION POUR SAVE
				moduleManager->GetModule<SceneManager>()->SaveScenesToFile("project");


			}
			if (ImGui::Button("Load", ImVec2(120, 0))) {
				ImGui::CloseCurrentPopup();
				showPopupProject = false;
				selectedGameObject = nullptr;

				std::string file = filesDirs.ConvertWideStringToString(filesDirs.openFileDialog());
				AddLog("File :" + file);
				std::string filename;
				std::string ext;
				filesDirs.ExtractFilenameAndExtension(file, filename, ext);
				if (ext == "json") {
					filesDirs.createEngineDataDirectory();
					filesDirs.createANFileInDirectory();
					filesDirs.addPathToANFile(filesDirs.ConvertStringToWideString(file));
					//----------------------------------------------------------------------
					// TA FONCTION POUR LOAD (en utilisant "file" pour le path)
					moduleManager->GetModule<SceneManager>()->LoadSceneFromFile(filename);


				}
			}
			if (ImGui::Button("New", ImVec2(120, 0))) {
				ImGui::CloseCurrentPopup();
				showPopupProject = false;
				selectedGameObject = nullptr;

				//----------------------------------------------------------------------
				// TA FONCTION POUR NEW


			}

			filesDirs.createEngineDataDirectory();
			filesDirs.createANFileInDirectory();
			filesDirs.addPathToANFile(filesDirs.ConvertStringToWideString("CoolMonsieur.json"));

			std::vector<std::wstring> pathsAN = filesDirs.readANFile();

			ImGui::Separator();
			ImGui::Text("Already added projects :");
			ImGui::BeginChild("ScrollingRegion", ImVec2(0, 100), true, ImGuiWindowFlags_HorizontalScrollbar);
			for (const auto& wpath : pathsAN) 
			{
				std::string spath = filesDirs.ConvertWideStringToString(wpath);
				ImGui::Text(spath.c_str());
				ImGui::SameLine();
				if (ImGui::Button("Load", ImVec2(50, 20))) {
					showPopupProject = false;
					selectedGameObject = nullptr;

					std::ifstream file(spath);
					if (file.good()) {
						moduleManager->GetModule<SceneManager>()->LoadAllScenesSPath(spath);
					}
					else {
						AddLog("Warning : Failed to open the file");
					}
				}
			}
			ImGui::EndChild();

			ImGui::EndPopup();
		}
	}

}





void ImGuiModule::DisplayTransform(Transform* _transform) {
	if (!_transform) return;

	// Position
	glm::vec3 position = _transform->GetPosition();
	if (ImGui::DragFloat3("Position", &position[0], 0.1f, 0.0f, 0.0f, "%.2f")) {
		_transform->SetPosition(position);
	}
	if (ImGui::Button("Position Reset", ImVec2(110, 25))) {
		_transform->SetPosition(glm::vec3(0, 0, 0));
	}

	// Rotation
	glm::vec3 rotationDegrees = _transform->GetRotationDegrees();
	if (ImGui::DragFloat3("Rotation", &rotationDegrees[0], 0.1f, 0.0f, 0.0f, "%.2f")) {
		_transform->SetRotationDegrees(rotationDegrees);
	}
	if (ImGui::Button("Rotation Reset", ImVec2(110,25))) {
		_transform->SetRotationDegrees(glm::vec3(0,0,0));
	}

	// Scale
	glm::vec3 scale = _transform->GetScale();
	glm::vec3 scale_buff = _transform->GetScale();
	if (ImGui::DragFloat3("Scale", &scale[0],0.1f,0.0f,0.0f,"%.2f")) {
		if (changeScaleLinked) {
			if (scale.x - scale_buff.x != 0) {
				scale.y += scale.x - scale_buff.x;
				scale.z += scale.x - scale_buff.x;
			}
			else if (scale.y - scale_buff.y != 0) {
				scale.x += scale.y - scale_buff.y;
				scale.z += scale.y - scale_buff.y;
			}
			else if (scale.z - scale_buff.z != 0) {
				scale.x += scale.z - scale_buff.z;
				scale.y += scale.y - scale_buff.y;
			}
		}
		_transform->SetScale(scale);
	}


	if (ImGui::Button("Scale Reset", ImVec2(110, 25))) {
		_transform->SetScale(glm::vec3(1, 1, 1));
	}

	if (ImGui::Checkbox("Linked Change", &changeScaleLinked)) {

	}

}

// ----------========== POPUPS ==========---------- //

void ImGuiModule::ShowRenamePopup() {
	// Gestion de la fenêtre popup pour renommer un gameobject
	if (isRenamePopupOpen && selectedGameObject) {
		ImGui::OpenPopup("Rename Entity");
		if (ImGui::BeginPopup("Rename Entity")) {
			ImGui::InputText("##edit", renameBuffer, IM_ARRAYSIZE(renameBuffer));
			if (ImGui::Button("OK##Rename")) {
				RenameGameObject(selectedGameObject, std::string(renameBuffer));
				isRenamePopupOpen = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel##Rename")) {
				isRenamePopupOpen = false;
			}
			ImGui::EndPopup();
		}
	}

	// Gestion de la fenêtre popup pour renommer la scène
	if (sceneToRename >= 0) {
		ImGui::OpenPopup("Rename Scene");
		if (ImGui::BeginPopup("Rename Scene")) {
			ImGui::InputText("New Name", renameSceneBuffer, IM_ARRAYSIZE(renameSceneBuffer));
			if (ImGui::Button("OK")) {
				const auto& scenes = sceneManager->GetScenes();
				if (sceneToRename < scenes.size()) {
					sceneManager->RenameScene(scenes[sceneToRename]->GetName(), renameSceneBuffer);
				}
				sceneToRename = -1;
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				sceneToRename = -1;
			}
			ImGui::EndPopup();
		}
	}
}

// ----------========== RENAME / DELETE / DUPLICATE ==========---------- //

void ImGuiModule::RenameGameObject(GameObject* _gameObject, const std::string& _newName) {
	if (_gameObject) {
		std::cout << "Renamed GameObject: " << _gameObject->GetName() << " to " << _newName << std::endl;
		_gameObject->SetName(_newName);
	}
}
//void ImGuiModule::DeleteGameObject(GameObject* _gameObject) {
//	//AddLog("Try 1 delete");
//	if (_gameObject) {
//		//AddLog("Try 2 delete");
//		BaseScene* currentScene = sceneManager->GetCurrentScene();
//		if (currentScene) {
//			//AddLog("Try 3 delete");
//
//			// Forcément vu que personne a fait de delete sur les gameobjects
//			currentScene->RemoveObject(_gameObject, true); // Suppression de l'objet
//		}
//	}
//}
//void ImGuiModule::DuplicateGameObject(GameObject* _gameObject) {
//}

void            ImGuiModule::CreateSpecificGameObject(const GameObjectType _type, const int _otherType) {
	if (BaseScene* current_scene = sceneManager->GetCurrentScene()) {
		GameObject* new_game_object = nullptr;
		switch (_type) {
		case GameObjectType::Cube:
			new_game_object = current_scene->CreateCubeGameObject(_otherType);
			break;
		case GameObjectType::Light:
			new_game_object = current_scene->CreateLightGameObject();
			break;
		case GameObjectType::Plane:
			new_game_object = current_scene->CreatePlaneGameObject();
			break;
		case GameObjectType::Vase:
			new_game_object = current_scene->CreateVaseGameObject(_otherType);
			break;
		case GameObjectType::Girl:
			new_game_object = current_scene->CreateGirlGameObject();
			break;
		case GameObjectType::Noob:
			new_game_object = current_scene->CreateNoobGameObject();
			break;
		case GameObjectType::Sphere:
			new_game_object = current_scene->CreateSphereGameObject();
			break;
		case GameObjectType::Multiple:
			new_game_object = current_scene->CreateMultipleGameObject(_otherType);
			break;

		}

		if (new_game_object) {
			current_scene->AddRootObject(new_game_object);
		}
	}
}

void ImGuiModule::HandleShortcuts()
{
	ImGuiIO& io = ImGui::GetIO();

	bool ctrlPressed = io.KeyCtrl;
	bool shiftPressed = io.KeyShift;
	bool altPressed = io.KeyAlt;

	if (ctrlPressed && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_D)))
	{
		if (selectedGameObject != nullptr) {
			DuplicateGameObject(selectedGameObject);
		}
	}

	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete))) {
		if (selectedGameObject != nullptr) {
			DeleteGameObject(selectedGameObject);
		}
	}


}

void ImGuiModule::DuplicateGameObject(GameObject* _gameObject) {
	auto& gameObjects = sceneManager->GetCurrentScene()->rootObjects;
	std::vector<GameObject*> updatedObjects = gameObjects;

	const auto newGameObject = GameObject::CreatePGameObject();
	newGameObject->SetName(_gameObject->GetName());
	newGameObject->SetFileModel(_gameObject->GetFileModel());
	newGameObject->SetModel(_gameObject->GetModel());
	newGameObject->GetTransform()->SetPosition(_gameObject->GetPosition());
	newGameObject->GetTransform()->SetScale(_gameObject->GetScale());
	newGameObject->GetTransform()->SetRotation(_gameObject->GetRotation());
	newGameObject->SetTexture(_gameObject->GetTexture());


	//sceneManager->GetCurrentScene()->AddRootObject(newGameObject);

	updatedObjects.push_back(newGameObject);
	gameObjects = updatedObjects;
	// Le game object est pas instant add avec le "AddRootObject"
	selectedGameObject = gameObjects[gameObjects.size() - 1];

	AddLog("One object as been duplicated ! From : "+_gameObject->GetName());
}

void ImGuiModule::DeleteGameObject(GameObject* _gameObject) {
	auto& gameObjects = sceneManager->GetCurrentScene()->rootObjects;
	std::vector<GameObject*> updatedObjects;

	for (auto obj : gameObjects) {
		if (obj->GetId() != _gameObject->GetId()) {
			updatedObjects.push_back(obj);
		}
		else {
			std::string name = _gameObject->GetName();
			obj->SetModel(nullptr);
			delete obj;
			AddLog("Object Deleted : " + name);
		}
	}

	//sceneManager->GetCurrentScene()->RemoveAllObjects();
	//std::cout << "Size 1: " << sceneManager->GetCurrentScene()->rootObjects.size(); // 13
	//std::cout << "Size 2:" << updatedObjects.size(); // 12
	sceneManager->GetCurrentScene()->rootObjects = updatedObjects;
	selectedGameObject = nullptr;
}
