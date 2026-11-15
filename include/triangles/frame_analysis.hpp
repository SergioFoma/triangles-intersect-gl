#ifndef FRAME_ANALYSIS_HPP_
#define FRAME_ANALYSIS_HPP_

#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <atomic>
#include <cassert>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <vector>
#include "triangles/triangles_storage.hpp"

namespace triangles {
template <typename T = double>
requires std::is_floating_point_v<T> class FrameAnalyzer {
 public:
  FrameAnalyzer(storage::TrianglesStorage<T>& storage,
                std::vector<std::pair<size_t, size_t>>&& potential_pairs)
      : storage_(storage), potential_pairs_(potential_pairs) {}

  void Analyze() {
    auto& triangles = storage_.GetTriangles();
    auto& predicates = storage_.GetPredicates();
    if (predicates.size() != triangles.size()) {
      std::vector<std::atomic_bool> resized(triangles.size());
      predicates.swap(resized);
    }

    tbb::parallel_for(
        tbb::blocked_range<size_t>{0, potential_pairs_.size()},
        [&](const tbb::blocked_range<size_t>& range) {
          for (size_t i = range.begin(); i < range.end(); ++i) {
            const auto [first, second] = potential_pairs_[i];

            if (triangles[first].DoesIntersect(triangles[second])) {
              predicates[first].store(true, std::memory_order_relaxed);
              predicates[second].store(true, std::memory_order_relaxed);
            }
          }
        });
  }

 private:
  storage::TrianglesStorage<T>& storage_;
  std::vector<std::pair<size_t, size_t>> potential_pairs_;
};
} // namespace triangles

#endif
