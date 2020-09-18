#include <Engine_pch.h>

#include "Windows_Window.h"

#include "Insight/Core/Application.h"
#include "Renderer/Renderer.h"

#include "Insight/Core/Log.h"
#include "Insight/Utilities/String_Helper.h"
#include "Platform/Windows/Window_Resources/Resource.h"

#include "Insight/Events/Key_Event.h"
#include "Insight/Events/Mouse_Event.h"
#include "Insight/Events/Application_Event.h"


namespace Insight {


	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		m_Data.WindowTitle = props.Title;
		m_Data.WindowTitle_wide = StringHelper::StringToWide(m_Data.WindowTitle);
		m_Data.WindowClassName = props.Title + " Class";
		m_Data.WindowClassName_wide = StringHelper::StringToWide(m_Data.WindowClassName);
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;
	}




	LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{

		switch (uMsg) {
		case WM_NCCREATE:
		{
			const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
			WindowsWindow::WindowData* data = reinterpret_cast<WindowsWindow::WindowData*>(pCreate->lpCreateParams);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(data));

			return 1;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			WindowsWindow::WindowData data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			WindowCloseEvent event;
			data.EventCallback(event);
			return 0;
		}
		// Mouse Input
		case WM_MOUSEMOVE:
		{
			WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			MouseMovedEvent event(LOWORD(lParam), HIWORD(lParam));
			data.EventCallback(event);
			return 0;
		}
		case WM_MOUSEWHEEL:
		{
			WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			float yOffset = GET_WHEEL_DELTA_WPARAM(wParam) / 120.0f;
			MouseScrolledEvent event(0.0f, yOffset);
			data.EventCallback(event);
			return 0;
		}
		case WM_MOUSEHWHEEL:
		{
			WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			float xOffset = GET_WHEEL_DELTA_WPARAM(wParam) / 120.0f;
			MouseScrolledEvent event(xOffset, 0.0f);
			data.EventCallback(event);
			return 0;
		}
		case WM_LBUTTONDOWN:
		{
			WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			MouseButtonPressedEvent event(0);
			data.EventCallback(event);
			return 0;
		}
		case WM_LBUTTONUP:
		{
			WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			MouseButtonReleasedEvent event(0);
			data.EventCallback(event);
			return 0;
		}
		case WM_RBUTTONDOWN:
		{
			WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			MouseButtonPressedEvent event(1);
			data.EventCallback(event);
			
			//RECT clientRect = {};
			//POINT Point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			//TrackPopupMenu(data.hContextMenu, TPM_LEFTALIGN | TPM_TOPALIGN, Point.x, Point.y, 0, hWnd, &clientRect);

			return 0;
		}
		case WM_RBUTTONUP:
		{
			WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			MouseButtonReleasedEvent event(1);
			data.EventCallback(event);
			return 0;
		}
		case WM_MBUTTONDOWN:
		{
			WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			MouseButtonPressedEvent event(2);
			data.EventCallback(event);
			return 0;
		}
		case WM_MBUTTONUP:
		{
			WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			MouseButtonReleasedEvent event(2);
			data.EventCallback(event);
			return 0;
		}
		// Keyboard Input
		case WM_CHAR:
		{
			WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			KeyTypedEvent event((char)wParam);
			data.EventCallback(event);
			return 0;
		}
		case WM_KEYDOWN:
		{
			WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			// Debug force engine close Escape key
			if (wParam == VK_ESCAPE)
			{
				PostQuitMessage(0);
				WindowCloseEvent event;
				data.EventCallback(event);
				return 0;
			}

			if (wParam == VK_F11)
			{
				if (data.FullScreenEnabled)
				{
					data.FullScreenEnabled = false;
					WindowToggleFullScreenEvent event(false);
					data.EventCallback(event);
				}
				else
				{
					data.FullScreenEnabled = true;
					WindowToggleFullScreenEvent event(true);
					data.EventCallback(event);
				}

			}
			KeyPressedEvent event((char)wParam, 0);
			data.EventCallback(event);
			return 0;
		}
		case WM_KEYUP:
		{
			WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			KeyReleasedEvent event((char)wParam);
			data.EventCallback(event);
			return 0;
		}
		// Aplication Events
		case WM_COMPACTING:
		{
			IE_CORE_WARN("System memory is low!");
			return 0;
		}
		case WM_EXITSIZEMOVE:
		{
			WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			RECT clientRect = {};
			GetClientRect(hWnd, &clientRect);
			WindowResizeEvent event(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, wParam == SIZE_MINIMIZED);
			data.EventCallback(event);

			IE_CORE_INFO("Window size has changed");
			return 0;
		}
		case WM_SIZE:
		{
			WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

			if (data.IsFirstLaunch) {
				data.IsFirstLaunch = false;
				return 0;
			}
			RECT clientRect = {};
			GetClientRect(hWnd, &clientRect);
			WindowResizeEvent event(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, wParam == SIZE_MINIMIZED);
			data.EventCallback(event);
			return 0;
		}
		case WM_INPUT:
		{
			WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			UINT dataSize;
			GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));

			if (dataSize > 0)
			{
				std::unique_ptr<BYTE[]> rawdata = std::make_unique<BYTE[]>(dataSize);
				if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawdata.get(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
				{
					RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawdata.get());
					if (raw->header.dwType == RIM_TYPEMOUSE)
					{
						MouseRawMoveEvent event(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
						data.EventCallback(event);
					}
				}

			}
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		case WM_DROPFILES:
		{
			/*WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			UINT iFile;
			LPSTR lpszFile;
			UINT cch;
			DragQueryFileA((HDROP)wParam, iFile, lpszFile, cch);
			IE_CORE_INFO("File dropped on window");*/
		}
		// Menu Bar Events
		case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// Parse the menu selections:
			switch (wmId)
			{
			case IDM_NEW_SCENE:
			{
				WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);


				break;
			}
			case IDM_EDITOR_TOGGLE:
			{
				WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
				data.EditorUIEnabled = !data.EditorUIEnabled;
				IE_STRIP_FOR_GAME_DIST(Application::Get().GetEditorLayer().SetUIEnabled(data.EditorUIEnabled);)
				break;
			}
			case IDM_EDITOR_RELOAD_SCRIPTS:
			{
				WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
				AppScriptReloadEvent event;
				data.EventCallback(event);
				break;
			}
			case IDM_BEGIN_PLAY:
			{
				WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
				AppBeginPlayEvent event;
				data.EventCallback(event);
				break;
			}
			case IDM_END_PLAY:
			{
				WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
				AppEndPlayEvent event;
				data.EventCallback(event);
				break;
			}
			case IDM_SCENE_SAVE:
			{
				WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
				SceneSaveEvent event;
				data.EventCallback(event);
				IE_CORE_INFO("Scene Saved");
				break;
			}
			case IDM_ABOUT:
			{
				WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
				
				wchar_t AboutMsgBuffer[256];
				int APIVersion = ((int)Renderer::GetAPI()) + 10;
				const wchar_t* RTEnabled = Renderer::GetIsRayTraceEnabled() ? L"Enabled" : L"Disabled";
				swprintf_s(AboutMsgBuffer, L"Version - 1.8 \nRenderer - Direct3D %i (Ray Tracing: %s) \n\nVendor Runtime: \nMono - v6.8.0.123 \nAssimp - v3.3.1 \nRapidJson - v1.0.0 \nImGui - v1.75", APIVersion, RTEnabled);
				data.pWindow->CreateMessageBox(AboutMsgBuffer, L"About Insight Editor");
				
				break;
			}
			case IDM_EXIT:
			{
				WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
				PostQuitMessage(0);
				WindowCloseEvent event;
				data.EventCallback(event);
				break;
			}
			case IDM_VISUALIZE_FINAL_RESULT:
			{
				WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

				IE_CORE_INFO("Visualize final result");
				//ModifyMenuW(data.hGraphicsVisualizeSubMenu, IDM_VISUALIZE_FINAL_RESULT, MF_CHECKED, IDM_VISUALIZE_FINAL_RESULT, L"&Final Result");

				break;
			}
			case IDM_VISUALIZE_LIGHT_PASS_RESULT:
			{
				IE_CORE_INFO("Visualize light pass result");
				break;
			}
			case IDM_VISUALIZE_ALBEDO_BUFFER:
			{
				IE_CORE_INFO("Visualize albedo buffer");
				break;
			}
			case IDM_VISUALIZE_NORMAL_BUFFER:
			{
				IE_CORE_INFO("Visualize normal buffer");
				break;
			}
			case IDM_VISUALIZE_ROUGHNESS_BUFFER:
			{
				IE_CORE_INFO("Visualize roughness buffer");
				break;
			}
			case IDM_VISUALIZE_METALLIC_BUFFER:
			{
				IE_CORE_INFO("Visualize metallic buffer");
				break;
			}
			case IDM_VISUALIZE_AO_BUFFER:
			{
				IE_CORE_INFO("Visualize ambient occlusion buffer");
				break;
			}
			case IDM_RENDERER_D3D_11:
			{
				IE_CORE_INFO("Switch render context to D3D 11");
				WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
				Renderer::GraphicsSettings Settings = {};
				Settings.TargetRenderAPI = Renderer::eTargetRenderAPI::D3D_11;
				FileSystem::SaveEngineUserSettings(Settings);
				data.pWindow->CreateMessageBox(L"You must relaunch engine for changes to take effect.", L"Graphics API changed to DirectX 11");
				break;
			}
			case IDM_RENDERER_D3D_12:
			{
				IE_CORE_INFO("Switch render context to D3D 12");
				WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
				Renderer::GraphicsSettings Settings = {};
				Settings.TargetRenderAPI = Renderer::eTargetRenderAPI::D3D_12;
				FileSystem::SaveEngineUserSettings(Settings);
				data.pWindow->CreateMessageBox(L"You must relaunch engine for changes to take effect.", L"Graphics API changed to DirectX 12");
				break;
			}
			case IDM_RELOAD_SHADERS:
			{
				IE_CORE_INFO("Reloading scripts.");
				WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
				ShaderReloadEvent event;
				data.EventCallback(event);
			}
			default:
				return DefWindowProcW(hWnd, uMsg, wParam, lParam);
			}
		}
		default:
		{
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		}
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	bool WindowsWindow::Init(const WindowProps& props)
	{
		HRESULT hr = ::CoInitialize(NULL);
		if (FAILED(hr)){
			IE_CORE_ERROR("Failed to initialize COM library.");
		}

		static bool raw_input_initialized = false;
		if (raw_input_initialized == false) {

			RAWINPUTDEVICE rid;

			rid.usUsagePage = 0x01; // Mouse
			rid.usUsage = 0x02;
			rid.dwFlags = 0;
			rid.hwndTarget = NULL;

			if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
			{
				IE_CORE_ERROR("Failed to register raw input devices. Error: {0}", StringHelper::WideToString(std::wstring(GetLastWindowsError())));
				return false;
			}
			raw_input_initialized = true;
		}

		RegisterWindowClass();

		int centerScreenX = ::GetSystemMetrics(SM_CXSCREEN) / 2 - m_Data.Width / 2;
		int centerScreenY = ::GetSystemMetrics(SM_CYSCREEN) / 2 - m_Data.Height / 2;

		// Center the window on the users monitor
		m_WindowRect.left = centerScreenX;
		m_WindowRect.top = centerScreenY + 35;
		m_WindowRect.right = m_WindowRect.left + m_Data.Width;
		m_WindowRect.bottom = m_WindowRect.top + m_Data.Height;
		::AdjustWindowRect(&m_WindowRect, WS_OVERLAPPEDWINDOW | WS_EX_ACCEPTFILES, FALSE);

		// Create the menu bar
		IE_STRIP_FOR_GAME_DIST(InitializeMenuBar();)

		// Create mouse Right-Click context menu
		IE_STRIP_FOR_GAME_DIST(InitializeContextMenu();)

		// Create the main window for the engine/game
		m_hWindow = ::CreateWindowExW(
			WS_EX_ACCEPTFILES,						// Window Styles
			m_Data.WindowClassName_wide.c_str(),	// Window Class
			m_Data.WindowTitle_wide.c_str(),		// Window Title
			WS_OVERLAPPEDWINDOW,					// Window Style

			m_WindowRect.left,							// Start X
			m_WindowRect.top,							// Start Y
			m_WindowRect.right - m_WindowRect.left,		// Width
			m_WindowRect.bottom - m_WindowRect.top,		// Height

			NULL,					// Parent window
			m_hMenuBar,				// Menu
			*m_WindowsAppInstance,	// Current Windows program application instance passed from WinMain
			&m_Data					// Additional application data
		);

		if (m_hWindow == NULL) {
			IE_ERROR("Unable to create Windows window.");
			IE_ERROR("    Error: {0}", StringHelper::WideToString(std::wstring(GetLastWindowsError())));
			return false;
		}

		m_Data.hGraphicsVisualizeSubMenu = &m_hGraphicsVisualizeSubMenu;
		m_Data.pWindow = this;

		//m_nCmdShowArgs = SW_SHOWMAXIMIZED;

		::ShowWindow(m_hWindow, m_nCmdShowArgs);
		::SetForegroundWindow(m_hWindow);
		::SetFocus(m_hWindow);

		IE_CORE_TRACE("Window Initialized");
		return true;
	}

	void WindowsWindow::PostInit()
	{
		RECT ClientRect = {};
		::GetClientRect(m_hWindow, &ClientRect);
		WindowResizeEvent Event(ClientRect.right - ClientRect.left, ClientRect.bottom - ClientRect.top, false);
		m_Data.EventCallback(Event);
	}

	void WindowsWindow::RegisterWindowClass()
	{
		WNDCLASSEX wc = {};
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = 0;
		wc.lpfnWndProc = WindowProcedure;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = *m_WindowsAppInstance;
		wc.hIcon = ::LoadIcon(0, IDI_WINLOGO);
		wc.hCursor = ::LoadCursor(0, IDC_ARROW);
		wc.lpszMenuName = 0;
		wc.hbrBackground = 0;
		wc.lpszClassName = m_Data.WindowClassName_wide.c_str();

		::RegisterClassEx(&wc);
		DWORD error = ::GetLastError();
		if (error > 0)
		{
			IE_CORE_ERROR("An error occured while registering window class: {0} ", m_Data.WindowClassName);
			IE_CORE_ERROR("    Error: {1}", StringHelper::WideToString(std::wstring(GetLastWindowsError())));
		}
	}

	void WindowsWindow::InitializeMenuBar()
	{
		m_hMenuBar = ::CreateMenu();
		if (m_hMenuBar == NULL) {
			IE_CORE_ERROR("Failed to create menu bar for window \"{0}\"", m_Data.WindowTitle);
			return;
		}

		// File SubMenu
		{
			m_hFileSubMenu = ::CreateMenu();
			::AppendMenuW(m_hMenuBar, MF_POPUP, (UINT_PTR)m_hFileSubMenu, L"&File");
			::AppendMenuW(m_hFileSubMenu, MF_STRING, IDM_SCENE_SAVE, L"&Save Scene");
			//::AppendMenuW(m_hFileSubMenu, MF_STRING, IDM_NEW_SCENE, L"New Scene");
			::AppendMenuW(m_hFileSubMenu, MF_STRING, IDM_ABOUT, L"&About");
			::AppendMenuW(m_hFileSubMenu, MF_STRING, IDM_EXIT, L"&Exit");
		}

		// Edit SubMenu
		{
			m_hEditSubMenu = ::CreateMenu();
			::AppendMenuW(m_hMenuBar, MF_POPUP, (UINT_PTR)m_hEditSubMenu, L"&Edit");

		}

		// Editor SubMenu
		{
			m_hEditorSubMenu = ::CreateMenu();
			::AppendMenuW(m_hMenuBar, MF_POPUP, (UINT_PTR)m_hEditorSubMenu, L"&Editor");
			::AppendMenuW(m_hEditorSubMenu, MF_STRING, IDM_BEGIN_PLAY, L"&Play");
			::AppendMenuW(m_hEditorSubMenu, MF_STRING, IDM_END_PLAY, L"&Stop");
			::AppendMenuW(m_hEditorSubMenu, MF_STRING, IDM_EDITOR_RELOAD_SCRIPTS, L"&Reload Scripts");
			::AppendMenuW(m_hEditorSubMenu, MF_STRING, IDM_EDITOR_TOGGLE, L"&Toggle Editor UI");

			m_Data.hEditorSubMenu = &m_hEditorSubMenu;
		}

		// Graphics SubMenu
		{
			m_hGraphicsSubMenu = ::CreateMenu();
			m_hGraphicsVisualizeSubMenu = ::CreateMenu();

			m_hGraphicsCurrentRenderContextSubMenu = ::CreateMenu();
			::AppendMenuW(m_hGraphicsSubMenu, MF_STRING, IDM_RELOAD_SHADERS, L"Relead Shaders");
			::AppendMenuW(m_hGraphicsSubMenu, MF_POPUP, (UINT_PTR)m_hGraphicsCurrentRenderContextSubMenu, L"&Renderer");
			::AppendMenuW(m_hGraphicsCurrentRenderContextSubMenu, MF_UNCHECKED, IDM_RENDERER_D3D_11, L"&Direct3D 11");
			::AppendMenuW(m_hGraphicsCurrentRenderContextSubMenu, MF_UNCHECKED, IDM_RENDERER_D3D_12, L"&Direct3D 12");

			::AppendMenuW(m_hMenuBar, MF_POPUP, (UINT_PTR)m_hGraphicsSubMenu, L"&Graphics");
			//AppendMenuW(m_GraphicsSubMenuHandle, MF_STRING, (UINT_PTR)m_GraphicsSubMenuHandle, L"&Reload Post-Fx Pass Shader");
			//AppendMenuW(m_GraphicsSubMenuHandle, MF_STRING, (UINT_PTR)m_GraphicsSubMenuHandle, L"&Reload Geometry Pass Shader");
			//AppendMenuW(m_GraphicsSubMenuHandle, MF_STRING, (UINT_PTR)m_GraphicsSubMenuHandle, L"&Reload Light Pass Shader");
			/*::AppendMenuW(m_hGraphicsSubMenu, MF_POPUP, (UINT_PTR)m_hGraphicsVisualizeSubMenu, L"&Visualize G-Buffer");
			::AppendMenuW(m_hGraphicsVisualizeSubMenu, MF_UNCHECKED, IDM_VISUALIZE_FINAL_RESULT, L"&Final Result");
			::AppendMenuW(m_hGraphicsVisualizeSubMenu, MF_SEPARATOR, 0, 0);
			::AppendMenuW(m_hGraphicsVisualizeSubMenu, MF_UNCHECKED, IDM_VISUALIZE_LIGHT_PASS_RESULT, L"&Light Pass Result");
			::AppendMenuW(m_hGraphicsVisualizeSubMenu, MF_SEPARATOR, 0, 0);
			::AppendMenuW(m_hGraphicsVisualizeSubMenu, MF_UNCHECKED, IDM_VISUALIZE_ALBEDO_BUFFER, L"&Albedo");
			::AppendMenuW(m_hGraphicsVisualizeSubMenu, MF_SEPARATOR, 0, 0);
			::AppendMenuW(m_hGraphicsVisualizeSubMenu, MF_UNCHECKED, IDM_VISUALIZE_NORMAL_BUFFER, L"&Normal");
			::AppendMenuW(m_hGraphicsVisualizeSubMenu, MF_SEPARATOR, 0, 0);
			::AppendMenuW(m_hGraphicsVisualizeSubMenu, MF_UNCHECKED, IDM_VISUALIZE_ROUGHNESS_BUFFER, L"&Roughness");
			::AppendMenuW(m_hGraphicsVisualizeSubMenu, MF_SEPARATOR, 0, 0);
			::AppendMenuW(m_hGraphicsVisualizeSubMenu, MF_UNCHECKED, IDM_VISUALIZE_METALLIC_BUFFER, L"&Metallic");
			::AppendMenuW(m_hGraphicsVisualizeSubMenu, MF_SEPARATOR, 0, 0);
			::AppendMenuW(m_hGraphicsVisualizeSubMenu, MF_UNCHECKED, IDM_VISUALIZE_AO_BUFFER, L"&Ambient Occlusion (PBR Texture)");
			::AppendMenuW(m_hGraphicsVisualizeSubMenu, MF_SEPARATOR, 0, 0);*/

			m_Data.hGraphicsVisualizeSubMenu = &m_hGraphicsVisualizeSubMenu;
		}
	}

	void WindowsWindow::InitializeContextMenu()
	{
		m_hContextMenu = ::CreatePopupMenu();
		{
			::AppendMenuW(m_hContextMenu, MF_STRING, IDM_VISUALIZE_AO_BUFFER, L"&Hello");
			::AppendMenuW(m_hContextMenu, MF_STRING, IDM_VISUALIZE_AO_BUFFER, L"&World");
		}
		m_Data.hContextMenu = &m_hContextMenu;
	}

	LPCTSTR WindowsWindow::GetLastWindowsError()
	{
		LPVOID lpMsgBuf;
		LPVOID lpDisplayBuf;
		DWORD dw = ::GetLastError();

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);

		lpDisplayBuf = (LPVOID)::LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)"") + 40) * sizeof(TCHAR));
		StringCchPrintf(
			(LPTSTR)lpDisplayBuf,
			::LocalSize(lpDisplayBuf) / sizeof(TCHAR),
			TEXT("%s failed with error %d: %s"),
			"", dw, lpMsgBuf
		);
		return (LPCTSTR)lpDisplayBuf;
	}

	void WindowsWindow::Resize(UINT newWidth, UINT newHeight, bool isMinimized)
	{
		m_Data.Width = newWidth;
		m_Data.Height = newHeight;
		Renderer::SetWindowWidthAndHeight(newWidth, newHeight, isMinimized);
	}

	void WindowsWindow::ToggleFullScreen(bool enabled)
	{
		m_Data.FullScreenEnabled = enabled;
		Renderer::OnWindowFullScreen();
	}

	bool WindowsWindow::ProccessWindowMessages()
	{
		MSG msg;
		::ZeroMemory(&msg, sizeof(MSG));

		while (::PeekMessage(&msg,	// Where to store message (if one exists)
			m_hWindow,			// Handle to window we are checking messages for
			0,						// Minimum Filter Msg Value - We are not filterinf for specific messages but min and max could be used to do so
			0,						// Maximum Filter Msg Value
			PM_REMOVE))				// Remove mesage after captureing it via PeekMessage
		{
			if (msg.message == WM_QUIT)
				return false;
			::TranslateMessage(&msg);		// Translate message from virtual key message into character messages
			::DispatchMessage(&msg);		// Dispatch message to our WindowProc for this window
		}

		IE_ASSERT(_CrtCheckMemory(), "Heap is currupted!");
		return true;
	}

	WindowsWindow::~WindowsWindow()
	{
		IE_CORE_WARN("Destroying window: {0}", m_Data.WindowTitle);
		Shutdown();
	}

	void WindowsWindow::OnUpdate(const float& deltaTime)
	{
		ProccessWindowMessages();
	}

	void WindowsWindow::OnFramePreRender()
	{
		Renderer::OnPreFrameRender();
	}

	void WindowsWindow::OnRender()
	{
		Renderer::OnRender();
	}

	void WindowsWindow::ExecuteDraw()
	{
		Renderer::ExecuteDraw();
		Renderer::SwapBuffers();
	}

	bool WindowsWindow::SetWindowTitle(const std::string& NewText, bool CompletlyOverride)
	{
		BOOL succeeded = true;
		if (CompletlyOverride) {
			succeeded = SetWindowText(m_hWindow, StringHelper::StringToWide(NewText).c_str());
		}
		else {
			m_Data.WindowTitle_wide = m_Data.WindowTitle_wide + L" - " + StringHelper::StringToWide(NewText);
			succeeded = SetWindowText(m_hWindow, m_Data.WindowTitle_wide.c_str());
		}
		return succeeded;
	}

	bool WindowsWindow::SetWindowTitleFPS(float fps)
	{
		std::wstring windowTitle = m_Data.WindowTitle_wide + L" FPS: " + std::to_wstring((UINT)fps);
		return static_cast<bool>(SetWindowText(m_hWindow, windowTitle.c_str()));
	}

	void* WindowsWindow::GetNativeWindow() const
	{
		return m_hWindow;
	}

	void WindowsWindow::CreateMessageBox(const wchar_t* Message, const wchar_t* Title)
	{
		MessageBox(m_hWindow, Message, Title, MB_OK);
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		IE_CORE_INFO("V-sync: " + enabled ? "enabled" : "disabled");
		m_Data.VSyncEnabled = enabled;
		Renderer::SetVSyncEnabled(m_Data.VSyncEnabled);
	}

	const bool& WindowsWindow::IsVsyncActive() const
	{
		return m_Data.VSyncEnabled;
	}

	const bool& WindowsWindow::IsFullScreenActive() const
	{
		return m_Data.FullScreenEnabled;
	}

	void WindowsWindow::Shutdown()
	{
		::CoUninitialize();

		if (m_hWindow != NULL)
		{
			::UnregisterClass(this->m_Data.WindowClassName_wide.c_str(), *m_WindowsAppInstance);
			::DestroyWindow(m_hWindow);
		}

		Renderer::Destroy();
	}

	void WindowsWindow::EndFrame()
	{
		Renderer::ExecuteDraw();
		Renderer::SwapBuffers();
	}

}

