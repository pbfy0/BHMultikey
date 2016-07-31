#pragma once

#include <Windows.h>
#include <string>
#include <cstdint>
#include <map>

class RawKeyboard
{
public:
	RawKeyboard(HANDLE h);
	~RawKeyboard();

	void tick();
	
	unsigned get_down_state(bool is_ui);
	unsigned get_frame_action();
	void set_mapping(uint32_t key, uint32_t val, bool is_ui);
	void clear_mappings(bool is_ui);
	std::string get_name();

	void handle_input(uint32_t key, bool down);

	uint32_t id;
private:
	HANDLE handle;
	std::string name;
	unsigned frame_action;
	unsigned ui_state;
	unsigned g_state;
	std::map<unsigned, unsigned> ui_map;
	std::map<unsigned, unsigned> g_map;
};

