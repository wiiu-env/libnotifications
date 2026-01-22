#pragma once

#include "notification_defines.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Returns a NotificationModuleStatus as a string.
 *
 * @param[in] status The status code to convert.
 * @return String representation of a given status.
**/
const char *NotificationModule_GetStatusStr(NotificationModuleStatus status);

/**
 * Initializes the library and connects to the NotificationModule.
 *
 * This function has to be called before any other function of this lib (except NotificationModule_GetVersion) can be used.
 * It attempts to acquire the `homebrew_notifications` module via OSDynLoad.
 *
 * @return The status of the initialization.
 * @retval NOTIFICATION_MODULE_RESULT_SUCCESS                 The library has been initialized successfully. Other functions can now be used.
 * @retval NOTIFICATION_MODULE_RESULT_MODULE_NOT_FOUND        The `homebrew_notifications` module could not be found. Ensure WUMSLoader is running and the module is installed.
 * @retval NOTIFICATION_MODULE_RESULT_MODULE_MISSING_EXPORT   The module is loaded but missing an expected export.
 * @retval NOTIFICATION_MODULE_RESULT_UNSUPPORTED_VERSION     The version of the loaded module is not compatible with this version of the lib.
**/
NotificationModuleStatus NotificationModule_InitLibrary();

/**
 * Deinitializes the NotificationModule lib and releases resources.
 *
 * @return The status of the deinitialization.
 * @retval NOTIFICATION_MODULE_RESULT_SUCCESS                 The library was deinitialized successfully.
 */
NotificationModuleStatus NotificationModule_DeInitLibrary();

/**
 * Retrieves the API Version of the loaded NotificationModule. <br>
 * <br>
 * Requires NotificationModule API version 1 or higher.
 *
 * @param[out] outVersion pointer to the variable where the version will be stored.
 *
 * @return The status of the operation.
 * @retval NOTIFICATION_MODULE_RESULT_SUCCESS                 The API version has been stored in the version ptr.
 * @retval NOTIFICATION_MODULE_RESULT_MODULE_NOT_FOUND        The module could not be found. Make sure the module is loaded.
 * @retval NOTIFICATION_MODULE_RESULT_MODULE_MISSING_EXPORT   The module is missing an expected export.
 * @retval NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT        Invalid version pointer.
 * @retval NOTIFICATION_MODULE_RESULT_UNKNOWN_ERROR           Retrieving the module version failed.
**/
NotificationModuleStatus NotificationModule_GetVersion(NotificationModuleAPIVersion *outVersion);

/**
 * Checks if the Overlay for Notification is ready.  <br>
 * Notifications can only be added if the overlay is ready.  <br>
 * <br>
 * Requires NotificationModule API version 1 or higher. <br>
 * <br>
 * @param[out] outIsReady pointer to the variable where the result will be stored.
 * @return The status of the operation.
 * @retval NOTIFICATION_MODULE_RESULT_SUCCESS                 The result has been stored in the outIsReady pointer.
 * @retval NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT        Invalid version pointer.
 * @retval NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED       The library is not initialized.
 * @retval NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND     The loaded module version doesn't not support this function.
 */
NotificationModuleStatus NotificationModule_IsOverlayReady(bool *outIsReady);

