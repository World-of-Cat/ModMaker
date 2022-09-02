#pragma once
#include <filesystem>

#include "global.h"
#include "ScreenManager.h"
#include "Text.h"

enum class FactorioVersion
{
	v1_1, v1_0, v0_18, v0_17, v0_16, v0_15, v0_14, v0_13, v0_12
};

struct FactorioModVersion
{
	int major, minor, patch;

	friend bool operator==(const FactorioModVersion& lhs, const FactorioModVersion& rhs)
	{
		return lhs.major == rhs.major
			&& lhs.minor == rhs.minor
			&& lhs.patch == rhs.patch;
	}

	friend bool operator!=(const FactorioModVersion& lhs, const FactorioModVersion& rhs)
	{
		return !(lhs == rhs);
	}
};

struct FactorioModDependency
{
	enum class DependencyType
	{
		Hard, Optional, Incompatibility, HiddenOptional, DoesNotAffectLoadOrder
	};

	DependencyType depType = DependencyType::Hard;
	std::string modname;
	std::optional<PartialInequality<FactorioModVersion> > version_inequality;
	std::string str() const;
};

inline PartialInequality<FactorioModVersion> factorio_default_mod_version_inequality()
{
	return PartialInequality{RelationalOperator::opGreaterEqual, FactorioModVersion{0, 0, 0} };
}

struct FactorioProject
{
	std::filesystem::path mod_folder;
	std::string name, title, author, contact, homepage, description;
	FactorioModVersion version;
	FactorioVersion factorio_version;
	std::vector<FactorioModDependency> dependencies = {
		FactorioModDependency{.modname = "base"}
	};
	void write_info_file();

};

class NewFactorioModScreen : public Screen {
public:
	void reset_buffers();

	NewFactorioModScreen(const std::string &title, UIInfo &ui_info)
		: Screen(title, ui_info) {
		reset_buffers();
	}

	void draw_ui() override;
	void draw_main_menu() override;
	void on_show() override;

private:

	static constexpr size_t MODNAME_MAX_LENGTH = 100;
	static constexpr int MIN_VERSION_NUM = 0;
	static constexpr int MAX_VERSION_NUM = 65535;
	static constexpr size_t MODTITLE_MAX_LENGTH = 100;
	static constexpr size_t MODAUTHOR_BUFFER_SIZE = 256;
	static constexpr size_t MODCONTACT_BUFFER_SIZE = 256;
	static constexpr size_t MODHOMEPAGE_BUFFER_SIZE = 256;
	static constexpr size_t MODDESCRIPTION_BUFFER_SIZE = 65536;

	char project_folder_str[PATH_BUFFER_SIZE];
	char modname_buffer[MODNAME_MAX_LENGTH];
	FactorioModVersion mod_version{0,1,0};
	char modtitle_buffer[MODTITLE_MAX_LENGTH];
	char modauthor_buffer[MODAUTHOR_BUFFER_SIZE];
	char modcontact_buffer[MODCONTACT_BUFFER_SIZE];
	char modhomepage_buffer[MODHOMEPAGE_BUFFER_SIZE];
	char moddescription_buffer[MODDESCRIPTION_BUFFER_SIZE];
	int factorio_version_i = 0;

	bool editing_dep = false;
	int cur_dep = -1;

	FactorioProject project{};

	int depeditor_deptype = 0;
	const char *depeditor_deptypes[5] = {
		"Hard", "Optional [?]", "Incompatibility [!]", "Hidden Optional [(?)]", "Doesn't Affect Load Order [~]"
	};

	int depeditor_version_comp = 5;
	const char *depeditor_versioncomps[6] = {
		">", ">=", "<", "<=", "=", "*"
	};

	int depeditor_version[3] = {0, 0, 0};

	char depeditor_modname[MODNAME_MAX_LENGTH];

};


class EditFactorioModScreen : public Screen
{
public:
	EditFactorioModScreen(const std::string& title, UIInfo& ui_info)
		: Screen(title, ui_info)
	{
	}


	void set_project(const FactorioProject &proj);


	void draw_ui() override;
	void on_show() override;

private:

	FactorioProject project;

};