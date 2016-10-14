#include <cassert>
#include <string>
#include "Windows.h"

#include <FlashRuntimeExtensions.h>
#include "MultiKeyboard.h"

MultiKeyboard *mk_instance = nullptr;
HMODULE dll_module = nullptr;

FREObject mk_init(FREContext ctx, void *funcData, uint32_t argc, FREObject argv[]) {
	//assert(argc == 0);
	if (argc != 0) return nullptr;

	mk_instance = new MultiKeyboard();
	FREObject ret;
	FRENewObjectFromBool(mk_instance != nullptr, &ret);
	return ret;
}

FREObject mk_activate(FREContext ctx, void *funcData, uint32_t argc, FREObject argv[]) {
	//assert(argc == 0);
	if (argc != 0) return nullptr;

	FREObject ret;
	FRENewObjectFromUint32(mk_instance->activate(), &ret);
	return ret;
}

FREObject mk_tickmessages(FREContext ctx, void *funcData, uint32_t argc, FREObject argv[]) {
	//assert(argc == 0);
	if (argc != 0) return nullptr;

	FREObject ret;
	FRENewObjectFromUint32(mk_instance->tick_messages(), &ret);
	return ret;
}

FREObject mk_getdownstate(FREContext ctx, void *funcData, uint32_t argc, FREObject argv[]) {
	//assert(argc == 2);
	if (argc != 2) return nullptr;

	FREResult res;
	uint32_t kbd_id;
	res = FREGetObjectAsUint32(argv[0], &kbd_id);
	if (res != FRE_OK) return nullptr;
	uint32_t is_ui;
	res = FREGetObjectAsBool(argv[1], &is_ui);
	if (res != FRE_OK) return nullptr;

	FREObject ret;
	FRENewObjectFromUint32(mk_instance->get_down_state(kbd_id, (bool)is_ui), &ret);
	return ret;
}

FREObject mk_getframeaction(FREContext ctx, void *funcData, uint32_t argc, FREObject argv[]) {
	//assert(argc == 1);
	if (argc != 1) return nullptr;

	FREResult res;
	uint32_t kbd_id;
	res = FREGetObjectAsUint32(argv[0], &kbd_id);
	if (res != FRE_OK) return nullptr;

	FREObject ret;
	FRENewObjectFromUint32(mk_instance->get_frame_action(kbd_id), &ret);
	return ret;
}

FREObject mk_setinputmapping(FREContext ctx, void *funcData, uint32_t argc, FREObject argv[]) {
	//assert(argc == 4);
	if (argc != 4) return nullptr;

	FREResult res;
	uint32_t kbd_id;
	res = FREGetObjectAsUint32(argv[0], &kbd_id);
	if (res != FRE_OK) return nullptr;
	uint32_t key;
	res = FREGetObjectAsUint32(argv[1], &key);
	if (res != FRE_OK) return nullptr;
	uint32_t val;
	res = FREGetObjectAsUint32(argv[2], &val);
	if (res != FRE_OK) return nullptr;
	uint32_t is_ui;
	res = FREGetObjectAsBool(argv[3], &is_ui);
	if (res != FRE_OK) return nullptr;

	mk_instance->set_mapping(kbd_id, key, val, (bool)is_ui);
	return nullptr;
}

FREObject mk_clearinputmappings(FREContext ctx, void *funcData, uint32_t argc, FREObject argv[]) {
	//assert(argc == 2);
	if (argc != 2) return nullptr;

	FREResult res;
	uint32_t kbd_id;
	res = FREGetObjectAsUint32(argv[0], &kbd_id);
	if (res != FRE_OK) return nullptr;
	uint32_t is_ui;
	res = FREGetObjectAsBool(argv[1], &is_ui);
	if (res != FRE_OK) return nullptr;

	FREObject ret = nullptr;
	mk_instance->clear_mappings(kbd_id, (bool)is_ui);
	return ret;
}

FREObject mk_getkeyboardname(FREContext ctx, void *funcData, uint32_t argc, FREObject argv[]) {
	//assert(argc == 1);
	if (argc != 1) return nullptr;

	FREResult res;
	uint32_t kbd_id;
	res = FREGetObjectAsUint32(argv[0], &kbd_id);
	if (res != FRE_OK) return nullptr;

	FREObject ret;
	std::string st = mk_instance->get_name(kbd_id);
	FRENewObjectFromUTF8(st.length()+1, (const uint8_t *)st.c_str(), &ret);
	return ret;
}



void contextInitializer(
	void *extData,
	const uint8_t *ctxType,
	FREContext ctx,
	uint32_t *n_functions,
	const FRENamedFunction **function_table) {

	static FRENamedFunction extension_funcs[] = {
		{ (const uint8_t *) "MultiKeyboard_Init", nullptr, mk_init },
		{ (const uint8_t *) "MultiKeyboard_ActivateRawKeyboard", nullptr, mk_activate },
		{ (const uint8_t *) "MultiKeyboard_TickMessages", nullptr, mk_tickmessages },
		{ (const uint8_t *) "MultiKeyboard_GetDownState", nullptr, mk_getdownstate },
		{ (const uint8_t *) "MultiKeyboard_GetActionThisFrame", nullptr, mk_getframeaction },
		{ (const uint8_t *) "MultiKeyboard_SetInputMapping", nullptr, mk_setinputmapping },
		{ (const uint8_t *) "MultiKeyboard_ClearInputMappings", nullptr, mk_clearinputmappings },
		{ (const uint8_t *) "MultiKeyboard_GetRawKeyboardName", nullptr, mk_getkeyboardname },
	};

	*n_functions = sizeof(extension_funcs) / sizeof(extension_funcs[0]);
	*function_table = extension_funcs;
}

void contextFinalizer(void *extData) {
	delete mk_instance;
}


extern "C" {
	__declspec(dllexport) void ExtensionInitializer(void **extData, FREContextInitializer* ctxInitializer, FREContextFinalizer* ctxFinalizer) {
		*ctxInitializer = &contextInitializer;
		*ctxFinalizer = &contextFinalizer;
	}

	__declspec(dllexport) void ExtensionFinalizer(void *extData) {
		return;
	}

	BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
		dll_module = hModule;
		return true;
	}
}