/**
 * Can be used to override the default settings for a certain Notification Type.<br>
 * See the NotificationModuleNotificationType and NotificationModuleNotificationOption enums for more information. <br>
 * <br>
 * These default values will be use when calling: <br>
 * - NotificationModule_AddInfoNotification/NotificationModule_AddInfoNotificationWithCallback (type = NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO) <br>
 * - NotificationModule_AddErrorNotification/NotificationModule_AddErrorNotificationWithCallback  (type = NOTIFICATION_MODULE_NOTIFICATION_TYPE_ERROR) <br>
 * - NotificationModule_AddDynamicNotification/NotificationModule_AddDynamicNotificationWithCallback  (type = NOTIFICATION_MODULE_NOTIFICATION_TYPE_DYNAMIC) <br>
 * <br>
 * The "WithCallback" function will NOT take the default callback + context. <br>
 *
 * @param[in] type Type of Notification for which the default value will be set.
 * @param[in] optionType Defines which option will be set.
 * @param[in] ... Expected to be a single value matching the `optionType`:
 * - **BACKGROUND_COLOR**: Expects `NMColor`.
 * - **TEXT_COLOR**: Expects `NMColor`.
 * - **DURATION_BEFORE_FADE_OUT**: Expects `double`.
 * - **FINISH_FUNCTION**: Expects `NotificationModuleNotificationFinishedCallback`.
 * - **FINISH_FUNCTION_CONTEXT**: Expects `void*`.
 * - **KEEP_UNTIL_SHOWN**: Expects `bool`.
 *
 * @return The status of the operation.
 * @retval NOTIFICATION_MODULE_RESULT_SUCCESS                 The default value has been set.
 * @retval NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT        The given notification or option type was invalid.
 * @retval NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED       The library is not initialized.
 */
NotificationModuleStatus NotificationModule_SetDefaultValue(NotificationModuleNotificationType type,
                                                            NotificationModuleNotificationOption optionType,
                                                            ...);

/**
 * Displays a Notification that fade outs after a given time. <br>
 * Notification will appear in the top left corner. It's possible to display multiple notifications at the same time. <br>
 * The newest notification will always be at the top, the oldest at the bottom. Notifications will fade out to the left side. <br>
 * <br>
 * Requires NotificationModule API version 1 or higher. <br>
 * <br>
 * @param[in] text Content of the notification.
 * @param[in] durationBeforeFadeOutInSeconds Time in seconds before fading out.
 * @param[in] textColor Text color (RGBA) of the Notification.
 * @param[in] backgroundColor Background color (RGBA) of the Notification.
 * @param[in] callback Function that will be called then the Notification fades out.
 * @param[in] callbackContext Context that will be passed to the callback.
 * @param[in] keepUntilShown The Notification will be stored in a queue until it can be shown - even across application starts.
 * @return The status of the operation.
 * @retval NOTIFICATION_MODULE_RESULT_SUCCESS                 The notification was successfully added or queued.
 * @retval NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT        text was NULL.
 * @retval NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND     The loaded module version doesn't not support this function.
 * @retval NOTIFICATION_MODULE_RESULT_ALLOCATION_FAILED       Allocation of the Notification has failed.
 * @retval NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED       The library is not initialized.
 */
NotificationModuleStatus NotificationModule_AddInfoNotificationEx(const char *text,
                                                                  float durationBeforeFadeOutInSeconds,
                                                                  NMColor textColor,
                                                                  NMColor backgroundColor,
                                                                  NotificationModuleNotificationFinishedCallback callback,
                                                                  void *callbackContext,
                                                                  bool keepUntilShown);

/**
 * Convenience wrapper for NotificationModule_AddInfoNotificationEx().
 *
 * Uses default values for all parameters except `text`.
 * Default values can be overridden via NotificationModule_SetDefaultValue().
 *
 * @param[in] text Content of the Notification.
 * @return See NotificationModule_AddInfoNotificationEx() for return values.
 * @see NotificationModule_AddInfoNotificationEx
 * @see NotificationModule_SetDefaultValue
 */
NotificationModuleStatus NotificationModule_AddInfoNotification(const char *text);

/**
 * Convenience wrapper for NotificationModule_AddInfoNotificationEx().
 *
 * Uses default values for all parameters except `text`, `callback`, and `callbackContext`.
 * Default values can be overridden via NotificationModule_SetDefaultValue().
 *
 * @param[in] text Content of the Notification.
 * @param[in] callback Function that will be called then the Notification fades out.
 * @param[in] callbackContext Context that will be passed to the callback.
 * @return See NotificationModule_AddInfoNotificationEx() for return values.
 * @see NotificationModule_AddInfoNotificationEx
 * @see NotificationModule_SetDefaultValue
 */
