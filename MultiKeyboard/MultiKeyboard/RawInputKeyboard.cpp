#include "RawInputKeyboard.h"
#include "dllMain.h"

#include <Windows.h>

std::map<HANDLE, RawInputKeyboard *> RawInputKeyboard::h_kbs;

RawInputKeyboard::RawInputKeyboard(HANDLE h)
{
	handle = h;
	id = 0;
	//i = 0;
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


RawInputKeyboard::~RawInputKeyboard()
{
}

void RawInputKeyboard::handle_input(LPARAM lParam) {
	HRAWINPUT ri = (HRAWINPUT)lParam;
	UINT bufferSize;
	GetRawInputData(ri, RID_INPUT, NULL, &bufferSize, sizeof(RAWINPUTHEADER));
	LPBYTE dataBuffer = new BYTE[bufferSize];

	GetRawInputData(ri, RID_INPUT, dataBuffer, &bufferSize, sizeof(RAWINPUTHEADER));
	RAWINPUT* raw = (RAWINPUT*)dataBuffer;

	USHORT virtualKeyCode = raw->data.keyboard.VKey;
	bool keyPressed = raw->data.keyboard.Flags & RI_KEY_BREAK ? false : true;
	HANDLE dev = raw->header.hDevice;

	delete[] dataBuffer;

	if (virtualKeyCode == VK_SHIFT) {
		virtualKeyCode = VK_OEM_3;
	}

	RawInputKeyboard *rk;
	if (h_kbs.find(dev) == h_kbs.end()) {
		h_kbs[dev] = rk = new RawInputKeyboard(dev);
		mk_instance->add_kbd(rk);
	}
	else {
		rk = h_kbs[dev];
	}
	rk->handle_input(virtualKeyCode, keyPressed);

}

void RawInputKeyboard::tick()
{
	if(id != 0) frame_action = 0;
}

unsigned RawInputKeyboard::get_down_state(bool is_ui)
{
	/*i = (i + 1) % 60;
	if (i == 0) {
		wchar_t msg[100];
		_snwprintf_s(msg, 100, _TRUNCATE, L"getting id %d", id);
		OutputDebugString(msg);
	}*/
	return is_ui ? ui_state : g_state;
}

unsigned RawInputKeyboard::get_frame_action()
{
	return frame_action;
}

void RawInputKeyboard::set_mapping(uint32_t key, uint32_t val, bool is_ui)
{
	(is_ui ? ui_map : g_map)[key] = val;
}

void RawInputKeyboard::clear_mappings(bool is_ui)
{
	(is_ui ? ui_map : g_map).clear();
}

std::string RawInputKeyboard::get_name()
{
	//return name;
	char x[100];
	snprintf(x, 100, "Keyboard %d", id);
	return std::string(x);
}

void RawInputKeyboard::handle_input(uint32_t key, bool down)
{
	/*wchar_t msg[100];
	_snwprintf_s(msg, 100, _TRUNCATE, L"id %d key %d down %d", id, key, down);
	OutputDebugString(msg);*/
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
