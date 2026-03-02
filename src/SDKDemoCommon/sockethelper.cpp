#include "sockethelper.h"
#include "zmq.h"
#include "iostream"
#include "glog/logging.h"
#include <thread>
#include "sn3dsdkpluginpub.h"
#include <QJsonDocument>
#include <QJsonParseError>

#define SOCKET_TCP 1
#define SOCKET_UDP 0
#pragma comment(lib, "Ws2_32.lib")
SocketHelper::SocketHelper()
{
}

SocketHelper::~SocketHelper()
{
}


int SocketHelper::initSocket(const std::string& ip, int port)
{
    WSADATA ws;
    int res = WSAStartup(MAKEWORD(2, 2), &ws);
    int socketType = SOCKET_TCP;
    if (0 != res)
    {
        return -1;
    }
    if (socketType == 0)
        m_socketClient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    else if (socketType == 1)
        m_socketClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (INVALID_SOCKET == m_socketClient)
    {
        WSACleanup();
        return -1;
    }

    m_serverAddr.sin_family = AF_INET;//TCPIP协议族
    m_serverAddr.sin_port = htons(port);//扫描仪软件监听端口号
    m_serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//本机全部地址
    ::inet_pton(AF_INET, ip.c_str(), &m_serverAddr.sin_addr);
    res =::connect(m_socketClient, (SOCKADDR*)&m_serverAddr, sizeof(SOCKADDR));
    if (SOCKET_ERROR == res)
    {
        closesocket(m_socketClient);
        WSACleanup();
        return -1;
    }


    return 0;
}

int SocketHelper::releaseSocket()
{
    if (m_socketClient)
    {
        int ret = closesocket(m_socketClient);
        WSACleanup();
        return ret;
    }    
    return SNSDK_ERR_OK;
}


void SocketHelper::sendMsg(const std::string& sendMsgStr)
{
    if (m_socketClient)
    {
        int ret = ::send(m_socketClient, sendMsgStr.c_str(), sendMsgStr.size(), 0);
        if (0 == ret)
        {

        }
    }

}

bool SocketHelper::receiveMsg(std::string& recvMsgStr)
{
    int iMode = 1; // 0:阻塞
    char recvBuf[4096];
    ioctlsocket(m_socketClient, FIONBIO, (u_long FAR*)&iMode);
    int bytesRead = recv(m_socketClient, recvBuf, sizeof(recvBuf), 0);
    if (0 == bytesRead)
    {
        closesocket(m_socketClient);
        return false;
    }
    if (-1 == bytesRead)
        return false;

	const char *head = recvBuf;
	do
	{
		QJsonParseError parseError = {};
		QJsonDocument jsonDec = QJsonDocument::fromJson(head, &parseError);

		if (parseError.error == QJsonParseError::NoError) 
		{
			recvMsgStr.append(jsonDec.toJson(QJsonDocument::Indented));
			int moveLength = jsonDec.toJson(QJsonDocument::Indented).size() + 1;//这里加1 是因为需要将指针偏移到结束符的后面
			head += moveLength;
		}
		else
		{
			break;
		}
	} while (head < recvBuf + bytesRead);

    return true;
}
