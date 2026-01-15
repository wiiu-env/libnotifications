#pragma once

#include "notification_defines.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Returns a NotificationModuleStatus as a string
 * @param status
 * @return String representation of a given status
**/
const char *NotificationModule_GetStatusStr(NotificationModuleStatus status);

/**
 * This function has to be called before any other function of this lib (except NotificationModule_GetVersion) can be used.
 *
 * @return  NOTIFICATION_MODULE_RESULT_SUCCESS:                 The library has been initialized successfully. Other functions can now be used.<br>
 *          NOTIFICATION_MODULE_RESULT_MODULE_NOT_FOUND:        The module could not be found. Make sure the module is loaded.<br>
 *          NOTIFICATION_MODULE_RESULT_MODULE_MISSING_EXPORT:   The module is missing an expected export.<br>
 *          NOTIFICATION_MODULE_RESULT_UNSUPPORTED_API_VERSION: The version of the loaded module is not compatible with this version of the lib.
**/
NotificationModuleStatus NotificationModule_InitLibrary();

/**
 * Deinitializes the NotificationModule lib
 * @return NOTIFICATION_MODULE_RESULT_SUCCESS or NOTIFICATION_MODULE_RESULT_UNKNOWN_ERROR
 */
NotificationModuleStatus NotificationModule_DeInitLibrary();

/**
 * Retrieves the API Version of the loaded NotificationModule. <br>
 * <br>
 * Requires NotificationModule API version 1 or higher
 * @param outVersion pointer to the variable where the version will be stored.
 *
 * @return NOTIFICATION_MODULE_RESULT_SUCCESS:               The API version has been store in the version ptr.<br>
 *         NOTIFICATION_MODULE_RESULT_MODULE_NOT_FOUND:      The module could not be found. Make sure the module is loaded.<br>
 *         NOTIFICATION_MODULE_RESULT_MODULE_MISSING_EXPORT: The module is missing an expected export.<br>
 *         NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT:      Invalid version pointer.<br>
 *         NOTIFICATION_MODULE_RESULT_UNKNOWN_ERROR:         Retrieving the module version failed.
**/
NotificationModuleStatus NotificationModule_GetVersion(NotificationModuleAPIVersion *outVersion);

/**
 * Checks if the Overlay for Notification is ready.  <br>
 * Notifications can only be added if the overlay is ready.  <br>
 * <br>
 * Requires NotificationModule API version 1 or higher <br>
 * <br>
 * @param outIsReady pointer to the variable where the result will be stored.
 * @return NOTIFICATION_MODULE_RESULT_SUCCESS:               The result has been stored in the outIsReady pointer.<br>
 *         NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT:      Invalid version pointer.<br>
 *         NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED:     The library is not initialized<br>
 *         NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND:   The loaded module version doesn't not support this function.<br>
 */
NotificationModuleStatus NotificationModule_IsOverlayReady(bool *outIsReady);

/**
 * Can be used to override the default settings for a certain Notification Type.<br>
 * See the NotificationModuleNotificationType and NotificationModuleNotificationOption enums for more information. <br>
 * <br>
 * These default values will be use when calling <br>
 * - NotificationModule_AddInfoNotification/NotificationModule_AddInfoNotificationWithCallback (type = NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO) <br>
 * - NotificationModule_AddErrorNotification/NotificationModule_AddErrorNotificationWithCallback  (type = NOTIFICATION_MODULE_NOTIFICATION_TYPE_ERROR) <br>
 * - NotificationModule_AddDynamicNotification/NotificationModule_AddDynamicNotificationWithCallback  (type = NOTIFICATION_MODULE_NOTIFICATION_TYPE_DYNAMIC) <br>
 *  <br>
 * The "WithCallback" function will NOT take the default callback + context. <br>
 *
 * @param type Type of Notification for which the default value will be set.
 * @param optionType Defines which option will be set
 * @param ... Expected to be a single value. Depends on the option type. See NotificationModuleNotificationOption enum for more information.
 * @return NOTIFICATION_MODULE_RESULT_SUCCESS:               The default value has been set.<br>
 *         NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT:      The given notification or option type was invalid <br>
 *         NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED:     The library is not initialized<br>
 */
