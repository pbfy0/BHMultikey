#pragma once
#include "Keyboard.h"
#include <Windows.h>
#include <vector>
#include <queue>

#define MK_ADD 0x1000
#define MK_ACTIVATE 0x1001
#define MK_REMOVE 0x1002
#define MK_KEYDOWN 0x1010
#define MK_KEYUP 0x1012
#define MK_ISUI 1
#define MK_ADDNAME 0x1020
#define MK_POLLONE 0x1030
//#define MK_POLLALL 0x1031

#define MK_HANDSHAKE 0x1100
#define MK_MAPPING_GAME 0x1110
#define MK_MAPPING_UI 0x1111
#define MK_CLEARMAP_GAME 0x1112
#define MK_CLEARMAP_UI 0x1113


class MessageKeyboard :
	public Keyboard
{
public:
	static LRESULT handle_message(UINT message, WPARAM wParam, LPARAM lParam);

	void tick();

	unsigned get_down_state(bool is_ui);

	unsigned get_frame_action();

	void set_mapping(uint32_t key, uint32_t val, bool is_ui);

	void clear_mappings(bool is_ui);

	std::string get_name();

	MessageKeyboard(HWND hWnd, uint16_t id);
	virtual ~MessageKeyboard();

private:
	HWND hwnd;
	unsigned id;
	void handle_input(uint32_t keyval, bool pressed, bool is_ui);
	unsigned frame_action;
	unsigned ui_state;
	unsigned g_state;
	static std::vector<MessageKeyboard *> kbs;

	std::string name;

	std::queue<uint32_t> map_queue;
};

