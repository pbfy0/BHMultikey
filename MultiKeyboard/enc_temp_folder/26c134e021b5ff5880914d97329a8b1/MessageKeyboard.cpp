#include "MessageKeyboard.h"
#include "dllMain.h"

std::vector<MessageKeyboard *> MessageKeyboard::kbs;

MessageKeyboard::MessageKeyboard(HWND hWnd, uint16_t id)
{
	hwnd = hWnd;
	this->id = id;
}


MessageKeyboard::~MessageKeyboard()
{
}

void MessageKeyboard::handle_input(uint32_t keyval, bool pressed, bool is_ui) {
	if (pressed) {
		if (is_ui) frame_action |= keyval;
		(is_ui ? ui_state : g_state) |= keyval;
	}
	else {
		(is_ui ? ui_state : g_state) &= ~keyval;
	}
}

#define kb_bad(var) (var > kbs.size() || kbs[var-1] == nullptr)

LRESULT MessageKeyboard::handle_message(UINT message, WPARAM wParam, LPARAM lParam)
{
	MessageKeyboard *kb = nullptr;
	if (message == MK_ADD) {
		kb = new MessageKeyboard((HWND)wParam, (uint16_t)kbs.size() + 1);
		kbs.push_back(kb);
		return kb->id;
	}
	if (wParam == 0 || wParam > kbs.size() || kbs[wParam - 1] == nullptr) return -1;

	switch (message) {
	case MK_KEYDOWN:
	case MK_KEYDOWN | MK_ISUI:
		kbs[wParam-1]->handle_input(lParam, true, message & MK_ISUI);
		break;
	case MK_KEYUP:
	case MK_KEYUP | MK_ISUI:
		kbs[wParam-1]->handle_input(lParam, false, message & MK_ISUI);
		break;
	case MK_ACTIVATE:
		mk_instance->add_kbd(kbs[wParam - 1]);
		break;
	case MK_REMOVE:
		mk_instance->remove_kbd(kbs[wParam-1]);
		kbs[wParam - 1] = nullptr;
		delete kbs[wParam-1];
		break;
	case MK_ADDNAME:
		kb = kbs[wParam-1];
		char s[4]; 
		*(uint32_t *)s = lParam;
		kb->name.append((s[3] == '\0') ? s : std::string(s, 4));
		break;
	/*case MK_POLLALL: {
		for (auto const& val : kbs) {
			if (!val->map_queue.empty()) {
				uint32_t v = val->map_queue.front();
				val->map_queue.pop();
				return v;
			}
		}
		return 0;
	}*/
	case MK_POLLONE: {
		kb = kbs[wParam-1];
		if (kb->map_queue.empty()) return 0;
		uint32_t v = kb->map_queue.front();
		kb->map_queue.pop();
		return kb->id << 16 | v;
	}
	}
	return 0;
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
	//unsigned long v;
	//_BitScanReverse(&v, val);
	map_queue.push(key | (val & 0x7fffff) << 8 | is_ui << 31);
	//PostMessage(hwnd, is_ui ? MK_MAPPING_UI : MK_MAPPING_GAME, id, );
}

void MessageKeyboard::clear_mappings(bool is_ui) {
	map_queue.empty();
	map_queue.push(0x7fffffff | is_ui << 31);
	//PostMessage(hwnd, is_ui ? MK_CLEARMAP_GAME : MK_CLEARMAP_GAME, id, 0);
}

std::string MessageKeyboard::get_name() {
	return name;
}