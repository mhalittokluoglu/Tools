#include "LogMessageFilter.hpp"
#include <cstring>
LogMessageFilter::LogMessageFilter()
{
    memset(m_LogLevelFilter, true, sizeof(m_LogLevelFilter));
}

void LogMessageFilter::SetAllMessages(const std::vector<LogMessage *> *allMessageList)
{
    m_AllMessageList = allMessageList;
}

void LogMessageFilter::SetLevelFilter(bool *logLevelFilter)
{
    memcpy(m_LogLevelFilter, logLevelFilter, 4);
}

const std::vector<const LogMessage *> &LogMessageFilter::GetFilteredList() const
{
    return m_FilteredMessageList;
}

void LogMessageFilter::ApplyFilter()
{
    m_FilteredMessageList.clear();
    for (int32_t i = 0; i < m_AllMessageList->size(); i++)
    {
        if (m_LogLevelFilter[(int8_t)(*m_AllMessageList)[i]->level])
        {
            m_FilteredMessageList.push_back((*m_AllMessageList)[i]);
        }
    }
}
