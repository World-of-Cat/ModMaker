#include "main.h"

#include "factorio_screens.h"

bool show_demo_win = false;

App *app;

App::App()
{
	glfwInit();
	static const char *glsl_version_str = "#version 460 core";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_MAXIMIZED, true);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(800, 800, "Mod Maker", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	IMGUI_CHECKVERSION();
	
	ImGui::CreateContext();
	m_ui_info.io_ = &ImGui::GetIO();
	m_ui_info.io().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	m_ui_info.titleFont = m_ui_info.io_->Fonts->AddFontFromMemoryTTF(fonts::roboto_regular, fonts::roboto_regular_size, 22, nullptr, m_ui_info.io().Fonts->GetGlyphRangesDefault());
	m_ui_info.normalFont = m_ui_info.io_->Fonts->AddFontFromMemoryTTF(fonts::roboto_regular, fonts::roboto_regular_size, 16, nullptr, m_ui_info.io().Fonts->GetGlyphRangesDefault());
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version_str);
}

App::~App()
{
	glfwTerminate();
}

void App::run()
{
	app = this;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::PushFont(m_ui_info.normalFont);

		m_screen_manager.draw_screen();

		ImGui::PopFont();
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}
}

UIInfo& App::ui_info()
{
	return m_ui_info;
}

ScreenManager<AppScreen>& App::screen_manager()
{
	return m_screen_manager;
}

void MainScreen::draw_ui()
{
	ImGui::TextWrapped(texts::main_menu_blurb);
	ImGui::Spacing();
	ImGui::PushFont(ui_info.titleFont);
	ImGui::Text("Create a Mod");
	ImGui::PopFont();
	if (ImGui::Button("Factorio"))
	{
		app->screen_manager().set_screen(AppScreen::NewFactorioMod);
	}
	ImGui::SameLine();
	if (ImGui::Button("Minecraft"))
	{
		app->screen_manager().set_screen(AppScreen::NewMinecraftMod);
	}
	ImGui::Spacing();
	ImGui::PushFont(ui_info.titleFont);
	ImGui::Text("Edit a Mod");
	ImGui::PopFont();
	if (ImGui::Button("Factorio"))
	{
		// open factorio mod
	}
	ImGui::SameLine();
	if (ImGui::Button("Minecraft")) {
		// open minecraft mod
	}


}

void MainScreen::draw_extra_windows()
{
	Screen::draw_extra_windows();
}

void mainMenuBar()
{
	if (ImGui::BeginMenu("File")) {

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Edit")) {

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("View")) {
		if (ImGui::MenuItem("Main Menu")) {
			app->screen_manager().set_screen(AppScreen::Main);
		}
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Project")) {

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Tools")) {

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Help")) {
		ImGui::MenuItem("Demo Window", nullptr, &show_demo_win);
		ImGui::EndMenu();
	}
}

void demo_win()
{
	if (show_demo_win) {
		ImGui::ShowDemoWindow(&show_demo_win);
	}
}

void MainScreen::draw_main_menu()
{
	Screen::draw_main_menu();
}

int main()
{
	App app;
	app.screen_manager().add_screen(AppScreen::Main, std::make_shared<MainScreen>("Mod Maker", app.ui_info()));
	app.screen_manager().add_screen(AppScreen::NewFactorioMod, std::make_shared<NewFactorioModScreen>("New Factorio Mod Project", app.ui_info()));
	app.screen_manager().add_screen(AppScreen::EditFactorioMod, std::make_shared<EditFactorioModScreen>("Editing: <none>", app.ui_info()));
	app.screen_manager().set_screen(AppScreen::Main);

	app.run();

	return 0;
}
