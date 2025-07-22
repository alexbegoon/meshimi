#include "../meshimi.h"
#include "meshimi_icons.h"

void meshimi_scene_connect_on_enter(void* context) {
    Meshimi* meshimi = context;
    MeshimiConfigMode current_mode = meshimi_mode_get(meshimi->config);

    widget_reset(meshimi->widget);

    switch(current_mode) {
        case ModeMeshtastic:
            meshimi->meshtastic_scene = 0;
            scene_manager_next_scene(meshimi->scene_manager, MeshimiSceneMeshtasticStatus);
            break;
        default:
            widget_add_icon_element(meshimi->widget, 0, 0, &I_Meshimi_128x64);
            break;
    }

    view_dispatcher_switch_to_view(meshimi->view_dispatcher, MeshimiViewIdWidget);
}

bool meshimi_scene_connect_on_event(void* context, SceneManagerEvent event) {
    Meshimi* meshimi = context;

    if(event.type == SceneManagerEventTypeBack) {
        return scene_manager_previous_scene(meshimi->scene_manager);
    }

    return false;
}

void meshimi_scene_connect_on_exit(void* context) {
    Meshimi* meshimi = context;
    widget_reset(meshimi->widget);
}