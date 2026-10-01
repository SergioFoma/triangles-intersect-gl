#ifndef TRIANGLE_HANDLER_HPP_
#define TRIANGLE_HANDLER_HPP_

#include <vector>
#include <istream>

#include "triangle.hpp"

namespace triangles {

class TriangleHandler {
  public:
    
    explicit TriangleHandler(std::istream& in);

    void Sort();

  private:
    
    std::vector<Triangle3D> triangles_;

    unsigned int sz_ = 0;
    static constexpr unsigned int kLowBound = 0;
    static constexpr unsigned int kUpperBound = 1'000'000;

    static constexpr auto comparator_ = [](const Triangle3D& first,
                                          const Triangle3D& second) {
      return first.GetSmallerX() < second.GetSmallerX();
    };
};


} // namespace triangles 

#endif
