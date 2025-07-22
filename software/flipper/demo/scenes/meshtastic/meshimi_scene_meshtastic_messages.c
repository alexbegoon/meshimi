#include "../../meshimi.h"
#include "meshimi_scene_meshtastic.h"
#include "meshimi_custom_event.h"
#include <furi.h>

#define TAG "MeshimiMessages"

static void meshimi_delayed_popup_timer_callback(void* context) {
    furi_assert(context);
    Meshimi* meshimi = context;

    // Send custom event instead of directly switching views
    scene_manager_handle_custom_event(
        meshimi->scene_manager, MeshimiEventMeshtasticNewMessageReceived);
    furi_timer_stop(meshimi->timer);
    furi_timer_free(meshimi->timer);
    meshimi->timer = NULL;
}

void meshimi_scene_meshtastic_messages_on_enter(void* context) {
    Meshimi* meshimi = context;

    FURI_LOG_I("Meshtastic", "Messages scene entered");

    size_t total_scenes;
    get_meshtastic_scene_map(&total_scenes);

    widget_add_string_element(meshimi->widget, 0, 0, AlignLeft, AlignTop, FontPrimary, "MSG");
    widget_add_text_scroll_element(
        meshimi->widget,
        0,
        17,
        128,
        13,
        "b768: Hello mesh! How ar...\nd5bc: Hi, do you read me?\nb2ac: test\n");
    if(meshimi->meshtastic_scene > 0) {
        widget_add_button_element(
            meshimi->widget, GuiButtonTypeLeft, "", button_callback, meshimi);
    }
    if(meshimi->meshtastic_scene < total_scenes - 1) {
        widget_add_button_element(
            meshimi->widget, GuiButtonTypeRight, "", button_callback, meshimi);
    }
    // Add a button to send a new message
    widget_add_button_element(
        meshimi->widget, GuiButtonTypeCenter, "+MSG", button_callback, meshimi);

    if(meshimi->text_store[0] == 'Q') {
        // Create and start timer to show popup after 10 seconds
        if(!meshimi->timer) {
            meshimi->timer =
                furi_timer_alloc(meshimi_delayed_popup_timer_callback, FuriTimerTypeOnce, meshimi);
            furi_timer_start(
                meshimi->timer, furi_ms_to_ticks(10000)); // 10 seconds in milliseconds
        }
    }

    view_dispatcher_switch_to_view(meshimi->view_dispatcher, MeshimiViewIdWidget);
}

bool meshimi_scene_meshtastic_messages_on_event(void* context, SceneManagerEvent event) {
    Meshimi* meshimi = context;

    if(event.type == SceneManagerEventTypeBack) {
        return scene_manager_previous_scene(meshimi->scene_manager);
    } else if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeCenter) {
            FURI_LOG_I("Meshtastic", "Center button pressed");
            // Handle the new message button press
            scene_manager_next_scene(meshimi->scene_manager, MeshimiSceneMeshtasticNewMessage);
            return true;
        } else if(event.event == MeshimiEventMeshtasticNewMessageReceived) {
            scene_manager_next_scene(
                meshimi->scene_manager, MeshimiSceneMeshtasticNewMessageReceived);

            return true;
        }

        return handle_navigation_event(meshimi, event.event);
    }

    return false;
}

void meshimi_scene_meshtastic_messages_on_exit(void* context) {
    Meshimi* meshimi = context;

    widget_reset(meshimi->widget);
}