NotificationModuleStatus NotificationModule_AddInfoNotificationWithCallback(const char *text,
                                                                            NotificationModuleNotificationFinishedCallback callback,
                                                                            void *callbackContext);

/**
 * Displays a (error) Notification that shakes and fade outs after a given time. <br>
 * Notification will appear in the top left corner. It's possible to display multiple notifications at the same time. <br>
 * The newest notification will always be at the top, the oldest at the bottom. Notifications will fade out to the left side. <br>
 * The Notification will at first shake for "shakeDurationInSeconds" seconds, then fades out after "durationBeforeFadeOutInSeconds" <br>
 * have been passed (since displaying the Notification, this include the shake duration).
 * <br>
 * Requires NotificationModule API version 1 or higher. <br>
 * <br>
 * @param[in] text Content of the notification.
 * @param[in] durationBeforeFadeOutInSeconds  Time in seconds before fading out.
 * @param[in] shakeDurationInSeconds Time in seconds the notification will "shake".
 * @param[in] textColor Text color (RGBA) of the Notification.
 * @param[in] backgroundColor Background color (RGBA) of the Notification.
 * @param[in] callback Function that will be called then the Notification fades out.
 * @param[in] callbackContext Context that will be passed to the callback.
 * @param[in] keepUntilShown The Notification will be stored in a queue until it can be shown - even across application starts.
 * @return The status of the operation.
 * @retval NOTIFICATION_MODULE_RESULT_SUCCESS                 The notification was successfully added or queued.
 * @retval NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT        text was NULL.
 * @retval NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND     The loaded module version doesn't not support this function.
 * @retval NOTIFICATION_MODULE_RESULT_ALLOCATION_FAILED       Allocation of the Notification has failed.
 * @retval NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED       The library is not initialized.
 */
NotificationModuleStatus NotificationModule_AddErrorNotificationEx(const char *text,
                                                                   float durationBeforeFadeOutInSeconds,
                                                                   float shakeDurationInSeconds,
                                                                   NMColor textColor,
                                                                   NMColor backgroundColor,
                                                                   NotificationModuleNotificationFinishedCallback callback,
                                                                   void *callbackContext,
                                                                   bool keepUntilShown);

/**
 * Convenience wrapper for NotificationModule_AddErrorNotificationEx().
 *
 * Uses default values for all parameters except `text`.
 * Default values can be overridden via NotificationModule_SetDefaultValue().
 *
 * @param[in] text Content of the notification.
 * @return See NotificationModule_AddErrorNotificationEx() for return values.
 * @see NotificationModule_AddErrorNotificationEx
 * @see NotificationModule_SetDefaultValue
 */
NotificationModuleStatus NotificationModule_AddErrorNotification(const char *text);

/**
 * Convenience wrapper for NotificationModule_AddErrorNotificationEx().
 *
 * Uses default values for all parameters except `text`, `callback`, and `callbackContext`.
 * Default values can be overridden via NotificationModule_SetDefaultValue().
 *
 * @param[in] text Content of the notification.
 * @param[in] callback Function that will be called then the Notification fades out.
 * @param[in] callbackContext Context that will be passed to the callback.
 * @return See NotificationModule_AddErrorNotificationEx() for return values.
 * @see NotificationModule_AddErrorNotificationEx
 * @see NotificationModule_SetDefaultValue
 */
NotificationModuleStatus NotificationModule_AddErrorNotificationWithCallback(const char *text,
                                                                             NotificationModuleNotificationFinishedCallback callback,
                                                                             void *callbackContext);

