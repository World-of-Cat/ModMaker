#include "factorio_screens.h"

#include <iostream>
#include <sstream>
#include <unordered_set>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/filewritestream.h>

#include "main.h"

std::string dept_to_str(FactorioModDependency::DependencyType dt)
{
	switch (dt)
	{
	case FactorioModDependency::DependencyType::Hard: return "";
	case FactorioModDependency::DependencyType::Optional: return "? ";
	case FactorioModDependency::DependencyType::Incompatibility: return "! ";
	case FactorioModDependency::DependencyType::HiddenOptional: return "(?) ";
	case FactorioModDependency::DependencyType::DoesNotAffectLoadOrder: return "~ ";
	default: return "";
	}
}

std::string op2str(RelationalOperator op)
{
	switch (op)
	{
	case RelationalOperator::opGreater: return ">";
	case RelationalOperator::opGreaterEqual: return ">=";
	case RelationalOperator::opLess: return "<";
	case RelationalOperator::opLessEqual: return "<=";
	case RelationalOperator::opEqual:
	default: return "=";
	}
}

std::string FactorioModDependency::str() const
{
	std::string basic = dept_to_str(depType) + modname;
	if (version_inequality.has_value())
	{
		std::stringstream ss;
		ss << basic << " " << op2str(version_inequality.value().op) << " " <<  version_inequality.value().right.major << "." << version_inequality.value().right.minor << "." << version_inequality.value().right.patch;
		return ss.str();
	}
	return basic;
}

rapidjson::GenericStringRef<char> rjstringref(const std::string& str)
{
	return rapidjson::StringRef(str.c_str(), str.size());
}

void FactorioProject::write_info_file()
{
	std::filesystem::path infoFile = mod_folder / "info.json";
	rapidjson::Document doc(rapidjson::Type::kObjectType);
	auto verstr = std::format("{}.{}.{}", version.major, version.minor, version.patch);
	doc.AddMember("name", rjstringref(name), doc.GetAllocator());
	doc.AddMember("version", rjstringref(verstr), doc.GetAllocator());
	doc.AddMember("title", rjstringref(title), doc.GetAllocator());
	doc.AddMember("author", rjstringref(author), doc.GetAllocator());
	doc.AddMember("contact", rjstringref(contact), doc.GetAllocator());
	doc.AddMember("homepage", rjstringref(homepage), doc.GetAllocator());
	doc.AddMember("description", rjstringref(description), doc.GetAllocator());

	rapidjson::Value deps;
	deps.SetArray();
	std::vector<std::string> depstrs;
	for (const auto& dep : dependencies)
	{
		depstrs.push_back(dep.str());
	}

	for (const auto& dep : depstrs)
	{
		deps.PushBack(rjstringref(dep), doc.GetAllocator());
	}

	doc.AddMember("dependencies", deps, doc.GetAllocator());

	FILE *fd;
	fopen_s(&fd, infoFile.string().c_str(), "w+");
	static char writeBuffer[65536];
	rapidjson::FileWriteStream write_stream(fd, writeBuffer, sizeof(writeBuffer));
	rapidjson::PrettyWriter writer(write_stream);
	writer.SetIndent(' ', 2);
	doc.Accept(writer);
	fclose(fd);
}

void NewFactorioModScreen::reset_buffers()
{
	memset(project_folder_str, 0, PATH_BUFFER_SIZE);
	memset(modname_buffer, 0, MODNAME_MAX_LENGTH);
	mod_version = FactorioModVersion{0, 1, 0};
	memset(modtitle_buffer, 0, MODTITLE_MAX_LENGTH);
	memset(modauthor_buffer, 0, MODAUTHOR_BUFFER_SIZE);
	memset(modcontact_buffer, 0, MODCONTACT_BUFFER_SIZE);
	memset(modhomepage_buffer, 0, MODHOMEPAGE_BUFFER_SIZE);
	memset(moddescription_buffer, 0, MODDESCRIPTION_BUFFER_SIZE);
	factorio_version_i = 0;
}

std::unordered_set<char> valid_modname_chars = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '_'};

int modname_filter(ImGuiInputTextCallbackData* cbd)
{
	return !valid_modname_chars.contains(static_cast<char>(cbd->EventChar));
}

const char *factorio_versions[9] = {
	"1.1", "1.0", "0.18", "0.17", "0.16", "0.15", "0.14", "0.13", "0.12"
};

