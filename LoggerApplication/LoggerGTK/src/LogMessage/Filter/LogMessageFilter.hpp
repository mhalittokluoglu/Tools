#ifndef _LOGMESSAGEFILTER_HPP_
#define _LOGMESSAGEFILTER_HPP_
#include "LogMessage/LogMessage.hpp"
#include <vector>
class LogMessageFilter
{
public:
    LogMessageFilter();
    void SetAllMessages(const std::vector<LogMessage *> *allMessageList);
    void SetLevelFilter(bool *logLevelFilter);
    const std::vector<const LogMessage *> &GetFilteredList() const;
    void ApplyFilter();

private:
    bool m_LogLevelFilter[4];
    const std::vector<LogMessage *> *m_AllMessageList;
    std::vector<const LogMessage *> m_FilteredMessageList;
};
#endif