#include <Windows.h>
#include <future>

#include "MultiKeyboard.h"
#include "dllMain.h"



BOOL CALLBACK _enum_proc_callback(HWND hwnd, LPARAM lParam) {
	MultiKeyboard *self = (MultiKeyboard *)lParam;
	return self->enum_proc_callback(hwnd);
}

LRESULT CALLBACK _wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_INPUT:
		mk_instance->handle_raw_input((HRAWINPUT)lParam);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

MultiKeyboard::MultiKeyboard()
{
	hwnd = 0;
	wclAtom = 0;
}


MultiKeyboard::~MultiKeyboard()
{
	if(hwnd) DestroyWindow(hwnd);
	if(wclAtom) UnregisterClass((LPCWSTR)wclAtom, dll_module);
}

void MultiKeyboard::handle_raw_input(HRAWINPUT ri)
{
	UINT bufferSize;
	GetRawInputData(ri, RID_INPUT, NULL, &bufferSize, sizeof(RAWINPUTHEADER));
	LPBYTE dataBuffer = new BYTE[bufferSize];

	GetRawInputData(ri, RID_INPUT, dataBuffer, &bufferSize, sizeof(RAWINPUTHEADER));
	RAWINPUT* raw = (RAWINPUT*)dataBuffer;

	USHORT virtualKeyCode = raw->data.keyboard.VKey;
	bool keyPressed = raw->data.keyboard.Flags & RI_KEY_BREAK ? false : true;
	HANDLE dev = raw->header.hDevice;

	RawKeyboard *rk;
	if (h_kbs.find(dev) == h_kbs.end()) {
		rk = add_kbd(dev);
	} else {
		rk = h_kbs[dev];
	}
	rk->handle_input(virtualKeyCode, keyPressed);
}

RawKeyboard * MultiKeyboard::add_kbd(HANDLE h)
{
	RawKeyboard *rk = new RawKeyboard(h);
	h_kbs[h] = rk;
	unassigned_kbs.push(rk);
	return rk;
}



bool MultiKeyboard::c_init()
{
	WNDCLASS wcl;
	wcl.style = CS_OWNDC;
	wcl.lpfnWndProc = _wnd_proc;
	wcl.cbClsExtra = 0;
	wcl.cbWndExtra = 0;
	wcl.hInstance = dll_module;
	wcl.hIcon = NULL;
	wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcl.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;
	wcl.lpszMenuName = NULL;
	wcl.lpszClassName = L"MultiKeyboard capture";

	wclAtom = RegisterClass(&wcl);
	if (wclAtom == 0) return false;

	hwnd = CreateWindow(
		(LPWSTR)wclAtom,
		L"MultiKeyboard capture window",
		WS_OVERLAPPED,
		CW_USEDEFAULT, CW_USEDEFAULT,
		256, 256,
		HWND_MESSAGE,
		NULL,
		dll_module,
		NULL);

	if (hwnd == nullptr) return false;

	RAWINPUTDEVICE input_device[1];
	input_device[0].usUsagePage = 1;
	input_device[0].usUsage = 6;
	input_device[0].dwFlags = RIDEV_INPUTSINK;
	input_device[0].hwndTarget = hwnd;
	bool res = RegisterRawInputDevices(input_device, 1, sizeof(input_device[0]));
	if (!res) return false;

	/*unsigned n_devices;
	GetRawInputDeviceList(nullptr, &n_devices, sizeof(RAWINPUTDEVICELIST));
	RAWINPUTDEVICELIST *list = new RAWINPUTDEVICELIST[n_devices];
	GetRawInputDeviceList(list, &n_devices, sizeof(RAWINPUTDEVICELIST));
	for (int i = 0; i < n_devices; i++) {
		if (list[i].dwType == RIM_TYPEKEYBOARD) {
			add_kbd(list[i].hDevice);
		}
	}*/
	return true;
}

bool MultiKeyboard::enum_proc_callback(HWND hwnd) {
	DWORD win_pid;
	static wchar_t *cls = L"ApolloRuntimeContentWindow";
	GetWindowThreadProcessId(hwnd, &win_pid);
	if (pid == win_pid) {
		wchar_t buf[32];
		GetClassName(hwnd, buf, 32);
		if (wcsncmp(cls, buf, 32) == 0) {
			this->air_hwnd = hwnd;
			return false;
		}
	}
	return true;
}

bool MultiKeyboard::init()
{
	pid = GetCurrentProcessId();
	EnumWindows(_enum_proc_callback, (LPARAM)this);

	return c_init();
}

unsigned MultiKeyboard::activate()
{
	RawKeyboard *kb = unassigned_kbs.front();
	unassigned_kbs.pop();
	
	kbs.push_back(kb);
	unsigned id = kbs.size();
	kb->id = id;
	return id;
}

unsigned MultiKeyboard::tick_messages()
{
	/*if (has_gc == 1) {
		has_gc = 0;
		OutputDebugString(L"returned 1 from tick");
		return 1;
	}*/

	has_focus = (GetActiveWindow() == air_hwnd || air_hwnd == 0);

	for (auto const& val : kbs) {
		val->tick();
	}

	return unassigned_kbs.size();
}

unsigned MultiKeyboard::get_down_state(uint32_t kbd_id, bool is_ui)
{
	if(!has_focus) return 0;
	return kbs[kbd_id - 1]->get_down_state(is_ui);
}

unsigned MultiKeyboard::get_frame_action(uint32_t kbd_id)
{
	if (!has_focus) return 0;
	return kbs[kbd_id - 1]->get_frame_action();
}

void MultiKeyboard::set_mapping(uint32_t kbd_id, uint32_t key, uint32_t val, bool is_ui)
{
	kbs[kbd_id - 1]->set_mapping(key, val, is_ui);
}

void MultiKeyboard::clear_mappings(uint32_t kbd_id, bool is_ui)
{
	kbs[kbd_id - 1]->clear_mappings(is_ui);
}

std::string MultiKeyboard::get_name(uint32_t kbd_id)
{
	return kbs[kbd_id - 1]->get_name();
}
