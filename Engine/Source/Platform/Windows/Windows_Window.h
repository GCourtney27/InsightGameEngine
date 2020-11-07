#pragma once

#include "Insight/Core/Window.h"


namespace Insight {

	class WindowsWindow : public Window
	{
	public:
		struct WindowData
		{
			std::string WindowClassName = "Retina Engine Class";
			std::wstring WindowClassName_wide;
			std::string WindowTitle = "Retina Editor";
			std::wstring WindowTitle_wide;
			UINT Width, Height;
			bool VSyncEnabled = true;
			bool FullScreenEnabled = false;
			bool EditorUIEnabled = true;
			bool IsFirstLaunch = true;
			WindowsWindow* pWindow;

			HMENU* hGraphicsVisualizeSubMenu;
			HMENU* hEditorSubMenu;
			HMENU* hContextMenu;

			EventCallbackFn EventCallback;
		};

	public:

		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		virtual void OnUpdate() override;
		virtual void Shutdown() override;

		virtual void PostInit() override;

		inline virtual uint32_t GetWidth() const override { return m_Data.Width; }
		inline virtual uint32_t GetHeight() const override { return m_Data.Height; }
		inline virtual std::pair<uint32_t, uint32_t> GetDimensions() const override { return std::make_pair(m_Data.Width, m_Data.Height); }
		virtual void* GetNativeWindow() const override;
		virtual void Resize(UINT newWidth, UINT newHeight, bool isMinimized) override;
		virtual void ToggleFullScreen(bool enabled) override;
		virtual bool ProccessWindowMessages() override;

		bool SetWindowTitle(const std::string& newText, bool completlyOverride = false) override;
		bool SetWindowTitleFPS(float fps) override;

		void SetWindowsSessionProps(HINSTANCE& hInstance, int nCmdShow) { SetWindowsApplicationInstance(hInstance); SetCmdArgs(nCmdShow); }
		inline HINSTANCE& GetWindowsApplicationReference() const { return *m_WindowsAppInstance; }
		inline HWND& GetWindowHandleRef() { return m_hWindow; }
		inline RECT& GetWindowRect() { return m_WindowRect; }

		void CreateMessageBox(const wchar_t* Message, const wchar_t* Title);

		// Window Attributes
		virtual inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		virtual void SetVSync(bool enabled) override;
		virtual const bool& IsVsyncActive() const override;
		virtual const bool& IsFullScreenActive() const override;
		virtual bool Init(const WindowProps& props);
		virtual inline float GeAspectRatio() const
		{
			return static_cast<float>(m_Data.Width) / static_cast<float>(m_Data.Height);
		}

	private:
		inline void SetWindowsApplicationInstance(HINSTANCE& hInstance) { m_WindowsAppInstance = &hInstance; }
		inline void SetCmdArgs(int nCmdShow) { m_nCmdShowArgs = nCmdShow; }
		void RegisterWindowClass();
		void InitializeMenuBar();
		void InitializeContextMenu();
		LPCTSTR GetLastWindowsError();
	private:

		HWND	m_hWindow;
		HMENU m_hContextMenu;
		HMENU	m_hMenuBar;
		HMENU	m_hFileSubMenu;
		HMENU	m_hEditSubMenu;
		HMENU	m_hEditorSubMenu;
		HMENU	m_hGraphicsSubMenu;
		HMENU	m_hGraphicsVisualizeSubMenu;
		HMENU	m_hGraphicsCurrentRenderContextSubMenu;

		HINSTANCE* m_WindowsAppInstance;
		int m_nCmdShowArgs;
		RECT m_WindowRect;

		WindowData m_Data;
	};

}
