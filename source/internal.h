#pragma once

#include "notifications/notification_defines.h"

struct NMDefaultValueStore {
    float durationBeforeFadeOutInSeconds                        = 2.0f;
    float shakeDurationOnErrorInSeconds                         = 0.5f;
    NMColor backgroundColor                                     = {100, 100, 100, 255};
    NMColor textColor                                           = {255, 255, 255, 255};
    void (*finishFunc)(NotificationModuleHandle, void *context) = nullptr;
    void *finishFuncContext                                     = nullptr;
};