#ifndef _LOGLISTENER_HPP_
#define _LOGLISTENER_HPP_
#include "LogMessage/LogMessage.hpp"
#include <string>
#include <inttypes.h>
#include <arpa/inet.h>
#include <thread>
class LoggerHandler;
class LogListener
{
typedef void (*CallBackFunction)(LogMessage*);
public:
    LogListener();
    bool StartListening(const char *ipAddress, int32_t port);
    void CloseConnection();
    void Receive();
    void SetMessageCallBack(void(*callBack)(LogMessage*));

private:
    LogMessage *ParseMessage(const char *message);
    struct sockaddr_in m_ServerAddress;
    int32_t m_SocketFileDescriptor;
    bool m_IsListening;
    const char *m_IpAddress;
    int32_t m_PortNumber;
    std::thread *m_ReceiverThread;
    CallBackFunction m_CallBack;
};
#endif