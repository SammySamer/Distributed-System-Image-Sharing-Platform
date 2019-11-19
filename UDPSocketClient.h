#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
using namespace std;
#ifndef UDPSOCKETCLIENT_H
#define UDPSOCKETCLIENT_H
class UDPSocketClient {
public:
  int s;
  UDPSocketClient() { initializeClient(); }
  bool initializeClient() {

    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      // perror("cannot create socket");
      cout << "error make client socket" << endl;
      return 0;
    }

    struct sockaddr_in clientAddress;

    memset((char *)&clientAddress, 0, sizeof(clientAddress));
    clientAddress.sin_family = AF_INET;
    clientAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    clientAddress.sin_port = htons(0);

    if (bind(s, (struct sockaddr *)&clientAddress, sizeof(clientAddress)) < 0) {
      // perror("bind failed");
      cout << "error bind client socket" << endl;
      return 0;
    }
  }
  ~UDPSocketClient() {}
};
#endif // UDPCLIENTSOCKET_H