NotificationModuleStatus NotificationModule_SetDefaultValue(NotificationModuleNotificationType type,
                                                            NotificationModuleNotificationOption optionType,
                                                            ...);

/**
 * Displays a Notification that fade outs after a given time. <br>
 * Notification will appear in the top left corner. It's possible to display multiple notifications at the same time. <br>
 * The newest notification will always be at the top, the oldest at the bottom. Notifications will fade out to the left side. <br>
 * <br>
 * Requires NotificationModule API version 1 or higher <br>
 * <br>
 * @param text Content of the notification.
 * @param durationBeforeFadeOutInSeconds Time in seconds before fading out
 * @param textColor Text color of the Notification
 * @param backgroundColor Background color of the Notification
 * @param callback Function that will be called then the Notification fades out.
 * @param callbackContext Context that will be passed to the callback.
 * @param keepUntilShown The Notification will be stored in a queue until can be shown - even accross application starts
 * @return NOTIFICATION_MODULE_RESULT_SUCCESS:               The default value has been set.<br>
 *         NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT:      text was NULL.<br>
 *         NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND:   The loaded module version doesn't not support this function.<br>
 *         NOTIFICATION_MODULE_RESULT_OVERLAY_NOT_READY:     The overlay is not ready. See NotificationModule_IsOverlayReady.<br>
 *         NOTIFICATION_MODULE_RESULT_ALLOCATION_FAILED:     Allocation of the Notification has failed.<br>
 *         NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED:     The library is not initialized.<br>
 */
NotificationModuleStatus NotificationModule_AddInfoNotificationEx(const char *text,
                                                                  float durationBeforeFadeOutInSeconds,
                                                                  NMColor textColor,
                                                                  NMColor backgroundColor,
                                                                  NotificationModuleNotificationFinishedCallback callback,
                                                                  void *callbackContext,
                                                                  bool keepUntilShown);

/**
 * Displays a Notification that fade outs after a given time. <br>
 * Similar to NotificationModule_AddInfoNotificationEx, but uses default values for all parameters except "text". <br>
 * The default values are NotificationModule_AddInfoNotificationEx(text, 2.0f, {255, 255, 255, 255}, {100, 100, 100, 255}, nullptr, nullptr); but can be <br>
 * overridden via NotificationModule_SetDefaultValue(NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO, xxx, yyy); <br>
 * <br>
 * Requires NotificationModule API version 1 or higher <br>
 * <br>
 * @param text Content of the Notification
 * @return NOTIFICATION_MODULE_RESULT_SUCCESS:               The default value has been set.<br>
 *         NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT:      text was NULL.<br>
 *         NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND:   The loaded module version doesn't not support this function.<br>
 *         NOTIFICATION_MODULE_RESULT_OVERLAY_NOT_READY:     The overlay is not ready. See NotificationModule_IsOverlayReady.<br>
 *         NOTIFICATION_MODULE_RESULT_ALLOCATION_FAILED:     Allocation of the Notification has failed.<br>
 *         NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED:     The library is not initialized.<br>
 */
NotificationModuleStatus NotificationModule_AddInfoNotification(const char *text);

/**
 * Similar to NotificationModule_AddInfoNotification, but uses default values for all parameters except "text", callback, callbackContext<br>
 * <br>
 * Requires NotificationModule API version 1 or higher <br>
 * <br>
 * @param text Content of the Notification
 * @param callback Function that will be called then the Notification fades out.
 * @param callbackContext Context that will be passed to the callback.
 * @return NOTIFICATION_MODULE_RESULT_SUCCESS:               The default value has been set.<br>
 *         NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT:      text was NULL.<br>
 *         NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND:   The loaded module version doesn't not support this function.<br>
 *         NOTIFICATION_MODULE_RESULT_OVERLAY_NOT_READY:     The overlay is not ready. See NotificationModule_IsOverlayReady.<br>
 *         NOTIFICATION_MODULE_RESULT_ALLOCATION_FAILED:     Allocation of the Notification has failed.<br>
 *         NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED:     The library is not initialized.<br>
 */
NotificationModuleStatus NotificationModule_AddInfoNotificationWithCallback(const char *text,
                                                                            NotificationModuleNotificationFinishedCallback callback,
                                                                            void *callbackContext);

