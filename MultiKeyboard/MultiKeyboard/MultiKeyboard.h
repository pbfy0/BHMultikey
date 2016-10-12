#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <future>

#include "RawInputKeyboard.h"
class MultiKeyboard
{
public:
	MultiKeyboard();
	~MultiKeyboard();

	unsigned activate();
	unsigned tick_messages();

	unsigned get_down_state(uint32_t kbd_id, bool is_ui);
	unsigned get_frame_action(uint32_t kbd_id);
	void set_mapping(uint32_t kbd_id, uint32_t key, uint32_t val, bool is_ui);
	void clear_mappings(uint32_t kbd_id, bool is_ui);
	std::string get_name(uint32_t kbd_id);
	bool enum_proc_callback(HWND hwnd);
	void add_kbd(Keyboard * k);
	void remove_kbd(Keyboard * k);
private:

	bool win_init();

	std::vector<Keyboard *> kbs;
	std::queue<Keyboard *> unassigned_kbs;

	DWORD pid;
	ATOM wclAtom;
	HWND air_hwnd;
	HWND hwnd;
	bool has_focus;
};