#pragma once

#include <Insight/Core.h>

#include "Insight/Systems/Managers/Resource_Manager.h"


namespace Insight {

	namespace Runtime {
		class APlayerCharacter;
		class APlayerStart;
		class ACamera;
	}

	class SceneNode;

	class INSIGHT_API Scene
	{
	public:
		Scene();
		~Scene();

		// Get the scene root of the application that all 
		// other game actors in the world are children too.
		SceneNode* GetRootNode() const { return m_pSceneRoot; }
		// Write scene out to JSON file.
		bool WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer);

		// Initialize the scene
		bool Init(const std::string& fileName);
		// Post-Initialize of the scene. Used for dispatching 
		// initialization commands for actors on the GPU (Mesh 
		// creation, texture generation etc..)
		bool PostInit();
		// Called when the world begins a play session. Or a new Scene 
		// is loaded during game runtime 
		void BeginPlay();
		// Called once per frame during game runtime. 
		void Tick(const float DeltaMs);
		// Called once per frame during the application's runtime. Some resoruces
		// aways need to be updated, such as pushing geometry to the GPU for example. This
		// guarantees it get exexcuted regardless if the game is simmulating or not.
		void OnUpdate(const float DeltaMs);
		// Render an ImGui widget for thie module.
		void OnImGuiRender();
		// Destroys the scene and releases resources. DOES NOT save scene.
		void Destroy();
		// Close the current scene and open a new scene.
		bool FlushAndOpenNewScene(const std::string& NewScene);

		SceneNode& GetSceneRoot() { return *m_pSceneRoot; }
		Runtime::ACamera& GetSceneCamera() { return *m_pCamera; }

		// Add an actor to the scene.
		void AddActor(Runtime::AActor* pActor) { m_pSceneRoot->AddChild(pActor); }

		// Set the name of the level.
		void SetDisplayName(const std::string& name) { m_DisplayName = name; }
		// Gets the name of the level.
		std::string GetDisplayName() { return m_DisplayName; }
		// Ends the game runtime simulation for the editor.
		void EndPlaySession();
		// Resize the number of actors the scene owns. Usually only needs to be 
		// done when a new scene is being loaded.
		void ResizeSceneGraph(size_t NewSceneSize) { m_pSceneRoot->ResizeNumChildren(NewSceneSize); }
		// Get the number of actors that are currently in the scene.
		uint32_t GetNumSceneActors() { return m_pSceneRoot->GetNumChildrenNodes(); }


	private:
		Runtime::APlayerCharacter* m_pPlayerCharacter = nullptr;
		Runtime::APlayerStart* m_pPlayerStart = nullptr;
		Runtime::ACamera* m_pCamera = nullptr;
		Runtime::ViewTarget m_EditorViewTarget;

		ieVector3 newPos;
		Runtime::AActor* m_pSelectedActor = nullptr;

		SceneNode* m_pSceneRoot = nullptr;
		std::string m_DisplayName;
		
	private:
		ResourceManager m_ResourceManager;
		// ECS::EntityAdmin m_EntityAdmin;
	};

}