#include <notifications/notifications.h>

// Helper macros to switch between Valid and Invalid data
#ifdef MAKE_VALID
    #define ARG(valid, invalid) (valid)
#else
    #define ARG(valid, invalid) (invalid)
#endif

// Dummy callback
void my_cb(NotificationModuleHandle h, void* c) { (void)h; (void)c; }
void my_broken_cb(NotificationModuleHandle h, void* c, int) { (void)h; (void)c; }

int main(int argc, char **argv) {
    NotificationModule_InitLibrary();    

    // ---------------------------------------------------------
    // TEST CASE: FAIL_COLOR (Background Color)
    // ---------------------------------------------------------
    #ifdef TEST_FAIL_COLOR
        NMColor valid_col = {255, 0, 0, 255};
        NotificationModule_SetDefaultValue(
            NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO,
            NOTIFICATION_MODULE_DEFAULT_OPTION_BACKGROUND_COLOR,
            ARG(valid_col, 0xFFFFFFFF) // Invalid: int
        );
    #endif

    // ---------------------------------------------------------
    // TEST CASE: FAIL_DURATION (Float)
    // ---------------------------------------------------------
    #ifdef TEST_FAIL_DURATION
        NotificationModule_SetDefaultValue(
            NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO,
            NOTIFICATION_MODULE_DEFAULT_OPTION_DURATION_BEFORE_FADE_OUT,
            ARG(5.0f, 5) // Invalid: int literal
        );
    #endif

    // ---------------------------------------------------------
    // TEST CASE: FAIL_CALLBACK (Function Pointer)
    // ---------------------------------------------------------
    #ifdef TEST_FAIL_CALLBACK
        NotificationModule_SetDefaultValue(
            NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO,
            NOTIFICATION_MODULE_DEFAULT_OPTION_FINISH_FUNCTION,
            ARG(my_cb, my_broken_cb) // Invalid: nullptr
        );
    #endif

    // ---------------------------------------------------------
    // TEST CASE: FAIL_CONTEXT (Void Pointer)
    // ---------------------------------------------------------
    #ifdef TEST_FAIL_CONTEXT
        NotificationModule_SetDefaultValue(
            NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO,
            NOTIFICATION_MODULE_DEFAULT_OPTION_FINISH_FUNCTION_CONTEXT,
            ARG(nullptr, 1.5f) // Invalid: float (nullptr is valid for void*)
        );
    #endif

    // ---------------------------------------------------------
    // TEST CASE: FAIL_BOOL (Boolean)
    // ---------------------------------------------------------
    #ifdef TEST_FAIL_BOOL
		#ifdef MAKE_VALID
			bool valid_bool = true;
		#else
			int dummy_int = 0;
		#endif
        NotificationModule_SetDefaultValue(
            NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO,
            NOTIFICATION_MODULE_DEFAULT_OPTION_KEEP_UNTIL_SHOWN,
            ARG(valid_bool, &dummy_int) // Invalid: pointer
        );
    #endif

    NotificationModule_DeInitLibrary();
    return 0;
}