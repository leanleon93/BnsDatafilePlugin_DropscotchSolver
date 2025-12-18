#include "BnsDatafilePlugin.h"
#include <thread>
#include "GlobalState.h"
#include "UiPanel.h"
#include <DatafilePluginsdk.h>
#include <imgui_plugin_api.h>
#include <pluginversion.h>
#include "pluginhelpers.h"

/**
 * @file BnsDatafilePlugin.cpp
 * @brief Template implementation for a datafile plugin using the BnsDatafilePlugin SDK.
 * @author LEaN
 *
 * This file provides a starting point for plugin creators, including example hooks,
 * ImGui panel registration, and plugin metadata definitions.
 */

using namespace PluginGlobalState;



/**
 * @brief Example detour function for the "item" table.
 *
 * This function demonstrates how to intercept lookups for items in the datafile.
 * Plugin authors can modify or redirect lookups as needed.
 *
 * @param params Parameters for the plugin execution, including the table and key.
 * @return PluginReturnData Optionally returns a replacement element. Only return if you absolutely have to. Prefer modifying the element in place.
 */
static PluginReturnData __fastcall DatafileItemDetour(PluginExecuteParams* params) {
	PLUGIN_DETOUR_GUARD_REGION(params);

	if (!g_pluginConfig.configData.enabled || !g_isReady.load(std::memory_order_acquire)) {
		return {};
	}
	unsigned __int64 key = params->key;

	// Example to replace one item with another when the game looks it up

	//if (key == 4295877296) {
	//	BnsTables::Shared::DrEl* result = params->oFind(params->table, 4295917336);
	//	//params->displaySystemChatMessage(L"ExampleItemPlugin: Redirected item key 4295902840 to 4294967396", false);
	//	return { result };
	//}

	return {};
}

/**
 * @brief Asynchronous setup function.
 *
 * This function performs any necessary setup operations asynchronously.
 * It sets the `isReady` flag to indicate that the plugin is ready for use.
 * If you need any asynchronous data setup, do it here. This runs in a separate thread and does not block the game.
 */
static void AsyncSetup() {
	g_isReady.store(true, std::memory_order_release); //Indicate that setup is complete
}

/**
 * @brief Initializes the plugin and registers the ImGui panel.
 *
 * Called by the host application when the plugin is loaded.
 *
 * @param params Initialization parameters provided by the host.
 */
static void __fastcall Init(PluginInitParams* params) {
	g_pluginConfig.Load();

	if (params && params->registerImGuiPanel && params->unregisterImGuiPanel && params->imgui)
	{
		g_imgui = params->imgui;
		g_registerUiPanel = params->registerImGuiPanel;
		g_unregisterUiPanel = params->unregisterImGuiPanel;
		ImGuiPanelDesc desc = { PLUGIN_NAME, ConfigUiPanel, nullptr };
		g_mainPanelHandle = g_registerUiPanel(&desc, false);
	}
	if (params && params->dataManager && params->oFind) {
		g_dataManager = params->dataManager;
		g_oFind = params->oFind;

		// Asynchronous setup
		std::thread([]() { AsyncSetup(); }).detach();
	}
}

/**
 * @brief Unregisters the ImGui panel and performs cleanup.
 *
 * Called by the host application when the plugin is unloaded.
 */
static void __fastcall Unregister() {
	if (g_unregisterUiPanel && g_mainPanelHandle != 0) {
		g_unregisterUiPanel(g_mainPanelHandle);
		g_mainPanelHandle = 0;
	}
	g_pluginConfig.Save();
}

/// @brief Table handler array mapping table names to detour functions.
PluginTableHandler handlers[] = {
	{ L"item", &DatafileItemDetour }
};

// Plugin metadata and registration macros
DEFINE_PLUGIN_API_VERSION()
DEFINE_PLUGIN_IDENTIFIER(PLUGIN_NAME)
DEFINE_PLUGIN_VERSION(PLUGIN_VERSION)
DEFINE_PLUGIN_INIT(Init, Unregister)
DEFINE_PLUGIN_TABLE_HANDLERS(handlers)



//! Example to get a data record
//auto itemRecord = GetRecord<BnsTables::EU::item_Record>(g_dataManager, L"item", 4295917336, g_oFind);

//! Example to iterate data records
//ForEachRecord<BnsTables::EU::item_Record>(g_dataManager, L"item", [](BnsTables::EU::item_Record* record, size_t index) {
//	// Do something with each item record
//	std::wcout << L"Item Name: " << record->alias << std::endl;
//	return true; //Return true to continue the loop. Return false to break the loop.
//	});

//! Log Example: Don't spam the log.
//Logger::Log(std::string(PLUGIN_NAME) + " (v" + std::string(PLUGIN_VERSION) + ") async setup complete.");