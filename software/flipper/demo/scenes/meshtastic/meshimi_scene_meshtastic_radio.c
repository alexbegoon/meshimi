#include "../../meshimi.h"
#include "meshimi_scene_meshtastic.h"

void meshimi_scene_meshtastic_radio_on_enter(void* context) {
    furi_assert(context);
    Meshimi* meshimi = context;

    size_t total_scenes;
    get_meshtastic_scene_map(&total_scenes);

    widget_add_string_element(meshimi->widget, 0, 0, AlignLeft, AlignTop, FontPrimary, "CH");
    widget_add_text_scroll_element(meshimi->widget, 0, 17, 128, 13, "RSSI: -102dBm\nSNR: 7.5 dB\nChannel: #LongFast\nNodes: 43 active");
    if (meshimi->meshtastic_scene > 0) {
        widget_add_button_element(
            meshimi->widget, GuiButtonTypeLeft, "", button_callback, meshimi);
    }
    if (meshimi->meshtastic_scene < total_scenes - 1) {
        widget_add_button_element(
            meshimi->widget, GuiButtonTypeRight, "", button_callback, meshimi);
    }
}

bool meshimi_scene_meshtastic_radio_on_event(void* context, SceneManagerEvent event) {
    Meshimi* meshimi = context;

    if(event.type == SceneManagerEventTypeBack) {
        return scene_manager_previous_scene(meshimi->scene_manager);
    }
    else if(event.type == SceneManagerEventTypeCustom) {
        return handle_navigation_event(meshimi, event.event);
    }

    return false;
}

void meshimi_scene_meshtastic_radio_on_exit(void* context) {
    Meshimi* meshimi = context;
    widget_reset(meshimi->widget);
}