#include <string>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <iostream>
#include <thread>
#include <mutex>

#pragma comment(lib, "ws2_32.lib")
using namespace std;

mutex mtx;

void sendMessage();
void receiveMessage();

int main() {
    thread sendThread(sendMessage);
    thread receiveThread(receiveMessage);

    sendThread.join();
    receiveThread.join();

    return 0;
}

void sendMessage() {
    // Lock Thread
    mtx.lock();

    // Incoming Data
    const short maxChar = 500;
    char outgoingMessage[maxChar];
    const char quitKey[] = "q";

    // Designate Source and Destination IPs
    const char* pkt = "Message to be sent";

    // Local Host
    const char* srcIP = "127.0.0.1";
    const unsigned short port = 3514;

    //setup a socket and connection tools
    sockaddr_in dest;

    WSAData data;
    WSAStartup(MAKEWORD(2, 2), &data);

    // Create Socket
    SOCKET senderSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    // convert short integer value for port must be converted into network byte order
    dest.sin_family = AF_INET;

    // open stream oriented socket with internet address
    // & keep track of the socket descriptor
    inet_pton(AF_INET, srcIP, &dest.sin_addr.s_addr);
    dest.sin_port = htons(port);

    bind(senderSocket, (sockaddr*)&dest, sizeof(dest));

    mtx.unlock();

    while (true)
    {
        cout << ">> Enter a message, or [q] to disconnect: ";
        cin.getline(outgoingMessage, maxChar);

        if (strcmp(outgoingMessage, quitKey))
        {
            sendto(senderSocket, outgoingMessage, strlen(outgoingMessage), 0, (sockaddr*)&dest, sizeof(dest));
            cout << "\n" << "Message from [" << srcIP << "]: " << outgoingMessage << "\n";
        }
        else
        {
            cout << "\n Closing the program...";
            closesocket(senderSocket);
            WSACleanup();

            exit(0);
        }
    }
}

void receiveMessage() {
    // Lock Thread
    mtx.lock();

    // Incoming Data
    const short maxChar = 500;
    char incomingMessage[maxChar];

    // Local Host
    const char* srcIP = "127.0.0.1";
    const unsigned short port = 3515;

    //setup a socket and connection tools
    sockaddr_in local;
    int localLength = sizeof(local);

    WSAData data;
    WSAStartup(MAKEWORD(2, 2), &data);

    // Create Socket
    SOCKET receiverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    // convert short integer value for port must be converted into network byte order
    local.sin_family = AF_INET;

    // open stream oriented socket with internet address
    // & keep track of the socket descriptor
    inet_pton(AF_INET, srcIP, &local.sin_addr.s_addr);
    local.sin_port = htons(port);

    bind(receiverSocket, (sockaddr*)&local, sizeof(local));

    mtx.unlock();

    // > Listen Accept Messages
    // > Loop to grab message from Command Line
    // Receive

    while (true)
    {
        // Redeive text from sender
        recvfrom(receiverSocket, incomingMessage, maxChar, 0, (sockaddr*)&local, &localLength);

        mtx.lock();

        cout << "\n" << "Message from [" << srcIP << "]: " << incomingMessage << "\n";

        mtx.unlock();
    }
}