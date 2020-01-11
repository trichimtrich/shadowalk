#pragma once

#include "pluginsdk/bridgemain.h"
#include "pluginsdk/_plugins.h"

#include "pluginsdk/_scriptapi_debug.h"
#include "pluginsdk/_scriptapi_gui.h"

#ifdef _WIN64
#pragma comment(lib, "pluginsdk/x64dbg.lib")
#pragma comment(lib, "pluginsdk/x64bridge.lib")
#else
#pragma comment(lib, "pluginsdk/x32dbg.lib")
#pragma comment(lib, "pluginsdk/x32bridge.lib")
#endif //_WIN64

//plugin data
#define PLUGIN_NAME "Shadowalk"
#define PLUGIN_VERSION 1

//marcos
#define dprintf(x, ...) _plugin_logprintf("[" PLUGIN_NAME "] " x, __VA_ARGS__)
#define dputs(x) _plugin_logputs("[" PLUGIN_NAME "] " x)
#define PLUG_EXPORT extern "C" __declspec(dllexport)

//export functions

PLUG_EXPORT bool pluginit(PLUG_INITSTRUCT*);
PLUG_EXPORT bool plugstop();
PLUG_EXPORT void plugsetup(PLUG_SETUPSTRUCT*);

PLUG_EXPORT void CBMENUENTRY(CBTYPE, PLUG_CB_MENUENTRY*);
PLUG_EXPORT void CBINITDEBUG(CBTYPE, PLUG_CB_INITDEBUG*);

//internal functions

void cbBreakpoint(CBTYPE, void*);
bool removeShadowBp();
bool setShadowBp();

void registerBpCallback();
void unregisterBpCallback();