/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "bindings/core/v8/V8GCForContextDispose.h"

#include "bindings/core/v8/V8PerIsolateData.h"
#include "wtf/StdLibExtras.h"
#include <v8.h>
/// M: send long idle notification when context dispose happenned frequently @{
#include <base/process/process_metrics.h>
#include "base/logging.h"
#include "wtf/CurrentTime.h"
/// @}

namespace blink {

V8GCForContextDispose::V8GCForContextDispose()
    : m_pseudoIdleTimer(this, &V8GCForContextDispose::pseudoIdleTimerFired)
    , m_didDisposeContextForMainFrame(false)
/// M: send long idle notification when context dispose happenned frequently @{
    , m_detectFrequentContextDispose(false)
    , m_previousContextDisposeTime(0)
    , m_successiveFrequentContextDisposeCount(0)
/// @}
{
    /// M: send long idle notification when context dispose happenned frequently @{
    // only apply to 64-bit, 1GB ram device
#if defined(__aarch64__)
    base::SystemMemoryInfoKB meminfo;
    if (base::GetSystemMemoryInfo(&meminfo)) {
        // total memory is larger than 512MB
        if (meminfo.total > 512 * 1024) {
            m_detectFrequentContextDispose = true;
        }
    }
#endif // defined(__aarch64__)
    /// @}
}

void V8GCForContextDispose::notifyContextDisposed(bool isMainFrame)
{
    /// M: send long idle notification when context dispose happenned frequently @{
    // only apply to 64-bit, 1GB ram device
#if defined(__aarch64__)
    if (m_detectFrequentContextDispose) {
        double now = monotonicallyIncreasingTime();
        // if this context dispose is within 1 second of the previous context dispose,
        // it's counted as frequent context dispose
        if (now < (m_previousContextDisposeTime + 1.0)) {
            ++m_successiveFrequentContextDisposeCount;
        } else {
            m_successiveFrequentContextDisposeCount = 0;
        }
        m_previousContextDisposeTime = now;
    }
#endif // defined(__aarch64__)
    /// @}
    m_didDisposeContextForMainFrame = isMainFrame;
    V8PerIsolateData::mainThreadIsolate()->ContextDisposedNotification();
    if (!m_didDisposeContextForMainFrame && !m_pseudoIdleTimer.isActive())
        m_pseudoIdleTimer.startOneShot(0.8, FROM_HERE);
}

void V8GCForContextDispose::notifyIdleSooner(double maximumFireInterval)
{
    if (!m_didDisposeContextForMainFrame && m_pseudoIdleTimer.isActive()) {
        double nextFireInterval = m_pseudoIdleTimer.nextFireInterval();
        if (nextFireInterval > maximumFireInterval) {
            m_pseudoIdleTimer.stop();
            m_pseudoIdleTimer.startOneShot(maximumFireInterval, FROM_HERE);
        }
    }
}

V8GCForContextDispose& V8GCForContextDispose::instanceTemplate()
{
    DEFINE_STATIC_LOCAL(V8GCForContextDispose, staticInstance, ());
    return staticInstance;
}

void V8GCForContextDispose::pseudoIdleTimerFired(Timer<V8GCForContextDispose>*)
{
    /// M: send long idle notification when context dispose happenned frequently @{
    // only apply to 64-bit, 1GB ram device
#if defined(__aarch64__)
    // if there are many frequent context disposes, and heap size is not low, do majorGC
    if (!m_didDisposeContextForMainFrame && m_successiveFrequentContextDisposeCount >= 2) {
        v8::HeapStatistics heap_stats;
        V8PerIsolateData::mainThreadIsolate()->GetHeapStatistics(&heap_stats);
        size_t used_heap_byte = heap_stats.used_heap_size();
        // in 64-bit vellamo test, after majorGC, remaining heap size is usually about 10MB,
        // if the heap size is more than 2 * 10MB, guess that there are some garbage to collect
        const size_t heap_size_threshold = 20 * 1024 * 1024;
        if (used_heap_byte > heap_size_threshold) {
            V8PerIsolateData::mainThreadIsolate()->IdleNotification(1000);
            return;
        }
    }
#endif // defined(__aarch64__)
    /// @}

    const int idlePauseInMs = 10;
    if (!m_didDisposeContextForMainFrame)
        V8PerIsolateData::mainThreadIsolate()->IdleNotification(idlePauseInMs);
}

} // namespace blink