/**
 * Displays a Notification that can be updated and stays on the screen until `NotificationModule_FinishDynamicNotification*` has been called. <br>
 * <br>
 * This functions give you a NotificationHandle which is needed to finish or update this notification. <br>
 * <br>
 * Use the `NotificationModule_UpdateDynamicNotificationText*` functions to update the notification after creating it. <br>
 * <br>
 * The Notification will be deleted automatically when the running application changes.
 *
 * @param[in] text Content of the notification.
 * @param[out] outHandle Pointer where the resulting handle will be stored.
 * @param[in] textColor Text color (RGBA) of the notification.
 * @param[in] backgroundColor Background color (RGBA) of the notification.
 * @param[in] callback Function that will be called then the Notification fades out.
 * @param[in] callbackContext Context that will be passed to the callback.
 * @param[in] keepUntilShown The Notification will be stored in a queue until it can be shown - even across application starts.
 * @return The status of the operation.
 * @retval NOTIFICATION_MODULE_RESULT_SUCCESS                 The notification was successfully created.
 * @retval NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT        text or outHandle was NULL.
 * @retval NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND     The loaded module version doesn't not support this function.
 * @retval NOTIFICATION_MODULE_RESULT_ALLOCATION_FAILED       Allocation of the Notification has failed.
 * @retval NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED       The library is not initialized.
 */
NotificationModuleStatus NotificationModule_AddDynamicNotificationEx(const char *text,
                                                                     NotificationModuleHandle *outHandle,
                                                                     NMColor textColor,
                                                                     NMColor backgroundColor,
                                                                     NotificationModuleNotificationFinishedCallback callback,
                                                                     void *callbackContext,
                                                                     bool keepUntilShown);

/**
 * Convenience wrapper for NotificationModule_AddDynamicNotificationEx().
 *
 * Uses default values for all parameters except `text` and `outHandle`.
 * Default values can be overridden via NotificationModule_SetDefaultValue().
 *
 * @param[in] text Content of the notification.
 * @param[out] outHandle Pointer where the resulting handle will be stored.
 * @return See NotificationModule_AddDynamicNotificationEx() for return values.
 * @see NotificationModule_AddDynamicNotificationEx
 * @see NotificationModule_SetDefaultValue
*/
NotificationModuleStatus NotificationModule_AddDynamicNotification(const char *text,
                                                                   NotificationModuleHandle *outHandle);

/**
 * Convenience wrapper for NotificationModule_AddDynamicNotificationEx().
 *
 * Uses default values for all parameters except `text`, `outHandle`, `callback`, and `callbackContext`.
 * Default values can be overridden via NotificationModule_SetDefaultValue().
 *
 * @param[in] text Content of the notification.
 * @param[out] outHandle Pointer where the resulting handle will be stored.
 * @param[in] callback Function that will be called then the Notification fades out.
 * @param[in] callbackContext Context that will be passed to the callback.
 * @return See NotificationModule_AddDynamicNotificationEx() for return values.
 * @see NotificationModule_AddDynamicNotificationEx
 * @see NotificationModule_SetDefaultValue
*/
NotificationModuleStatus NotificationModule_AddDynamicNotificationWithCallback(const char *text,
                                                                               NotificationModuleHandle *outHandle,
                                                                               NotificationModuleNotificationFinishedCallback callback,
                                                                               void *callbackContext);

/**
 * Updates the text of a dynamic notification.
 * <br>
 * Requires NotificationModule API version 1 or higher. <br>
 * <br>
 * @param[in] handle Handle of the notification.
 * @param[in] text New text content.
 * @return The status of the operation.
 * @retval NOTIFICATION_MODULE_RESULT_SUCCESS                 The text has been updated.
 * @retval NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND     The loaded module version doesn't support this function.
 * @retval NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT        handle was NULL.
 * @retval NOTIFICATION_MODULE_RESULT_INVALID_HANDLE          handle was not found.
 * @retval NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED       The library is not initialized.
 */
NotificationModuleStatus NotificationModule_UpdateDynamicNotificationText(NotificationModuleHandle handle,
                                                                          const char *text);

/**
 * Updates the background color of a dynamic notification.
 * <br>
 * Requires NotificationModule API version 1 or higher. <br>
 * <br>
 * @param[in] handle Handle of the notification.
 * @param[in] backgroundColor New background color (RGBA).
 * @return The status of the operation.
 * @retval NOTIFICATION_MODULE_RESULT_SUCCESS                 The background color has been updated.
 * @retval NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND     The loaded module version doesn't support this function.
 * @retval NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT        handle was NULL.
 * @retval NOTIFICATION_MODULE_RESULT_INVALID_HANDLE          handle was not found.
 * @retval NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED       The library is not initialized.
 */
