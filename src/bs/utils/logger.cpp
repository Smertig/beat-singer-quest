#include "logger.h"

#include <bs/mod_info.h>

Logger& getLogger() {
    // Logger::~Logger() is deleted ¯\_(ツ)_/¯
    static const auto logger = new Logger(getModInfo());
    return *logger;
}
