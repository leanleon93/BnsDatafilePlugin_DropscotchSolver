#pragma once
#include <atomic>
#include "plugin_config.h" // Include the PluginConfig header
#include <Data.h>
#include <imgui_plugin_api.h>
#include <DrEl.h>

// Namespace to encapsulate global variables
namespace PluginGlobalState {
	inline int g_mainPanelHandle = 0;
	inline RegisterImGuiPanelFn g_registerUiPanel = nullptr;
	inline UnregisterImGuiPanelFn g_unregisterUiPanel = nullptr;
	inline PluginImGuiAPI* g_imgui = nullptr;

	inline Data::DataManager* g_dataManager = nullptr;
	inline DrEl* (__fastcall* g_oFind)(DrMultiKeyTable* thisptr, unsigned __int64 key) = nullptr;
	inline std::atomic<bool> g_isReady = false;

	inline PluginConfig g_pluginConfig = {};
}
