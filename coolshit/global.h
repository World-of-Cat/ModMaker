#pragma once
#include "Text.h"
#include <nfd.h>
#include <string>
#include <optional>
#include <array>

#include <imgui.h>

class App;
extern App *app;

void mainMenuBar();
void demo_win();

constexpr size_t PATH_BUFFER_SIZE = 256;

bool PathWidget(char buffer[PATH_BUFFER_SIZE], const std::string &label, bool require_empty = false);
bool RequiredTextInput(const std::string &label, char *buffer, size_t size, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr);
bool RequiredInt3Input(const std::string &label, int *buffer, std::array<int,3> invalid_value = {0, 0, 0});

enum class RelationalOperator {
	opGreater, opGreaterEqual, opLess, opLessEqual, opEqual
};

template<typename T>
struct PartialInequality
{
	RelationalOperator op;
	T right;
};