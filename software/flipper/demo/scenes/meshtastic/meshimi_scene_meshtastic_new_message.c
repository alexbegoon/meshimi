#include "../../meshimi.h"
#include "../../meshimi_custom_event.h"
#include <stdbool.h>
#include <string.h>

void meshimi_scene_meshtastic_new_message_popup_callback(void* context) {
    furi_assert(context);
    Meshimi* meshimi = context;
    scene_manager_next_scene(meshimi->scene_manager, MeshimiSceneMeshtasticMessages);
}

void meshimi_scene_meshtastic_new_message_input_callback(void* context) {
    furi_assert(context);
    Meshimi* meshimi = context;
    view_dispatcher_send_custom_event(meshimi->view_dispatcher, MeshimiEventMeshtasticNewMessage);
}

bool validator_message_callback(const char* text, FuriString* error, void* context) {
    UNUSED(context);

    size_t len = strlen(text);
    if(len == 0) {
        furi_string_printf(error, "Message\ncannot be\nempty");
        return false;
    }

    if(len > MESHTASTIC_MAX_MESSAGE_TEXT_LEN) {
        furi_string_printf(error, "Message too\nlong (max\n230 chars)");
        return false;
    }

    return true;
}

void meshimi_scene_meshtastic_new_message_on_enter(void* context) {
    furi_assert(context);
    Meshimi* meshimi = context;

    text_input_set_header_text(meshimi->text_input, "New Message:");
    text_input_set_result_callback(
        meshimi->text_input,
        meshimi_scene_meshtastic_new_message_input_callback,
        meshimi,
        meshimi->text_store,
        MESHTASTIC_MAX_MESSAGE_TEXT_LEN,
        true);

    text_input_set_validator(meshimi->text_input, validator_message_callback, meshimi);

    view_dispatcher_switch_to_view(meshimi->view_dispatcher, MeshimiViewIdTextInput);
}

bool meshimi_scene_meshtastic_new_message_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);
    Meshimi* meshimi = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == MeshimiEventMeshtasticNewMessage) {
            // Here you would handle sending the message via Meshtastic
            // meshimi->text_store contains the message text
            if (strlen(meshimi->text_store) > 0) {
                // Send the message using the Meshtastic API
                // For example: meshtastic_send_message(meshimi->bt, meshimi->text_store);

                popup_set_text(meshimi->popup, "Message\nsent", 95, 32, AlignCenter, AlignCenter);
                popup_set_timeout(meshimi->popup, 3000);
                popup_set_icon(meshimi->popup, 0, 3, &I_DolphinDone_80x58);
                popup_set_callback(meshimi->popup, meshimi_scene_meshtastic_new_message_popup_callback);
                popup_set_context(meshimi->popup, meshimi);
                popup_enable_timeout(meshimi->popup);

                view_dispatcher_switch_to_view(meshimi->view_dispatcher, MeshimiViewIdPopup);

                FURI_LOG_I("Meshtastic", "Sending message: %s", meshimi->text_store);
            } else {
                FURI_LOG_E("Meshtastic", "Message is empty, not sending.");
            }

            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_next_scene(meshimi->scene_manager, MeshimiSceneMeshtasticMessages);
        consumed = true;
    }

    return consumed;
}

void meshimi_scene_meshtastic_new_message_on_exit(void* context) {
    Meshimi* meshimi = context;

    popup_reset(meshimi->popup);
    text_input_set_validator(meshimi->text_input, NULL, NULL);
    text_input_reset(meshimi->text_input);
}