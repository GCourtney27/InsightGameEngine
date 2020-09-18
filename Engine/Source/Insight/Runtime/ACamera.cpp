#include <Engine_pch.h>


#include "ACamera.h"
#include "imgui.h"
#include "Insight/Runtime/Components/Actor_Component.h"
#include "Insight/Runtime/APlayer_Character.h"
#include "Insight/Core/Application.h"
#include "Insight/Input/Input.h"


namespace Insight {

	namespace Runtime {


		ACamera* ACamera::s_Instance = nullptr;

		ACamera::ACamera(ViewTarget ViewTarget)
			: AActor(0, "Camera")
		{
			IE_CORE_ASSERT(!s_Instance, "Cannot have more than one camera in the world at once!");
			s_Instance = this;

			SetViewTarget(ViewTarget, false, true);
		}

		ACamera::~ACamera()
		{
		}

		void ACamera::BeginPlay()
		{
		}

		void ACamera::OnUpdate(const float DeltaMs)
		{
			// Strip this out for game distribution we dont need it 
			// during runtime character controller will do this for us
			IE_ADD_FOR_GAME_DIST(return);

			if (Application::Get().IsPlaySessionUnderWay()) {
				return;
			}

			if (Input::IsMouseButtonPressed(IE_MOUSEBUTTON_MIDDLE)) {

				auto [x, y] = Input::GetRawMousePosition();
				float SpeedMultiplier = 4.0f;
				if (x < 0.0f) {
					ProcessKeyboardInput(CameraMovement::LEFT, DeltaMs * SpeedMultiplier);
				}
				if (x > 0.0f) {
					ProcessKeyboardInput(CameraMovement::RIGHT, DeltaMs * SpeedMultiplier);
				}
				if (y < 0.0f) {
					ProcessKeyboardInput(CameraMovement::UP, DeltaMs * SpeedMultiplier);
				}
				if (y > 0.0f) {
					ProcessKeyboardInput(CameraMovement::DOWN, DeltaMs * SpeedMultiplier);
				}
			}

			if (Input::IsKeyPressed(VK_SHIFT)) {
				m_MovementSpeed = BOOST_SPEED;
			}
			else {
				m_MovementSpeed = m_BaseMovementSpeed;
			}

			if (Input::IsMouseButtonPressed(IE_MOUSEBUTTON_RIGHT)) {

				auto [x, y] = Input::GetRawMousePosition();
				ProcessMouseMovement((float)x, (float)y);

				if (Input::IsKeyPressed('W')) {
					ProcessKeyboardInput(CameraMovement::FORWARD, DeltaMs);
				}
				if (Input::IsKeyPressed('S')) {
					ProcessKeyboardInput(CameraMovement::BACKWARD, DeltaMs);
				}
				if (Input::IsKeyPressed('A')) {
					ProcessKeyboardInput(CameraMovement::LEFT, DeltaMs);
				}
				if (Input::IsKeyPressed('D')) {
					ProcessKeyboardInput(CameraMovement::RIGHT, DeltaMs);
				}
				if (Input::IsKeyPressed('E')) {
					ProcessKeyboardInput(CameraMovement::UP, DeltaMs);
				}
				if (Input::IsKeyPressed('Q')) {
					ProcessKeyboardInput(CameraMovement::DOWN, DeltaMs);
				}
			}
		}

		void ACamera::EditorEndPlay()
		{

		}

		void ACamera::ProcessMouseScroll(float yOffset)
		{
		}

		void ACamera::ProcessMouseMovement(float xPos, float yPos)
		{
			GetTransformRef().Rotate(yPos * m_MouseSensitivity, xPos * m_MouseSensitivity, 0.0f);

			UpdateViewMatrix();
			GetTransformRef().UpdateLocalDirectionVectors();
		}

