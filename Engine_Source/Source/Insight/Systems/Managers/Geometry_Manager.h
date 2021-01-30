#pragma once

#include <Insight/Core.h>

#include "Insight/Rendering/Geometry/Model.h"

namespace Insight {


	class INSIGHT_API GeometryManager
	{
	public:
		typedef std::vector<std::shared_ptr<Model>> SceneModels;
		typedef uint64_t VertexBufferHandle;
		typedef uint64_t IndexBufferHandle;
		friend class D3D12GeometryManager;
		friend class D3D11GeometryManager;
	public:
		GeometryManager();
		virtual ~GeometryManager();

		static bool CreateInstance();
		static void Shutdown();

		static GeometryManager& Get() { return *s_Instance; }
		SceneModels* GetSceneModels() { return &m_OpaqueModels; }

		static bool Init() { return s_Instance->Init_Impl(); }

		// Issue draw commands to all models attached to the geometry manager.
		static void Render(RenderPassType RenderPass) { s_Instance->Render_Impl(RenderPass); }
		// Gather all geometry in the scene and uplaod their constant buffers to the GPU.
		// Should only be called once, before 'Render()'. Does not draw models.
		static void GatherGeometry() { s_Instance->GatherGeometry_Impl(); }
		// UnRegister all model in the model cache. Usually used 
		// when switching scenes.
		static void FlushModelCache();
		
		// Register a model with an opaque material to be drawn in the geometry pass
		static void RegisterOpaqueModel(StrongModelPtr Model) { s_Instance->m_OpaqueModels.push_back(Model); }
		// Unregister a model with an opaque material to not be drawn in the geometry pass
		static void UnRegisterOpaqueModel(StrongModelPtr Model);
		
		// Register a model with a translucent material to be drawn in the translucency pass
		static void RegisterTranslucentModel(StrongModelPtr Model) { s_Instance->m_TranslucentModels.push_back(Model); }
		// Unregister a model with a translucent material to not be drawn in the translucency pass
		static void UnRegisterTranslucentModel(StrongModelPtr Model);

		static inline VertexBufferHandle CreateVertexBuffer() { return s_Instance->CreateVertexBuffer_Impl(); }
		static inline IndexBufferHandle CreateIndexBuffer() { return s_Instance->CreateIndexBuffer_Impl(); }


	protected:
		virtual bool Init_Impl() = 0;
		virtual void Render_Impl(RenderPassType RenderPass) = 0;
		virtual void GatherGeometry_Impl() = 0;
		virtual VertexBufferHandle CreateVertexBuffer_Impl() = 0;
		virtual IndexBufferHandle CreateIndexBuffer_Impl() = 0;

	protected:
		SceneModels m_OpaqueModels;
		SceneModels m_TranslucentModels;

	private:
		static GeometryManager* s_Instance;
	};

}
