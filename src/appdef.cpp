#include <iostream>
#include <string_view>
#include "appdef.h"

namespace App {
  void log(std::string_view message, const LogType& log_type) {
    static constexpr std::string_view lookupTable[3] = {
      "INFO - ", "ERROR - ", "WARN - "
    };
  
    std::cout << LOG_STRING << lookupTable[log_type] << message << std::endl; 
  }
}