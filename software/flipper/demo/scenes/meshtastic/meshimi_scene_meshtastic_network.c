#include "../../meshimi.h"
#include "meshimi_scene_meshtastic.h"

void meshimi_scene_meshtastic_network_on_enter(void* context) {
    furi_assert(context);
    Meshimi* meshimi = context;

    size_t total_scenes;
    get_meshtastic_scene_map(&total_scenes);

    widget_add_string_element(meshimi->widget, 0, 0, AlignLeft, AlignTop, FontPrimary, "NET");
    widget_add_text_scroll_element(meshimi->widget, 0, 17, 128, 13, "TX: 31643\nRX: 76024\nAirUtilTX: 1.18%\nUptime: 9d 14h 3m 11s");
    
    if (meshimi->meshtastic_scene > 0) {
        widget_add_button_element(
            meshimi->widget, GuiButtonTypeLeft, "", button_callback, meshimi);
    }
    
    if (meshimi->meshtastic_scene < total_scenes - 1) {
        widget_add_button_element(
            meshimi->widget, GuiButtonTypeRight, "", button_callback, meshimi);
    }

    view_dispatcher_switch_to_view(meshimi->view_dispatcher, MeshimiViewIdWidget);
}

bool meshimi_scene_meshtastic_network_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);
    Meshimi* meshimi = context;

    if(event.type == SceneManagerEventTypeBack) {
        return scene_manager_previous_scene(meshimi->scene_manager);
    }
    else if(event.type == SceneManagerEventTypeCustom) {
        return handle_navigation_event(meshimi, event.event);
    }

    return false;
}

void meshimi_scene_meshtastic_network_on_exit(void* context) {
    Meshimi* meshimi = context;
    widget_reset(meshimi->widget);
}