#ifndef NOTIFICATIONS_TYPECHECK_GCC_H
#define NOTIFICATIONS_TYPECHECK_GCC_H

#ifdef __cplusplus
#include <cstddef> /* For std::nullptr_t */
#else
#include <stddef.h> /* For NULL */
#endif

/* * Warning generators (Common to C and C++)
 * These define static functions that trigger a compiler warning when called.
 */
#define _NM_WARNING(id, message)                              \
    static void __attribute__((__warning__(message)))         \
    __attribute__((__unused__)) __attribute__((__noinline__)) \
    id(void) { __asm__(""); }

#ifdef __cplusplus
extern "C" {
#endif

_NM_WARNING(_nm_warn_NMColor, "NotificationModule_SetDefaultValue expects 'NMColor' for this option.")
_NM_WARNING(_nm_warn_float, "NotificationModule_SetDefaultValue expects 'float' or 'double' for this option.")
_NM_WARNING(_nm_warn_callback, "NotificationModule_SetDefaultValue expects 'NotificationModuleNotificationFinishedCallback' for this option.")
_NM_WARNING(_nm_warn_context, "NotificationModule_SetDefaultValue expects 'void*' for this option.")
_NM_WARNING(_nm_warn_bool, "NotificationModule_SetDefaultValue expects 'bool' (or 'int') for this option.")

#ifdef __cplusplus
}
#endif


/* =========================================================================
 * IMPLEMENTATION SELECTION
 * ========================================================================= */

#if defined(__cplusplus)

/* ==========================================
     * C++17 Implementation
     * Only active if -std=c++17 or higher is used.
     * ========================================== */
#if __cplusplus >= 201703L

namespace NM_Check {
    /* NMColor Checker */
    inline bool check_NMColor(NMColor) { return true; }
    inline bool check_NMColor(void *) { return false; } /* Sink for NULL */
    template<typename T>
    inline bool check_NMColor(T) { return false; }

    /* Float Checker (accepts float/double) */
    inline bool check_float(float) { return true; }
    inline bool check_float(double) { return true; }
    inline bool check_float(void *) { return false; } /* Sink for NULL */
    template<typename T>
    inline bool check_float(T) { return false; }

    /* Bool Checker (accepts bool/int) */
    inline bool check_bool(bool) { return true; }
    inline bool check_bool(int) { return true; }
    inline bool check_bool(void *) { return false; } /* Sink for NULL */
    template<typename T>
    inline bool check_bool(T) { return false; }

    /* Callback Checker */
    inline bool check_callback(NotificationModuleNotificationFinishedCallback) { return true; }
    /* Explicit void* (e.g. casting) */
    inline bool check_callback(void *) { return true; }
    inline bool check_callback(int i) { return i == 0; }
    inline bool check_callback(long i) { return i == 0; }
    inline bool check_callback(std::nullptr_t) { return true; }
    template<typename T>
    inline bool check_callback(T) { return false; }

    /* Context Checker */
    inline bool check_context(void *) { return true; }
    inline bool check_context(int i) { return i == 0; }
    inline bool check_context(long i) { return i == 0; }
    inline bool check_context(std::nullptr_t) { return true; }
    template<typename T>
    inline bool check_context(T) { return false; }
} // namespace NM_Check

/* Macros mapping to C++ namespace calls */
#define _nm_is_NMColor(x)  NM_Check::check_NMColor(x)
#define _nm_is_float(x)    NM_Check::check_float(x)
#define _nm_is_bool(x)     NM_Check::check_bool(x)
#define _nm_is_callback(x) NM_Check::check_callback(x)
#define _nm_is_context(x)  NM_Check::check_context(x)

#else
/* ==========================================
     * Pre-C++17 Implementation
     * Checks are DISABLED to avoid errors in older versions.
     * ========================================== */
#define _nm_is_NMColor(x)  (1)
#define _nm_is_float(x)    (1)
#define _nm_is_bool(x)     (1)
#define _nm_is_callback(x) (1)
#define _nm_is_context(x)  (1)
#endif

#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L

/* ==========================================
 * C11 Implementation (using _Generic)
 * ========================================== */

#define _nm_is_NMColor(x) _Generic((x),         \
                                   NMColor : 1, \
                                   default : 0)

#define _nm_is_float(x) _Generic((x),        \
                                 float : 1,  \
                                 double : 1, \
                                 default : 0)

#define _nm_is_bool(x) _Generic((x),       \
                                _Bool : 1, \
                                int : 1,   \
                                default : 0)

#define _nm_is_callback(x) _Generic((x),                                                \
                                    NotificationModuleNotificationFinishedCallback : 1, \
                                    void * : 1,                                         \
                                    default : 0)

#define _nm_is_context(x) _Generic((x),        \
                                   void * : 1, \
                                   default : 0)

#else

/* ==========================================
 * Legacy C Implementation
 * Partial checking only.
 * ========================================== */

#define _nm_is_type(x, type) __builtin_types_compatible_p(__typeof__(x), type)

/* Disable complex checks in old C */
#define _nm_is_NMColor(x)    (1)
#define _nm_is_callback(x)   (1)
#define _nm_is_context(x)    (1)

/* Scalars are safe to check */
#define _nm_is_float(x)      (_nm_is_type(x, float) || _nm_is_type(x, double))
#define _nm_is_bool(x)       (_nm_is_type(x, int) || _nm_is_type(x, _Bool))

#endif

#define NotificationModule_SetDefaultValue(type, option, value)                                                        \
    __extension__({                                                                                                    \
        if (__builtin_constant_p(option)) {                                                                            \
            if ((option == NOTIFICATION_MODULE_DEFAULT_OPTION_BACKGROUND_COLOR) && !_nm_is_NMColor(value))             \
                _nm_warn_NMColor();                                                                                    \
            else if ((option == NOTIFICATION_MODULE_DEFAULT_OPTION_TEXT_COLOR) && !_nm_is_NMColor(value))              \
                _nm_warn_NMColor();                                                                                    \
            else if ((option == NOTIFICATION_MODULE_DEFAULT_OPTION_DURATION_BEFORE_FADE_OUT) && !_nm_is_float(value))  \
                _nm_warn_float();                                                                                      \
            else if ((option == NOTIFICATION_MODULE_DEFAULT_OPTION_FINISH_FUNCTION) && !_nm_is_callback(value))        \
                _nm_warn_callback();                                                                                   \
            else if ((option == NOTIFICATION_MODULE_DEFAULT_OPTION_FINISH_FUNCTION_CONTEXT) && !_nm_is_context(value)) \
                _nm_warn_context();                                                                                    \
            else if ((option == NOTIFICATION_MODULE_DEFAULT_OPTION_KEEP_UNTIL_SHOWN) && !_nm_is_bool(value))           \
                _nm_warn_bool();                                                                                       \
        }                                                                                                              \
        (NotificationModule_SetDefaultValue)(type, option, value);                                                     \
    })

#endif /* NOTIFICATIONS_TYPECHECK_GCC_H */