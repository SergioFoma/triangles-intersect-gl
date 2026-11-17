#ifndef FRAME_ANALYSIS_HPP_
#define FRAME_ANALYSIS_HPP_

#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <atomic>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <utility>
#include <vector>
#include "triangles/triangle.hpp"
#include "triangles_storage.hpp"

namespace triangles {

template <std::floating_point T>
class TrianglesStorage;

template <std::floating_point T = double>
class FrameAnalyzer : public StorageUpdater<T> {
 public:
  void ChangeStorage(TrianglesStorage<T>& storage,
                     TrianglesStorage<T>::UpdaterContext* ctx) override {
    assert(ctx);

    const auto& triangles = storage.GetCurrentPos();
    auto& predicates = ctx->predicates;
    const auto& potential_pairs = ctx->potential_pairs;
    assert(predicates.size() == triangles.size() && "Incorrect size");

    tbb::parallel_for(tbb::blocked_range<size_t>{0, predicates.size()},
                      [&](const tbb::blocked_range<size_t>& range) {
                        for (size_t i = range.begin(); i < range.end(); ++i) {
                          predicates[i].store(false, std::memory_order_relaxed);
                        }
                      });

    tbb::parallel_for(
        tbb::blocked_range<size_t>{0, potential_pairs.size()},
        [&](const tbb::blocked_range<size_t>& range) {
          for (size_t i = range.begin(); i < range.end(); ++i) {
            const auto [first, second] = potential_pairs[i];

            if (triangles[first].DoesIntersect(triangles[second])) {
              predicates[first].store(true, std::memory_order_relaxed);
              predicates[second].store(true, std::memory_order_relaxed);
            }
          }
        });
  }

};
}  // namespace triangles

#endif