/**
 * Displays a (error) Notification that shakes and fade outs after a given time. <br>
 * Notification will appear in the top left corner. It's possible to display multiple notifications at the same time. <br>
 * The newest notification will always be at the top, the oldest at the bottom. Notifications will fade out to the left side. <br>
 * The Notification will at first shake for "shakeDurationInSeconds" seconds, then fades out after "durationBeforeFadeOutInSeconds" <br>
 * have been passed (since displaying the Notification, this include the shake duration)
 * <br>
 * Requires NotificationModule API version 1 or higher <br>
 * <br>
 * @param text Content of the notification.
 * @param durationBeforeFadeOutInSeconds  Time in seconds before fading out
 * @param shakeDurationInSeconds Time in seconds the notification will "shake"
 * @param textColor Text color of the Notification
 * @param backgroundColor Background color of the Notification
 * @param callback Function that will be called then the Notification fades out.
 * @param callbackContext Context that will be passed to the callback.
 * @param keepUntilShown The Notification will be stored in a queue until can be shown - even accross application starts
 * @return NOTIFICATION_MODULE_RESULT_SUCCESS:               The default value has been set.<br>
 *         NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT:      text was NULL.<br>
 *         NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND:   The loaded module version doesn't not support this function.<br>
 *         NOTIFICATION_MODULE_RESULT_OVERLAY_NOT_READY:     The overlay is not ready. See NotificationModule_IsOverlayReady.<br>
 *         NOTIFICATION_MODULE_RESULT_ALLOCATION_FAILED:     Allocation of the Notification has failed.<br>
 *         NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED:     The library is not initialized.<br>
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
 * Displays a (error) Notification (default background color: red) that shakes and fade outs after a given time. <br>
 * Similar to NotificationModule_AddErrorNotificationEx, but uses default values for all parameters except "text". <br>
 * The default values are NotificationModule_AddErrorNotificationEx(text, 2.0f, 0.5f, {255, 255, 255, 255}, {237, 28, 36, 255}, nullptr, nullptr); but can be <br>
 * overridden via NotificationModule_SetDefaultValue(NOTIFICATION_MODULE_NOTIFICATION_TYPE_ERROR, xxx, yyy); <br>
 * <br>
 * Requires NotificationModule API version 1 or higher <br>
 * <br>
 * @param text Content of the notification.
 * @return NOTIFICATION_MODULE_RESULT_SUCCESS:               The default value has been set.<br>
 *         NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT:      text was NULL.<br>
 *         NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND:   The loaded module version doesn't not support this function.<br>
 *         NOTIFICATION_MODULE_RESULT_OVERLAY_NOT_READY:     The overlay is not ready. See NotificationModule_IsOverlayReady.<br>
 *         NOTIFICATION_MODULE_RESULT_ALLOCATION_FAILED:     Allocation of the Notification has failed.<br>
 *         NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED:     The library is not initialized.<br>
 */
NotificationModuleStatus NotificationModule_AddErrorNotification(const char *text);

/**
 * Similar to NotificationModule_AddErrorNotification, but uses default values for all parameters except "text", callback, callbackContext<br>
 * <br>
 * Requires NotificationModule API version 1 or higher <br>
 * <br>
 * @param text Content of the notification.
 * @param callback Function that will be called then the Notification fades out.
 * @param callbackContext Context that will be passed to the callback.
 * @return NOTIFICATION_MODULE_RESULT_SUCCESS:               The notification will show up.<br>
 *         NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT:      text was NULL.<br>
 *         NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND:   The loaded module version doesn't not support this function.<br>
 *         NOTIFICATION_MODULE_RESULT_OVERLAY_NOT_READY:     The overlay is not ready. See NotificationModule_IsOverlayReady.<br>
 *         NOTIFICATION_MODULE_RESULT_ALLOCATION_FAILED:     Allocation of the Notification has failed.<br>
 *         NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED:     The library is not initialized.<br>
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
 * @param text Content of the notification
 * @param outHandle Pointer where the resulting
 * @param textColor Text color of the notification
 * @param backgroundColor Background color of the notification
 * @param callback Function that will be called then the Notification fades out.
 * @param callbackContext Context that will be passed to the callback.
 * @param keepUntilShown The Notification will be stored in a queue until can be shown - even accross application starts
 * @return NOTIFICATION_MODULE_RESULT_SUCCESS:               The default value has been set.<br>
 *         NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT:      text or outHandle was NULL <br>
 *         NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND:   The loaded module version doesn't not support this function.<br>
 *         NOTIFICATION_MODULE_RESULT_OVERLAY_NOT_READY:     The overlay is not ready. See NotificationModule_IsOverlayReady.<br>
 *         NOTIFICATION_MODULE_RESULT_ALLOCATION_FAILED:     Allocation of the Notification has failed.<br>
 *         NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED:     The library is not initialized.<br>
 */
