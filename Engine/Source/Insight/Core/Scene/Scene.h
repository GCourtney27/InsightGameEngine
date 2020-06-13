#pragma once

#include <Insight/Core.h>

#include "Scene_Node.h"
#include "Insight/Systems/Managers/Resource_Manager.h"
#include "Insight/Runtime/APlayer_Character.h"
#include "Insight/Rendering/Rendering_Context.h"
#include "Insight/Rendering/APost_Fx.h"
#include "Insight/Systems/File_System.h"

namespace Insight {

	class INSIGHT_API Scene
	{
	public:
		Scene();
		~Scene();

		SceneNode* GetRootNode() const { return m_pSceneRoot; }
		bool LoadFromJson(const std::string& fileName);

		bool Init(const std::string fileName);
		bool PostInit();
		void BeginPlay();
		void Tick(const float& deltaMs);
		void OnUpdate(const float& deltaMs);
		void OnImGuiRender();
		void OnPreRender();
		void OnRender();
		void OnMidFrameRender();
		void OnPostRender();
		void Destroy();
		void Flush();

		// Editor
		void SetSelectedActor(AActor* actor) { m_pSelectedActor = actor; }
		void SetDisplayName(const std::string& name) { m_DisplayName = name; }

	private:
		void RenderSceneHeirarchy();
		void RenderInspector();
		void RenderCreatorWindow();
	private:
		APlayerCharacter* m_pPlayerCharacter = nullptr;
		Vector3 newPos;
		AActor* m_pSelectedActor = nullptr;

		SceneNode* m_pSceneRoot = nullptr;
		std::shared_ptr<RenderingContext> m_Renderer = nullptr;
		std::string m_DisplayName;

	private:
		// TODO move this to engine class
		FileSystem m_FileSystem;
		ResourceManager m_ResourceManager;

	};

}