NotificationModuleStatus NotificationModule_UpdateDynamicNotificationBackgroundColor(NotificationModuleHandle handle,
                                                                                     NMColor backgroundColor);

/**
 * Updates the text color of a dynamic notification. <br>
 * <br>
 * Requires NotificationModule API version 1 or higher. <br>
 * <br>
 * @param[in] handle Handle of the notification.
 * @param[in] textColor New text color (RGBA).
 * @return The status of the operation.
 * @retval NOTIFICATION_MODULE_RESULT_SUCCESS                 The text color has been updated.
 * @retval NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND     The loaded module version doesn't support this function.
 * @retval NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT        handle was NULL.
 * @retval NOTIFICATION_MODULE_RESULT_INVALID_HANDLE          handle was not found.
 * @retval NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED       The library is not initialized.
 */
NotificationModuleStatus NotificationModule_UpdateDynamicNotificationTextColor(NotificationModuleHandle handle,
                                                                               NMColor textColor);

/**
 * Fades out a existing dynamic notification.
 *
 * Calls the callback of the notification afters fading out.
 *
 * Requires NotificationModule API version 1 or higher. <br>
 *
 * @param[in] handle handle of the notification to fade out.
 * @param[in] durationBeforeFadeOutInSeconds duration before fading out in seconds.
 * @return The status of the operation.
 * @retval NOTIFICATION_MODULE_RESULT_SUCCESS                 Finish the given notification.
 * @retval NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND     The loaded module version doesn't support this function.
 * @retval NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT        handle was NULL.
 * @retval NOTIFICATION_MODULE_RESULT_INVALID_HANDLE          handle was not found.
 * @retval NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED       The library is not initialized.
 * @see NotificationModule_FinishDynamicNotificationWithShake
 */
NotificationModuleStatus NotificationModule_FinishDynamicNotification(NotificationModuleHandle handle,
                                                                      float durationBeforeFadeOutInSeconds);

/**
 * Fades out a existing dynamic notification after shaking it.
 *
 * The Notification will at first shake for "shakeDuration" seconds, then fades out after "durationBeforeFadeOutInSeconds".
 * Calls the callback of the notification after fading out.
 *
 * Requires NotificationModule API version 1 or higher. <br>
 *
 * @param[in] handle handle of the notification to fade out.
 * @param[in] durationBeforeFadeOutInSeconds duration before fading out in seconds.
 * @param[in] shakeDuration shake duration in seconds.
 * @return The status of the operation.
 * @retval NOTIFICATION_MODULE_RESULT_SUCCESS                 Finish the given notification with a shake.
 * @retval NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND     The loaded module version doesn't  support this function.
 * @retval NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT        handle was NULL.
 * @retval NOTIFICATION_MODULE_RESULT_INVALID_HANDLE          handle was not found.
 * @retval NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED       The library is not initialized.
 * @see NotificationModule_FinishDynamicNotification
 */
NotificationModuleStatus NotificationModule_FinishDynamicNotificationWithShake(NotificationModuleHandle handle,
                                                                               float durationBeforeFadeOutInSeconds,
                                                                               float shakeDuration);

#ifdef __cplusplus
}
#endif

// Idea taken from libcurl
#if defined(__GNUC__) && defined(__GNUC_MINOR__) && \
        ((__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3))
#include "typechecks-gcc.h"
#else
#if defined(__STDC__) && (__STDC__ >= 1)
/* This preprocessor magic that replaces a call with the exact same call is
       only done to make sure application authors pass exactly three arguments
       to these functions. */

#define NotificationModule_SetDefaultValue(type, valueType, param) NotificationModule_SetDefaultValue(type, valueType, param)
#endif /* __STDC__ >= 1 */
#endif /* gcc >= 4.3 */