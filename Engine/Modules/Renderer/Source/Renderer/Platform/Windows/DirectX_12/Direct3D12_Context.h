#pragma once

#include <Insight/Core.h>

#include "Renderer/Renderer.h"

#include "Platform/Windows/Error/COM_Exception.h"
#include "Renderer/Platform/Windows/DirectX_12/D3D12_Helper.h"
#include "Renderer/Platform/Windows/DirectX_12/Descriptor_Heap_Wrapper.h"
#include "Renderer/Platform/Windows/DirectX_Shared/Constant_Buffer_Types.h"
#include "Renderer/Platform/Windows/DirectX_12/Ray_Tracing/Ray_Trace_Helpers.h"
#include "Renderer/Platform/Windows/DirectX_12/ie_D3D12_Screen_Quad.h"

#include "Insight/Rendering/Lighting/ADirectional_Light.h"
#include "Renderer/Platform/Windows/DirectX_12/D3D12_Constant_Buffer_Wrapper.h"

/*
	Render context for Windows Direct3D 12 API. 
*/

using Microsoft::WRL::ComPtr;

#define IE_D3D12_FrameIndex m_d3dDeviceResources.GetFrameIndex()

namespace Insight {

	class WindowsWindow;
	class GeometryManager;
	class ieD3D12SphereRenderer;

	class INSIGHT_API Direct3D12Context : public Renderer
	{
	public:
		friend class Renderer;
		friend class D3D12Helper;
	public:
		virtual bool Init_Impl() override;
		virtual void Destroy_Impl() override;
		virtual bool PostInit_Impl() override;
		virtual void OnUpdate_Impl(const float DeltaMs) override;
		virtual void OnPreFrameRender_Impl() override;
		virtual void OnRender_Impl() override;
		virtual void OnMidFrameRender_Impl() override;
		virtual void ExecuteDraw_Impl() override;
		virtual void SwapBuffers_Impl() override;
		virtual void OnWindowResize_Impl() override;
		virtual void OnWindowFullScreen_Impl() override;
		virtual void OnShaderReload_Impl() override;

		virtual void SetVertexBuffers_Impl(uint32_t StartSlot, uint32_t NumBuffers, ieVertexBuffer* pBuffers) override;
		virtual void SetIndexBuffer_Impl(ieIndexBuffer* pBuffer) override;
		virtual void DrawIndexedInstanced_Impl(uint32_t IndexCountPerInstance, uint32_t NumInstances, uint32_t StartIndexLocation, uint32_t BaseVertexLoaction, uint32_t StartInstanceLocation) override;

		virtual void RenderSkySphere_Impl() override;
		virtual bool CreateSkybox_Impl() override;
		virtual void DestroySkybox_Impl() override;

		WindowsWindow& GetWindowRef() const { return *m_pWindowRef; }

		inline ID3D12Device& GetDeviceContext() const { return m_d3dDeviceResources.GetDeviceContext(); }

		inline ID3D12GraphicsCommandList& GetScenePassCommandList() const { return *m_pScenePass_CommandList.Get(); }
		inline ID3D12GraphicsCommandList& GetPostProcessPassCommandList() const { return *m_pPostEffectsPass_CommandList.Get(); }
		inline ID3D12GraphicsCommandList& GetShadowPassCommandList() const { return *m_pShadowPass_CommandList.Get(); }
		inline ID3D12GraphicsCommandList& GetTransparencyPassCommandList() const { return *m_pTransparencyPass_CommandList.Get(); }

		inline ID3D12CommandQueue& GetCommandQueue() const { return m_d3dDeviceResources.GetCommandQueue(); }
		inline CDescriptorHeapWrapper& GetCBVSRVDescriptorHeap() { return m_cbvsrvHeap; }
		
		inline ID3D12Resource& GetConstantBufferPerObjectUploadHeap() const { return *m_CBPerObject[IE_D3D12_FrameIndex].GetResource(); }
		inline UINT8* GetPerObjectCBVGPUHeapAddress() { return m_CBPerObject[IE_D3D12_FrameIndex].GetGPUAddress(); }
		inline ID3D12Resource& GetConstantBufferPerObjectMaterialUploadHeap() const { return *m_CBPerObjectMaterial[IE_D3D12_FrameIndex].GetResource(); }
		inline UINT8* GetPerObjectMaterialAdditiveCBVGPUHeapAddress() { return m_CBPerObjectMaterial[IE_D3D12_FrameIndex].GetGPUAddress(); }

		const CB_PS_VS_PerFrame& GetPerFrameCB() const { return m_CBPerFrame.Data; }

