#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../meshimi_scene.h"

const uint8_t* get_meshtastic_scene_map(size_t* total_scenes);

bool handle_navigation_event(Meshimi* meshimi, GuiButtonType button_type);

void button_callback(GuiButtonType button, InputType type, void* context);