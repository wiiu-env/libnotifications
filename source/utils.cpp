#include "internal.h"
#include "logger.h"
#include <coreinit/debug.h>
#include <coreinit/dynload.h>
#include <cstdarg>
#include <map>
#include <notifications/notifications.h>

static OSDynLoad_Module sModuleHandle = nullptr;

static NotificationModuleStatus (*sNMGetVersion)(NotificationModuleAPIVersion *) = nullptr;
static NotificationModuleStatus (*sNMIsOverlayReady)(bool *)                     = nullptr;
static NotificationModuleStatus (*sNMAddStaticNotification)(const char *,
                                                            NotificationModuleNotificationType,
                                                            float,
                                                            float,
                                                            NMColor,
                                                            NMColor,
                                                            void (*)(void *context),
                                                            void *)              = nullptr;

static NotificationModuleStatus (*sNMAddDynamicNotification)(const char *,
                                                             NMColor,
                                                             NMColor,
                                                             void (*)(void *context),
                                                             void *) = nullptr;

static NotificationModuleStatus (*sNMUpdateDynamicNotificationText)(NotificationModuleHandle,
                                                                    const char *) = nullptr;

static NotificationModuleStatus (*sNMUpdateDynamicNotificationBackgroundColor)(NotificationModuleHandle,
                                                                               NMColor) = nullptr;

static NotificationModuleStatus (*sNMUpdateDynamicNotificationTextColor)(NotificationModuleHandle,
                                                                         NMColor) = nullptr;

static NotificationModuleStatus (*sNMFinishDynamicNotification)(NotificationModuleHandle handle,
                                                                NotificationModuleStatusFinish finishMode,
                                                                float durationBeforeFadeOutInSeconds,
                                                                float shakeDurationInSeconds) = nullptr;

static bool sLibInitDone = false;
std::map<NotificationModuleNotificationType, NMDefaultValueStore> sDefaultValues;

static NotificationModuleAPIVersion sNotificationModuleVersion = NOTIFICATION_MODULE_API_VERSION_ERROR;

const char *NotificationModule_GetStatusStr(NotificationModuleStatus status) {
    switch (status) {
        case NOTIFICATION_MODULE_RESULT_SUCCESS:
            return "NOTIFICATION_MODULE_RESULT_SUCCESS";
        case NOTIFICATION_MODULE_RESULT_MODULE_NOT_FOUND:
            return "NOTIFICATION_MODULE_RESULT_MODULE_NOT_FOUND";
        case NOTIFICATION_MODULE_RESULT_MODULE_MISSING_EXPORT:
            return "NOTIFICATION_MODULE_RESULT_MODULE_MISSING_EXPORT";
        case NOTIFICATION_MODULE_RESULT_UNSUPPORTED_VERSION:
            return "NOTIFICATION_MODULE_RESULT_UNSUPPORTED_VERSION";
        case NOTIFICATION_MODULE_RESULT_UNKNOWN_ERROR:
            return "NOTIFICATION_MODULE_RESULT_UNKNOWN_ERROR";
        case NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT:
            return "NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT";
        case NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED:
            return "NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED";
        case NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND:
            return "NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND";
        case NOTIFICATION_MODULE_RESULT_OVERLAY_NOT_READY:
            return "NOTIFICATION_MODULE_RESULT_OVERLAY_NOT_READY";
        case NOTIFICATION_MODULE_RESULT_UNSUPPORTED_TYPE:
            return "NOTIFICATION_MODULE_RESULT_UNSUPPORTED_TYPE";
        case NOTIFICATION_MODULE_RESULT_ALLOCATION_FAILED:
            return "NOTIFICATION_MODULE_RESULT_ALLOCATION_FAILED";
        case NOTIFICATION_MODULE_RESULT_INVALID_HANDLE:
            return "NOTIFICATION_MODULE_RESULT_INVALID_HANDLE";
    }
    return "NOTIFICATION_MODULE_RESULT_UNKNOWN_ERROR";
}