		// Ray Tracing
		// -----------
		inline ID3D12GraphicsCommandList4& GetRayTracePassCommandList() const { return *m_pRayTracePass_CommandList.Get(); }
		ID3D12Resource* GetRayTracingSRV() const { return m_RayTraceOutput_SRV.Get(); }
		[[nodiscard]] uint32_t RegisterGeometryWithRTAccelerationStucture(ComPtr<ID3D12Resource> pVertexBuffer, ComPtr<ID3D12Resource> pIndexBuffer, uint32_t NumVerticies, uint32_t NumIndices, DirectX::XMMATRIX MeshWorldMat);
		void UpdateRTAccelerationStructureMatrix(uint32_t InstanceArrIndex, DirectX::XMMATRIX NewWorldMat) { m_RTHelper.UpdateInstanceTransformByIndex(InstanceArrIndex, NewWorldMat); }

		ID3D12Resource* GetSwapChainRenderTarget() const { return m_pRenderTargets[IE_D3D12_FrameIndex].Get(); }
		const UINT GetNumLightPassRTVs() const { return m_NumLightPassRTVs; }
		inline D3D12_CPU_DESCRIPTOR_HANDLE GetSwapChainRTV() const
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle;
			handle.ptr = m_SwapChainRTVHeap.hCPUHeapStart.ptr + m_SwapChainRTVHeap.HandleIncrementSize * IE_D3D12_FrameIndex;
			return handle;
		}

		
	private:
		Direct3D12Context(WindowsWindow* windowHandle);
		virtual ~Direct3D12Context();

		void CloseCommandListAndSignalCommandQueue();
		
		// Threading

		void LoadContexts();
		void WorkerThread(uint8_t ThreadIndex);

		// Per-Frame
		
		void BindShadowPass();
		void BindGeometryPass();
		void BindLightingPass();
		void BindSkyPass();
		void BindTransparencyPass();
		void BindRayTracePass();
		void BindPostFxPass();

		// D3D12 Initialize
		
		void CreateSwapChainRTVDescriptorHeap();

		// Create app resources
		
		void CreateDSVs();
		void CreateRTVs();
		void CreateCBVs();
		void CreateSRVs();
		void CreateDeferredShadingRootSignature();
		void CreateForwardShadingRootSignature();

		void CreateShadowPassPSO();
		void CreateGeometryPassPSO();
		void CreateSkyPassPSO();
		void CreateTransparencyPassPSO();
		void CreateLightPassPSO();
		void CreatePostEffectsPassPSO();

		// Create window resources
		
		void CreateCommandAllocators();
		void CreateViewport();
		void CreateScissorRect();
		void CreateScreenQuad();
		
		// Close GPU handle and release resources for the D3D 12 context.
		void InternalCleanup();
		// Resize render targets and depth stencil. Usually called from 'OnWindowResize'.
		void UpdateSizeDependentResources();

		void ResourceBarrier(ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* pResource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter);
		
	private:
		WindowsWindow*		m_pWindowRef = nullptr;
		D3D12Helper			m_d3dDeviceResources;
		RayTraceHelpers		m_RTHelper;

		ieD3D12ScreenQuad	m_ScreenQuad;
		D3D12_VIEWPORT		m_ShadowPass_ViewPort = {};
		D3D12_RECT			m_ShadowPass_ScissorRect = {};

		ieD3D12SphereRenderer*	m_pSkySphere_Geometry;

		// Threading
		HANDLE m_WorkerThreadPreFrameRender[s_NumRenderContexts];
		HANDLE m_WorkerThreadRender[s_NumRenderContexts];
		HANDLE m_WorkerThreadExecuteDraw[s_NumRenderContexts];
		struct ThreadParameter
		{
			uint8_t ThreadIndex;
		};
		ThreadParameter m_ThreadParameters[s_NumRenderContexts];

		static const UINT	m_NumLightPassRTVs = 5;
		bool				m_WindowResizeComplete = true;
		bool				m_UseWarpDevice = false;

		// D3D 12 Usings
		ComPtr<ID3D12GraphicsCommandList>	m_pActiveCommandList;

		ComPtr<ID3D12GraphicsCommandList4>	m_pRayTracePass_CommandList;
		ComPtr<ID3D12CommandAllocator>		m_pRayTracePass_CommandAllocators[m_FrameBufferCount];
		ComPtr<ID3D12GraphicsCommandList>	m_pShadowPass_CommandList;
		ComPtr<ID3D12CommandAllocator>		m_pShadowPass_CommandAllocators[m_FrameBufferCount];
		ComPtr<ID3D12GraphicsCommandList>	m_pScenePass_CommandList;
		ComPtr<ID3D12CommandAllocator>		m_pScenePass_CommandAllocators[m_FrameBufferCount];
		ComPtr<ID3D12GraphicsCommandList>	m_pTransparencyPass_CommandList;
		ComPtr<ID3D12CommandAllocator>		m_pTransparencyPass_CommandAllocators[m_FrameBufferCount];
		ComPtr<ID3D12GraphicsCommandList>	m_pPostEffectsPass_CommandList;
		ComPtr<ID3D12CommandAllocator>		m_pPostEffectsPass_CommandAllocators[m_FrameBufferCount];