NotificationModuleStatus NotificationModule_AddDynamicNotificationEx(const char *text,
                                                                     NotificationModuleHandle *outHandle,
                                                                     NMColor textColor,
                                                                     NMColor backgroundColor,
                                                                     NotificationModuleNotificationFinishedCallback callback,
                                                                     void *callbackContext,
                                                                     bool keepUntilShown);

/**
 * Displays a Notification that can be updated and stays on the screen until `NotificationModule_FinishDynamicNotification*` has been called. <br>
 * Similar to NotificationModule_AddDynamicNotificationEx, but uses default values for all parameters except "text". <br>
 * The default values are NotificationModule_AddDynamicNotificationEx(text, outHandle, {255, 255, 255, 255}, {100, 100, 100, 255}, nullptr, nullptr); but can be <br>
 * overridden via NotificationModule_SetDefaultValue(NOTIFICATION_MODULE_NOTIFICATION_TYPE_DYNAMIC, xxx, yyy); <br>
 * <br>
 * Requires NotificationModule API version 1 or higher <br>
 * @param text Content of the notification
 * @param outHandle Pointer where the resulting
 * @return NOTIFICATION_MODULE_RESULT_SUCCESS:               The default value has been set.<br>
 *         NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT:      text or outHandle was NULL <br>
 *         NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND:   The loaded module version doesn't not support this function.<br>
 *         NOTIFICATION_MODULE_RESULT_OVERLAY_NOT_READY:     The overlay is not ready. See NotificationModule_IsOverlayReady.<br>
 *         NOTIFICATION_MODULE_RESULT_ALLOCATION_FAILED:     Allocation of the Notification has failed.<br>
 *         NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED:     The library is not initialized.<br>
*/
NotificationModuleStatus NotificationModule_AddDynamicNotification(const char *text,
                                                                   NotificationModuleHandle *outHandle);

/**
 * Displays a Notification that can be updated and stays on the screen until `NotificationModule_FinishDynamicNotification*` has been called. <br>
 * Similar to NotificationModule_AddDynamicNotificationEx, but uses default values for all parameters except "text", callback and callbackContext. <br>
 * The default values are NotificationModule_AddDynamicNotificationEx(text, outHandle, {255, 255, 255, 255}, {100, 100, 100, 255}, callback, callbackContext); but can be <br>
 * overridden via NotificationModule_SetDefaultValue(NOTIFICATION_MODULE_NOTIFICATION_TYPE_DYNAMIC, xxx, yyy); <br>
 * <br>
 * Requires NotificationModule API version 1 or higher <br>
 *
 * @param text Content of the notification
 * @param outHandle Pointer where the resulting
 * @param callback Function that will be called then the Notification fades out.
 * @param callbackContext Context that will be passed to the callback.
 * @return NOTIFICATION_MODULE_RESULT_SUCCESS:               The default value has been set.<br>
 *         NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT:      text or outHandle was NULL <br>
 *         NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND:   The loaded module version doesn't not support this function.<br>
 *         NOTIFICATION_MODULE_RESULT_OVERLAY_NOT_READY:     The overlay is not ready. See NotificationModule_IsOverlayReady.<br>
 *         NOTIFICATION_MODULE_RESULT_ALLOCATION_FAILED:     Allocation of the Notification has failed.<br>
 *         NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED:     The library is not initialized.<br>
*/
NotificationModuleStatus NotificationModule_AddDynamicNotificationWithCallback(const char *text,
                                                                               NotificationModuleHandle *outHandle,
                                                                               NotificationModuleNotificationFinishedCallback callback,
                                                                               void *callbackContext);

