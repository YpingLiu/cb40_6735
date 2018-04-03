// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/android/sys_utils.h"

#include "base/android/build_info.h"
#include "base/sys_info.h"
#include "jni/SysUtils_jni.h"

/// M: to get resolution of device
#include "ui/gfx/android/device_display_info.h"

namespace base {
namespace android {

bool SysUtils::Register(JNIEnv* env) {
  return RegisterNativesImpl(env);
}

bool SysUtils::IsLowEndDeviceFromJni() {
  JNIEnv* env = AttachCurrentThread();
  return Java_SysUtils_isLowEndDevice(env);
}

/// M: Record OnDraw, DrawGL and PerformIdleTasks times @{
/// ondraw duration
static base::Time g_ondraw_start_time;
static base::Time g_ondraw_end_time;
/// drawgl duration
static base::Time g_drawgl_start_time;
static base::Time g_drawgl_end_time;

/// touch time
static base::Time g_ontouch_time;

/// scroll time
static base::Time g_scrollto_time;

/// lock to protect g_ontouch_time and g_scrollto_time
static base::Lock g_record_touch_time_lock;
/// lock to protect g_ondraw_start/end_time, g_draw_gl_start/end_time
static base::Lock g_record_time_lock;

/// ondraw duration history
static std::deque<base::TimeDelta> g_ondraw_durations;
static base::TimeDelta g_total_ondraw_duration;

/// perform idle task duration history
static base::Time g_perform_idle_task_start_time;
static std::deque<base::TimeDelta> g_perform_idle_task_durations;
static base::TimeDelta g_total_perform_idle_task_duration;

#if ENABLE_LOG
static base::Time g_last_ondraw_start_time;
#endif
static bool is_imagination_vendor_;
/// @}

/// M: Schedule the Idle tasks to avoid block drawGL @{
///  IMG gpu is using AsyncPixelTransferManagerIdle
///  whick upload texture on render thread
///  To Avoid block drawGL, calculate how much time can perform idle task.
static const base::Time g_zero_time;
static const base::TimeDelta g_vsync_duration = base::TimeDelta::FromMillisecondsD(16.67f);

#define MAX_NUM_DURATION_RECORD 10

/// run on render thread
bool SysUtils::CanPerformMoreIdleTask() {
  if (!IsPerformSyncIdle())
    return true;

  base::Time now = base::Time::Now();
  base::AutoLock lock(g_record_time_lock);
  base::TimeDelta ondraw_start_to_now = now - g_ondraw_start_time;

#if ENABLE_LOG
  LOG(INFO)<<"[CanPerformMoreIdleTask] =a= ["<<ondraw_start_to_now.InMillisecondsF()<<"] ondrawStart=["
    <<g_ondraw_start_time.ToJavaTime()<<"] end=["<<g_ondraw_end_time.ToJavaTime()<<"]";
#endif
  /// last on draw is longer than a vsync
  if (ondraw_start_to_now > g_vsync_duration)
    return true;

  /// in drawGL fuction, dont perform
  if (g_drawgl_start_time > g_zero_time && g_drawgl_end_time == g_zero_time)
    return false;

///
//  The situation we care, current in on drawing.
//               Now
//          vsync |       vsync
//            +   |         +
//+------------------+----------------+
//            |OnDraw|      |
//            +------+      |
//            |   |         |
//            |   |         |
//            |   |         |
//+------------------+----------------+
//  DrawGL    |   |  |DrawGL|
//            +   |  +------+
//                +
//               Now
  if (ondraw_start_to_now < g_vsync_duration
      && g_ondraw_end_time == g_zero_time) {
    double average_perform_idle
      = g_total_perform_idle_task_duration.InMillisecondsF() / g_perform_idle_task_durations.size();
    double average_ondraw = g_total_ondraw_duration.InMillisecondsF() / g_ondraw_durations.size();
#if ENABLE_LOG
    LOG(INFO)<<"[CanPerformMoreIdleTask] =b= ["<<average_perform_idle<<"]["<<average_ondraw<<"]";
#endif
    if ((average_ondraw - ondraw_start_to_now.InMillisecondsF()) > (average_perform_idle + 0.8))
      return true;
  }

  return false;
}

/// ONLY for main thread
void SysUtils::RecordOnDrawStartTime() {
  base::AutoLock lock(g_record_time_lock);
  g_ondraw_start_time = base::Time::Now();
#if ENABLE_LOG
  LOG(INFO)<<"[OnDraw] ++++++++ ["<<g_ondraw_start_time.ToJavaTime()<<"] ["
    <<(((g_ondraw_start_time-g_last_ondraw_start_time).InMillisecondsF() > 15)?"XXXXX":"")<<"]";
  g_last_ondraw_start_time = g_ondraw_start_time;
#endif
}

/// ONLY for main thread
void SysUtils::RecordOnDrawEndTime() {
  base::AutoLock lock(g_record_time_lock);
  g_ondraw_end_time = base::Time::Now();
  base::TimeDelta duration = g_ondraw_end_time - g_ondraw_start_time;
  if (g_ondraw_durations.size() == MAX_NUM_DURATION_RECORD) {
    g_total_ondraw_duration -= g_ondraw_durations.front();
    g_ondraw_durations.pop_front();
  }
  g_ondraw_durations.push_back(duration);
  g_total_ondraw_duration += duration;
#if ENABLE_LOG
  LOG(INFO)<<"[OnDraw] -------- ["<<g_ondraw_end_time.ToJavaTime()<<"] duration=["
    <<duration.InMillisecondsF()<<"] total=["<<g_total_ondraw_duration.InMillisecondsF()<<"]["
    <<g_ondraw_durations.size()<<"]";
#endif
}

/// ONLY for render thread
void SysUtils::RecordDrawGLStartTime() {
  g_drawgl_start_time = base::Time::Now();
  g_drawgl_end_time = g_zero_time;
#if ENABLE_LOG
  LOG(INFO)<<"[DrawGL] ++++++++ ["<<g_drawgl_start_time.ToJavaTime()<<"]";
#endif
}

/// ONLY for render thread
void SysUtils::RecordDrawGLEndTime() {
  g_drawgl_end_time = base::Time::Now();
  bool need_reset = true;
  {
    base::AutoLock lock(g_record_time_lock);
    // check the drawgl is triggered by last ondraw
    if (!(g_ondraw_start_time > g_drawgl_start_time)) {
      g_ondraw_start_time = g_zero_time;
      g_ondraw_end_time = g_zero_time;
    }
  }

#if ENABLE_LOG
  LOG(INFO)<<"[DrawGL] -------- ["<<g_drawgl_end_time.ToJavaTime()<<"] duration=["
    <<(g_drawgl_end_time-g_drawgl_start_time).InMillisecondsF()<<"]";
#endif
}

/// ONLY for render thread
void SysUtils::RecordPerformIdleTaskStartTime() {
  g_perform_idle_task_start_time = base::Time::Now();
}

/// Only For render thread
void SysUtils::RecordPerformIdleTaskEndTime() {
  base::TimeDelta duration = base::Time::Now() - g_perform_idle_task_start_time;
  if (g_perform_idle_task_durations.size() == MAX_NUM_DURATION_RECORD) {
    g_total_perform_idle_task_duration -= g_perform_idle_task_durations.front();
    g_perform_idle_task_durations.pop_front();
  }
  g_perform_idle_task_durations.push_back(duration);
  g_total_perform_idle_task_duration += duration;
#if ENABLE_LOG
  LOG(INFO)<<"[RecordPerformIdleTaskEndTime] ["<<duration.InMillisecondsF()<<"] total=["
    <<g_total_perform_idle_task_duration.InMillisecondsF()<<"] size=["<<g_perform_idle_task_durations.size()<<"]";
#endif
}

/// Only for main thread
void SysUtils::RecordOnTouchTime() {
  base::AutoLock lock(g_record_touch_time_lock);
  g_ontouch_time = base::Time::Now();
}

/// Only for main thread
void SysUtils::RecordScrollToTime() {
  base::AutoLock lock(g_record_touch_time_lock);
  g_scrollto_time = base::Time::Now();
}

/// only for main thread
bool SysUtils::HasTouchEventDuringVsync() {
  base::Time now = base::Time::Now();
  if (now - g_ontouch_time < g_vsync_duration)
    return true;
  return false;
}

/// Only for Main thread
bool SysUtils::CanExecuteCompositorTask() {
  if (!IsPerformSyncIdle())
    return true;
  base::Time now = base::Time::Now();
  base::TimeDelta touch_to_now = now - g_ontouch_time;

  static const base::TimeDelta max_task_threshold = base::TimeDelta::FromMillisecondsD(12.0f);
#if ENABLE_LOG
  LOG(INFO)<<"[CanExecuteCompositorTask] ["<<now.ToJavaTime()<<"]["<<g_ontouch_time.ToJavaTime()<<"] ["
    <<touch_to_now.InMillisecondsF()<<"]";
#endif
  if (touch_to_now < g_vsync_duration
       && touch_to_now > max_task_threshold)
    return false;
  return true;
}

const base::TimeDelta SysUtils::GetPeriodFromLastTouch() {
  return base::Time::Now() - g_ontouch_time;
}

const base::TimeDelta SysUtils::GetPeriodFromLastScroll() {
  return base::Time::Now() - g_scrollto_time;
}

///  This Function is used to provide a flag
///    that need perform shcedule PerformIdleTask on render thread
///  IMG and Low device is using AsyncPixelTransferIdle
///  For lowend device, performIdleTask will check if has touch event
///   becauase this would influence the benchmark (Vellamo, CSS 3d fish)
bool SysUtils::IsPerformSyncIdle() {
  if (is_imagination_vendor_)
    return true;
  if (IsLowEndDeviceFromJni()) {
    base::Time now = base::Time::Now();
    base::AutoLock lock(g_record_touch_time_lock);
    if ((now - g_ontouch_time < g_vsync_duration)
        || (now - g_scrollto_time < g_vsync_duration))
      return true;
  }
  return false;
}

void SysUtils::SetIsImagination(bool isIMG) {
  LOG(INFO)<<"[SetIsImagination] ["<<isIMG<<"]";
  is_imagination_vendor_ = isIMG;
}

/// @}

/// M: for multi raster thread @{
///  for high resolution, need to enable multi-raster threads
///  this is platform-specific.
int SysUtils::GetRasterThreadNum() {
  if (base::SysInfo::IsBrowserProcess()) {
    // mark the 2k high end device temporary.
    //if (base::SysInfo::Is2KHighendDevice())
    //  return 2;

    gfx::DeviceDisplayInfo display_info;
    BuildInfo* build_info = BuildInfo::GetInstance();
    if ((!strcmp(build_info->device_prefix(), "mt") && build_info->device_num() == 0x1a4f)
        && display_info.GetDisplayWidth() * display_info.GetDisplayHeight() > 1080 * 1770)
      return 2;
  }
  return 1;
}
/// @}

}  // namespace android
}  // namespace base