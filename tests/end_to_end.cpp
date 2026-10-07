#include <iostream>
#include <fstream>
#include <stdexcept>
#include <array>
#include <string>
#include <vector>
#include <cassert>

#include "read_data.hpp"

namespace {

const size_t kTestsNumber = 5;

std::vector<bool> ReadCorrectStatuses(const std::string& file_name) {
  std::ifstream file(file_name, std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("ReadCorrectStatuses: error of opening file!");
  }

  std::vector<bool> correct_status;

  int intersect_status = 0;

  while (file >> intersect_status) {
    correct_status.push_back(static_cast<bool>(intersect_status));
  }

  return correct_status;
}

void ComparingValues(const std::vector<bool>& intersect_status,
                     const std::vector<bool>& correct_status,
                     const std::string& file_name) {
  bool flag = true;
  for (int ind = 0; ind < correct_status.size() && flag; ++ind) {
    flag = (intersect_status[ind] == correct_status[ind]);
  }

  std::cout << "RUNNING:  " << file_name << '\n';
  if (flag) {
    std::cout << "STATUS: \033[32m[PASSED]\033[0m\n\n";
  } else {
    std::cout << "STATUS: \033[31m[FAILED]\033[0m\n\n";
  }
}
} // namespace

int main() {

  std::array<const std::string, kTestsNumber> input_data = {
    "materials/triangles_100.txt",
    "materials/triangles_1000.txt",
    "materials/triangles_10000.txt",
    "materials/triangles_100000.txt",
    "materials/triangles_1000000.txt"
  };

  std::array<const std::string, kTestsNumber> output_data = {
    "materials/output_100.txt",
    "materials/output_1000.txt",
    "materials/output_10000.txt",
    "materials/output_100000.txt",
    "materials/output_1000000.txt"
  };

  for (size_t ind = 0; ind < kTestsNumber; ++ind) {
    triangles::TriangleArr triangles = reader::ReadData(input_data[ind]);
    triangles::InterAnalyzer analyzer(std::move(triangles));

    const std::vector<bool>& intersect_status = analyzer.GetInterStatuses();
    const std::vector<bool>& correct_status = ReadCorrectStatuses(output_data[ind]);

    assert(intersect_status.size() == correct_status.size());

    ComparingValues(intersect_status, correct_status, input_data[ind]);
  }

  return 0;
}