/**
 * Updates the text of a dynamic notification.
 * <br>
 * Requires NotificationModule API version 1 or higher <br>
 * <br>
 * @param handle Handle of the notification
 * @param textColor New background color
 * @return NOTIFICATION_MODULE_RESULT_SUCCESS:              The text has been updated.<br>
 *         NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND:  The loaded module version doesn't not support this function.<br>
 *         NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT:     handle was NULL.<br>
 *         NOTIFICATION_MODULE_RESULT_INVALID_HANDLE:       handle was not found.<br>
 *         NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED:    The library is not initialized.<br>
 */
NotificationModuleStatus NotificationModule_UpdateDynamicNotificationText(NotificationModuleHandle handle,
                                                                          const char *text);

/**
 * Updates the background color of a dynamic notification.
 * <br>
 * Requires NotificationModule API version 1 or higher <br>
 * <br>
 * @param handle Handle of the notification
 * @param textColor New background color
 * @return NOTIFICATION_MODULE_RESULT_SUCCESS:              The background color has been updated.<br>
 *         NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND:  The loaded module version doesn't not support this function.<br>
 *         NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT:     handle was NULL.<br>
 *         NOTIFICATION_MODULE_RESULT_INVALID_HANDLE:       handle was not found.<br>
 *         NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED:    The library is not initialized.<br>
 */
NotificationModuleStatus NotificationModule_UpdateDynamicNotificationBackgroundColor(NotificationModuleHandle handle,
                                                                                     NMColor backgroundColor);

/**
 * Updates the text color of a dynamic notification. <br>
 * <br>
 * Requires NotificationModule API version 1 or higher <br>
 * <br>
 * @param handle Handle of the notification
 * @param textColor New text color
 * @return NOTIFICATION_MODULE_RESULT_SUCCESS:              The text color has been updated.<br>
 *         NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND:  The loaded module version doesn't not support this function.<br>
 *         NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT:     handle was NULL.<br>
 *         NOTIFICATION_MODULE_RESULT_INVALID_HANDLE:       handle was not found.<br>
 *         NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED:    The library is not initialized.<br>
 */
NotificationModuleStatus NotificationModule_UpdateDynamicNotificationTextColor(NotificationModuleHandle handle,
                                                                               NMColor textColor);

/**
 * Fades out a existing dynamic notification `durationBeforeFadeOutInSeconds` seconds after calling this funcion. <br>
 * Calls the callback of the notification before fading out.  <br>
 * <br>
 * Requires NotificationModule API version 1 or higher <br>
 * <br>
 * @param handle handle of the notification to fade out.
 * @param durationBeforeFadeOutInSeconds duration before fading out in seconds.
 * @return NOTIFICATION_MODULE_RESULT_SUCCESS:              Finish the given notification.<br>
 *         NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND:  The loaded module version doesn't not support this function.<br>
 *         NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT:     handle was NULL.<br>
 *         NOTIFICATION_MODULE_RESULT_INVALID_HANDLE:       handle was not found.<br>
 *         NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED:    The library is not initialized.<br>
 */
NotificationModuleStatus NotificationModule_FinishDynamicNotification(NotificationModuleHandle handle,
                                                                      float durationBeforeFadeOutInSeconds);

/**
 * Shakes an exiting notification for `shakeDuration` seconds and fades it out `durationBeforeFadeOutInSeconds` seconds after calling this function. <br>
 * Calls the callback of the notification before fading out.  <br>
 * <br>
 * Requires NotificationModule API version 1 or higher <br>
 * <br>
 * @param handle handle of the notification to fade out.
 * @param durationBeforeFadeOutInSeconds duration before fading out in seconds.
 * @param shakeDuration shake duration in seconds.
 * @return NOTIFICATION_MODULE_RESULT_SUCCESS:              Finish the given notification with a shake.<br>
 *         NOTIFICATION_MODULE_RESULT_UNSUPPORTED_COMMAND:  The loaded module version doesn't not support this function.<br>
 *         NOTIFICATION_MODULE_RESULT_INVALID_ARGUMENT:     handle was NULL.<br>
 *         NOTIFICATION_MODULE_RESULT_INVALID_HANDLE:       handle was not found.<br>
 *         NOTIFICATION_MODULE_RESULT_LIB_UNINITIALIZED:    The library is not initialized.<br>
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
