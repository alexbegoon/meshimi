#include "../../meshimi.h"
#include "meshimi_icons.h"
#include "meshimi_scene_meshtastic.h"

void meshimi_scene_meshtastic_status_on_enter(void* context) {
    furi_assert(context);
    Meshimi* meshimi = context;

    size_t total_scenes;
    get_meshtastic_scene_map(&total_scenes);

    widget_add_icon_element(meshimi->widget, 35, 14, &I_Meshtastic_Logo_58x32);
    widget_add_string_element(meshimi->widget, 0, 0, AlignLeft, AlignTop, FontSecondary, "EU_868");
    widget_add_string_element(
        meshimi->widget, 128, 0, AlignRight, AlignTop, FontSecondary, "2.6.10");
    widget_add_string_element(
        meshimi->widget, 27, 54, AlignLeft, AlignTop, FontPrimary, "meshtastic.org");
    if(meshimi->meshtastic_scene > 0) {
        widget_add_button_element(
            meshimi->widget, GuiButtonTypeLeft, "", button_callback, meshimi);
    }
    if(meshimi->meshtastic_scene < total_scenes - 1) {
        widget_add_button_element(
            meshimi->widget, GuiButtonTypeRight, "", button_callback, meshimi);
    }

    view_dispatcher_switch_to_view(meshimi->view_dispatcher, MeshimiViewIdWidget);
}

bool meshimi_scene_meshtastic_status_on_event(void* context, SceneManagerEvent event) {
    Meshimi* meshimi = context;

    if(event.type == SceneManagerEventTypeBack) {
        scene_manager_next_scene(meshimi->scene_manager, MeshimiSceneStart);
        return true;
    }

    else if(event.type == SceneManagerEventTypeCustom) {
        return handle_navigation_event(meshimi, event.event);
    }

    return false;
}

void meshimi_scene_meshtastic_status_on_exit(void* context) {
    Meshimi* meshimi = context;
    widget_reset(meshimi->widget);
}
