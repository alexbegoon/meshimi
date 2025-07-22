#include "meshimi.h"
#include <furi_hal_serial.h>

// UART Configuration
#define RX_BUF_SIZE 256
#define UART_BAUD_RATE 115200
#define LED_NOTIFICATION_COOLDOWN_MS 600

// UART Structure
struct MeshimiUart {
    Meshimi* app;
    FuriThread* rx_thread;
    FuriStreamBuffer* rx_stream;
    uint8_t rx_buf[RX_BUF_SIZE + 1];
    FuriHalSerialHandle* serial_handle;
    uint32_t last_notification_time;  // Added for rate limiting
};

typedef enum {
    WorkerEvtStop = (1 << 0),
    WorkerEvtRxDone = (1 << 1),
} WorkerEvtFlags;

const NotificationMessage message_green_128 = {
    .type = NotificationMessageTypeLedGreen,
    .data.led.value = 0x80,  // 50% brightness
};

const NotificationMessage message_delay_150 = {
    .type = NotificationMessageTypeDelay,
    .data.delay.length = 150,
};

// LED Notification Sequences
const NotificationSequence sequence_blink_uart_led = {
    &message_green_0,
    &message_blue_255,

    &message_delay_150,

    &message_green_128,
    &message_blue_0,

    &message_delay_150,

    &message_green_0,
    &message_blue_255,

    &message_delay_150,

    &message_green_128,
    &message_blue_0,

    &message_delay_150,

    &message_blue_0,
    &message_green_0,
    NULL,
};

// TODO: Move UART code to a dedicated library or file
// UART IRQ Callback
void meshimi_uart_on_irq_cb(
    FuriHalSerialHandle* handle,
    FuriHalSerialRxEvent event,
    void* context) {

    struct MeshimiUart* uart = (struct MeshimiUart*)context;
    if(event == FuriHalSerialRxEventData) {
        uint8_t data = furi_hal_serial_async_rx(handle);
        furi_stream_buffer_send(uart->rx_stream, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(uart->rx_thread), WorkerEvtRxDone);
    }
}

// UART Worker Thread
static int32_t meshimi_uart_worker(void* context) {
    struct MeshimiUart* uart = (struct MeshimiUart*)context;

    while(1) {
        uint32_t events = furi_thread_flags_wait(
            WorkerEvtStop | WorkerEvtRxDone,
            FuriFlagWaitAny,
            FuriWaitForever
        );

        furi_check((events & FuriFlagError) == 0);

        if(events & WorkerEvtStop) break;

        if(events & WorkerEvtRxDone) {
            size_t len = furi_stream_buffer_receive(uart->rx_stream, uart->rx_buf, RX_BUF_SIZE, 0);
            if(len > 0) {
                // Null terminate the received data
                uart->rx_buf[len] = '\0';

                // Blink LED when data is received, but rate limit to prevent excessive blinking
                if(uart->app && uart->app->notifications) {
                    uint32_t current_time = furi_get_tick();
                    uint32_t time_since_last = current_time - uart->last_notification_time;

                    // Only send notification if enough time has passed since the last one
                    if(time_since_last >= furi_ms_to_ticks(LED_NOTIFICATION_COOLDOWN_MS)) {
                        notification_message(uart->app->notifications, &sequence_blink_uart_led);
                        uart->last_notification_time = current_time;
                    }
                }

                // Log the received data
                FURI_LOG_D("UART", "Received %zu bytes: %s", len, uart->rx_buf);
            }
        }
    }

    furi_stream_buffer_free(uart->rx_stream);
    return 0;
}

// Initialize UART
struct MeshimiUart* meshimi_uart_init(Meshimi* app) {
    struct MeshimiUart* uart = malloc(sizeof(struct MeshimiUart));
    uart->app = app;
    uart->last_notification_time = 0;  // Initialize notification time tracking

