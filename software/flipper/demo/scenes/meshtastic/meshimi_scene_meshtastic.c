#include "../../meshimi.h"

static const uint8_t scene_map[] = {
    MeshimiSceneMeshtasticStatus, // Scene 0
    MeshimiSceneMeshtasticNetwork, // Scene 1
    MeshimiSceneMeshtasticMessages, // Scene 2
    MeshimiSceneMeshtasticRadio, // Scene 3
};

const uint8_t* get_meshtastic_scene_map(size_t* total_scenes) {
    *total_scenes = sizeof(scene_map) / sizeof(scene_map[0]);
    return scene_map;
}

void button_callback(GuiButtonType button, InputType type, void* context) {
    furi_assert(context);
    Meshimi* meshimi = context;
    if(type == InputTypeShort) {
        scene_manager_handle_custom_event(meshimi->scene_manager, button);
    }
}

bool handle_navigation_event(Meshimi* meshimi, GuiButtonType button_type) {
    furi_assert(meshimi);
    size_t total_scenes;
    get_meshtastic_scene_map(&total_scenes);

    FURI_LOG_I("Meshtastic", "handle_navigation_event: %i", button_type);



    if(button_type == GuiButtonTypeLeft) {
        meshimi->meshtastic_scene = (meshimi->meshtastic_scene + total_scenes - 1) % total_scenes;
        FURI_LOG_I("Meshtastic", "Scene changed: %d", meshimi->meshtastic_scene);
        scene_manager_previous_scene(meshimi->scene_manager);
        return true;
    } else if(button_type == GuiButtonTypeRight) {
        meshimi->meshtastic_scene = (meshimi->meshtastic_scene + 1) % total_scenes;
        FURI_LOG_I("Meshtastic", "Scene changed: %d", meshimi->meshtastic_scene);
        scene_manager_next_scene(meshimi->scene_manager, scene_map[meshimi->meshtastic_scene]);
        return true;
    }
    return false;
}
