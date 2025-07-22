#pragma once

#include "meshimi_types.h"
#include "applications_user/meshimi/scenes/meshimi_scene.h"
#include "helpers/meshimi_config.h"
#include "bt/bt_service/bt.h"
#include "views/meshimi_view_mode.h"
#include "power/power_service/power.h"

#include <assets_icons.h>
#include <dialogs/dialogs.h>
#include <gui/gui.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/popup.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include <notification/notification_messages.h>
#include <furi_hal_power.h>
#include <furi_hal.h>

#define MESHIMI_VERSION "1.0"
#define MESHIMI_GITHUB "github.com/BegoonLab/meshimi"
#define MESHIMI_GET_MODULE "begoonlab.tech/meshimi"
#define BACKLIGHT_ALWAYS_ON
#define LOG_TAG "Meshimi"
#define TEXT_STORE_SIZE 250U
#define MESHTASTIC_MAX_MESSAGE_TEXT_LEN 230U

typedef struct Meshimi Meshimi;

struct Meshimi {
    MeshimiConfig* config;
    Power* power;

    Gui* gui;
    NotificationApp* notifications;

    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;

    char text_store[TEXT_STORE_SIZE];
    Submenu* submenu;
    Popup* popup;
    TextInput* text_input;
    Widget* widget;
    DialogsApp* dialogs;
    VariableItemList* variable_item_list;
    MeshimiViewMode* meshimi_view_mode;
    FuriTimer* timer;
    FuriSemaphore *semaphore;

    struct MeshimiUart* uart;

    uint8_t meshtastic_scene;
};

Meshimi* meshimi_alloc();
void meshimi_free(Meshimi* meshimi);