    // Initialize stream buffer and thread
    uart->rx_stream = furi_stream_buffer_alloc(RX_BUF_SIZE, 1);
    uart->rx_thread = furi_thread_alloc();
    furi_thread_set_name(uart->rx_thread, "MeshimiUartRxThread");
    furi_thread_set_stack_size(uart->rx_thread, 1024);
    furi_thread_set_context(uart->rx_thread, uart);
    furi_thread_set_callback(uart->rx_thread, meshimi_uart_worker);
    furi_thread_start(uart->rx_thread);

    // Initialize UART hardware
    uart->serial_handle = furi_hal_serial_control_acquire(FuriHalSerialIdUsart);
    furi_check(uart->serial_handle);
    furi_hal_serial_init(uart->serial_handle, UART_BAUD_RATE);
    furi_hal_serial_async_rx_start(uart->serial_handle, meshimi_uart_on_irq_cb, uart, false);

    return uart;
}

// Free UART Resources
void meshimi_uart_free(struct MeshimiUart* uart) {
    furi_assert(uart);

    furi_hal_serial_async_rx_stop(uart->serial_handle);
    furi_hal_serial_deinit(uart->serial_handle);
    furi_hal_serial_control_release(uart->serial_handle);

    furi_thread_flags_set(furi_thread_get_id(uart->rx_thread), WorkerEvtStop);
    furi_thread_join(uart->rx_thread);
    furi_thread_free(uart->rx_thread);

    free(uart);
}

// Send data via UART
void meshimi_uart_tx(struct MeshimiUart* uart, uint8_t* data, size_t len) {
    if(uart && uart->serial_handle) {
        furi_hal_serial_tx(uart->serial_handle, data, len);
    }
}

// Original callback functions remain unchanged
bool meshimi_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    Meshimi* meshimi = context;
    return scene_manager_handle_custom_event(meshimi->scene_manager, event);
}

bool meshimi_back_event_callback(void* context) {
    furi_assert(context);
    Meshimi* meshimi = context;
    return scene_manager_handle_back_event(meshimi->scene_manager);
}

void meshimi_tick_event_callback(void* context) {
    furi_assert(context);
    Meshimi* meshimi = context;
    scene_manager_handle_tick_event(meshimi->scene_manager);
}

Meshimi* meshimi_alloc() {
    Meshimi* meshimi = malloc(sizeof(Meshimi));

    // Meshimi Configuration
    meshimi->config = meshimi_config_alloc();

    // Power
    meshimi->power = furi_record_open(RECORD_POWER);

    // GUI
    meshimi->gui = furi_record_open(RECORD_GUI);

    // View Dispatcher
    meshimi->view_dispatcher = view_dispatcher_alloc();

    meshimi->scene_manager = scene_manager_alloc(&meshimi_scene_handlers, meshimi);
    view_dispatcher_set_event_callback_context(meshimi->view_dispatcher, meshimi);
    view_dispatcher_set_custom_event_callback(
        meshimi->view_dispatcher, meshimi_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        meshimi->view_dispatcher, meshimi_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        meshimi->view_dispatcher, meshimi_tick_event_callback, 500);

    // Open Notification record
    meshimi->notifications = furi_record_open(RECORD_NOTIFICATION);

    // SubMenu
    meshimi->submenu = submenu_alloc();
    view_dispatcher_add_view(
        meshimi->view_dispatcher, MeshimiViewIdMenu, submenu_get_view(meshimi->submenu));

    // Popup
    meshimi->popup = popup_alloc();
    view_dispatcher_add_view(
        meshimi->view_dispatcher, MeshimiViewIdPopup, popup_get_view(meshimi->popup));

    // Text Input
    meshimi->text_input = text_input_alloc();
    view_dispatcher_add_view(
        meshimi->view_dispatcher,
        MeshimiViewIdTextInput,
        text_input_get_view(meshimi->text_input));

    // Custom Widget
    meshimi->widget = widget_alloc();
    view_dispatcher_add_view(
        meshimi->view_dispatcher, MeshimiViewIdWidget, widget_get_view(meshimi->widget));

    // Dialog
    meshimi->dialogs = furi_record_open(RECORD_DIALOGS);

    // Views
    meshimi->meshimi_view_mode = meshimi_view_mode_alloc(meshimi->config);
    view_dispatcher_add_view(
        meshimi->view_dispatcher,
        MeshimiViewIdMode,
        meshimi_view_mode_get_view(meshimi->meshimi_view_mode));

    // Variable Item List
    meshimi->variable_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        meshimi->view_dispatcher,
        MeshimiViewIdVariableItemList,
        variable_item_list_get_view(meshimi->variable_item_list));

    meshimi->meshtastic_scene = 0;
    meshimi->semaphore = furi_semaphore_alloc(1, 0);

    // Initialize UART
    meshimi->uart = meshimi_uart_init(meshimi);

    return meshimi;
}