		ComPtr<ID3D12Resource>				m_pRenderTargetTextures[m_NumLightPassRTVs];
		ComPtr<ID3D12Resource>				m_pRenderTargetTextures_PostFxPass[m_FrameBufferCount];
		ComPtr<ID3D12Resource>				m_pRenderTargets[m_FrameBufferCount];
		
		//-----Light Pass-----
		// 0: Albedo
		// 1: Normal
		// 2: (R)Roughness/(G)Metallic/(B)AO
		// 3: World Position
		// -----Post-Fx Pass-----
		// 4: Bloom
		CDescriptorHeapWrapper				m_rtvHeap;
		// Number of decriptors depends on frame buffer count. Start slot is 0.
		CDescriptorHeapWrapper				m_SwapChainRTVHeap;
		//0:  SceneDepth
		//1:  ShadowDepth
		CDescriptorHeapWrapper				m_dsvHeap;

		ComPtr<ID3D12Resource>				m_pFinalImage_UAV;

		ComPtr<ID3D12Resource>				m_pSceneDepthStencilTexture;
		ComPtr<ID3D12Resource>				m_pShadowDepthTexture;
		ComPtr<ID3D12Resource>				m_RayTraceOutput_SRV;

		ComPtr<ID3D12RootSignature>			m_pDeferredShadingPass_RS;
		ComPtr<ID3D12RootSignature>			m_pForwardShadingPass_RS;

		ComPtr<ID3D12PipelineState>			m_pShadowPass_PSO;
		ComPtr<ID3D12PipelineState>			m_pGeometryPass_PSO;
		ComPtr<ID3D12PipelineState>			m_pLightingPass_PSO;
		ComPtr<ID3D12PipelineState>			m_pSkyPass_PSO;
		ComPtr<ID3D12PipelineState>			m_pTransparency_PSO;
		ComPtr<ID3D12PipelineState>			m_pPostFxPass_PSO;

		//-----Pipeline-----
		//0:   SRV-Albedo(RTV->SRV)
		//1:   SRV-Normal(RTV->SRV)
		//2:   SRV-(R)Roughness/(G)Metallic/(B)AO(RTV->SRV)
		//3:   SRV-Position(RTV->SRV)
		//4:   SRV-Scene Depth(DSV->SRV)
		//5:   SRV-Bloom Pass Result(RTV->SRV)
		//6:   UAV-Final Image
		//7:   UAV-Ray Trace Output(RTHelper UAV(COPY)->SRV)
		//8:   SRV-Shadow Depth(DSV->SRV)
		//-----PerObject-----
		//9:   SRV-Albedo(SRV)
		//10:  SRV-Normal(SRV)
		//11:  SRV-Roughness(SRV)
		//12:  SRV-Metallic(SRV)
		//13:  SRV-AO(SRV)
		//14:  SRV-Sky Irradiance(SRV)
		//15:  SRV-Sky Environment(SRV)
		//16:  SRV-Sky BRDF LUT(SRV)
		//17:  SRV-Sky Diffuse(SRV)
		CDescriptorHeapWrapper				m_cbvsrvHeap;


		DXGI_SAMPLE_DESC					m_SampleDesc = {};
		D3D12_DEPTH_STENCIL_VIEW_DESC		m_ScenePass_DsvDesc = {};
		float								m_ScreenClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		DXGI_FORMAT							m_DsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		DXGI_FORMAT							m_RtvFormat[5] = { 
												DXGI_FORMAT_R11G11B10_FLOAT,	// Albedo buffer
												DXGI_FORMAT_R8G8B8A8_SNORM,		// Normal
												DXGI_FORMAT_R11G11B10_FLOAT,	// (R)Roughness/(G)Metallic/(B)AO
												DXGI_FORMAT_R32G32B32A32_FLOAT, // Position
												DXGI_FORMAT_R11G11B10_FLOAT,	// Light Pass result
											};
		float								m_DepthClearValue = 1.0f;
		DXGI_FORMAT							m_ShadowMapFormat = DXGI_FORMAT_D32_FLOAT;

		const UINT m_ShadowMapWidth = 1024U;
		const UINT m_ShadowMapHeight = 1024U;

		// Constant Buffers
		
		ieD3D12ConstantBuffer<CB_PS_Lights>			m_CBLights;
		ieD3D12ConstantBuffer<CB_PS_PostFx>			m_CBPostFx;
		ieD3D12ConstantBuffer<CB_PS_VS_PerFrame>	m_CBPerFrame;
		ieD3D12ConstantBuffer<CB_VS_PerObject>		m_CBPerObject[m_FrameBufferCount];
		ieD3D12ConstantBuffer<CB_PS_VS_PerObjectMaterialAdditives> m_CBPerObjectMaterial[m_FrameBufferCount];

	};

}