void NewFactorioModScreen::draw_ui()
{
	bool valid_config = PathWidget(project_folder_str, "Project Folder", true);
	if (ImGui::CollapsingHeader("Mod Settings"))
	{
		valid_config = RequiredTextInput("Name", modname_buffer, MODNAME_MAX_LENGTH, ImGuiInputTextFlags_CallbackCharFilter, modname_filter) && valid_config;
		valid_config = RequiredInt3Input("Version", reinterpret_cast<int *>(&mod_version)) && valid_config;
		valid_config = RequiredTextInput("Title", modtitle_buffer, MODTITLE_MAX_LENGTH) && valid_config;
		valid_config = RequiredTextInput("Author", modauthor_buffer, MODAUTHOR_BUFFER_SIZE) && valid_config;
		ImGui::SetNextItemWidth(512);
		ImGui::InputText("Contact", modcontact_buffer, MODCONTACT_BUFFER_SIZE) && valid_config;
		ImGui::SetNextItemWidth(512);
		ImGui::InputText("Homepage", modhomepage_buffer, MODHOMEPAGE_BUFFER_SIZE) && valid_config;
		ImGui::SetNextItemWidth(512);
		ImGui::InputTextMultiline("Description", moddescription_buffer, MODDESCRIPTION_BUFFER_SIZE) && valid_config;
		ImGui::SetNextItemWidth(512);
		ImGui::Combo("Factorio Version", &factorio_version_i, factorio_versions, 9);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::SetNextItemWidth(512);
		ImGui::BeginListBox("Dependencies");
		for (int i = 0 ; i < project.dependencies.size() ; i++)
		{
			std::string label = "deplistbox_" + i;
			ImGui::PushID(label.c_str());
			if (ImGui::Selectable(project.dependencies[i].str().c_str(), i == cur_dep))
			{
				cur_dep = i;
			}
			if (i == cur_dep)
			{
				ImGui::SetItemDefaultFocus();
			}
			ImGui::PopID();
		}
		ImGui::EndListBox();

		if (ImGui::Button("Add", ImVec2{171, 0}))
		{
			project.dependencies.push_back(FactorioModDependency{});
			cur_dep = project.dependencies.size() - 1;

			memset(depeditor_modname, 0, MODNAME_MAX_LENGTH);
			depeditor_version_comp = 5;
			memset(depeditor_version, 0, sizeof(int) * 3);
			depeditor_deptype = 0;

			ImGui::OpenPopup("Add Dependency");
		}
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 8);
		bool was_d = false;
		if (cur_dep == -1)
		{
			ImGui::BeginDisabled();
			was_d = true;
		}
		if (ImGui::Button("Edit", ImVec2{170, 0}))
		{
			auto d = project.dependencies[cur_dep];
			memset(depeditor_modname, 0, MODNAME_MAX_LENGTH);
			memcpy_s(depeditor_modname, MODNAME_MAX_LENGTH, d.modname.c_str(), d.modname.size());
			if (d.version_inequality.has_value())
			{
				depeditor_version_comp = static_cast<int>(d.version_inequality.value().op);
				depeditor_version[0] = d.version_inequality.value().right.major;
				depeditor_version[1] = d.version_inequality.value().right.minor;
				depeditor_version[2] = d.version_inequality.value().right.patch;
			} else
			{
				memset(depeditor_version, 0, sizeof(int) * 3);
				depeditor_version_comp = 5;
			}
			depeditor_deptype = static_cast<int>(d.depType);


			ImGui::OpenPopup("Edit Dependency");
		}
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 8);
		if (ImGui::Button("Remove", ImVec2{171, 0}))
		{
			project.dependencies.erase(project.dependencies.begin() + cur_dep);
			cur_dep = -1;
		}

		if (was_d)
		{
			ImGui::EndDisabled();
		}
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("Add Dependency", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

			ImGui::SetNextItemWidth(256);
			ImGui::Combo("Dependency Type", &depeditor_deptype, depeditor_deptypes, 5);
			ImGui::SetNextItemWidth(256);
			ImGui::InputText("Mod Name", depeditor_modname, MODNAME_MAX_LENGTH);
			ImGui::SetNextItemWidth(64);
			ImGui::Combo("##vc", &depeditor_version_comp, depeditor_versioncomps, 6);

			ImGui::SameLine();
			if (depeditor_version_comp == 5)
			{
				ImGui::BeginDisabled();
				ImGui::SetNextItemWidth(184);
				ImGui::InputInt3("Version", depeditor_version);
				ImGui::EndDisabled();
			} else
			{
				ImGui::SetNextItemWidth(184);
				ImGui::InputInt3("Version", depeditor_version);
			}

			if (ImGui::Button("Save"))
			{
				ImGui::CloseCurrentPopup();
				if (depeditor_modname[0] == 0)
				{
					project.dependencies.erase(project.dependencies.begin() + cur_dep);
					cur_dep = -1;
				} else
				{
					project.dependencies[cur_dep].depType = static_cast<FactorioModDependency::DependencyType>(depeditor_deptype);
					project.dependencies[cur_dep].modname = depeditor_modname;
					if (depeditor_version_comp == 5) {
						project.dependencies[cur_dep].version_inequality = std::nullopt;
					}
					else {
						project.dependencies[cur_dep].version_inequality = PartialInequality<FactorioModVersion>{static_cast<RelationalOperator>(depeditor_version_comp), FactorioModVersion{depeditor_version[0], depeditor_version[1], depeditor_version[2]}};
					}

				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				project.dependencies.erase(project.dependencies.begin() + cur_dep);
				cur_dep = -1;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}


		if (ImGui::BeginPopupModal("Edit Dependency", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

			ImGui::SetNextItemWidth(256);
			ImGui::Combo("Dependency Type", &depeditor_deptype, depeditor_deptypes, 5);
			ImGui::SetNextItemWidth(256);
			ImGui::InputText("Mod Name", depeditor_modname, MODNAME_MAX_LENGTH);
			ImGui::SetNextItemWidth(64);
			ImGui::Combo("##vc", &depeditor_version_comp, depeditor_versioncomps, 6);

			ImGui::SameLine();
			if (depeditor_version_comp == 5) {
				ImGui::BeginDisabled();
				ImGui::SetNextItemWidth(184);
				ImGui::InputInt3("Version", depeditor_version);
				ImGui::EndDisabled();
			}
			else {
				ImGui::SetNextItemWidth(184);
				ImGui::InputInt3("Version", depeditor_version);
			}

			if (ImGui::Button("Save")) {
				ImGui::CloseCurrentPopup();
				project.dependencies[cur_dep].depType = static_cast<FactorioModDependency::DependencyType>(depeditor_deptype);
				if (depeditor_modname[0] != 0)
					project.dependencies[cur_dep].modname = depeditor_modname;
				if (depeditor_version_comp == 5) {
					project.dependencies[cur_dep].version_inequality = std::nullopt;
				}
				else {
					project.dependencies[cur_dep].version_inequality = PartialInequality<FactorioModVersion>{static_cast<RelationalOperator>(depeditor_version_comp), FactorioModVersion{depeditor_version[0], depeditor_version[1], depeditor_version[2]}};
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

	} else
	{
		// validate
		valid_config = valid_config && modname_buffer[0] != 0 && mod_version != FactorioModVersion{0, 0, 0};\
		valid_config = valid_config && modtitle_buffer[0] != 0 && modauthor_buffer[0] != 0;
	}
	
	valid_config = valid_config && mod_version.major >= MIN_VERSION_NUM && mod_version.minor >= MIN_VERSION_NUM && mod_version.patch >= MIN_VERSION_NUM;
	valid_config = valid_config && mod_version.major <= MAX_VERSION_NUM && mod_version.minor <= MAX_VERSION_NUM && mod_version.patch <= MAX_VERSION_NUM;


	if (!valid_config)
	{
		ImGui::BeginDisabled();
		ImGui::Button("Create");
		ImGui::EndDisabled();
	} else
	{
		if (ImGui::Button("Create"))
		{
			project.mod_folder = project_folder_str;
			project.description = moddescription_buffer;
			project.author = modauthor_buffer;
			project.contact = modcontact_buffer;
			project.factorio_version = static_cast<FactorioVersion>(factorio_version_i);
			project.homepage = modhomepage_buffer;
			project.name = modname_buffer;
			project.title = modtitle_buffer;
			project.version = mod_version;

			project.write_info_file();

			auto scr = app->screen_manager().get_screen(AppScreen::EditFactorioMod);

			std::dynamic_pointer_cast<EditFactorioModScreen>(scr)->set_project(project);
			app->screen_manager().set_screen(AppScreen::EditFactorioMod);
		}
	}
}

void NewFactorioModScreen::draw_main_menu()
{
	Screen::draw_main_menu();
}

void NewFactorioModScreen::on_show()
{
	Screen::on_show();
	reset_buffers();
}

void EditFactorioModScreen::set_project(const FactorioProject& proj)
{
	project = proj;
	set_title("Editing: " + proj.title);
}

void EditFactorioModScreen::draw_ui()
{
}

void EditFactorioModScreen::on_show()
{
	Screen::on_show();
}
