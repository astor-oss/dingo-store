// Copyright (c) 2023 dingodb.com, Inc. All Rights Reserved
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef DINGODB_VECTOR_INDEX_MANAGER_H_
#define DINGODB_VECTOR_INDEX_MANAGER_H_

#include <atomic>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "butil/status.h"
#include "common/safe_map.h"
#include "log/segment_log_storage.h"
#include "meta/store_meta_manager.h"
#include "proto/common.pb.h"
#include "vector/vector_index.h"
#include "vector/vector_index_snapshot.h"

namespace dingodb {

// Rebuild vector index task
class RebuildVectorIndexTask : public TaskRunnable {
 public:
  RebuildVectorIndexTask(VectorIndexWrapperPtr vector_index_wrapper, bool force)
      : vector_index_wrapper_(vector_index_wrapper), force_(force) {}
  ~RebuildVectorIndexTask() override = default;

  void Run() override;

 private:
  VectorIndexWrapperPtr vector_index_wrapper_;
  bool force_;
};

// Save vector index task
class SaveVectorIndexTask : public TaskRunnable {
 public:
  SaveVectorIndexTask(VectorIndexWrapperPtr vector_index_wrapper) : vector_index_wrapper_(vector_index_wrapper) {}
  ~SaveVectorIndexTask() override = default;

  void Run() override;

 private:
  VectorIndexWrapperPtr vector_index_wrapper_;
};

// Load or build vector index task
class LoadOrBuildVectorIndexTask : public TaskRunnable {
 public:
  LoadOrBuildVectorIndexTask(VectorIndexWrapperPtr vector_index_wrapper)
      : vector_index_wrapper_(vector_index_wrapper) {}
  ~LoadOrBuildVectorIndexTask() override = default;

  void Run() override;

 private:
  VectorIndexWrapperPtr vector_index_wrapper_;
};

// Manage vector index, e.g. build/rebuild/save/load vector index.
class VectorIndexManager {
 public:
  VectorIndexManager(std::shared_ptr<RawEngine> raw_engine, std::shared_ptr<MetaReader> meta_reader,
                     std::shared_ptr<MetaWriter> meta_writer) {}

  ~VectorIndexManager() = default;

  static bool Init(std::vector<store::RegionPtr> regions);

  // Check whether should hold vector index.
  static bool NeedHoldVectorIndex(uint64_t region_id);

  // Load vector index for already exist vector index at bootstrap.
  // Priority load from snapshot, if snapshot not exist then load from rocksdb.
  static butil::Status LoadOrBuildVectorIndex(VectorIndexWrapperPtr vector_index_wrapper);
  static void LaunchLoadOrBuildVectorIndex(VectorIndexWrapperPtr vector_index_wrapper);
  // Parallel load or build vector index at server bootstrap.
  static butil::Status ParallelLoadOrBuildVectorIndex(std::vector<store::RegionPtr> regions, int concurrency);

  // Save vector index snapshot.
  static butil::Status SaveVectorIndex(VectorIndexWrapperPtr vector_index_wrapper);
  // Launch save vector index at execute queue.
  static void LaunchSaveVectorIndex(VectorIndexWrapperPtr vector_index_wrapper);

  // Invoke when server running.
  static butil::Status RebuildVectorIndex(VectorIndexWrapperPtr vector_index_wrapper);
  // Launch rebuild vector index at execute queue.
  static void LaunchRebuildVectorIndex(VectorIndexWrapperPtr vector_index_wrapper, bool force);

  static butil::Status ScrubVectorIndex();

  static std::atomic<int> vector_index_task_running_num;
  static int GetVectorIndexTaskRunningNum() { return vector_index_task_running_num.load(); }
  static void IncVectorIndexTaskRunningNum() { vector_index_task_running_num.fetch_add(1); }
  static void DecVectorIndexTaskRunningNum() { vector_index_task_running_num.fetch_sub(1); }

  static std::atomic<int> vector_index_rebuild_task_running_num;
  static int GetVectorIndexRebuildTaskRunningNum() { return vector_index_rebuild_task_running_num.load(); }
  static void IncVectorIndexRebuildTaskRunningNum() { vector_index_rebuild_task_running_num.fetch_add(1); }
  static void DecVectorIndexRebuildTaskRunningNum() { vector_index_rebuild_task_running_num.fetch_sub(1); }

  static std::atomic<int> vector_index_save_task_running_num;
  static int GetVectorIndexSaveTaskRunningNum() { return vector_index_save_task_running_num.load(); }
  static void IncVectorIndexSaveTaskRunningNum() { vector_index_save_task_running_num.fetch_add(1); }
  static void DecVectorIndexSaveTaskRunningNum() { vector_index_save_task_running_num.fetch_sub(1); }

 private:
  // Build vector index with original data(rocksdb).
  // Invoke when server starting.
  static std::shared_ptr<VectorIndex> BuildVectorIndex(VectorIndexWrapperPtr vector_index_wrapper);

  // Replay log to vector index.
  static butil::Status ReplayWalToVectorIndex(std::shared_ptr<VectorIndex> vector_index, uint64_t start_log_id,
                                              uint64_t end_log_id);

  // Scrub vector index.
  static butil::Status ScrubVectorIndex(store::RegionPtr region, bool need_rebuild, bool need_save);

  static butil::Status TrainForBuild(std::shared_ptr<VectorIndex> vector_index, std::shared_ptr<Iterator> iter,
                                     const std::string &start_key, [[maybe_unused]] const std::string &end_key);
};

}  // namespace dingodb

#endif  // DINGODB_VECTOR_INDEX_MANAGER_H_
