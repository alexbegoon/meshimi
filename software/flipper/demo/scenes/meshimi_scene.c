#include "meshimi_scene.h"

// Generate the scene on_enter handler array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_enter,
void (*const meshimi_on_enter_handlers[])(void*) = {
#include "meshimi_scene_config.h"
};
#undef ADD_SCENE

// Generate the scene on_event handler array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_event,
bool (*const meshimi_on_event_handlers[])(void* context, SceneManagerEvent event) = {
#include "meshimi_scene_config.h"
};
#undef ADD_SCENE

// Generate the scene on_exit handler array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_exit,
void (*const meshimi_on_exit_handlers[])(void* context) = {
#include "meshimi_scene_config.h"
};
#undef ADD_SCENE

// Initialize scene handlers configuration structure
const SceneManagerHandlers meshimi_scene_handlers = {
        .on_enter_handlers = meshimi_on_enter_handlers,
        .on_event_handlers = meshimi_on_event_handlers,
        .on_exit_handlers = meshimi_on_exit_handlers,
        .scene_num = MeshimiSceneNum,
};

