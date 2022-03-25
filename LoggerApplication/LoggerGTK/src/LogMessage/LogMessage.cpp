#include "LogMessage.hpp"

const char *EnumOperations::ToString(EnumLogLevel logLevel)
{
    switch (logLevel)
    {
    case EnumLogLevel::DEBUG:
        return "DEBUG";
        break;
    case EnumLogLevel::WARNING:
        return "WARNING";
        break;
    case EnumLogLevel::ERROR:
        return "ERROR";
        break;
    case EnumLogLevel::FATAL:
        return "FATAL";
        break;

    default:
        break;
    }
    return "INVALID";
}

EnumLogLevel EnumOperations::ParseToEnum(const char *logLevelStr)
{
    EnumLogLevel logLevel = EnumLogLevel::INVALID;
    bool bValid = true;
    {
        const char *debug = "DEBUG";
        for (int8_t i = 0; debug[i] != 0; i++)
        {
            if (logLevelStr[i] != debug[i])
            {
                bValid = false;
                break;
            }
        }
        logLevel = EnumLogLevel::DEBUG;
    }
    if (!bValid)
    {
        const char *warning = "WARNING";
        for (int8_t i = 0; warning[i] != 0; i++)
        {
            if (logLevelStr[i] != warning[i])
            {
                bValid = false;
                break;
            }
        }
        logLevel = EnumLogLevel::WARNING;
    }
    if (!bValid)
    {
        const char *error = "ERROR";
        for (int8_t i = 0; error[i] != 0; i++)
        {
            if (logLevelStr[i] != error[i])
            {
                bValid = false;
                break;
            }
        }
        logLevel = EnumLogLevel::ERROR;
    }
    if (!bValid)
    {
        const char *fatal = "FATAL";
        for (int8_t i = 0; fatal[i] != 0; i++)
        {
            if (logLevelStr[i] != fatal[i])
            {
                bValid = false;
                break;
            }
        }
        logLevel = EnumLogLevel::FATAL;
    }
    return logLevel;
}

LogMessage::LogMessage()
{
    level = EnumLogLevel::INVALID;
}

LogMessage::LogMessage(EnumLogLevel logLevel, const std::string &logMessage)
{
    level = logLevel;
    message = logMessage;
}

std::string LogMessage::ToString()
{
    std::string displayMessage = "[";
    displayMessage += EnumOperations::ToString(level);
    displayMessage += "]: " + message;
    return displayMessage;
}