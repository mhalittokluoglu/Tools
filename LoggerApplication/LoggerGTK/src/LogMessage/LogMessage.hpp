#ifndef _LOGMESSAGE_HPP_
#define _LOGMESSAGE_HPP_
#include <string>
enum class EnumLogLevel
{
    INVALID = -1,
    DEBUG = 0,
    WARNING = 1,
    ERROR = 2,
    FATAL = 3

};

namespace EnumOperations
{
    const char *ToString(EnumLogLevel logLevel);
    EnumLogLevel ParseToEnum(const char *logLevelStr);
}

struct LogMessage
{
    EnumLogLevel level;
    std::string message;
    LogMessage();
    LogMessage(EnumLogLevel logLevel, const std::string &logMessage);
    std::string ToString();
};
#endif