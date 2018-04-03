// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_ANDROID_SYS_UTILS_H_
#define BASE_ANDROID_SYS_UTILS_H_

#include "base/android/jni_android.h"
#include "base/time/time.h"

namespace base {
namespace android {

class BASE_EXPORT SysUtils {
 public:
  static bool Register(JNIEnv* env);

  // Returns true iff this is a low-end device.
  static bool IsLowEndDeviceFromJni();

  /// M: Record OnDraw, DrawGL and PerformIdleTasks times @{
  static bool CanPerformMoreIdleTask();
  static void RecordPerformIdleTaskStartTime();
  static void RecordPerformIdleTaskEndTime();

  static const size_t GetMaxIdleTasksCanPerform() {
    return IsPerformSyncIdle() ? 3 : 1;
  }
  static void SetIsImagination(bool isIMG);

  static void RecordOnDrawStartTime();
  static void RecordOnDrawEndTime();
  static void RecordDrawGLStartTime();
  static void RecordDrawGLEndTime();
  static void RecordOnTouchTime();
  static void RecordScrollToTime();

  static bool HasTouchEventDuringVsync();
  static bool CanExecuteCompositorTask();

  static const base::TimeDelta GetPeriodFromLastTouch();
  static const base::TimeDelta GetPeriodFromLastScroll();

  static bool IsPerformSyncIdle();
  /// @}

  /// M: for multi raster thread
  static int GetRasterThreadNum();
};

}  // namespace android
}  // namespace base

#endif  // BASE_ANDROID_SYS_UTILS_H_
