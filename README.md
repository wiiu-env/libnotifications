[![Publish Docker Image](https://github.com/wiiu-env/libnotifications/actions/workflows/push_image.yml/badge.svg)](https://github.com/wiiu-env/libnotifications/actions/workflows/push_image.yml)

# libnotifications

A Wii U homebrew library for displaying system-wide notifications.

**Prerequisites:**
* **Runtime:** Requires the [NotificationModule](https://github.com/wiiu-env/NotificationModule) to be running via [WUMSLoader](https://github.com/wiiu-env/WUMSLoader).
* **Build:** Requires [wut](https://github.com/devkitPro/wut) for building.

## Installation

You can install the library via the Makefile:
```
make install
```
## Build Setup

To use this library in your project, ensure your `Makefile` or build system is configured correctly.

1.  **Define WUMS_ROOT**:
```
WUMS_ROOT := $(DEVKITPRO)/wums
```
2.  **Update Library Flags**:
Add `-lnotifications` to your `LIBS` and include the WUMS path in `LIBDIRS`.
```
LIBS    := ... -lnotifications
LIBDIRS := ... $(WUMS_ROOT)
```
## Usage Examples

Include the main header to access the API:
```
#include <notifications/notifications.h>
```
### 1. Initialization
You **must** initialize the library before calling any notification functions. It is good practice to check the specific error code to diagnose issues (e.g., missing module).
```
void setup() {
    NotificationModuleStatus status = NotificationModule_InitLibrary();
    
    if (status != NOTIFICATION_MODULE_RESULT_SUCCESS) {
        if (status == NOTIFICATION_MODULE_RESULT_MODULE_NOT_FOUND) {
            // Critical Error: WUMSLoader or NotificationModule is not running.
        } else if (status == NOTIFICATION_MODULE_RESULT_UNSUPPORTED_VERSION) {
            // The installed NotificationModule version is incompatible.
        }
        // Do not proceed with using the library.
        return;
    }
}

void cleanup() {
    NotificationModule_DeInitLibrary();
}
```
### 2. Simple Notifications
Even simple notifications can fail if the overlay isn't ready or memory is low.

**Info Notification (Fades out automatically):**
```
if (const auto status = NotificationModule_AddInfoNotification("File saved successfully!") != NOTIFICATION_MODULE_RESULT_SUCCESS) {
    // Failed to display notification
    OSReport("Failed to display notification: Error %s\n", NotificationModule_GetStatusStr(status));
}
```


**Error Notification (Shakes and fades out):**
```
if (const auto status = NotificationModule_AddErrorNotification("Connection failed!") != NOTIFICATION_MODULE_RESULT_SUCCESS) {
    // Failed to display notification
    OSReport("Failed to display notification: Error %s\n", NotificationModule_GetStatusStr(status));
}
```
### 3. Dynamic Notifications
For dynamic notifications, it is critical to ensure the notification was successfully created before attempting to update or finish it.
```
NotificationModuleHandle handle;
NotificationModuleStatus status = NotificationModule_AddDynamicNotification("Downloading... 0%", &handle);

if (status == NOTIFICATION_MODULE_RESULT_SUCCESS) {
    // 1. Update the text during your process
    NotificationModule_UpdateDynamicNotificationText(handle, "Downloading... 50%");
    [...]
    // 2. Finish it when done (fades out after 2 seconds)
    NotificationModule_FinishDynamicNotification(handle, 2.0f);
} else {
    // Handle failure (e.g., NOTIFICATION_MODULE_RESULT_ALLOCATION_FAILED)
    // Note: 'handle' is undefined here and should not be used.
}
```

This example shows how to trigger a callback function when a dynamic notification finishes.
```
// 1. Define the callback
void OnFinished(NotificationModuleHandle handle, void* context) {
    // called when the notification finished fading out.
}

// 2. Usage
void ShowProgress() {
    NotificationModuleHandle handle;
    int myData = 42; // Example context data

    NotificationModuleStatus status = NotificationModule_AddDynamicNotificationWithCallback(
        "Loading... 0%", 
        &handle, 
        OnFinished, 
        (void *) &myData
    );

    if (status == NOTIFICATION_MODULE_RESULT_SUCCESS) {
        // Update status...
        NotificationModule_UpdateDynamicNotificationText(handle, "Loading... 100%");
        
        // Finish (triggers callback after fade out)
        NotificationModule_FinishDynamicNotification(handle, 1.5f);
    }
}
```
### 4. Customizing Defaults
Setting default values returns a status code which can indicate invalid arguments (e.g., passing a float when an int is expected).
```
// Define a custom color
NMColor green = {0, 128, 0, 255};

NotificationModuleStatus status = NotificationModule_SetDefaultValue(
    NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO,
    NOTIFICATION_MODULE_DEFAULT_OPTION_BACKGROUND_COLOR,
    green
);

if (status != NOTIFICATION_MODULE_RESULT_SUCCESS) {
    // This might happen if the type or option is invalid.
}
```
## Docker Integration

A prebuilt version of this lib can found on dockerhub. To use it for your projects, add this to your `Dockerfile`.
```
# ... previous stages ...
COPY --from=ghcr.io/wiiu-env/libnotifications:[tag] /artifacts $DEVKITPRO
# ... build stages ...
```
> **Note:** It is highly recommended to pin the version to a specific date tag (e.g., `:20230101`) rather than using `:latest` to ensure build reproducibility. Available tags can be found [here](https://github.com/wiiu-env/libnotifications/pkgs/container/libnotifications/versions).

## Development

**Format Code:**
```
docker run --rm -v ${PWD}:/src ghcr.io/wiiu-env/clang-format:13.0.0-2 -r ./source ./include -i
```