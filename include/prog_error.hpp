#ifndef PROG_ERROR_HPP_
#define PROG_ERROR_HPP_

#include <exception>
#include <string>

namespace prog_error {

enum class ErrorCode {
  kCorrect      = 0,
  kGeomBuff     = 1,
  kShader       = 2,
  kRendering    = 3,
  kUtility      = 4,
  kAnalyzer     = 5,
  kTriangle     = 6,
  kAdapter      = 7,
  kMain         = 8,
  kReadData     = 9,
  kTest         = 10,
  kBasics       = 11,
  kBox          = 12
};

struct ErrorInfo {
  ErrorCode code;
  std::string err_msg;
  std::string func_name;
};

class ProgError : public std::exception {
 public:
  explicit ProgError(const ErrorInfo& error_info)
    : error_info_(error_info) {}

  explicit ProgError(ErrorInfo&& error_info)
    : error_info_(std::move(error_info)) {}

  const ErrorInfo& GetErrorInfo() const noexcept {
    return error_info_;
  }

  const char* what() const noexcept override {
    return error_info_.err_msg.c_str();
  }

 private:
  ErrorInfo error_info_;
};
} // namespace prog_except





#endif
