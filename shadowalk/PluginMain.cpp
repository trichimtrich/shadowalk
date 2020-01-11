#include "PluginMain.h"

int pluginHandle;
int hMenuDisasm;

bool bSystemBp = TRUE;

char* lpFileName = NULL;
bool bRunOnInit = FALSE;

duint duiShadowAddr = 0;
bool bShadowInBp = FALSE;
BPMAP bpMapNormal;
BPMAP bpMapHardware;
BPMAP bpMapMemory;

enum {
	MENU_RUN_HERE,
	MENU_RESTART_HERE,
	MENU_SYSTEM_BREAKPOINT,
};


// to make sure we dont delete exists bp
bool removeShadowBp() {
	if (bShadowInBp) {
		bShadowInBp = FALSE;
		// Do nothing
	}
	else {
		if (!Script::Debug::DeleteBreakpoint(duiShadowAddr)) {
			return FALSE;
		}
	}
	duiShadowAddr = 0;
	return TRUE;
}

// to check if shadow bp is valid one
bool setShadowBp() {
	BPXTYPE x = DbgGetBpxTypeAt(duiShadowAddr);
	if (x == bp_normal || x == bp_hardware || x == bp_memory) {
		// There is a breakpoint at shadow address
		bShadowInBp = TRUE;

		// TODO: Better remove its original content (with condition/log/...) 
		// and use a clean bp to make sure it hit
	}
	// Set/Enable BP
	return Script::Debug::SetBreakpoint(duiShadowAddr);
}

void cbBreakpoint(CBTYPE cbType, void* callbackInfo) {
	if (cbType == CB_BREAKPOINT) {
		if (PLUG_CB_BREAKPOINT* info = reinterpret_cast<PLUG_CB_BREAKPOINT*>(callbackInfo)) {
			dprintf("Reach BP Address: %x\n", info->breakpoint->addr);
			if (info->breakpoint->addr == duiShadowAddr) {
				dputs("That Shadow Address. Clean up callback and shadow breakpoint");
				unregisterBpCallback();
				removeShadowBp();

				//enableAllBpMap();
				//if (!bShadowInBp) {
				//	Script::Debug::DeleteBreakpoint(duiShadowAddr);
				//	duiShadowAddr = 0;
				//}
				//else {
				//	bShadowInBp = FALSE;
				//}
			}
			else {
				DbgCmdExec("run");
			}
		}
	}
	else if (cbType == CB_SYSTEMBREAKPOINT) {
		dputs("Reach System Breakpoint");
		// just run
		DbgCmdExec("run");
	}
}

void registerBpCallback() {
	_plugin_registercallback(pluginHandle, CB_BREAKPOINT, cbBreakpoint);
	if (!bSystemBp) {
		// System BP is rejected
		_plugin_registercallback(pluginHandle, CB_SYSTEMBREAKPOINT, cbBreakpoint);
	}
}

void unregisterBpCallback() {
	_plugin_unregistercallback(pluginHandle, CB_BREAKPOINT);
	_plugin_unregistercallback(pluginHandle, CB_SYSTEMBREAKPOINT);
}

//void _disableBpMap(BPMAP *bpMap, BPXTYPE bpType) {
//	DbgGetBpList(bpType, bpMap);
//	for (unsigned int i = 0; i < bpMap->count; i++) {
//		dprintf("Disable addr: %x\n", bpMap->bp[i].addr);
//		Script::Debug::DisableBreakpoint(bpMap->bp[i].addr);
//		if (bpMap->bp[i].addr == duiShadowAddr) {
//			dputs("Shadow breakpint in BPMAP");
//			bShadowInBp = TRUE;
//		}
//	}
//}
//
//void disableAllBpMap() {
//	bShadowInBp = FALSE;
//	_disableBpMap(&bpMapNormal, bp_normal);
//	_disableBpMap(&bpMapHardware, bp_hardware);
//	_disableBpMap(&bpMapMemory, bp_memory);
//}
//
//void _enableBpMap(BPMAP *bpMap) {
//	for (unsigned int i = 0; i < bpMap->count; i++) {
//		if (bpMap->bp[i].enabled) {
//			Script::Debug::SetBreakpoint(bpMap->bp[i].addr);
//		}
//	}
//	memset(bpMap, 0, sizeof(BPMAP));
//}
//
//void enableAllBpMap() {
//	_enableBpMap(&bpMapNormal);
//	_enableBpMap(&bpMapHardware);
//	_enableBpMap(&bpMapMemory);
//}

