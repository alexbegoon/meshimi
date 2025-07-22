#pragma once

#include <gui/scene_manager.h>

// Generate the scene id and total number
#define ADD_SCENE(prefix, name, id) MeshimiScene##id,
typedef enum {
#include "meshimi_scene_config.h"

    MeshimiSceneNum,
} MeshimiScene;
#undef ADD_SCENE

extern const SceneManagerHandlers meshimi_scene_handlers;

// Generate the scene on_enter handler declaration
#define ADD_SCENE(prefix, name, id) void prefix##_scene_##name##_on_enter(void*);
#include "meshimi_scene_config.h"
#undef ADD_SCENE

// Generate the scene on_event handler declaration
#define ADD_SCENE(prefix, name, id) \
    bool prefix##_scene_##name##_on_event(void* context, SceneManagerEvent event);
#include "meshimi_scene_config.h"
#undef ADD_SCENE

// Generate the scene on_exit handler declaration
#define ADD_SCENE(prefix, name, id) void prefix##_scene_##name##_on_exit(void* context);
#include "meshimi_scene_config.h"
#undef ADD_SCENE
