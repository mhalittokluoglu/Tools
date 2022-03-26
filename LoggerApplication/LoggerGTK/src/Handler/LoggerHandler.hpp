#ifndef _LOGGERHANDLER_HPP_
#define _LOGGERHANDLER_HPP_
#include <gtk/gtk.h>
#include <mutex>
#include <vector>
#include "LogMessage/Filter/LogMessageFilter.hpp"
#include "LogListener/LogListener.hpp"

struct TextData
{
    GtkTextBuffer *buffer;
    const char * data;
    int sizeOfData;
};

class LoggerHandler
{
public:
    LoggerHandler();
    void StartListening(const char *ipAddress, int32_t port);
    void StopListening();
    void FilterChanged(EnumLogLevel toggledLevel);
    
    void SetCallBack(void (*callBack)(std::vector<LogMessage *>));
    void SetLoggerTextWidget(GtkTextBuffer *textBuffer);
    void OnMessageReceivedCallBack(std::vector<LogMessage*> messageList);
    void ClearScreen();
    void SaveMessage(const char *fileName);
private:
    GtkTextBuffer *m_TextBuffer;
    bool m_LogLevels[4];
    LogMessageFilter m_Filter;
    LogListener m_Listener;
    std::vector<LogMessage*> m_LogMessageList;
    const std::vector<const LogMessage*> *m_FilteredLogMessageList;
    static gboolean AddTextToData(gpointer data);
    void DisplayText();
    void GetFullText();
    char m_Buffer[32768];
    int64_t m_BufferSize;
    bool m_MessageReceived;

    std::mutex messageListMutex;
    std::mutex m_BufferMutex;
};
#endif