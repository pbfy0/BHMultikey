#include "MessageKeyboard.h"
#include "dllMain.h"

std::vector<MessageKeyboard *> MessageKeyboard::kbs;

MessageKeyboard::MessageKeyboard(HWND hWnd, uint16_t id)
{
	hwnd = hWnd;
	this->id = id;
	PostMessage(hWnd, MK_HANDSHAKE, id, 0);
}


MessageKeyboard::~MessageKeyboard()
{
}

void MessageKeyboard::handle_input(uint32_t keyval, bool pressed, bool is_ui) {
	if (pressed) {
		frame_action |= keyval;
		(is_ui ? ui_state : g_state) |= 1<<keyval;
	}
	else {
		(is_ui ? ui_state : g_state) &= ~(1<<keyval);
	}
}

void MessageKeyboard::handle_message(UINT message, WPARAM wParam, LPARAM lParam)
{
	MessageKeyboard *kb = nullptr;
	switch (message) {
	case MK_KEYDOWN:
	case MK_KEYDOWN | MK_ISUI:
		kbs[wParam]->handle_input(lParam, true, message & MK_ISUI);
		break;
	case MK_KEYUP:
	case MK_KEYUP | MK_ISUI:
		kbs[wParam]->handle_input(lParam, false, message & MK_ISUI);
		break;
	case MK_ADD:
		kb = new MessageKeyboard((HWND)wParam, kbs.size());
		kbs.push_back(kb);
		mk_instance->add_kbd(kb);
		break;
	case MK_REMOVE:
		mk_instance->remove_kbd(kbs[wParam]);
		break;
	case MK_RESETNAME:
		kbs[wParam]->name.clear();
		break;
	case MK_ADDNAME:
		kb = kbs[wParam];
		char s[4];
		*(uint32_t *)s = lParam;
		kb->name.append((s[3] == '\0') ? s : std::string(s, 4));
		break;
	}
}

void MessageKeyboard::tick()
{
	if (id != 0) frame_action = 0;
}

unsigned MessageKeyboard::get_down_state(bool is_ui) {
	return is_ui ? ui_state : g_state;
}

unsigned MessageKeyboard::get_frame_action() {
	return frame_action;
}

void MessageKeyboard::set_mapping(uint32_t key, uint32_t val, bool is_ui) {
	unsigned long v;
	_BitScanReverse(&v, val);
	PostMessage(hwnd, is_ui ? MK_MAPPING_UI : MK_MAPPING_GAME, id, key << 16 | (uint16_t)v);
}

void MessageKeyboard::clear_mappings(bool is_ui) {
	PostMessage(hwnd, is_ui ? MK_CLEARMAP_GAME: MK_CLEARMAP_GAME, id, 0);
}

std::string MessageKeyboard::get_name() {
	return name;
}