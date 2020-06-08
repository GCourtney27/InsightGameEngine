#include <ie_pch.h>

#include "ADirectional_Light.h"
#include "Platform/DirectX12/Direct3D12_Context.h"
#include "imgui.h"

namespace Insight {



	ADirectionalLight::ADirectionalLight(ActorId id, ActorType type)
		: AActor(id, type)
	{
		Direct3D12Context& graphicsContext = Direct3D12Context::Get();
		graphicsContext.AddDirectionalLight(this);
	}

	ADirectionalLight::~ADirectionalLight()
	{
	}

	bool ADirectionalLight::LoadFromJson(const rapidjson::Value& jsonDirectionalLight)
	{
		AActor::LoadFromJson(jsonDirectionalLight);

		float diffuseR, diffuseG, diffuseB, strength;
		const rapidjson::Value& emission = jsonDirectionalLight["Emission"];
		json::get_float(emission[0], "diffuseR", diffuseR);
		json::get_float(emission[0], "diffuseG", diffuseG);
		json::get_float(emission[0], "diffuseB", diffuseB);
		json::get_float(emission[0], "strength", strength);

		m_ShaderCB.diffuse = XMFLOAT3(diffuseR, diffuseG, diffuseB);
		m_ShaderCB.direction = AActor::GetTransformRef().GetRotationRef();
		m_ShaderCB.strength = strength;

		return true;
	}

	bool ADirectionalLight::OnInit()
	{
		return true;
	}

	bool ADirectionalLight::OnPostInit()
	{
		return true;
	}

	void ADirectionalLight::OnUpdate(const float& deltaMs)
	{
		m_ShaderCB.direction = SceneNode::GetTransformRef().GetPositionRef();
	}

	void ADirectionalLight::OnPreRender(XMMATRIX parentMat)
	{
	}

	void ADirectionalLight::OnRender()
	{
	}

	void ADirectionalLight::Destroy()
	{
	}

	void ADirectionalLight::OnEvent(Event& e)
	{
	}

	void ADirectionalLight::BeginPlay()
	{
	}

	void ADirectionalLight::Tick(const float& deltaMs)
	{
	}

	void ADirectionalLight::Exit()
	{
	}

	void ADirectionalLight::OnImGuiRender()
	{
		AActor::OnImGuiRender();

		ImGui::Text("Rendering");
		ImGuiColorEditFlags colorWheelFlags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_PickerHueWheel;
		// Imgui will edit the color values in a normalized 0 to 1 space. 
		// In the shaders we transform the color values back into 0 to 255 space.
		ImGui::ColorEdit3("Diffuse", &m_ShaderCB.diffuse.x, colorWheelFlags);
		ImGui::DragFloat("Strength", &m_ShaderCB.strength, 0.01f, 0.0f, 10.0f);
	}

}