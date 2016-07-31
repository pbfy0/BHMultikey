#include "RawKeyboard.h"

#include <Windows.h>

RawKeyboard::RawKeyboard(HANDLE h)
{
	handle = h;
	id = 0;
	/*unsigned size;
	GetRawInputDeviceInfo(h, RIDI_DEVICENAME, nullptr, &size);
	wchar_t *name = new wchar_t[size];
	GetRawInputDeviceInfo(h, RIDI_DEVICENAME, name, &size);
	HANDLE hid_handle = CreateFile(name, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
	if (hid_handle) {
		wchar_t dev_name[126];
		bool res = HidD_GetProductString(hid_handle, dev_name, sizeof(wchar_t) * 126);
		CloseHandle(hid_handle);
		if (res) {
			OutputDebugString(dev_name);
		} else {
			OutputDebugString(L"err");
		}
	}
	
	/*unsigned u8s = WideCharToMultiByte(CP_UTF8, 0, name, -1, nullptr, 0, nullptr, nullptr);
	char *s = new char[u8s];
	WideCharToMultiByte(CP_UTF8, 0, name, -1, s, u8s, nullptr, nullptr);
	this->name = "I am a keyboard";// std::string(s);
	delete name;
	//delete s;*/
}


RawKeyboard::~RawKeyboard()
{
}

void RawKeyboard::tick()
{
	if(id != 0) frame_action = 0;
}

unsigned RawKeyboard::get_down_state(bool is_ui)
{
	return is_ui ? ui_state : g_state;
}

unsigned RawKeyboard::get_frame_action()
{
	return frame_action;
}

void RawKeyboard::set_mapping(uint32_t key, uint32_t val, bool is_ui)
{
	(is_ui ? ui_map : g_map)[key] = val;
}

void RawKeyboard::clear_mappings(bool is_ui)
{
	(is_ui ? ui_map : g_map).clear();
}

std::string RawKeyboard::get_name()
{
	//return name;
	char x[100];
	snprintf(x, 100, "Keyboard %d", id);
	return std::string(x);
}

void RawKeyboard::handle_input(uint32_t key, bool down)
{
	if (down) {
		frame_action |= ui_map[key];
		ui_state |= ui_map[key];
		g_state |= g_map[key];
	} else {
		ui_state &= ~(ui_map[key]);
		g_state &= ~(g_map[key]);
	}
	/*wchar_t x[128];
	wsprintf(x, L"%d: %d %d", id, key, down);
	OutputDebugString(x);*/
}