void CBINITDEBUG(CBTYPE cbType, PLUG_CB_INITDEBUG* info) {
	lpFileName = _strdup(info->szFileName);
	if (bRunOnInit) { // is shadow restart
		dputs("Shadow restart");
		bRunOnInit = FALSE;
	}
	else {
		dputs("Manual restart");
		if (duiShadowAddr != 0) {
			dputs("ShadowBp is remained. Clearing callback and ShadowBp");
			// manual restart (maybe shadowbp doesn't hit) => remove callback + shadowbp
			unregisterBpCallback();
			removeShadowBp();
		}
	}
	
}

void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info) {
	switch (info->hEntry) {
	case MENU_RESTART_HERE:
	case MENU_RUN_HERE:
		if (!DbgIsDebugging()) {
			dputs("Must run in debugging mode");
			break;
		}

		duint duiAddr, duiJunk;
		if (!Script::Gui::Disassembly::SelectionGet(&duiAddr, &duiJunk)) {
			dputs("Can't get current address");
			break;
		}
		duiShadowAddr = duiAddr;
		dprintf("Get shadow address: %x\n", duiShadowAddr);

		// remove bp callback
		_plugin_unregistercallback(pluginHandle, CB_BREAKPOINT);

		//disableAllBpMap();
		if (setShadowBp()) {
			dputs("Set shadow breakpoint");

			// register new callback
			registerBpCallback();
			dputs("Set breakpoint callback");

			// if its not a restart cmd, just Run
			if (info->hEntry == MENU_RESTART_HERE) {
				size_t szCmd = strlen(lpFileName) + 10;
				char* lpCmd = (char*)malloc(szCmd);
				snprintf(lpCmd, szCmd, "init %s", lpFileName);
				bRunOnInit = TRUE; // indicator for shadow restart

				DbgCmdExec(lpCmd);
				dprintf("Trigger command: %s\n", lpCmd);
				
				free(lpCmd);
			}
			else {
				DbgCmdExec("run");
			}
		}
		else {
			dputs("Can't set shadow breakpoint");
		}
		break;
	case MENU_SYSTEM_BREAKPOINT:
		bSystemBp = !bSystemBp;
		_plugin_menuentrysetchecked(pluginHandle, MENU_SYSTEM_BREAKPOINT, bSystemBp);
		if (bSystemBp) {
			dputs("Respect system breakpoint");
		}
		else {
			dputs("Reject system breakpoint too");
		}
		break;
	}
}


bool pluginit(PLUG_INITSTRUCT* initStruct) {
	initStruct->pluginVersion = PLUGIN_VERSION;
	initStruct->sdkVersion = PLUG_SDKVERSION;
	strncpy_s(initStruct->pluginName, PLUGIN_NAME, _TRUNCATE);
	pluginHandle = initStruct->pluginHandle;

	return TRUE;
}

bool plugstop() {
	return TRUE;
}

void plugsetup(PLUG_SETUPSTRUCT* setupStruct) {
	hMenuDisasm = setupStruct->hMenuDisasm;

	_plugin_menuaddentry(hMenuDisasm, MENU_RUN_HERE, "Run to here");
	_plugin_menuentrysethotkey(pluginHandle, MENU_RUN_HERE, "F5");
	_plugin_menuaddentry(hMenuDisasm, MENU_RESTART_HERE, "Restart to here");
	_plugin_menuentrysethotkey(pluginHandle, MENU_RESTART_HERE, "F6");
	_plugin_menuaddseparator(hMenuDisasm);
	_plugin_menuaddentry(hMenuDisasm, MENU_SYSTEM_BREAKPOINT, "Respect System breakpoint");
	_plugin_menuentrysetchecked(pluginHandle, MENU_SYSTEM_BREAKPOINT, bSystemBp);
}