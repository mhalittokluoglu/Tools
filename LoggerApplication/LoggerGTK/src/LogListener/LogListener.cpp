#include "LogListener.hpp"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

LogListener::LogListener() : m_IsListening{false},
                             m_IpAddress{nullptr},
                             m_PortNumber{0},
                             m_SocketFileDescriptor{-1}
{
}

bool LogListener::StartListening(const char *ipAddr, int32_t port)
{
    if (m_IsListening)
    {
        std::cout << "Already Listening on " << m_IpAddress << ":" << m_PortNumber << std::endl;
        return false;
    }
    m_IpAddress = ipAddr;
    m_PortNumber = port;

    // Creating socket file descriptor
    if ((m_SocketFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed");
        return false;
    }

    memset(&m_ServerAddress, 0, sizeof(m_ServerAddress));

    // Filling server information
    m_ServerAddress.sin_family = AF_INET; // IPv4
    m_ServerAddress.sin_addr.s_addr = inet_addr(m_IpAddress);
    ;
    m_ServerAddress.sin_port = htons(m_PortNumber);

    // Bind the socket with the server address
    if (bind(m_SocketFileDescriptor, (const struct sockaddr *)&m_ServerAddress,
             sizeof(m_ServerAddress)) < 0)
    {
        perror("Binding failed");
        return false;
    }

    m_IsListening = true;
    m_ReceiverThread = new std::thread(&LogListener::Receive, this);
    return true;
}


void LogListener::SetMessageCallBack(void(*callBack)(LogMessage*))
{
    m_CallBack = callBack;
}


void LogListener::Receive()
{
    char buffer[4096];
    struct sockaddr clientAddress;
    memset(&clientAddress, 0, sizeof(clientAddress));
    int32_t clientLength = sizeof(clientAddress);

    while (m_IsListening)
    {
        memset(buffer, 0, sizeof(buffer));
        int n = recvfrom(m_SocketFileDescriptor, (char *)buffer,
                         4096, MSG_WAITALL, (struct sockaddr *)&clientAddress,
                         (socklen_t *)&clientLength);
        
        m_CallBack(ParseMessage(buffer));
    }
}

LogMessage *LogListener::ParseMessage(const char *message)
{
    LogMessage *parsedMessage = new LogMessage();
    parsedMessage->level = static_cast<EnumLogLevel>(message[0] - 48);
    const char *rest = &message[1];
    parsedMessage->message = rest;
    return parsedMessage;
}

void LogListener::CloseConnection()
{
    if (m_IsListening)
    {
        close(m_SocketFileDescriptor);
        // m_ReceiverThread->~thread();
        m_SocketFileDescriptor = -1;
        m_IsListening = false;
    }
}