void meshimi_free(Meshimi* meshimi) {
    furi_assert(meshimi);

    // Free UART first
    if(meshimi->uart) {
        meshimi_uart_free(meshimi->uart);
    }

    // TextInput
    view_dispatcher_remove_view(meshimi->view_dispatcher, MeshimiViewIdTextInput);
    text_input_free(meshimi->text_input);

    // Custom Widget
    view_dispatcher_remove_view(meshimi->view_dispatcher, MeshimiViewIdWidget);
    widget_free(meshimi->widget);

    // Dialog
    furi_record_close(RECORD_DIALOGS);

    // Variable Item List
    view_dispatcher_remove_view(meshimi->view_dispatcher, MeshimiViewIdVariableItemList);
    variable_item_list_free(meshimi->variable_item_list);

    // Submenu
    view_dispatcher_remove_view(meshimi->view_dispatcher, MeshimiViewIdMenu);
    submenu_free(meshimi->submenu);

    // Popup
    view_dispatcher_remove_view(meshimi->view_dispatcher, MeshimiViewIdPopup);
    popup_free(meshimi->popup);

    // Views
    view_dispatcher_remove_view(meshimi->view_dispatcher, MeshimiViewIdMode);
    meshimi_view_mode_free(meshimi->meshimi_view_mode);

    // Scene manager
    scene_manager_free(meshimi->scene_manager);

    // View Dispatcher
    view_dispatcher_free(meshimi->view_dispatcher);

    // GUI
    furi_record_close(RECORD_GUI);
    meshimi->gui = NULL;

    // Notifications
    furi_record_close(RECORD_NOTIFICATION);
    meshimi->notifications = NULL;

    // Power
    furi_record_close(RECORD_POWER);

    // Meshimi Configuration
    meshimi_config_free(meshimi->config);

    furi_semaphore_free(meshimi->semaphore);

    // The rest
    free(meshimi);
}

/**
 * @brief      Main function for Meshimi application.
 * @details    This function is the entry point for the Meshimi application.
 * @param p
 * @return 0 - Success
 */
int32_t meshimi_app(void* p) {
    UNUSED(p);
    FURI_LOG_I(LOG_TAG, "Start app");
    furi_log_set_level(FuriLogLevelDebug);

    Meshimi* meshimi = meshimi_alloc();

#ifdef BACKLIGHT_ALWAYS_ON
    notification_message(meshimi->notifications, &sequence_display_backlight_enforce_on);
#endif

    view_dispatcher_attach_to_gui(
        meshimi->view_dispatcher, meshimi->gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(meshimi->scene_manager, MeshimiSceneStart);

    furi_hal_power_suppress_charge_enter();

    // Initial LED indication that UART is active
    notification_message(meshimi->notifications, &sequence_blink_green_100);

    // Enable OTG power for UART
    power_enable_otg(meshimi->power, true);

    view_dispatcher_run(meshimi->view_dispatcher);

    // Final LED blink
    notification_message(meshimi->notifications, &sequence_blink_green_100);

    furi_hal_power_suppress_charge_exit();

#ifdef BACKLIGHT_ALWAYS_ON
    notification_message(meshimi->notifications, &sequence_display_backlight_enforce_auto);
#endif

    power_enable_otg(meshimi->power, false);

    meshimi_free(meshimi);

    return 0;
}