#include <notifications/notifications.h>
#include <stddef.h>

// Helper macros
#ifdef MAKE_VALID
    #define ARG(valid, invalid) (valid)
#else
    #define ARG(valid, invalid) (invalid)
#endif

void my_cb(NotificationModuleHandle h, void* c) { (void)h; (void)c; }
void my_broken_cb(NotificationModuleHandle h, void* c, int) { (void)h; (void)c; }

int main(int argc, char **argv) {
    NotificationModule_InitLibrary();

    // ---------------------------------------------------------
    // TEST CASE: FAIL_COLOR
    // ---------------------------------------------------------
    #ifdef TEST_FAIL_COLOR
        NMColor valid_col = {255, 0, 0, 255};
        NotificationModule_SetDefaultValue(
            NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO,
            NOTIFICATION_MODULE_DEFAULT_OPTION_BACKGROUND_COLOR,
            ARG(valid_col, 0xFFFFFFFF)
        );
    #endif

    // ---------------------------------------------------------
    // TEST CASE: FAIL_DURATION
    // ---------------------------------------------------------
    #ifdef TEST_FAIL_DURATION
        NotificationModule_SetDefaultValue(
            NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO,
            NOTIFICATION_MODULE_DEFAULT_OPTION_DURATION_BEFORE_FADE_OUT,
            ARG(5.0f, 5)
        );
    #endif

    // ---------------------------------------------------------
    // TEST CASE: FAIL_CALLBACK
    // ---------------------------------------------------------
    #ifdef TEST_FAIL_CALLBACK
        NotificationModule_SetDefaultValue(
            NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO,
            NOTIFICATION_MODULE_DEFAULT_OPTION_FINISH_FUNCTION,
            ARG(my_cb, my_broken_cb)
        );
    #endif

    // ---------------------------------------------------------
    // TEST CASE: FAIL_CONTEXT
    // ---------------------------------------------------------
    #ifdef TEST_FAIL_CONTEXT
        NotificationModule_SetDefaultValue(
            NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO,
            NOTIFICATION_MODULE_DEFAULT_OPTION_FINISH_FUNCTION_CONTEXT,
            ARG(NULL, 1.5f)
        );
    #endif

    // ---------------------------------------------------------
    // TEST CASE: FAIL_BOOL
    // ---------------------------------------------------------
    #ifdef TEST_FAIL_BOOL
		#ifdef MAKE_VALID
		#else
			int dummy_int = 0;
		#endif
        // For C, 1 is a valid boolean (int), so we use a pointer as invalid
        NotificationModule_SetDefaultValue(
            NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO,
            NOTIFICATION_MODULE_DEFAULT_OPTION_KEEP_UNTIL_SHOWN,
            ARG(1, &dummy_int)
        );
    #endif

    NotificationModule_DeInitLibrary();
    return 0;
}