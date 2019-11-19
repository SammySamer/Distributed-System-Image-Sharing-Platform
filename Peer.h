#ifndef PEER_H
#define PEER_H
#pragma once

#include "Message.h"
#include "UDPSocketClient.h"
#include <map>
#include <vector>
#include <thread>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>

//#include <QProcess>

#include <arpa/inet.h>
#include <locale>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <string>
#include <unistd.h>

#define BUFFER_SIZE 50000
#define LITTLE_BUFFER_SIZE 50000
#define PATH_MAX 500

using namespace std;

struct three_element_group{
    string name;
    string imagename;
    int views;
};

class Peer
{
private:
    char ipSender [INET_ADDRSTRLEN];
    uint16_t sender_port;

public:

    const int TIMEOUT_RETRIES = 10, TIMEOUT_MS = 1;

    map<string, vector<string>> users;
    map<string, vector<string>>::iterator it;
    
    char *DOS_IP;
    int DOS_port;
    struct sockaddr_in DOS_Socket;
    
    UDPSocketClient *sv;
    UDPSocketClient *sc;

    string username, password;
    
    map<string, int> sharedimgs;
    fstream imgfile;
    fstream Myimgsfile;
    map<string, vector<pair<string, int>>> myimages;


    int requestID = 0;
    pair<string, int> viewer;
    int n;

    unsigned char buffer[BUFFER_SIZE];
    int r;
    struct sockaddr_in recievedAddr;
    socklen_t addresslength = sizeof(recievedAddr);
    vector<pair<int, three_element_group>> requests_buffer;

    unsigned char Serverbuffer[BUFFER_SIZE];
    unsigned char Serverlittle_buffer[LITTLE_BUFFER_SIZE];


    Peer()
    {
        this->sv = new UDPSocketClient();
        this->sc = new UDPSocketClient();
    }

    ~Peer() {}

    void peerListen()
    {
        cout << "Peer is listening\n";
        while (getRequest())
            ;
    }

    bool getRequest()
    {

    }

    void sendReply(char *tempBuffer)
    {
        struct sockaddr_in addrRecieved;
        socklen_t addressLength = sizeof(addrRecieved);

        if (sendto(sv->s, tempBuffer, strlen(tempBuffer), 0, 
        (struct sockaddr *)&addrRecieved, addressLength) < 0)
            perror("Failed to 'sendto'");

    }

    static void makeDestSA(struct sockaddr_in *sa, char *hostname, int port) {
        struct hostent *host;
        sa->sin_family = AF_INET;
        
        //if there's no hostname, exit
        if ((host = gethostbyname(hostname)) == NULL)
            exit(-1);

        sa->sin_addr = *(struct in_addr *)(host->h_addr);
        sa->sin_port = htons(port);
    }
    

    int signUp(string username, string password) {

        bool special = true;

        for (int i = 0; i < username.length() && special; i++) 
            special = isalnum(username[i]);
        
        for (int i = 0; i < password.length() && special; i++) 
            special = isalnum(password[i]);
        
        //if there are special characters, return error as they're delimiters
        if (!special) 
            return 3;

        //begin the sign up process
        else {
            
            struct sockaddr_in DoS_SockAddr;
            makeDestSA(&DoS_SockAddr, DOS_IP, DOS_port);

            string msg = username + "*" + password;
            char *msg_char = new char[msg.length() + 1];
            strcpy(msg_char, msg.c_str());

            Message Request_SignUp(Request, msg_char, strlen(msg_char), 1001, ++requestID, 0, 0, 0);

            char marshalled_message[BUFFER_SIZE];
            string x = Request_SignUp.marshal();
            int n = x.length();
            strncpy(marshalled_message, x.c_str(), n + 1);

            int r = 1;

            // Timeout Tolerance
            int npoll = 0, tries = 0;
            struct pollfd ss;
            ss.fd = sc->s;
            ss.events = POLLIN;

            //while there's a timeout, then resend
            while(tries <= TIMEOUT_RETRIES && (npoll == 0 || npoll == -1)){ 

                if ((n = sendto(sc->s, marshalled_message,
                                strlen((const char *)marshalled_message), 0,
                                (struct sockaddr *)&DoS_SockAddr,
                                sizeof(struct sockaddr_in))) < 0){
                    return 6; // send failed
                }

                npoll = poll(&ss, 1, TIMEOUT_MS);
                tries++;
            }

            //retries finished and there's still a timeout
            if (npoll == 0 || npoll == -1) {
                return 2;
            }
                
            else {
                unsigned char tempBuffer[10];
                memset(tempBuffer, 0, sizeof(tempBuffer));

                struct sockaddr_in tempSocketAddress;
                socklen_t tempAddrlen = sizeof(tempSocketAddress);
                if ((r = recvfrom(sc->s, tempBuffer, 10, 0,
                                    (struct sockaddr *)&tempSocketAddress, &tempAddrlen)) <
                    0)
                    perror("Receive Failed");

                if (tempBuffer[0] == '1')
                    return 1;
                else if (tempBuffer[0] == '0')
                    return 5;
                else
                    return 8;
            }
        }
    }





};
#endif
