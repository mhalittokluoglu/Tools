#include "LoggerHandler.hpp"
#include <cstring>


LoggerHandler::LoggerHandler()
{
    m_Filter.SetAllMessages(&m_LogMessageList);
    memset(m_LogLevels, true, sizeof(m_LogLevels));

    m_Filter.SetLevelFilter(m_LogLevels);
    messageListMutex.lock();
    m_Filter.ApplyFilter();
    messageListMutex.unlock();
    m_FilteredLogMessageList = &m_Filter.GetFilteredList();
}

void LoggerHandler::SetCallBack(void (*callBack)(LogMessage *))
{
    m_Listener.SetMessageCallBack(callBack);
}

void LoggerHandler::StartListening(const char *ipAddress, int32_t port)
{
    m_Listener.StartListening(ipAddress, port);
}

void LoggerHandler::StopListening()
{
    m_Listener.CloseConnection();
}

void LoggerHandler::SetLoggerTextWidget(GtkTextBuffer *textBuffer)
{
    m_TextBuffer = textBuffer;
}

void LoggerHandler::FilterChanged(EnumLogLevel toggledLevel)
{
    if (m_LogLevels[(int8_t)toggledLevel])
        m_LogLevels[(int8_t)toggledLevel] = false;
    else
        m_LogLevels[(int8_t)toggledLevel] = true;

    m_Filter.SetLevelFilter(m_LogLevels);
    messageListMutex.lock();
    m_Filter.ApplyFilter();
    messageListMutex.unlock();
    m_FilteredLogMessageList = &m_Filter.GetFilteredList();

    TextData *data = g_new0(struct TextData, 1);
    data->buffer = m_TextBuffer;
    GetFullText();
    data->data = m_Buffer;
    gdk_threads_add_idle(AddTextToData, data);
}

gboolean LoggerHandler::AddTextToData(gpointer data)
{
    TextData *bufferData = static_cast<TextData*>(data); 
    gtk_text_buffer_set_text(bufferData->buffer, bufferData->data, -1);
    g_free(data);
    return G_SOURCE_REMOVE;
}

void LoggerHandler::OnMessageReceivedCallBack(LogMessage *message)
{
    messageListMutex.lock();
    if (m_LogMessageList.size() < 500)
    {
        m_LogMessageList.push_back(message);
    }
    else
    {
        m_LogMessageList.clear();
    }
    messageListMutex.unlock();

    m_Filter.ApplyFilter();

    TextData *data = g_new0(struct TextData, 1);
    data->buffer = m_TextBuffer;
    GetFullText();
    data->data = m_Buffer;
    gdk_threads_add_idle(AddTextToData, data);
    // const char *text = gtk_text_buffer_get_text (m_TextBuffer, &start, &end, FALSE);
}

void LoggerHandler::GetFullText()
{
    memset(m_Buffer, 0 ,32768);
    int32_t index = 0;
    for (int32_t i = 0; i < m_FilteredLogMessageList->size();i++)
    {
        if (index > 32768 - 400)
        {
            memset(m_Buffer, 0 ,32768);
            index = 0;
            m_LogMessageList.clear();
        }
        
        const char *levelStr = EnumOperations::ToString((*m_FilteredLogMessageList)[i]->level);
        memcpy(&m_Buffer[index], levelStr, strlen(levelStr));
        index += strlen(levelStr);
        memcpy(&m_Buffer[index], " -> ", 4);
        index += 4;
        memcpy(&m_Buffer[index], (*m_FilteredLogMessageList)[i]->message.c_str(),
        (*m_FilteredLogMessageList)[i]->message.size());
        index += (*m_FilteredLogMessageList)[i]->message.size();
    }
}

void LoggerHandler::ClearScreen()
{
    TextData *data = g_new0(struct TextData, 1);
    data->buffer = m_TextBuffer;
    m_LogMessageList.clear();
    m_Filter.ApplyFilter();
    m_FilteredLogMessageList = &m_Filter.GetFilteredList();
    GetFullText();
    data->data = m_Buffer;
    gdk_threads_add_idle(AddTextToData, data);
}