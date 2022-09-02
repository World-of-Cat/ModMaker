#pragma once
#include <memory>
#include <string>
#include <concepts>
#include <unordered_map>

#include <imgui.h>

#include "global.h"


struct UIInfo
{

	ImFont *normalFont, *titleFont;
	ImGuiIO *io_;

	ImGuiIO& io()
	{
		return *io_;
	}
};

class Screen
{
public:
	Screen(const std::string &title, UIInfo& ui_info);
	virtual ~Screen();

	void draw();

	[[nodiscard]] std::string get_title() const
	{
		return title;
	}

	void set_title(const std::string& title)
	{
		this->title = title;
	}

	virtual void draw_ui() = 0;

	virtual void draw_extra_windows() { demo_win(); }
	virtual void draw_main_menu() { mainMenuBar(); }
	virtual void on_show() {};

	
protected:
	UIInfo &ui_info;

private:

	std::string title;

};

template<typename T>
concept hashable = requires(T a) {
	{ std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};

template<hashable I>
class ScreenManager
{
public:
	ScreenManager()
	{

	}

	void add_screen(const I& index, const std::shared_ptr<Screen>& screen)
	{
		screens[index] = screen;
		reverse_screenmap[screen] = index;
	}

	void draw_screen()
	{
		if (current)
		{
			current->draw();
		}
	}

	void set_screen(const I& index)
	{
		auto s = get_screen(index);
		if (current != s) s->on_show();
		if (s != nullptr) current = s;
	}

	const I& get_current_screen_index() const
	{
		return reverse_screenmap.find(current)->second;
	}

	const std::shared_ptr<Screen>& get_current_screen() const
	{
		return current;
	}

	const I &get_screen_index(const std::shared_ptr<Screen> &screen) const
	{
		auto sc = reverse_screenmap.find(screen);
		return sc->second;
	}

	const std::shared_ptr<Screen>& get_screen(const I& index) const
	{
		auto sc = screens.find(index);
		if (sc == screens.end()) return nullptr;
		return sc->second;
	}

	void remove_screen(const I& index)
	{
		auto sc = screens.find(index);
		if (sc != screens.end())
		{
			reverse_screenmap.erase(sc->second);
			screens.erase(index);
		}
	}

	void remove_screen(const std::shared_ptr<Screen>& index)
	{
		auto sc = reverse_screenmap.find(index);
		if (sc != reverse_screenmap.end()) {
			screens.erase(sc->second);
			reverse_screenmap.erase(index);
		}
	}

private:

	std::unordered_map<I, std::shared_ptr<Screen> > screens;
	std::unordered_map<std::shared_ptr<Screen>, I> reverse_screenmap;

	std::shared_ptr<Screen> current;
};
