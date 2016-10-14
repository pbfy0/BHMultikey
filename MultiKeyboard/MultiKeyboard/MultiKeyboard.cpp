#include <Windows.h>
#include <future>

#include "MultiKeyboard.h"
#include "MessageKeyboard.h"
#include "dllMain.h"



BOOL CALLBACK _enum_proc_callback(HWND hwnd, LPARAM lParam) {
	MultiKeyboard *self = (MultiKeyboard *)lParam;
	return self->enum_proc_callback(hwnd);
}


LRESULT CALLBACK _wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_INPUT:
		RawInputKeyboard::handle_input(lParam);
		break;
	case MK_KEYDOWN:
	case MK_KEYDOWN | MK_ISUI:
	case MK_KEYUP:
	case MK_KEYUP | MK_ISUI:
	case MK_ADD:
	case MK_ACTIVATE:
	case MK_REMOVE:
	case MK_POLLONE:
	case MK_ADDNAME:
		return MessageKeyboard::handle_message(message, wParam, lParam);
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

MultiKeyboard::MultiKeyboard()
{
	hwnd = 0;
	wclAtom = 0;

	pid = GetCurrentProcessId();
	EnumWindows(_enum_proc_callback, (LPARAM)this);

	if (!win_init()) throw new std::exception("Error initializing MultiKeyboard window");

}


MultiKeyboard::~MultiKeyboard()
{
	if(hwnd) DestroyWindow(hwnd);
	if(wclAtom) UnregisterClass((LPCWSTR)wclAtom, dll_module);
}

void MultiKeyboard::add_kbd(Keyboard *k)
{
	unassigned_kbs.push(k);
}

void MultiKeyboard::remove_kbd(Keyboard *k)
{
	if(k->id != 0) kbs[k->id - 1] = nullptr;
}

bool MultiKeyboard::win_init()
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

	wchar_t x[128];
	_snwprintf_s(x, 100, _TRUNCATE, L"Capture hwnd: %p", hwnd);
	OutputDebugString(x);

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

unsigned MultiKeyboard::activate()
{
	Keyboard *kb = unassigned_kbs.front();
	unassigned_kbs.pop();

	
	kbs.push_back(kb);
	unsigned id = kbs.size();
	//wchar_t x[100];
	//_snwprintf_s(x, 100, _TRUNCATE, L"Assigned id %d to handle %p", id, kb->handle);
	//OutputDebugString(x);
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
		if(val != nullptr)
			val->tick();
	}

	return unassigned_kbs.size();
}

unsigned MultiKeyboard::get_down_state(uint32_t kbd_id, bool is_ui)
{
	if (!has_focus) return 0;
	if (kbs[kbd_id - 1] == nullptr) return -1;
	return kbs[kbd_id - 1/*is_ui ? id : (n_kbs - 1) - id*/]->get_down_state(is_ui);
}

unsigned MultiKeyboard::get_frame_action(uint32_t kbd_id)
{
	if (!has_focus) return 0;
	if (kbs[kbd_id - 1] == nullptr) return -1;
	return kbs[kbd_id - 1]->get_frame_action();
}

void MultiKeyboard::set_mapping(uint32_t kbd_id, uint32_t key, uint32_t val, bool is_ui)
{
	if (kbs[kbd_id - 1] == nullptr) return;
	kbs[kbd_id - 1]->set_mapping(key, val, is_ui);
}

void MultiKeyboard::clear_mappings(uint32_t kbd_id, bool is_ui)
{
	if (kbs[kbd_id - 1] == nullptr) return;
	kbs[kbd_id - 1]->clear_mappings(is_ui);
}

std::string MultiKeyboard::get_name(uint32_t kbd_id)
{
	if (kbs[kbd_id - 1] == nullptr) return "";
	return kbs[kbd_id - 1]->get_name();
}