		void ACamera::ProcessKeyboardInput(CameraMovement direction, float deltaTime)
		{
			float velocity = m_MovementSpeed * deltaTime;

			if (direction == CameraMovement::FORWARD) {
				GetTransformRef().GetPositionRef() += GetTransformRef().GetLocalForward() * velocity;
			}
			if (direction == CameraMovement::BACKWARD) {
				GetTransformRef().GetPositionRef() -= GetTransformRef().GetLocalForward() * velocity;
			}
			if (direction == CameraMovement::LEFT) {
				GetTransformRef().GetPositionRef() -= GetTransformRef().GetLocalRight() * velocity;
			}
			if (direction == CameraMovement::RIGHT) {
				GetTransformRef().GetPositionRef() += GetTransformRef().GetLocalRight() * velocity;
			}
			if (direction == CameraMovement::UP) {
				GetTransformRef().GetPositionRef() += Vector3::Up * velocity;
			}
			if (direction == CameraMovement::DOWN) {
				GetTransformRef().GetPositionRef() -= Vector3::Up * velocity;
			}
			UpdateViewMatrix();
		}

		void ACamera::OnEvent(Event& e)
		{
			EventDispatcher Dispatcher(e);
			Dispatcher.Dispatch<MouseScrolledEvent>(IE_BIND_EVENT_FN(ACamera::OnMouseScrolled));

		}

		void ACamera::SetPerspectiveProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
		{
			m_Fov = fovDegrees;
			m_NearZ = nearZ;
			m_FarZ = farZ;
			m_AspectRatio = aspectRatio;
			float fovRadians = fovDegrees * (3.14f / 180.0f);
			m_ProjectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, m_AspectRatio, m_NearZ, m_FarZ);
		}

		void ACamera::SetOrthographicsProjectionValues(float viewWidth, float viewHeight, float nearZ, float farZ)
		{
			m_ProjectionMatrix = XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ);
		}

		void ACamera::RenderSceneHeirarchy()
		{
			AActor::RenderSceneHeirarchy();
		}

		void ACamera::OnImGuiRender()
		{
			AActor::OnImGuiRender();

			ImGui::Text("View");
			if (!m_IsOrthographic) {
				ImGui::DragFloat("Field of View", &m_Fov, 0.5f, 1.0f, 180.0f);
			}
			ImGui::DragFloat("Near Z", &m_NearZ, 0.01f, 0.00001f, 5.0f);
			ImGui::DragFloat("Far Z", &m_FarZ, 1.0f, 1.0f, 10000.0f);

			ImGui::Text("Post-Processing");
			ImGui::DragFloat("Exposure", &m_Exposure, 0.01f, 0.0f, 1.0f);

			ImGui::Text("Projection");
			constexpr char* projectionMethods[] = { "Perspective", "Orthographic" };
			ImGui::Combo("Method", (int*)&m_IsOrthographic, projectionMethods, IM_ARRAYSIZE(projectionMethods));
			if (!m_IsOrthographic) {
				SetPerspectiveProjectionValues(m_Fov, m_AspectRatio, m_NearZ, m_FarZ);
			}
			else {
				SetOrthographicsProjectionValues(40.0f, 40.0f, m_NearZ, m_FarZ);
			}

		}

		void ACamera::UpdateViewMatrix()
		{
			m_CamRotationMatrix = XMMatrixRotationRollPitchYaw(GetTransformRef().GetRotation().x, GetTransformRef().GetRotation().y, 0.0f);
			m_CamTarget = XMVector3TransformCoord(Vector3::Forward, m_CamRotationMatrix);
			m_CamTarget += GetTransformRef().GetPosition();
			m_UpDir = XMVector3TransformCoord(Vector3::Up, m_CamRotationMatrix);
			m_ViewMatrix = XMMatrixLookAtLH(GetTransformRef().GetPosition(), m_CamTarget, m_UpDir);
		}

		bool ACamera::OnMouseScrolled(MouseScrolledEvent& e)
		{
			if (Application::Get().GetImGuiLayer().IsMouseOverUI()) {
				return false;
			}

			// Vertical scroll wheel
			if (e.GetYOffset() > 0.0f) {
				ProcessKeyboardInput(CameraMovement::FORWARD, 0.05f);
			}
			if (e.GetYOffset() < 0.0f) {
				ProcessKeyboardInput(CameraMovement::BACKWARD, 0.05f);
			}
			// Horizontal scroll wheel
			if (e.GetXOffset() > 0.0f) {
				ProcessKeyboardInput(CameraMovement::RIGHT, 0.05f);
			}
			if (e.GetXOffset() < 0.0f) {
				ProcessKeyboardInput(CameraMovement::LEFT, 0.05f);
			}
			return false;
		}

	} // end namespace Runtime
} // end namespace Insight