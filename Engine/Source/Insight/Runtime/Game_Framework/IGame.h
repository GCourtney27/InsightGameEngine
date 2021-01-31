#pragma once

//#include <Insight/Core.h>
//#include "Insight/Runtime/Archetypes/ACamera.h"
#include "Insight/Runtime/Game_Framework/GameCore.h"


namespace Insight
{

	enum class ModuleLoadStatus
	{
		Failed = 0,
		Success = 1,
		Waiting = 2,
	};
	//using namespace Runtime;

	class GAME_API IGame
	{
		friend class Application;
	protected:
		IGame() 
		{
			//constexpr bool d = IE_PLATFORM_BUILD_UWP
			//std::enable_if<IE_DEBUG == 1, Scene> i;
		}
		virtual ~IGame() {}
		

		// Called when the game simulation starts. 
		void OnSimulationStart();
		// Called when the main application updates.
		virtual void OnAppUpdate() {}
		// Called when the game simulation is updated.
		void OnSimulationUpdate();
		
		// Load a scene.
		void LoadScene(const char* SceneName);
		// Destroy the currently active scene and load a new one.
		void FlushScene(const char* SceneName);

		/*Actor* AddActor();
		void RemoveActor();*/

		virtual int TESTGetVal() = 0;

	public:
		// Called when the game dll is loaded.
		virtual void OnDllLoad() {}
		// Called when the game dll is uloaded
		virtual void OnDllUnload() {}

		virtual ModuleLoadStatus GetLoadStatus() { return ModuleLoadStatus::Failed; }
		static inline bool CheckLoadSuccess(const ModuleLoadStatus& Status) { return Status == ModuleLoadStatus::Success; }

	protected:
		//ACamera* m_Camera;
		//Scene m_Scene;
		// TODO ECS::World m_World
	};
}