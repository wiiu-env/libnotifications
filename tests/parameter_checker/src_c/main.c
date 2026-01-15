#include <coreinit/thread.h>
#include <notifications/notifications.h>

// Dummy callback for C
void my_c_callback(NotificationModuleHandle h, void* ctx) {
    (void)h;
    (void)ctx;
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    NotificationModule_InitLibrary();

    // Setup variables
    NMColor color = {255, 0, 0, 255};
    float duration = 3.0f;
    int ctx_data = 100;

    // Test 1: Background Color
    // C11+ uses _Generic to check types here.
    NotificationModule_SetDefaultValue(
        NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO,
        NOTIFICATION_MODULE_DEFAULT_OPTION_BACKGROUND_COLOR,
        color
    );

    // Test 2: Duration
    NotificationModule_SetDefaultValue(
        NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO,
        NOTIFICATION_MODULE_DEFAULT_OPTION_DURATION_BEFORE_FADE_OUT,
        duration
    );

    // Test 3: Callback
    NotificationModule_SetDefaultValue(
        NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO,
        NOTIFICATION_MODULE_DEFAULT_OPTION_FINISH_FUNCTION,
        my_c_callback
    );

    // Test 4: Context
    NotificationModule_SetDefaultValue(
        NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO,
        NOTIFICATION_MODULE_DEFAULT_OPTION_FINISH_FUNCTION_CONTEXT,
        (void*) &ctx_data
    );

    NotificationModule_AddInfoNotification("C Compatibility Test");

    NotificationModule_DeInitLibrary();
    return 0;
}