NotificationModuleStatus NotificationModule_InitLibrary() {
    if (sLibInitDone) {
        return NOTIFICATION_MODULE_RESULT_SUCCESS;
    }
    if (OSDynLoad_Acquire("homebrew_notifications", &sModuleHandle) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("OSDynLoad_Acquire failed.");
        return NOTIFICATION_MODULE_RESULT_MODULE_NOT_FOUND;
    }

    if (OSDynLoad_FindExport(sModuleHandle, FALSE, "NMGetVersion", (void **) &sNMGetVersion) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("FindExport NMGetVersion failed.");
        return NOTIFICATION_MODULE_RESULT_MODULE_MISSING_EXPORT;
    }

    auto res = NotificationModule_GetVersion(&sNotificationModuleVersion);
    if (res != NOTIFICATION_MODULE_RESULT_SUCCESS) {
        sNotificationModuleVersion = NOTIFICATION_MODULE_API_VERSION_ERROR;
        return NOTIFICATION_MODULE_RESULT_UNSUPPORTED_VERSION;
    }

    if (OSDynLoad_FindExport(sModuleHandle, FALSE, "NMIsOverlayReady", (void **) &sNMIsOverlayReady) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("FindExport NMIsOverlayReady failed.");
        sNMIsOverlayReady = nullptr;
    }
    if (OSDynLoad_FindExport(sModuleHandle, FALSE, "NMAddStaticNotification", (void **) &sNMAddStaticNotification) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("FindExport NMAddStaticNotification failed.");
        sNMAddStaticNotification = nullptr;
    }
    if (OSDynLoad_FindExport(sModuleHandle, FALSE, "NMAddDynamicNotification", (void **) &sNMAddDynamicNotification) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("FindExport NMAddDynamicNotification failed.");
        sNMAddStaticNotification = nullptr;
    }

    if (OSDynLoad_FindExport(sModuleHandle, FALSE, "NMUpdateDynamicNotificationText", (void **) &sNMUpdateDynamicNotificationText) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("FindExport NMUpdateDynamicNotificationText failed.");
        sNMAddStaticNotification = nullptr;
    }

    if (OSDynLoad_FindExport(sModuleHandle, FALSE, "NMUpdateDynamicNotificationBackgroundColor", (void **) &sNMUpdateDynamicNotificationBackgroundColor) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("FindExport NMUpdateDynamicNotificationBackgroundColor failed.");
        sNMAddStaticNotification = nullptr;
    }

    if (OSDynLoad_FindExport(sModuleHandle, FALSE, "NMUpdateDynamicNotificationTextColor", (void **) &sNMUpdateDynamicNotificationTextColor) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("FindExport NMUpdateDynamicNotificationTextColor failed.");
        sNMAddStaticNotification = nullptr;
    }

    if (OSDynLoad_FindExport(sModuleHandle, FALSE, "NMFinishDynamicNotification", (void **) &sNMFinishDynamicNotification) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("FindExport NMFinishDynamicNotification failed.");
        sNMAddStaticNotification = nullptr;
    }

    sDefaultValues.clear();

    sDefaultValues[NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO];
    sDefaultValues[NOTIFICATION_MODULE_NOTIFICATION_TYPE_ERROR].backgroundColor = {237, 28, 36, 255};
    sDefaultValues[NOTIFICATION_MODULE_NOTIFICATION_TYPE_DYNAMIC];

    sLibInitDone = true;
    return NOTIFICATION_MODULE_RESULT_SUCCESS;
}

NotificationModuleStatus NotificationModule_DeInitLibrary() {
    if (sLibInitDone) {
        sNMGetVersion              = nullptr;
        sNotificationModuleVersion = NOTIFICATION_MODULE_API_VERSION_ERROR;
        OSDynLoad_Release(sModuleHandle);
        sModuleHandle = nullptr;
        sDefaultValues.clear();
        sLibInitDone = false;
    }
    return NOTIFICATION_MODULE_RESULT_SUCCESS;
}

NotificationModuleStatus NotificationModule_GetVersion(NotificationModuleAPIVersion *outVersion) {
    if (sNMGetVersion == nullptr) {
        if (OSDynLoad_Acquire("homebrew_notifications", &sModuleHandle) != OS_DYNLOAD_OK) {
            DEBUG_FUNCTION_LINE_WARN("OSDynLoad_Acquire failed.");
            return NOTIFICATION_MODULE_RESULT_MODULE_NOT_FOUND;
        }

        if (OSDynLoad_FindExport(sModuleHandle, FALSE, "NMGetVersion", (void **) &sNMGetVersion) != OS_DYNLOAD_OK) {
            DEBUG_FUNCTION_LINE_WARN("FindExport NMGetVersion failed.");
            return NOTIFICATION_MODULE_RESULT_MODULE_MISSING_EXPORT;
        }
    }

    return reinterpret_cast<decltype(&NotificationModule_GetVersion)>(sNMGetVersion)(outVersion);
}

