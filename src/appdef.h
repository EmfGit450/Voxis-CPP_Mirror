#pragma once
#include <string_view>

namespace App {
  #define NAME "Voxis"
  #define VERSION "0.1.0"
  
  inline constexpr std::string_view LOG_STRING = "[" NAME " " VERSION "]: ";
  
  enum LogType {
    INFO = 0,
    ERROR,
    WARN
  };
  
  void log(std::string_view message, const LogType& log_type);
}