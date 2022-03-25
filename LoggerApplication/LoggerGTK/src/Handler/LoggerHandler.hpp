#ifndef _LOGGERHANDLER_HPP_
#define _LOGGERHANDLER_HPP_
#include <gtk/gtk.h>
#include <mutex>
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
    
    void SetCallBack(void (*callBack)(LogMessage *));
    void SetLoggerTextWidget(GtkTextBuffer *textBuffer);
    void OnMessageReceivedCallBack(LogMessage *message);
    void ClearScreen();
private:
    GtkTextBuffer *m_TextBuffer;
    bool m_LogLevels[4];
    LogMessageFilter m_Filter;
    LogListener m_Listener;
    std::vector<LogMessage*> m_LogMessageList;
    const std::vector<const LogMessage*> *m_FilteredLogMessageList;
    static gboolean AddTextToData(gpointer data);
    void GetFullText();
    char m_Buffer[32768];

    std::mutex messageListMutex;
};
#endif