NotificationModuleStatus NotificationModule_IsOverlayReady(bool *outIsReady) {
    if (sNotificationModuleVersion == NOTIFICATION_MODULE_API_VERSION_ERROR) {
        return NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED;
    }
    if (sNMIsOverlayReady == nullptr || sNotificationModuleVersion < 1) {
        return NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND;
    }

    if (outIsReady == nullptr) {
        return NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT;
    }

    return reinterpret_cast<decltype(&NotificationModule_IsOverlayReady)>(sNMIsOverlayReady)(outIsReady);
}

NotificationModuleStatus NotificationModule_AddDynamicNotificationExDeclare(const char *text,
                                                                            NMColor textColor,
                                                                            NMColor backgroundColor,
                                                                            NotificationModuleNotificationFinishedCallback callback,
                                                                            void *callbackContext,
                                                                            NotificationModuleHandle *outHandle);

NotificationModuleStatus NotificationModule_AddDynamicNotificationEx(const char *text,
                                                                     NotificationModuleHandle *outHandle,
                                                                     NMColor textColor,
                                                                     NMColor backgroundColor,
                                                                     void (*finishFunc)(NotificationModuleHandle, void *context),
                                                                     void *context) {
    if (sNotificationModuleVersion == NOTIFICATION_MODULE_API_VERSION_ERROR) {
        return NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED;
    }
    if (sNMAddDynamicNotification == nullptr || sNotificationModuleVersion < 1) {
        return NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND;
    }

    if (text == nullptr || outHandle == nullptr) {
        return NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT;
    }

    return reinterpret_cast<decltype(&NotificationModule_AddDynamicNotificationExDeclare)>(sNMAddDynamicNotification)(text,
                                                                                                                      textColor,
                                                                                                                      backgroundColor,
                                                                                                                      finishFunc,
                                                                                                                      context,
                                                                                                                      outHandle);
}

NotificationModuleStatus NotificationModule_AddDynamicNotification(const char *text, NotificationModuleHandle *outHandle) {
    auto &cur = sDefaultValues[NOTIFICATION_MODULE_NOTIFICATION_TYPE_DYNAMIC];
    return NotificationModule_AddDynamicNotificationEx(text,
                                                       outHandle,
                                                       cur.textColor,
                                                       cur.backgroundColor,
                                                       cur.finishFunc,
                                                       cur.finishFuncContext);
}

NotificationModuleStatus NotificationModule_AddDynamicNotificationWithCallback(const char *text,
                                                                               NotificationModuleHandle *outHandle,
                                                                               NotificationModuleNotificationFinishedCallback callback,
                                                                               void *callbackContext) {
    auto &cur = sDefaultValues[NOTIFICATION_MODULE_NOTIFICATION_TYPE_DYNAMIC];
    return NotificationModule_AddDynamicNotificationEx(text,
                                                       outHandle,
                                                       cur.textColor,
                                                       cur.backgroundColor,
                                                       callback,
                                                       callbackContext);
}

static NotificationModuleStatus NotificationModule_AddStaticNotification(const char *text,
                                                                         NotificationModuleNotificationType type,
                                                                         float durationBeforeFadeOutInSeconds,
                                                                         float shakeDurationInSeconds,
                                                                         NMColor textColor,
                                                                         NMColor backgroundColor,
                                                                         NotificationModuleNotificationFinishedCallback callback,
                                                                         void *callbackContext) {
    if (sNotificationModuleVersion == NOTIFICATION_MODULE_API_VERSION_ERROR) {
        return NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED;
    }
    if (sNMAddStaticNotification == nullptr || sNotificationModuleVersion < 1) {
        return NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND;
    }

    if (text == nullptr) {
        return NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT;
    }

    return reinterpret_cast<decltype(&NotificationModule_AddStaticNotification)>(sNMAddStaticNotification)(text,
                                                                                                           type,
                                                                                                           durationBeforeFadeOutInSeconds,
                                                                                                           shakeDurationInSeconds,
                                                                                                           textColor,
                                                                                                           backgroundColor,
                                                                                                           callback,
                                                                                                           callbackContext);
}

