// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_RESOURCES_RASTER_WORKER_POOL_H_
#define CC_RESOURCES_RASTER_WORKER_POOL_H_

#include "cc/resources/rasterizer.h"
#include "ui/gfx/geometry/size.h"

namespace base {
class SequencedTaskRunner;
}

namespace cc {

/// M: for tab performance @{
class CC_EXPORT RasterWorkerCoordinator {
public:
  static RasterWorkerCoordinator* GetInstance();
  RasterWorkerCoordinator();

  void ResetTileStats();
  void UpdateTileStats(int high_res_tiles, int low_res_tiles, int missing_tiles, int total_tiles);
  void CommitTileStats();
  void OnAttachToWindow();

  bool IsTriggered() {
    return trigger_;
  }

private:
  ~RasterWorkerCoordinator();

  enum RasterStatus {
    NORMAL,
    HIGH_DEMAND,
  };

  void SetupRasterStatus(RasterStatus status);

  int high_res_tile_count_;
  int low_res_tile_count_;
  int missing_tile_count_;
  int total_tile_count_;
  RasterStatus raster_status_;

  bool trigger_;
  bool enable_;
  DISALLOW_COPY_AND_ASSIGN(RasterWorkerCoordinator);
};
/// @}

class CC_EXPORT RasterWorkerPool {
 public:
  static unsigned kBenchmarkRasterTaskPriority;
  static unsigned kRasterFinishedTaskPriority;
  static unsigned kRasterTaskPriorityBase;

  RasterWorkerPool();
  virtual ~RasterWorkerPool();

  // Set the number of threads to use for the global TaskGraphRunner instance.
  // This can only be called once and must be called prior to
  // GetNumRasterThreads().
  static void SetNumRasterThreads(int num_threads);

  // Returns the number of threads used for the global TaskGraphRunner instance.
  static int GetNumRasterThreads();

  // Returns a pointer to the global TaskGraphRunner instance.
  static TaskGraphRunner* GetTaskGraphRunner();

  // Utility function that can be used to create a "raster finished" task that
  // posts |callback| to |task_runner| when run.
  static scoped_refptr<RasterizerTask> CreateRasterFinishedTask(
      base::SequencedTaskRunner* task_runner,
      const base::Closure& callback);

  // Utility function that can be used to call ::ScheduleOnOriginThread() for
  // each task in |graph|.
  static void ScheduleTasksOnOriginThread(RasterizerTaskClient* client,
                                          TaskGraph* graph);

  // Utility function that can be used to build a task graph. Inserts a node
  // that represents |task| in |graph|. See TaskGraph definition for valid
  // |priority| values.
  static void InsertNodeForTask(TaskGraph* graph,
                                RasterizerTask* task,
                                unsigned priority,
                                size_t dependencies);

  // Utility function that can be used to build a task graph. Inserts nodes that
  // represent |task| and all its image decode dependencies in |graph|.
  static void InsertNodesForRasterTask(
      TaskGraph* graph,
      RasterTask* task,
      const ImageDecodeTask::Vector& decode_tasks,
      unsigned priority);

  // Utility functions that transparently create a temporary bitmap and copy
  // pixels to buffer when necessary.
  static void AcquireBitmapForBuffer(SkBitmap* bitmap,
                                     uint8_t* buffer,
                                     ResourceFormat format,
                                     const gfx::Size& size,
                                     int stride);
  static void ReleaseBitmapForBuffer(SkBitmap* bitmap,
                                     uint8_t* buffer,
                                     ResourceFormat format);

  // Type-checking downcast routine.
  virtual Rasterizer* AsRasterizer() = 0;
};

}  // namespace cc

#endif  // CC_RESOURCES_RASTER_WORKER_POOL_H_
