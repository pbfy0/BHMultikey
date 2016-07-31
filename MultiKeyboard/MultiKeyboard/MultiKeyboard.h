#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <future>

#include "RawKeyboard.h"
class MultiKeyboard
{
public:
	MultiKeyboard();
	~MultiKeyboard();

	void handle_raw_input(HRAWINPUT ri);
	bool c_init();

	bool init();
	unsigned activate();
	unsigned tick_messages();

	unsigned get_down_state(uint32_t kbd_id, bool is_ui);
	unsigned get_frame_action(uint32_t kbd_id);
	void set_mapping(uint32_t kbd_id, uint32_t key, uint32_t val, bool is_ui);
	void clear_mappings(uint32_t kbd_id, bool is_ui);
	std::string get_name(uint32_t kbd_id);
	bool enum_proc_callback(HWND hwnd);
private:
	RawKeyboard * add_kbd(HANDLE h);

	std::vector<RawKeyboard *> kbs;
	std::map<HANDLE, RawKeyboard *> h_kbs;
	std::queue<RawKeyboard *> unassigned_kbs;

	DWORD pid;
	ATOM wclAtom;
	HWND air_hwnd;
	HWND hwnd;
	bool has_focus;
};