#undef NotificationModule_SetDefaultValue
NotificationModuleStatus NotificationModule_SetDefaultValue(NotificationModuleNotificationType type,
                                                            NotificationModuleNotificationOption valueType,
                                                            ...) {
    if (sModuleHandle == nullptr) {
        return NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED;
    }
    va_list va;

    if (!sDefaultValues.contains(type) && type == NOTIFICATION_MODULE_NOTIFICATION_TYPE_ERROR) {
        sDefaultValues[type].backgroundColor = {237, 28, 36, 255};
    }

    auto &cur = sDefaultValues[type];
    va_start(va, valueType);
    auto res = NOTIFICATION_MODULE_RESULT_SUCCESS;
    switch (valueType) {
        case NOTIFICATION_MODULE_DEFAULT_OPTION_BACKGROUND_COLOR: {
            auto arg            = va_arg(va, NMColor);
            cur.backgroundColor = arg;
            break;
        }
        case NOTIFICATION_MODULE_DEFAULT_OPTION_TEXT_COLOR: {
            auto arg      = va_arg(va, NMColor);
            cur.textColor = arg;
            break;
        }
        case NOTIFICATION_MODULE_DEFAULT_OPTION_DURATION_BEFORE_FADE_OUT: {
            auto arg                           = va_arg(va, double);
            cur.durationBeforeFadeOutInSeconds = (float) arg;
            break;
        }
        case NOTIFICATION_MODULE_DEFAULT_OPTION_FINISH_FUNCTION: {
            auto arg       = va_arg(va, void (*)(NotificationModuleHandle, void *));
            cur.finishFunc = arg;
            break;
        }
        case NOTIFICATION_MODULE_DEFAULT_OPTION_FINISH_FUNCTION_CONTEXT: {
            auto arg              = va_arg(va, void *);
            cur.finishFuncContext = arg;
            break;
        }
        default:
            res = NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT;
            break;
    }

    va_end(va);

    return res;
}

NotificationModuleStatus NotificationModule_AddInfoNotificationEx(const char *text,
                                                                  float durationBeforeFadeOutInSeconds,
                                                                  NMColor textColor,
                                                                  NMColor backgroundColor,
                                                                  NotificationModuleNotificationFinishedCallback callback,
                                                                  void *callbackContext) {
    return NotificationModule_AddStaticNotification(text,
                                                    NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO,
                                                    durationBeforeFadeOutInSeconds,
                                                    0.0f,
                                                    textColor,
                                                    backgroundColor,
                                                    callback,
                                                    callbackContext);
}

NotificationModuleStatus NotificationModule_AddInfoNotification(const char *text) {
    auto &cur = sDefaultValues[NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO];
    return NotificationModule_AddInfoNotificationEx(text,
                                                    cur.durationBeforeFadeOutInSeconds,
                                                    cur.textColor,
                                                    cur.backgroundColor,
                                                    cur.finishFunc,
                                                    cur.finishFuncContext);
}
NotificationModuleStatus NotificationModule_AddInfoNotificationWithCallback(const char *text,
                                                                            NotificationModuleNotificationFinishedCallback callback,
                                                                            void *callbackContext) {
    auto &cur = sDefaultValues[NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO];
    return NotificationModule_AddInfoNotificationEx(text,
                                                    cur.durationBeforeFadeOutInSeconds,
                                                    cur.textColor,
                                                    cur.backgroundColor,
                                                    callback,
                                                    callbackContext);
}

NotificationModuleStatus NotificationModule_AddErrorNotificationEx(const char *text,
                                                                   float durationBeforeFadeOutInSeconds,
                                                                   float shakeDurationInSeconds,
                                                                   NMColor textColor,
                                                                   NMColor backgroundColor,
                                                                   NotificationModuleNotificationFinishedCallback callback,
                                                                   void *callbackContext) {
    return NotificationModule_AddStaticNotification(text,
                                                    NOTIFICATION_MODULE_NOTIFICATION_TYPE_ERROR,
                                                    durationBeforeFadeOutInSeconds,
                                                    shakeDurationInSeconds,
                                                    textColor,
                                                    backgroundColor,
                                                    callback,
                                                    callbackContext);
}

NotificationModuleStatus NotificationModule_AddErrorNotification(const char *text) {
    if (sModuleHandle == nullptr) {
        return NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED;
    }

    auto &cur = sDefaultValues[NOTIFICATION_MODULE_NOTIFICATION_TYPE_ERROR];
    return NotificationModule_AddErrorNotificationEx(text,
                                                     cur.durationBeforeFadeOutInSeconds,
                                                     cur.shakeDurationOnErrorInSeconds,
                                                     cur.textColor,
                                                     cur.backgroundColor,
                                                     cur.finishFunc,
                                                     cur.finishFuncContext);
}

