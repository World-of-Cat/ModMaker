#include "ScreenManager.h"

Screen::Screen(const std::string& title_, UIInfo &ui_info_) : title(title_), ui_info(ui_info_)
{
}

Screen::~Screen()
{
}

void Screen::draw()
{

	if (ImGui::BeginMainMenuBar())
	{
		draw_main_menu();
		ImGui::EndMainMenuBar();
	}

	const ImGuiViewport *viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus);
	ImGui::PushFont(ui_info.titleFont);
	ImGui::Text(title.c_str());
	ImGui::PopFont();

	draw_ui();
	ImGui::End();

	draw_extra_windows();
}

void Screen::draw_ui()
{
}
