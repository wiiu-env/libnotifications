#pragma once

#include <stdint.h>
#include <wut.h>

typedef enum NotificationModuleStatus {
    NOTIFICATION_MODULE_RESULT_SUCCESS               = 0,
    NOTIFICATION_MODULE_RESULT_MODULE_NOT_FOUND      = -0x1,
    NOTIFICATION_MODULE_RESULT_MODULE_MISSING_EXPORT = -0x2,
    NOTIFICATION_MODULE_RESULT_UNSUPPORTED_VERSION   = -0x3,
    NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT      = -0x4,
    NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED     = -0x5,
    NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND   = -0x06,
    NOTIFICATION_MODULE_RESULT_OVERLAY_NOT_READY     = -0x10,
    NOTIFICATION_MODULE_RESULT_UNSUPPORTED_TYPE      = -0x11,
    NOTIFICATION_MODULE_RESULT_ALLOCATION_FAILED     = -0x12,
    NOTIFICATION_MODULE_RESULT_INVALID_HANDLE        = -0x13,
    NOTIFICATION_MODULE_RESULT_UNKNOWN_ERROR         = -0x1000,
} NotificationModuleStatus;

typedef uint32_t NotificationModuleAPIVersion;
typedef uint32_t NotificationModuleHandle;


typedef void (*NotificationModuleNotificationFinishedCallback)(NotificationModuleHandle, void *);

#define NOTIFICATION_MODULE_API_VERSION_ERROR 0xFFFFFFFF

typedef struct _NMColor {
    uint8_t r, g, b, a;
} NMColor;

typedef enum NotificationModuleNotificationType {
    NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO    = 0, /* Static notification, fades out after fixed time. Can not be updated after creation. */
    NOTIFICATION_MODULE_NOTIFICATION_TYPE_ERROR   = 1, /* Static notification, fades out after fixed time, shakes for a fixed time. Can not be updated after creation. */
    NOTIFICATION_MODULE_NOTIFICATION_TYPE_DYNAMIC = 2, /* Dynamic notification, only fades out when told to fade out. Can be updated after creation. */
} NotificationModuleNotificationType;

typedef enum NotificationModuleStatusFinish {
    NOTIFICATION_MODULE_STATUS_FINISH            = 0, /* Fades out the Notification after `durationBeforeFadeOutInSeconds` seconds */
    NOTIFICATION_MODULE_STATUS_FINISH_WITH_SHAKE = 1, /* Fades out the Notification after `durationBeforeFadeOutInSeconds` seconds, shakes it for `shakeDurationInSeconds` seconds */
} NotificationModuleStatusFinish;

typedef enum NotificationModuleNotificationOption {
    NOTIFICATION_MODULE_DEFAULT_OPTION_BACKGROUND_COLOR,         /* Background Color of the Notification. Type: NMColor */
    NOTIFICATION_MODULE_DEFAULT_OPTION_TEXT_COLOR,               /* Text Color of the Notification. Type: NMColor */
    NOTIFICATION_MODULE_DEFAULT_OPTION_DURATION_BEFORE_FADE_OUT, /* Time in seconds before the Notification will fade out: Type: float. Example: 2.5f = 2.5 seconds*/
    NOTIFICATION_MODULE_DEFAULT_OPTION_FINISH_FUNCTION,          /* Function that will be called when the Notification starts to fade out. Type: NotificationModuleNotificationFinishedCallback*/
    NOTIFICATION_MODULE_DEFAULT_OPTION_FINISH_FUNCTION_CONTEXT,  /* Context that will be passed to the NOTIFICATION_MODULE_DEFAULT_TYPE_FINISH_FUNCTION callback. Type: void* */
} NotificationModuleNotificationOption;
