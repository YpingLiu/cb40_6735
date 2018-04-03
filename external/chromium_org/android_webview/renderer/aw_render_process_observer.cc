// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "android_webview/renderer/aw_render_process_observer.h"

#include "android_webview/common/render_view_messages.h"
#include "ipc/ipc_message_macros.h"
#include "third_party/WebKit/public/web/WebCache.h"
#include "third_party/WebKit/public/web/WebNetworkStateNotifier.h"

/// M: dump memory status @{
#include "v8/include/v8.h"
#include "android/log.h"
#undef DUMP_MEM_USAGE_LOG
#define DUMP_MEM_USAGE_LOG(...) __android_log_print(ANDROID_LOG_INFO, "MemoryUsage", __VA_ARGS__)
/// @}

namespace android_webview {

AwRenderProcessObserver::AwRenderProcessObserver()
  : webkit_initialized_(false) {
}

AwRenderProcessObserver::~AwRenderProcessObserver() {
}

bool AwRenderProcessObserver::OnControlMessageReceived(
    const IPC::Message& message) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(AwRenderProcessObserver, message)
    IPC_MESSAGE_HANDLER(AwViewMsg_ClearCache, OnClearCache)
    IPC_MESSAGE_HANDLER(AwViewMsg_SetJsOnlineProperty, OnSetJsOnlineProperty)
    IPC_MESSAGE_UNHANDLED(handled = false)
    /// M: dump memory status
    IPC_MESSAGE_HANDLER(AwViewMsg_DumpMemoryStatus, OnDumpMemoryStatus)
  IPC_END_MESSAGE_MAP()
  return handled;
}

void AwRenderProcessObserver::WebKitInitialized() {
  webkit_initialized_ = true;
}

void AwRenderProcessObserver::OnClearCache() {
  if (webkit_initialized_)
    blink::WebCache::clear();
}

void AwRenderProcessObserver::OnSetJsOnlineProperty(bool network_up) {
  if (webkit_initialized_)
    blink::WebNetworkStateNotifier::setOnLine(network_up);
}

/// M: dump memory status @{
void AwRenderProcessObserver::OnDumpMemoryStatus() {
  if (webkit_initialized_) {

    DUMP_MEM_USAGE_LOG(" ");
    DUMP_MEM_USAGE_LOG("+++++ dump memory cache usage +++++");
    blink::WebCache::dumpMemoryCacheStatus();

    v8::HeapStatistics stat;
    // TODO(svenpanne) The call below doesn't take web workers into account, this
    // has to be done manually by iterating over all Isolates involved.
    v8::Isolate::GetCurrent()->GetHeapStatistics(&stat);
    DUMP_MEM_USAGE_LOG(" ");
    DUMP_MEM_USAGE_LOG("+++++ dump v8 memory usage +++++");
    DUMP_MEM_USAGE_LOG("v8 Heap: total_heap_size=%zu, used_heap_size=%zu, heap_size_limit=%zu",
        stat.total_heap_size(), stat.used_heap_size(), stat.heap_size_limit());
    DUMP_MEM_USAGE_LOG(" ");
  }
}
/// @}

}  // nanemspace android_webview
