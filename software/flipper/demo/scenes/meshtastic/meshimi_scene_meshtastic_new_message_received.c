#include "../../meshimi.h"
#include "meshimi_scene_meshtastic.h"
#include <stdbool.h>
#include <furi.h>

#define TAG "MeshimiNewMsg"


const NotificationSequence sequence_new_message = {
    &message_display_backlight_enforce_auto,
    &message_force_speaker_volume_setting_1f,
    &message_force_vibro_setting_on,
    &message_force_display_brightness_setting_1f,
    &message_vibro_on,

    &message_display_backlight_on,
    &message_note_c7,
    &message_green_255,
    &message_blue_255,
    &message_delay_250,
    &message_red_0,
    &message_blue_0,
    &message_green_0,

    &message_display_backlight_off,
    &message_note_c4,
    &message_red_255,
    &message_green_255,
    &message_delay_250,
    &message_red_0,
    &message_blue_0,
    &message_green_0,

    &message_display_backlight_on,
    &message_note_c7,
    &message_red_255,
    &message_blue_255,
    &message_delay_250,
    &message_red_0,
    &message_blue_0,
    &message_green_0,

    &message_display_backlight_off,
    &message_note_c4,
    &message_blue_255,
    &message_delay_250,
    &message_red_0,
    &message_blue_0,
    &message_green_0,

    &message_display_backlight_on,
    &message_note_c7,
    &message_red_255,
    &message_green_255,
    &message_delay_250,
    &message_red_0,
    &message_blue_0,
    &message_green_0,

    &message_display_backlight_off,
    &message_note_c4,
    &message_green_255,
    &message_blue_255,
    &message_delay_250,
    &message_red_0,
    &message_blue_0,
    &message_green_0,

    &message_sound_off,
    &message_vibro_off,
    NULL,
};

void meshimi_scene_meshtastic_new_message_received_on_enter(void* context) {
    furi_assert(context);
    Meshimi* meshimi = context;

    if (meshimi->text_store[0] == 'Q') {
        widget_add_string_element(
            meshimi->widget, 0, 0, AlignLeft, AlignTop, FontPrimary, "New Message");
        widget_add_string_element(
            meshimi->widget, 0, 20, AlignLeft, AlignTop, FontSecondary, "From 4b7d: Hello stranger <3");

        notification_message(meshimi->notifications, &sequence_new_message);

#ifdef BACKLIGHT_ALWAYS_ON
        notification_message(meshimi->notifications, &sequence_display_backlight_enforce_on);
#endif

        memset(meshimi->text_store, 0, TEXT_STORE_SIZE);
    } else {
        scene_manager_previous_scene(meshimi->scene_manager);
    }

    view_dispatcher_switch_to_view(meshimi->view_dispatcher, MeshimiViewIdWidget);
}

bool meshimi_scene_meshtastic_new_message_received_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);
    Meshimi* meshimi = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeBack) {
        scene_manager_next_scene(meshimi->scene_manager, MeshimiSceneMeshtasticMessages);
        consumed = true;
    }

    return consumed;
}

void meshimi_scene_meshtastic_new_message_received_on_exit(void* context) {
    Meshimi* meshimi = context;

    widget_reset(meshimi->widget);
}
