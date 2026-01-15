#include <coreinit/thread.h>
#include <notifications/notifications.h>

// Dummy callback for testing
void my_callback(NotificationModuleHandle h, void* ctx) {
    (void)h;
    (void)ctx;
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    // 1. Initialize the library
    NotificationModule_InitLibrary();

    // 2. Test Type Checks (Valid Cases)
    // These should compile successfully. If the type checks are broken, 
    // -Werror will cause the build to fail here.
    
    NMColor color = {255, 255, 255, 255};
    float duration = 5.0f;
    bool keep = true;
    int ctx_data = 1;

    // Background Color
    NotificationModule_SetDefaultValue(
        NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO,
        NOTIFICATION_MODULE_DEFAULT_OPTION_BACKGROUND_COLOR,
        color
    );

    // Duration
    NotificationModule_SetDefaultValue(
        NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO,
        NOTIFICATION_MODULE_DEFAULT_OPTION_DURATION_BEFORE_FADE_OUT,
        duration
    );

    // Callback
    NotificationModule_SetDefaultValue(
        NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO,
        NOTIFICATION_MODULE_DEFAULT_OPTION_FINISH_FUNCTION,
        my_callback
    );

    // Context
    NotificationModule_SetDefaultValue(
        NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO,
        NOTIFICATION_MODULE_DEFAULT_OPTION_FINISH_FUNCTION_CONTEXT,
        (void*)&ctx_data
    );

    // Keep
    NotificationModule_SetDefaultValue(
        NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO,
        NOTIFICATION_MODULE_DEFAULT_OPTION_KEEP_UNTIL_SHOWN,
        keep
    );

    // 3. Test API usage
    NotificationModule_AddInfoNotification("CI Test: Build Successful!");

    // Deinit
    NotificationModule_DeInitLibrary();
    
    return 0;
}