NotificationModuleStatus NotificationModule_AddErrorNotificationWithCallback(const char *text,
                                                                             NotificationModuleNotificationFinishedCallback callback,
                                                                             void *callbackContext) {
    if (sModuleHandle == nullptr) {
        return NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED;
    }

    auto &cur = sDefaultValues[NOTIFICATION_MODULE_NOTIFICATION_TYPE_ERROR];
    return NotificationModule_AddErrorNotificationEx(text,
                                                     cur.durationBeforeFadeOutInSeconds,
                                                     cur.shakeDurationOnErrorInSeconds,
                                                     cur.textColor,
                                                     cur.backgroundColor,
                                                     callback,
                                                     callbackContext);
}

NotificationModuleStatus NotificationModule_UpdateDynamicNotificationText(NotificationModuleHandle handle,
                                                                          const char *text) {
    if (sNotificationModuleVersion == NOTIFICATION_MODULE_API_VERSION_ERROR) {
        return NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED;
    }
    if (sNMUpdateDynamicNotificationText == nullptr || sNotificationModuleVersion < 1) {
        return NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND;
    }

    if (handle == 0 || text == nullptr) {
        return NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT;
    }

    return reinterpret_cast<decltype(&NotificationModule_UpdateDynamicNotificationText)>(sNMUpdateDynamicNotificationText)(handle,
                                                                                                                           text);
}

NotificationModuleStatus NotificationModule_UpdateDynamicNotificationBackgroundColor(NotificationModuleHandle handle,
                                                                                     NMColor backgroundColor) {
    if (sNotificationModuleVersion == NOTIFICATION_MODULE_API_VERSION_ERROR) {
        return NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED;
    }
    if (sNMUpdateDynamicNotificationBackgroundColor == nullptr || sNotificationModuleVersion < 1) {
        return NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND;
    }

    if (handle == 0) {
        return NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT;
    }

    return reinterpret_cast<decltype(&NotificationModule_UpdateDynamicNotificationBackgroundColor)>(sNMUpdateDynamicNotificationBackgroundColor)(handle,
                                                                                                                                                 backgroundColor);
}

NotificationModuleStatus NotificationModule_UpdateDynamicNotificationTextColor(NotificationModuleHandle handle,
                                                                               NMColor textColor) {
    if (sNotificationModuleVersion == NOTIFICATION_MODULE_API_VERSION_ERROR) {
        return NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED;
    }
    if (sNMUpdateDynamicNotificationTextColor == nullptr || sNotificationModuleVersion < 1) {
        return NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND;
    }

    if (handle == 0) {
        return NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT;
    }

    return reinterpret_cast<decltype(&NotificationModule_UpdateDynamicNotificationBackgroundColor)>(sNMUpdateDynamicNotificationTextColor)(handle,
                                                                                                                                           textColor);
}

static NotificationModuleStatus NotificationModule_FinishDynamicNotificationEx(NotificationModuleHandle handle,
                                                                               NotificationModuleStatusFinish finishMode,
                                                                               float durationBeforeFadeOutInSeconds,
                                                                               float shakeDurationInSeconds) {
    if (sNotificationModuleVersion == NOTIFICATION_MODULE_API_VERSION_ERROR) {
        return NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED;
    }
    if (sNMFinishDynamicNotification == nullptr || sNotificationModuleVersion < 1) {
        return NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND;
    }

    if (handle == 0) {
        return NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT;
    }

    return reinterpret_cast<decltype(&NotificationModule_FinishDynamicNotificationEx)>(sNMFinishDynamicNotification)(handle,
                                                                                                                     finishMode,
                                                                                                                     durationBeforeFadeOutInSeconds,
                                                                                                                     shakeDurationInSeconds);
}

NotificationModuleStatus NotificationModule_FinishDynamicNotification(NotificationModuleHandle handle,
                                                                      float durationBeforeFadeOutInSeconds) {
    return NotificationModule_FinishDynamicNotificationEx(handle,
                                                          NOTIFICATION_MODULE_STATUS_FINISH,
                                                          durationBeforeFadeOutInSeconds,
                                                          0.0f);
}

NotificationModuleStatus NotificationModule_FinishDynamicNotificationWithShake(NotificationModuleHandle handle,
                                                                               float durationBeforeFadeOutInSeconds,
                                                                               float shakeDuration) {
    return NotificationModule_FinishDynamicNotificationEx(handle,
                                                          NOTIFICATION_MODULE_STATUS_FINISH_WITH_SHAKE,
                                                          durationBeforeFadeOutInSeconds,
                                                          shakeDuration);
}