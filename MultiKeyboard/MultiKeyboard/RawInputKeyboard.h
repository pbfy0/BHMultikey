#pragma once

#include "Keyboard.h"
#include <Windows.h>
#include <string>
#include <cstdint>
#include <map>

class RawInputKeyboard : public Keyboard
{
public:
	static void handle_input(LPARAM lParam);

	RawInputKeyboard(HANDLE h);
	~RawInputKeyboard();

	void tick();
	void handle_input(uint32_t key, bool down);
	HANDLE handle;

	unsigned get_down_state(bool is_ui);
	unsigned get_frame_action();
	void set_mapping(uint32_t key, uint32_t val, bool is_ui);
	void clear_mappings(bool is_ui);
	std::string get_name();
private:
	static std::map<HANDLE, RawInputKeyboard *> h_kbs;
	//std::string name;
	unsigned frame_action;
	unsigned ui_state;
	unsigned g_state;
	//unsigned i;
	std::map<unsigned, unsigned> ui_map;
	std::map<unsigned, unsigned> g_map;
};

