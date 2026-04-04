#ifndef NOTIFICATIONS_TYPECHECK_GCC_H
#define NOTIFICATIONS_TYPECHECK_GCC_H

#ifdef __cplusplus
#include <cstddef> /* For std::nullptr_t */
#include <type_traits>
#else
#include <stddef.h> /* For NULL */
#endif

/* =========================================================================
 * Warning generators (Common to C and C++)
 * ========================================================================= */
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
/* ---------------------------------------------------------
     * C++ IMPLEMENTATION
     * --------------------------------------------------------- */
#if __cplusplus >= 201703L
namespace NM_Check {

    template<typename Actual>
    using Decayed = std::decay_t<Actual>;

    /* Strict matching: Must be the NMColor type or a raw integer (for hex literals) */
    template<typename Actual>
    constexpr bool is_NMColor = std::is_same_v<NMColor, Decayed<Actual>> || std::is_integral_v<Decayed<Actual>>;

    /* Strict matching: Float or Double only (Rejects implicit ints/pointers) */
    template<typename Actual>
    constexpr bool is_float = std::is_same_v<float, Decayed<Actual>> || std::is_same_v<double, Decayed<Actual>>;

    /* Strict matching: Bool or Int only (Rejects implicit pointers) */
    template<typename Actual>
    constexpr bool is_bool = std::is_same_v<bool, Decayed<Actual>> || std::is_same_v<int, Decayed<Actual>>;

    /* Strict matching: Callback type, void*, nullptr, or 0 */
    template<typename Actual>
    constexpr bool is_callback =
            std::is_same_v<NotificationModuleNotificationFinishedCallback, Decayed<Actual>> ||
            std::is_same_v<void *, Decayed<Actual>> ||
            std::is_same_v<std::nullptr_t, Decayed<Actual>> ||
            std::is_integral_v<Decayed<Actual>>;

    /* Strict matching: void*, nullptr, or 0 */
    template<typename Actual>
    constexpr bool is_context =
            std::is_same_v<void *, Decayed<Actual>> ||
            std::is_same_v<std::nullptr_t, Decayed<Actual>> ||
            std::is_integral_v<Decayed<Actual>>;

    /* Compile-time option router */
    template<int Option, typename Actual>
    inline void check_option() {
        if constexpr (Option == NOTIFICATION_MODULE_DEFAULT_OPTION_BACKGROUND_COLOR ||
                      Option == NOTIFICATION_MODULE_DEFAULT_OPTION_TEXT_COLOR) {
            if constexpr (!is_NMColor<Actual>) _nm_warn_NMColor();
        } else if constexpr (Option == NOTIFICATION_MODULE_DEFAULT_OPTION_DURATION_BEFORE_FADE_OUT) {
            if constexpr (!is_float<Actual>) _nm_warn_float();
        } else if constexpr (Option == NOTIFICATION_MODULE_DEFAULT_OPTION_FINISH_FUNCTION) {
            if constexpr (!is_callback<Actual>) _nm_warn_callback();
        } else if constexpr (Option == NOTIFICATION_MODULE_DEFAULT_OPTION_FINISH_FUNCTION_CONTEXT) {
            if constexpr (!is_context<Actual>) _nm_warn_context();
        } else if constexpr (Option == NOTIFICATION_MODULE_DEFAULT_OPTION_KEEP_UNTIL_SHOWN) {
            if constexpr (!is_bool<Actual>) _nm_warn_bool();
        }
    }
} // namespace NM_Check

/* C++17 Macro (Uses if constexpr) */
#define NotificationModule_SetDefaultValue(type, option, value)                                  \
    __extension__({                                                                              \
        NM_Check::check_option<__builtin_constant_p(option) ? (option) : -1, decltype(value)>(); \
        (NotificationModule_SetDefaultValue)(type, option, value);                               \
    })
#else
/* Fallback for old C++ (Checks disabled) */
#define NotificationModule_SetDefaultValue(type, option, value) \
    (NotificationModule_SetDefaultValue)(type, option, value)
#endif

#else
/* ---------------------------------------------------------
     * C IMPLEMENTATION
     * --------------------------------------------------------- */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
/* C11 Implementation */
#define _nm_is_NMColor(x)  _Generic((x), NMColor : 1, default : 0)
#define _nm_is_float(x)    _Generic((x), float : 1, double : 1, default : 0)
#define _nm_is_bool(x)     _Generic((x), _Bool : 1, int : 1, default : 0)
#define _nm_is_callback(x) _Generic((x), NotificationModuleNotificationFinishedCallback : 1, void * : 1, default : 0)
#define _nm_is_context(x)  _Generic((x), void * : 1, default : 0)
#else
/* Legacy C Implementation */
#define _nm_is_type(x, type) __builtin_types_compatible_p(__typeof__(x), type)
#define _nm_is_NMColor(x)    (1)
#define _nm_is_callback(x)   (1)
#define _nm_is_context(x)    (1)
#define _nm_is_float(x)      (_nm_is_type(x, float) || _nm_is_type(x, double))
#define _nm_is_bool(x)       (_nm_is_type(x, int) || _nm_is_type(x, _Bool))
#endif /* C11 Check */

/* Unified C Macro (C-only) */
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
#endif

#endif /* NOTIFICATIONS_TYPECHECK_GCC_H */