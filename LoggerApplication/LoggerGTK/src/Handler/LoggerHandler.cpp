#include "LoggerHandler.hpp"
#include <cstring>
#include <chrono>
#include <thread>
#include <fstream>
#include <string>
LoggerHandler::LoggerHandler()
{
    m_Filter.SetAllMessages(&m_LogMessageList);
    memset(m_LogLevels, true, sizeof(m_LogLevels));
    m_Filter.SetLevelFilter(m_LogLevels);
    messageListMutex.lock();
    m_Filter.ApplyFilter();
    messageListMutex.unlock();
    m_MessageReceived = false;
    m_FilteredLogMessageList = &m_Filter.GetFilteredList();
    std::thread *displayerThread = new std::thread(&LoggerHandler::DisplayText, this);
}

void LoggerHandler::SetCallBack(void (*callBack)(std::vector<LogMessage *>))
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
    m_MessageReceived = true;
}

void LoggerHandler::DisplayText()
{
    while (true)
    {
        if (m_MessageReceived)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(60));
            TextData *data = g_new0(struct TextData, 1);
            data->buffer = m_TextBuffer;
            m_BufferMutex.lock();
            GetFullText();
            data->data = m_Buffer;
            data->sizeOfData = m_BufferSize;
            gdk_threads_add_idle(AddTextToData, data);
            m_BufferMutex.unlock();
            m_MessageReceived = false;
        }
    }
}

void LoggerHandler::SaveMessage(const char *fileName)
{
    char buffer[32768];
    m_BufferMutex.lock();
    GetFullText();
    memcpy(buffer, m_Buffer, 32768);
    m_BufferMutex.unlock();

    std::string logOutFileName = fileName;
    logOutFileName += ".log";
    std::ofstream outLogFile(logOutFileName.c_str());
    outLogFile << buffer;
    outLogFile.close();

    char outBuffer[32768] = { 0 };
    int64_t outIndex = 0;
    int32_t timeCount = 0;
    for (int64_t i = 0; buffer[i]; i++)
    {
        if (buffer[i] == ' ' and 
            buffer[i+1] == '-' and 
            buffer[i+2] == '-' and 
            buffer[i+3] == '>' and
            buffer[i+4] == ' ')
        {
            outBuffer[outIndex] = ';';
            i+=4;
            outIndex++;
            timeCount = 0;
        }
        else
        {
            outBuffer[outIndex] = buffer[i];
            outIndex++;
            timeCount++;
            if (timeCount == 19)
            {
                outBuffer[outIndex] = ';';
                outIndex++;
                i++;
            }
        }
    }

    std::string csvOutFileName = fileName;
    csvOutFileName += ".csv";
    std::ofstream outCSVFile(csvOutFileName.c_str());
    outCSVFile << outBuffer;
    outCSVFile.close();
}

gboolean LoggerHandler::AddTextToData(gpointer data)
{
    TextData *bufferData = static_cast<TextData*>(data);
    gtk_text_buffer_set_text(bufferData->buffer, bufferData->data, bufferData->sizeOfData);
    g_free(data);
    return G_SOURCE_REMOVE;
}

void LoggerHandler::OnMessageReceivedCallBack(std::vector<LogMessage*> messageList)
{
    messageListMutex.lock();
    if (m_LogMessageList.size() < 500)
    {
        for (int32_t i = 0; i < messageList.size(); i++)
        {
            m_LogMessageList.push_back(messageList[i]);
        }
    }
    else
    {
        m_LogMessageList.clear();
    }
    messageListMutex.unlock();

    m_Filter.ApplyFilter();
    m_MessageReceived = true;
    // const char *text = gtk_text_buffer_get_text (m_TextBuffer, &start, &end, FALSE);
}

void LoggerHandler::GetFullText()
{
    memset(m_Buffer, 0 ,32768);
    m_BufferSize = 0;
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
        memcpy(&m_Buffer[index], " --> ", 5);
        index += 5;
        memcpy(&m_Buffer[index], (*m_FilteredLogMessageList)[i]->message.c_str(),
        (*m_FilteredLogMessageList)[i]->message.size());
        index += (*m_FilteredLogMessageList)[i]->message.size();
    }
    m_BufferSize = index;
}

void LoggerHandler::ClearScreen()
{
    m_LogMessageList.clear();
    m_Filter.ApplyFilter();
    m_FilteredLogMessageList = &m_Filter.GetFilteredList();
    m_MessageReceived = true;
}