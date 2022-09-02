#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include "Font.h"
#include "Text.h"
#include "ScreenManager.h"
#include "global.h"

enum class AppScreen {
	Main,
	NewFactorioMod, NewMinecraftMod,
	EditFactorioMod
};

class MainScreen : public Screen
{
public:
	MainScreen(const std::string& title, UIInfo& ui_info)
		: Screen(title, ui_info)
	{
	}

	void draw_ui() override;
	void draw_extra_windows() override;
	void draw_main_menu() override;
};


class App
{
public:

	App();
	~App();

	void run();

	UIInfo &ui_info();
	ScreenManager<AppScreen> &screen_manager();


private:

	UIInfo m_ui_info;
	GLFWwindow *window;
	ScreenManager<AppScreen> m_screen_manager;
};
