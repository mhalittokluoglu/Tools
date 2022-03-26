#ifndef _LOGLISTENER_HPP_
#define _LOGLISTENER_HPP_
#include "LogMessage/LogMessage.hpp"
#include <string>
#include <inttypes.h>
#include <arpa/inet.h>
#include <vector>
#include <thread>
class LoggerHandler;
class LogListener
{
typedef void (*CallBackFunction)(std::vector<LogMessage*>);
public:
    LogListener();
    bool StartListening(const char *ipAddress, int32_t port);
    void CloseConnection();
    void Receive();
    void SetMessageCallBack(void(*callBack)(std::vector<LogMessage*>));

private:
    std::vector<LogMessage*> ParseMessage(const char *message);
    struct sockaddr_in m_ServerAddress;
    int32_t m_SocketFileDescriptor;
    bool m_IsListening;
    const char *m_IpAddress;
    int32_t m_PortNumber;
    std::thread *m_ReceiverThread;
    CallBackFunction m_CallBack;
};
#endif