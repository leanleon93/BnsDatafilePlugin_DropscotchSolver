#include "BnsDatafilePlugin.h"
#include <thread>
#include "GlobalState.h"
#include "UiPanel.h"
#include <DatafilePluginsdk.h>
#include <imgui_plugin_api.h>
#include <pluginversion.h>
#include "pluginhelpers.h"
#include <EU/skillshow3/AAA_skillshow3_RecordBase.h>
#include <KR/skillshow3/AAA_skillshow3_RecordBase.h>
#include <algorithm>
#include <vector>

/**
 * @file BnsDatafilePlugin.cpp
 * @brief Template implementation for a datafile plugin using the BnsDatafilePlugin SDK.
 * @author LEaN
 *
 * This file provides a starting point for plugin creators, including example hooks,
 * ImGui panel registration, and plugin metadata definitions.
 */

using namespace PluginGlobalState;

//dropscotch correct plate ids
static const std::vector<int> skillShow3Ids = {
	5924645,5924646,5924647,5924648,5924649,5924650,5924651,5924652,5924653,5924654,5924655,5924656,5924657,5924658
};

static PluginReturnData __fastcall Skillshow3Detour(PluginExecuteParams* params) {
	PLUGIN_DETOUR_GUARD_REGION(params);

	if (!g_pluginConfig.configData.enabled || !g_isReady.load(std::memory_order_acquire) || params->key == 0) {
		return {};
	}
#ifdef BNSKR
	BnsTables::KR::skillshow3_Record::Key key = {};
#else
	BnsTables::EU::skillshow3_Record::Key key = {};
#endif
	key.key = params->key;
	auto skillId = key.id;

	//show bard shield on the correct glass panels
	if (std::find(skillShow3Ids.begin(), skillShow3Ids.end(), skillId) != skillShow3Ids.end()) {
		key.id = 241040;
		key.variation_id = 1;
		key.skillskin_id = 0;
		auto record = params->oFind(params->table, key.key);
		if (record != nullptr) {
			return { record };
		}
	}

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
	{ L"skillshow3", &Skillshow3Detour }
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