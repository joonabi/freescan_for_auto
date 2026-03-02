#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <thread>
class SocketHelper
{
public:
    SocketHelper();
    ~SocketHelper();
    int initSocket(const std::string& ip, int port);
    int releaseSocket();
    void sendMsg(const std::string& sendMsgStr);
    bool receiveMsg(std::string& recvMsgStr);
private:
    SOCKET m_socketClient;
    SOCKADDR_IN m_serverAddr;
};

