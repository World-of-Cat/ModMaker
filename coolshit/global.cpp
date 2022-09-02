#include "global.h"
#include <filesystem>


bool PathWidget(char buffer[PATH_BUFFER_SIZE], const std::string& label, bool require_empty)
{
	bool valid = false;
	std::string id = "##" + label;
	ImGui::SetNextItemWidth(512);
	ImGui::InputText(id.c_str(), buffer, PATH_BUFFER_SIZE);
	ImGui::SameLine();
	bool picker = ImGui::Button("Browse");
	if (!std::filesystem::exists(buffer))
	{
		if (buffer[0] != 0) {
			ImGui::SameLine();
			if (ImGui::Button("Create Folder")) {
				std::filesystem::create_directories(buffer);
			}
		}
		else {
			ImGui::SameLine();
			ImGui::BeginDisabled();
			ImGui::Button("Create Folder");
			ImGui::EndDisabled();
		}

		ImGui::SameLine();
		ImGui::TextColored({1.0f, 0.0f, 0.0f, 1.0f}, label.c_str());
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
		{
			ImGui::SetTooltip("Folder must exist");
		}
	}
	else if (!std::filesystem::is_empty(buffer) && require_empty) {
		ImGui::SameLine();
		ImGui::BeginDisabled();
		ImGui::Button("Create Folder");
		ImGui::EndDisabled();

		ImGui::SameLine();
		ImGui::TextColored({1.0f, 0.8f, 0.0f, 1.0f}, label.c_str());
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
			ImGui::SetTooltip("Folder must be empty");
		}
	}
	else {
		ImGui::SameLine();
		ImGui::BeginDisabled();
		ImGui::Button("Create Folder");
		ImGui::EndDisabled();
		ImGui::SameLine();
		ImGui::TextColored({0.0f, 1.0f, 0.0f, 1.0f}, label.c_str());
		valid = true;
	}

	if (picker)
	{
		char *b;
		auto res = NFD_PickFolder(nullptr, &b);
		if (res == NFD_CANCEL) return valid;
		if (res == NFD_ERROR) return valid;
		memset(buffer, 0, PATH_BUFFER_SIZE);
		memcpy_s(buffer, PATH_BUFFER_SIZE, b, strlen(b));
	}
	return valid && !picker;
}

bool RequiredTextInput(const std::string &label, char* buffer, size_t size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback)
{
	std::string id = "##" + label;
	ImGui::SetNextItemWidth(512);
	ImGui::InputText(id.c_str(), buffer, size, flags, callback);
	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
	if (buffer[0] == 0)
	{
		ImGui::TextColored({1.0f, 0.0f, 0.0f, 1.0f}, label.c_str());
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
			ImGui::SetTooltip("This field is required");
		}
		return false;
	} else
	{
		ImGui::Text(label.c_str());
	}

	return true;
}

bool RequiredInt3Input(const std::string& label, int* buffer, std::array<int, 3> invalid_value)
{
	std::string id = "##" + label;

	ImGui::SetNextItemWidth(512);
	ImGui::InputInt3(id.c_str(), buffer);
	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
	if (buffer[0] == invalid_value[0] && buffer[1] == invalid_value[1] && buffer[2] == invalid_value[2])
	{
		ImGui::TextColored({1.0f, 0.0f, 0.0f, 1.0f}, label.c_str());
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
			ImGui::SetTooltip("This field is required");
		}
		return false;
	}
	else {
		ImGui::Text(label.c_str());
	}
	return true;
}
