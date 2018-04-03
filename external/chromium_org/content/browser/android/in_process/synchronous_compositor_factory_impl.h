// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_ANDROID_IN_PROCESS_SYNCHRONOUS_COMPOSITOR_FACTORY_IMPL_H_
#define CONTENT_BROWSER_ANDROID_IN_PROCESS_SYNCHRONOUS_COMPOSITOR_FACTORY_IMPL_H_

#include "base/synchronization/lock.h"
#include "content/browser/android/in_process/synchronous_input_event_filter.h"
#include "content/renderer/android/synchronous_compositor_factory.h"
#include "content/renderer/media/android/stream_texture_factory_synchronous_impl.h"
#include "gpu/command_buffer/service/in_process_command_buffer.h"
#include "webkit/common/gpu/context_provider_web_context.h"
/// M: To release stream texture
#include "cc/base/completion_event.h"

namespace gpu {
class GLInProcessContext;
}

namespace webkit {
namespace gpu {
class WebGraphicsContext3DInProcessCommandBufferImpl;
}
}

namespace content {

class SynchronousCompositorFactoryImpl : public SynchronousCompositorFactory {
 public:
  SynchronousCompositorFactoryImpl();
  virtual ~SynchronousCompositorFactoryImpl();

  // SynchronousCompositorFactory
  virtual scoped_refptr<base::MessageLoopProxy> GetCompositorMessageLoop()
      OVERRIDE;
  virtual bool RecordFullLayer() OVERRIDE;
  virtual scoped_ptr<cc::OutputSurface> CreateOutputSurface(
      int routing_id,
      scoped_refptr<content::FrameSwapMessageQueue> frame_swap_message_queue)
      OVERRIDE;
  virtual InputHandlerManagerClient* GetInputHandlerManagerClient() OVERRIDE;
  virtual scoped_refptr<webkit::gpu::ContextProviderWebContext>
      CreateOffscreenContextProvider(
          const blink::WebGraphicsContext3D::Attributes& attributes,
          const std::string& debug_name) OVERRIDE;
  virtual scoped_refptr<StreamTextureFactory> CreateStreamTextureFactory(
      int view_id) OVERRIDE;
  virtual webkit::gpu::WebGraphicsContext3DImpl*
      CreateOffscreenGraphicsContext3D(
          const blink::WebGraphicsContext3D::Attributes& attributes) OVERRIDE;

  SynchronousInputEventFilter* synchronous_input_event_filter() {
    return &synchronous_input_event_filter_;
  }

  void SetDeferredGpuService(
      scoped_refptr<gpu::InProcessCommandBuffer::Service> service);
  void SetRecordFullDocument(bool record_full_document);
  void CompositorInitializedHardwareDraw();
  /// M: for stream texture leak
  void CompositorReleasedHardwareDraw(bool lost_context);

  scoped_refptr<cc::ContextProvider>
      CreateOnscreenContextProviderForCompositorThread();

  /// M: For ThreadProxy::BeginMainFrame @{
  cc::InterruptableCompletionEvent* GetInterruptableCompletionEvent() {
    interrupt_completion_event_.Reset();
    return &interrupt_completion_event_;
  }
  void DidBeginMainFrameScheduled();
  void DidBeginMainFrameStarted();
  void DidBeginMainFrameEnded();
  /// This MUST run on main thread (Chrome Render Thread)
  void RunInterruptClosures();
  /// @}

 private:
  bool CanCreateMainThreadContext();
  scoped_refptr<StreamTextureFactorySynchronousImpl::ContextProvider>
      TryCreateStreamTextureFactory();
  void RestoreContextOnMainThread();

  /// M: For ThreadProxy::BeginMainFrame @{
  void ReleaseVideoContextOnMainThread(cc::CompletionEvent*);
  bool CheckIsBeginMainFrameRunningAndInsertClosure(base::Closure& closure);
  /// @}

  SynchronousInputEventFilter synchronous_input_event_filter_;

  scoped_refptr<gpu::InProcessCommandBuffer::Service> service_;

  class VideoContextProvider;
  scoped_refptr<VideoContextProvider> video_context_provider_;

  bool record_full_layer_;

  // |num_hardware_compositor_lock_| is updated on UI thread only but can be
  // read on renderer main thread.
  base::Lock num_hardware_compositor_lock_;
  unsigned int num_hardware_compositors_;
  scoped_refptr<base::MessageLoopProxy> main_thread_proxy_;

  /// M: For ThreadProxy::BeginMainFrame @{
  cc::InterruptableCompletionEvent interrupt_completion_event_;
  enum BeginMainFrameState {
    BeginMainFrame_None,
    BeginMainFrame_Scheduled,
    BeginMainFrame_Started,
  };
  BeginMainFrameState begin_main_frame_state_;
  base::Lock begin_main_frame_state_lock_;
  std::vector<base::Closure> interrupt_callbacks_;
  /// @}

};

}  // namespace content

#endif  // CONTENT_BROWSER_ANDROID_IN_PROCESS_SYNCHRONOUS_COMPOSITOR_FACTORY_IMPL_H_
