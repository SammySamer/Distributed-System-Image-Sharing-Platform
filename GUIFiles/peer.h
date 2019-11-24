#include "message.h"
#include "udpsocketclient.h"
#include <QProcess>
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <locale>
#include <map>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <vector>
using namespace std;
//#include <limits.h> // for getcwd
#define BUFFER_SIZE 50000

#ifndef PEER_H
#define PEER_H
#define LITTLE_BUFFER_SIZE 50000
#define PATH_MAX 500

#include <QDebug>
#include <limits.h>
#include <string>
#include <unistd.h>
#include <string>
struct three_element_group{
    string name;
    string imagename;
    int views;
};

class Peer {
private:
  char sender_ip[INET_ADDRSTRLEN];
  uint16_t sender_port;

public:
  map<string, int> sharedimgs;
  fstream imgfile;
  fstream Myimgsfile;
  UDPSocketClient *sv;
  //char *dos_ip = "127.0.0.1";
  char *dos_ip = "10.40.37.126";
  int dos_port = 8080;
  UDPSocketClient *sc;
  struct sockaddr_in dosSocket;
  int requestID = 0;
  pair<string, int> viewer;
  map<string, vector<pair<string, int>>> myimages;
  const int timeout_loop = 10, timeout_time_ms = 100;
  int n;

  string username, password;
  unsigned char buffer[BUFFER_SIZE];
  int r;
  struct sockaddr_in recievedAddr;
  socklen_t addresslength = sizeof(recievedAddr);
  vector<pair<int, three_element_group>> requests_buffer;

  unsigned char Serverbuffer[BUFFER_SIZE];
  unsigned char Serverlittle_buffer[LITTLE_BUFFER_SIZE];

  map<string, vector<string>> users;
  map<string, vector<string>>::iterator it;

  Peer() {
    this->sv = new UDPSocketClient(); // UDPSocketServer(0); // dos_port
    this->sc = new UDPSocketClient();

  }
  ~Peer() {}

  void listenPeer() {
    cout << "Thread created and is listening" << endl;
    while (getRequest()) {
    }
  }


/* ------------------ SERVER FUNCTIONS ----------------------- */


  bool getRequest() {

    cout << "Peer's getting requests: " << endl;

    memset(Serverbuffer, 0, sizeof(Serverbuffer));

    // Receive Marshalled Message
    struct sockaddr_in tempSocketAddress;
    socklen_t tempAddrlen = sizeof(tempSocketAddress);
    if ((r = recvfrom(sv->s, Serverbuffer, BUFFER_SIZE, 0,
                      (struct sockaddr *)&tempSocketAddress, &tempAddrlen)) <
        0) {
      cout << "Failed to receive marshalled message..." << endl;
    }

    inet_ntop(AF_INET, &(tempSocketAddress.sin_addr), sender_ip,
              INET_ADDRSTRLEN);

    sender_port = htons((&tempSocketAddress)->sin_port);

    Message requestMsg(reinterpret_cast<char *>(Serverbuffer));
    int op_id_request = requestMsg.getOperation();
    int rpcid_request = requestMsg.getRPCId();
    string msg = requestMsg.getUnmarshalledMessage();

    switch (op_id_request) {

    case 2002: // view request
    {
      cout << "2002 request recieved" << endl;
      int cc = 0;
      string username = "", imageName = "", views_str = "";
      while (msg[cc] != '*') {
        username.append(1, msg[cc]);
        cc++;
      }
      cc++;
      while (msg[cc] != '*') {
        imageName.append(1, msg[cc]);
        cc++;
      }
      cc++;
      while (msg[cc] != '*') {
        views_str.append(1, msg[cc]);
        cc++;
      }

      cout << "User: " << username << endl;
      cout << "Requested Image Name: " << imageName << endl;

      memset(Serverlittle_buffer, 0, sizeof(Serverlittle_buffer));
       three_element_group r;
       r.name = username;
       r.imagename = imageName;
       r.views = stoi(views_str);
      pair<int, three_element_group> pair_temp =
          make_pair(2002, r);
      bool pair_found = false;
      for (int e = 0; e < requests_buffer.size() && !pair_found; e++) {
          pair_found = (requests_buffer[e].first == pair_temp.first && requests_buffer[e].second.name == pair_temp.second.name && requests_buffer[e].second.imagename == pair_temp.second.imagename && requests_buffer[e].second.views == pair_temp.second.views);
      }
      if (!pair_found) {
        requests_buffer.push_back(pair_temp);
        Serverlittle_buffer[0] = '1';
      } else {
        Serverlittle_buffer[0] = '0';
      }

      Serverlittle_buffer[1] = 0;

      char *msg_char = reinterpret_cast<char *>(this->Serverlittle_buffer);

      Message view_request_reply(Reply, msg_char, strlen(msg_char),
                                 op_id_request, rpcid_request, 0, 0, 0);
      string x_reply = view_request_reply.marshal();
      char marshalled_message_reply[BUFFER_SIZE];
      int n = x_reply.length();
      strncpy(marshalled_message_reply, x_reply.c_str(), n + 1);

      if (sendto(sv->s, marshalled_message_reply,
                 strlen((const char *)marshalled_message_reply), 0,
                 (struct sockaddr *)&tempSocketAddress, tempAddrlen) < 0) {
        perror("View Request reply sendto failed");
      }

      // End of changes

    } break; // end of view

    case 2004: // receive Image
    {
      // image length
      unsigned long current_received = 0;

      int fragd = requestMsg.getDidFrag();
      int MF = requestMsg.getMoreFrag();
      int fragc = requestMsg.getFragCount();
      int fraglength = requestMsg.getMessageSize();
      string image_owner = requestMsg.getImageOwner();
      string image_name = requestMsg.getImageName();

      string newFileName = image_owner + "_" + image_name;

      cout << "Received Successfully!! " << endl;
      cout << "Received Message Length = \n"
           << strlen((const char *)Serverbuffer);
      cout << "imfirstfrag length " << fraglength << endl;

      current_received += r;
      unsigned char little_buffer[20000];

      memset(little_buffer, 0, sizeof(little_buffer));
      sprintf((char *)(little_buffer), "%lu", current_received);

      // Marshal the reply and then send it
      char *msg_char = reinterpret_cast<char *>(little_buffer);

      cout << "little_buffer " << little_buffer << endl;

      Message image_reply(Reply, msg_char, strlen(msg_char), op_id_request,
                          rpcid_request, fragd, fragc, MF);

      string x_reply = image_reply.marshal();

      char marshalled_message_reply[BUFFER_SIZE];

      int n = x_reply.length();
      strncpy(marshalled_message_reply, x_reply.c_str(), n + 1);

      cout << "marshalled_message_reply " << marshalled_message_reply << endl;

      if (sendto(sv->s, marshalled_message_reply,
                 strlen(marshalled_message_reply), 0,
                 (struct sockaddr *)&tempSocketAddress, tempAddrlen) < 0) {
        perror("send to reply failed");
      }

      printf("%s.\n", "Start of image receive");

      std::ofstream os(newFileName);

      if (r > 0) {
        os << msg;
      }

      if (fragd) {

        // memset(buffer, 0, sizeof(buffer));
        memset(little_buffer, 0, sizeof(little_buffer));

        // Start Receiving Image
        while (MF) {
          cout << "Loop Start:" << endl;
          memset(little_buffer, 0, sizeof(little_buffer));

          r = recvfrom(sv->s, little_buffer, LITTLE_BUFFER_SIZE, 0,
                       (struct sockaddr *)&tempSocketAddress, &tempAddrlen);

          Message imageFrag(reinterpret_cast<char *>(little_buffer));

          string imfrag = imageFrag.getUnmarshalledMessage();
          int imfraglen = imageFrag.getMessageSize();
          int rpcid_new = imageFrag.getRPCId();
          int op_id_new = imageFrag.getOperation();
          int fragc_new = imageFrag.getFragCount();

          // string newY(imfrag,imfraglen);

          current_received += r;
          cout << "Received total " << current_received << " encoded bytes.\n";

          // Marshal the reply and then send it

          memset(little_buffer, 0, sizeof(little_buffer));
          sprintf((char *)(little_buffer), "%lu", current_received);

          char *msg_char = reinterpret_cast<char *>(little_buffer);

          Message image_reply(Reply, msg_char, strlen(msg_char), op_id_new,
                              rpcid_new, fragd, fragc_new, MF);

          string x_reply = image_reply.marshal();

          char marshalled_message_reply[BUFFER_SIZE];

          int n = x_reply.length();
          strncpy(marshalled_message_reply, x_reply.c_str(), n + 1);

          if (rpcid_request == rpcid_new && (++fragc) == fragc_new) {

            MF = imageFrag.getMoreFrag();

            if (r > 0) {
              os << imfrag;
            }

            if (sendto(sv->s, marshalled_message_reply,
                       strlen(marshalled_message_reply), 0,
                       (struct sockaddr *)&tempSocketAddress,
                       tempAddrlen) < 0) {
              perror("send to reply failed");
            }

            // End of changes

          } else if ((rpcid_request == rpcid_new) && (fragc == fragc_new)) {
            // Re-send the reply without writing the image
            // if the image fragment was sent before

            if (sendto(sv->s, marshalled_message_reply,
                       strlen(marshalled_message_reply), 0,
                       (struct sockaddr *)&tempSocketAddress,
                       tempAddrlen) < 0) {
              perror("send to reply failed");
            }
          }
        }
      }
      os.close();

      cout << "newFileName: " << newFileName << endl;

      string extract_command;

      extract_command = "steghide extract -sf " + newFileName +
                        " -p hk"; // get the embedded image

      QProcess::execute(QString::fromStdString(extract_command));

      string imagenodot;
      for (int j = image_name.length() - 1; j > 0; j--) {

        if (image_name[j] == '.') {
          imagenodot = image_name.substr(0, j);
          break;
        }
      }

      string newViewsName =
          this->username + "_" + imagenodot + "_" + "views.txt"; // views file name

      string views_extractcommand =
          "steghide extract -sf " + image_name + " -p hk"; // extract views file

      QProcess::execute(QString::fromStdString(views_extractcommand));

      // deleting the extracted image from the receiver's folder after
      // extracting the number of views
      string deletecommand = "rm " + image_name; // delete extracted image
      vector<string> images;

      QProcess::execute(QString::fromStdString(deletecommand));

      int views = 0;

      ifstream views_is(newViewsName);

      if (views_is) {

        views_is >> views;
        cout << "Image_name " << image_name << " image_owner " << image_owner
             << " views " << views << endl;
        this->newimg(image_name, image_owner, views);
        three_element_group r;
        r.name = image_owner;
        r.imagename = image_name;
        r.views = views;
        requests_buffer.push_back(
            make_pair(2004, r));
        views_is.close();
        string deletecommand2 = "rm " + newViewsName; // delete extracted views
        QProcess::execute(QString::fromStdString(deletecommand2));
      } else {
        cout << "couldn't open views file! " << endl;
      }
    } break;

    case 2005: // notify request
    {
      cout << "2005 request recieved" << endl;
      int cc = 0;
      string username = "", imageName = "", NewNoViews = "";
      while (msg[cc] != '*') {
        username.append(1, msg[cc]);
        cc++;
      }
      cc++;
      while (msg[cc] != '*') {
        imageName.append(1, msg[cc]);
        cc++;
      }
      cc++;
      while (msg[cc] != '*') {
        NewNoViews.append(1, msg[cc]);
        cc++;
      }
      cout << "User: " << username << endl;
      cout << "Requested Image Name: " << imageName << endl;
      cout << "New Requested No of Views: " << NewNoViews << endl;
      vector<pair<string, int>> v_vectors;
      v_vectors = myimages[imageName];
      for (int i = 0; i < v_vectors.size(); i++) {
        if (v_vectors[i].first == username)
          v_vectors[i].second = stoi(NewNoViews);
      }

      myimages[imageName] = v_vectors;
      memset(Serverlittle_buffer, 0, sizeof(Serverlittle_buffer));

      //      pair<int, pair<string, string>> pair_temp =
      //          make_pair(2002, make_pair(username, imageName));
      //      bool pair_found = false;
      //      for (int e = 0; e < requests_buffer.size() && !pair_found; e++) {
      //        pair_found = requests_buffer[e] == pair_temp;
      //      }
      //      if (!pair_found) {
      //        requests_buffer.push_back(pair_temp);
      //        Serverlittle_buffer[0] = '1';
      //      } else {
      //        Serverlittle_buffer[0] = '0';
      //      }

      Serverlittle_buffer[0] = '1';
      Serverlittle_buffer[1] = 0;

      char *msg_char = reinterpret_cast<char *>(this->Serverlittle_buffer);

      Message view_request_reply(Reply, msg_char, strlen(msg_char),
                                 op_id_request, rpcid_request, 0, 0, 0);
      string x_reply = view_request_reply.marshal();
      char marshalled_message_reply[BUFFER_SIZE];
      int n = x_reply.length();
      strncpy(marshalled_message_reply, x_reply.c_str(), n + 1);

      if (sendto(sv->s, marshalled_message_reply,
                 strlen((const char *)marshalled_message_reply), 0,
                 (struct sockaddr *)&tempSocketAddress, tempAddrlen) < 0) {
        perror("View Request reply sendto failed");
      }

      // End of changes

    } break; // end of view

    case 2006: // view request
    {
      cout << "2006 request recieved" << endl;
      int cc = 0;
      string username = "", imageName = "", NewNoViews = "";
      while (msg[cc] != '*') {
        username.append(1, msg[cc]);
        cc++;
      }
      cc++;
      while (msg[cc] != '*') {
        imageName.append(1, msg[cc]);
        cc++;
      }
      cc++;
      while (msg[cc] != '*') {
        NewNoViews.append(1, msg[cc]);
        cc++;
      }
      cout << "User: " << username << endl;
      cout << "Requested Image Name: " << imageName << endl;
      cout << "New Requested No of Views: " << NewNoViews << endl;

      bool c = updateimg(imageName, username, stoi(NewNoViews));
      if (!c) {
        Serverlittle_buffer[0] = '0';
        cout << "Views failed to update" << endl;
      }

      cout << "Views Updated Successfully!" << endl;
      Serverlittle_buffer[0] = '1';
      Serverlittle_buffer[1] = 0;

      char *msg_char = reinterpret_cast<char *>(this->Serverlittle_buffer);

      Message view_request_reply(Reply, msg_char, strlen(msg_char),
                                 op_id_request, rpcid_request, 0, 0, 0);
      string x_reply = view_request_reply.marshal();
      char marshalled_message_reply[BUFFER_SIZE];
      int n = x_reply.length();
      strncpy(marshalled_message_reply, x_reply.c_str(), n + 1);

      if (sendto(sv->s, marshalled_message_reply,
                 strlen((const char *)marshalled_message_reply), 0,
                 (struct sockaddr *)&tempSocketAddress, tempAddrlen) < 0) {
        perror("View Request reply sendto failed");
      }

      // End of changes

    } break; // end of view

    case 2007: // view request
    {
      cout << "2007 request recieved" << endl;
      int cc = 0;
      string username = "", imageName = "", NewNoViews = "";
      while (msg[cc] != '*') {
        username.append(1, msg[cc]);
        cc++;
      }
      cc++;
      while (msg[cc] != '*') {
        imageName.append(1, msg[cc]);
        cc++;
      }
      cc++;
      while (msg[cc] != '*') {
        NewNoViews.append(1, msg[cc]);
        cc++;
      }
      cout << "User: " << username << endl;
      cout << "Requested Image Name: " << imageName << endl;
      cout << "New Requested No of Views: " << NewNoViews << endl;

      memset(Serverlittle_buffer, 0, sizeof(Serverlittle_buffer));
       three_element_group r;
       r.name = username;
       r.imagename = imageName;
       r.views = stoi(NewNoViews);
      pair<int, three_element_group> pair_temp =
          make_pair(2007, r);
      bool pair_found = false;
      for (int e = 0; e < requests_buffer.size() && !pair_found; e++) {
        pair_found = (requests_buffer[e].first == pair_temp.first && requests_buffer[e].second.name == pair_temp.second.name && requests_buffer[e].second.imagename == pair_temp.second.imagename && requests_buffer[e].second.views == pair_temp.second.views);
      }
      if (!pair_found) {
        requests_buffer.push_back(pair_temp);
        Serverlittle_buffer[0] = '1';
      } else {
        Serverlittle_buffer[0] = '0';
      }

      Serverlittle_buffer[1] = 0;

      char *msg_char = reinterpret_cast<char *>(this->Serverlittle_buffer);

      Message view_request_reply(Reply, msg_char, strlen(msg_char),
                                 op_id_request, rpcid_request, 0, 0, 0);
      string x_reply = view_request_reply.marshal();
      char marshalled_message_reply[BUFFER_SIZE];
      int n = x_reply.length();
      strncpy(marshalled_message_reply, x_reply.c_str(), n + 1);

      if (sendto(sv->s, marshalled_message_reply,
                 strlen((const char *)marshalled_message_reply), 0,
                 (struct sockaddr *)&tempSocketAddress, tempAddrlen) < 0) {
        perror("View Request reply sendto failed");
      }

      // End of changes

    } break; // end of view

    default:
      break;
    }
    return true;
  }

  void sendReply(char *temp_buffer) {

    struct sockaddr_in recievedAddr;
    socklen_t addresslength = sizeof(recievedAddr);
    if (sendto(sv->s, temp_buffer, strlen(temp_buffer), 0,
               (struct sockaddr *)&recievedAddr, addresslength) < 0) {
      perror("sendto failed");
    }
  }

  // End of Server Functions

  static void makeDestSA(struct sockaddr_in *sa, char *hostname, int port) {
    struct hostent *host;
    sa->sin_family = AF_INET;
    if ((host = gethostbyname(hostname)) == NULL) {
        qDebug("Un kown Host");

       printf("Unknown host name\n");
      exit(-1);
    }

    qDebug("UWORKED");

   printf("known host name\n");
    sa->sin_addr = *(struct in_addr *)(host->h_addr);
    sa->sin_port = htons(port);
  }

  int sign_up(string username, string password) {
    const char* UN=username.c_str();
    const char* PW=password.c_str();
    qDebug("%s %s",UN, PW);
    bool nospecial = true;
    for (int i = 0; i < username.length() && nospecial; i++) {
      nospecial = isalnum(username[i]);
    }
    for (int i = 0; i < password.length() && nospecial; i++) {
      nospecial = isalnum(password[i]);
    }
    if (!nospecial) { // special chars
      return 3;       // error code for special chars as they are delimiters
    } else {
      struct sockaddr_in dosSocketAddress;

      const char* d_ip=dos_ip;
     // const char* PW=password.c_str();
      qDebug("%s",d_ip);
      makeDestSA(&dosSocketAddress, dos_ip, dos_port);

      string msg = username + "*" + password;
      char *msg_char = new char[msg.length() + 1];
      strcpy(msg_char, msg.c_str());

      Message sign_up_request(Request, msg_char, strlen(msg_char), 1001,
                              ++requestID, 0, 0, 0);

      char marshalled_message[BUFFER_SIZE];

      string x = sign_up_request.marshal();
      int n = x.length();
      strncpy(marshalled_message, x.c_str(), n + 1);

      int r = 1;

      // Timeout Tolerance
      int npoll = 0, no_tries = 0;
      struct pollfd ss;
      ss.fd = sc->s;
      ss.events = POLLIN;

      while(no_tries <= timeout_loop && (npoll == 0 || npoll == -1)){ // while timeout, resend
          if ((n = sendto(sc->s, marshalled_message,
                          strlen((const char *)marshalled_message), 0,
                          (struct sockaddr *)&dosSocketAddress,
                          sizeof(struct sockaddr_in))) < 0){
              return 6; // send failed
          }
          npoll = poll(&ss, 1, timeout_time_ms);
          no_tries++;
      }
        if (npoll == 0 || npoll == -1) { // timeout
          return 2;
        }
        else {
      unsigned char little_buffer[10];
      memset(little_buffer, 0, sizeof(little_buffer));
      struct sockaddr_in tempSocketAddress;
      socklen_t tempAddrlen = sizeof(tempSocketAddress);
      if ((r = recvfrom(sc->s, little_buffer, 10, 0,
                        (struct sockaddr *)&tempSocketAddress, &tempAddrlen)) <
          0)
        perror("Receive Failed");

      if (little_buffer[0] == '1')
        return 1;
      else if (little_buffer[0] == '0')
        return 5;
      else
        return 8;
    }
    }
  }

  int login(string username, string password) {
    bool nospecial = true;
    for (int i = 0; i < username.length() && nospecial; i++) {
      nospecial = isalnum(username[i]);
    }
    for (int i = 0; i < password.length() && nospecial; i++) {
      nospecial = isalnum(password[i]);
    }
    if (!nospecial) { // special chars
      return 3;       // error code for special chars as they are delimiters
    } else {

      makeDestSA(&(this->dosSocket), dos_ip, dos_port); // make dos socket

      string msg = username + "*" + password;
      char *msg_char = new char[msg.length() + 1];
      strcpy(msg_char, msg.c_str());
      cout << msg_char << endl;

      Message login_request(Request, msg_char, strlen(msg_char), 1002,
                            ++requestID, 0, 0, 0);

      char marshalled_message[BUFFER_SIZE];

      string x = login_request.marshal();
      int n = x.length();
      strncpy(marshalled_message, x.c_str(), n + 1);

      int r = 1;

      // Timeout Tolerance
      int npoll = 0, no_tries = 0;
      struct pollfd ss;
      ss.fd = sv->s;
      ss.events = POLLIN;

      while(no_tries <= timeout_loop && (npoll == 0 || npoll == -1)){ // while timeout, resend
          if ((n = sendto(sv->s, marshalled_message, // sc->s
                          strlen((const char *)marshalled_message), 0,
                          (struct sockaddr *)&(this->dosSocket),
                          sizeof(struct sockaddr_in))) < 0){
              return 6; // send failed
          }
          npoll = poll(&ss, 1, timeout_time_ms);
          no_tries++;
      }
         if (npoll == 0 || npoll == -1) {
           return 2;
         } else {
      unsigned char little_buffer[10];
      memset(little_buffer, 0, sizeof(little_buffer));
      struct sockaddr_in tempSocketAddress;
      socklen_t tempAddrlen = sizeof(tempSocketAddress);
      if ((r = recvfrom(sv->s, little_buffer, 10, 0, // sc->s
                        (struct sockaddr *)&tempSocketAddress, &tempAddrlen)) <
          0)
        perror("Receive Failed");
      if (little_buffer[0] == '1')
        return 1;
      else if (little_buffer[0] == '0')
        return 0;
      else if (little_buffer[0] == '5')
        return 5;
      else if (little_buffer[0] == '4')
        return 4;
      else
        return 8;
    }
  }
  }

  int logout() {

    string msg = this->username;
    char *msg_char = new char[msg.length() + 1];
    strcpy(msg_char, msg.c_str());

    Message logout_request(Request, msg_char, strlen(msg_char), 1003,
                           ++requestID, 0, 0, 0);
    char marshalled_message[BUFFER_SIZE];

    string x = logout_request.marshal();
    int n = x.length();
    strncpy(marshalled_message, x.c_str(), n + 1);

    int r = 1;

    // Timeout Tolerance
    int npoll = 0, no_tries = 0;
    struct pollfd ss;
    ss.fd = sc->s;
    ss.events = POLLIN;

    while(no_tries <= timeout_loop && (npoll == 0 || npoll == -1)){ // while timeout, resend
        if ((n = sendto(sc->s, marshalled_message,
                        strlen((const char *)marshalled_message), 0,
                        (struct sockaddr *)&(this->dosSocket),
                        sizeof(struct sockaddr_in))) < 0){
            return 6; // send failed
        }
        npoll = poll(&ss, 1, timeout_time_ms);
        no_tries++;
    }

       if (npoll == 0 || npoll == -1) {
         return 2;
       } else {

    unsigned char little_buffer[10];
    memset(little_buffer, 0, sizeof(little_buffer));
    struct sockaddr_in tempSocketAddress;
    socklen_t tempAddrlen = sizeof(tempSocketAddress);
    if ((r = recvfrom(sc->s, little_buffer, 10, 0,
                      (struct sockaddr *)&tempSocketAddress, &tempAddrlen)) < 0)
      perror("Receive Failed");
    // close bound sockets
    close(sc->s);
    close(sv->s);
    if (little_buffer[0] == '1')
      return 1;
    else
      return 0;
       }
  }

  std::string getexepath() {
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    return std::string(result, (count > 0) ? count : 0);
  }

  int upload(string imagepath) {
    cout << imagepath << endl;
    if (imagepath == "")
      return 6;

    string pathname, imagename;
    for (int j = imagepath.length() - 1; j > 0; j--) {
      if (imagepath[j] == '/') {
        pathname = imagepath.substr(0, j);
        imagename = imagepath.substr(j + 1, imagepath.length() - 1);
        break;
      }
    }

    bool nospecial = true;

    for (int i = 0; i < imagename.length() && nospecial; i++) {
      nospecial =
          isalnum(imagename[i]) || imagename[i] == '.' || imagename[i] == '/';
      if (!nospecial)
        cout << "Special char is " << imagename[i] << endl;
    }
    if (!nospecial) { // special chars
      return 3;       // error code for special chars as they are delimiters
    } else {
      string msg = this->username + "*" + imagename;
      char *msg_char = new char[msg.length() + 1];
      strcpy(msg_char, msg.c_str());

      Message upload_request(Request, msg_char, strlen(msg_char), 2001,
                             ++requestID, 0, 0, 0);

      char marshalled_message[BUFFER_SIZE];

      string x = upload_request.marshal();
      int n = x.length();
      strncpy(marshalled_message, x.c_str(), n + 1);

      int r = 1;

      // Timeout Tolerance
      int npoll = 0, no_tries = 0;
      struct pollfd ss;
      ss.fd = sc->s;
      ss.events = POLLIN;

      while(no_tries <= timeout_loop && (npoll == 0 || npoll == -1)){ // while timeout, resend
          if ((n = sendto(sc->s, marshalled_message,
                          strlen((const char *)marshalled_message), 0,
                          (struct sockaddr *)&(this->dosSocket),
                          sizeof(struct sockaddr_in))) < 0){
              return 30; // send failed
          }
          npoll = poll(&ss, 1, timeout_time_ms);
          no_tries++;
      }
        if (npoll == 0 || npoll == -1) {
          return 2;
        } else {
      unsigned char little_buffer[10];
      memset(little_buffer, 0, sizeof(little_buffer));
      struct sockaddr_in tempSocketAddress;
      socklen_t tempAddrlen = sizeof(tempSocketAddress);
      if ((r = recvfrom(sc->s, little_buffer, 10, 0,
                        (struct sockaddr *)&tempSocketAddress, &tempAddrlen)) <
          0)
        perror("Receive Failed");

      string projectpath = this->getexepath();

      for (int j = projectpath.length() - 1; j > 0; j--) {

        if (projectpath[j] == '/') {
          projectpath = projectpath.substr(0, j);
          break;
        }
      }

      cout << "projectpath " << projectpath << endl;
      if (little_buffer[0] == '1') {

        QProcess::execute(QString::fromStdString(
            "cp " + projectpath + "/default.jpeg " + projectpath + "/" +
            this->username + "_" + imagename));

        if (pathname != projectpath)
          QProcess::execute(QString::fromStdString(
              "cp " + imagepath + " " + projectpath + "/" + imagename));

        vector<pair<string, int>> newimage;
        myimages[imagename] = newimage;

        return 1;

      } else if (little_buffer[0] == '0') {
        return 0;
      } else if (little_buffer[0] == '9') {
        return 9;
      } else
        return 8;
    }
  }
  }

  int notify_views_by_viewer(string owner, string selectedImage,
                             int NewNoViews) {
    vector<string> images;
    int resultUsers = this->getUsers();
    if(resultUsers == 1){
        images = this->users[owner];
        struct sockaddr_in ownerSocket;
        cout << images[1] << endl;
        string temp_ip = images[1];
        cout << images[2] << endl;
        int remote_peer_port = std::stoi(images[2], nullptr, 0); // Refaay
        cout << "Remote IP " << temp_ip << ", port " << remote_peer_port << endl;
        char remote_peer_address[1024];
        char little_buffer[100];
        strcpy(remote_peer_address, temp_ip.c_str());

        makeDestSA(&ownerSocket, remote_peer_address, remote_peer_port);

        // string image_with_full_path = path_full + "/" + selectedImage;
        string msg = this->username + "*" + selectedImage + "*" +
                     to_string(NewNoViews) + "*";
        char *msg_char = new char[msg.length() + 1];
        strcpy(msg_char, msg.c_str());

        Message notify_views_message(Request, msg_char, strlen(msg_char), 2005,
                                     ++requestID, 0, 0, 0);
        //  char * marshalled_message = image_request.marshal();

        char marshalled_message[BUFFER_SIZE];

        string x = notify_views_message.marshal();
        int n = x.length();
        strncpy(marshalled_message, x.c_str(), n + 1);

        if ((n = sendto(sc->s, marshalled_message,
                        strlen((const char *)marshalled_message), 0,
                        (struct sockaddr *)&ownerSocket,
                        sizeof(struct sockaddr_in))) < 0)
          perror("Send failed\n");

        memset(little_buffer, 0, sizeof(little_buffer));

        // Preparing a temp socket address to store the address of the receiver
        // sending the reply in
        struct sockaddr_in tempSocketAddress;
        socklen_t tempAddrlen = sizeof(tempSocketAddress);

        if ((r = recvfrom(sc->s, little_buffer, sizeof(little_buffer), 0,
                          (struct sockaddr *)&tempSocketAddress, &tempAddrlen)) <
            0) {
          cout << "Receive Failed! " << endl;
          return 3;
        } else {
          // unmarshal the reply
          Message reply_view_request(reinterpret_cast<char *>(little_buffer));

          string msg_view_request_reply =
              reply_view_request.getUnmarshalledMessage();

          // Hassan's resend if lost
          int rpc_id_new = reply_view_request.getRPCId();

          if (rpc_id_new != requestID) // check if the request matches the reply
          {
            // if not resend the request
            int i = notify_views_by_viewer(owner, selectedImage, NewNoViews);
          }
          else{
              cout << "msg_received_length " << msg_view_request_reply << endl;
              if (msg_view_request_reply[0] == '1')
                return 1;
              else if (msg_view_request_reply[0] == '0')
                return 0;
              else
                return 2;
          }
        }
    }
    else if(resultUsers == 2){
        return 12;
    }
    else if(resultUsers == 0){
        return 10;
    }
    else{
        return 15;
    }

  }
  int update_views_by_owner(string viewer, string selectedImage,
                            int NewNoViews) {
    cout << "Update Views Invoked" << endl;
    vector<string> images;
    int resultUsers = this->getUsers();
    if(resultUsers == 1){
    images = this->users[viewer];
    struct sockaddr_in viewerSocket;

    cout << images[1] << endl;

    string temp_ip = images[1];
    cout << images[2] << endl;
    int remote_peer_port = std::stoi(images[2], nullptr, 0); // Refaay
    cout << "Remote IP " << temp_ip << ", port " << remote_peer_port << endl;
    char remote_peer_address[1024];
    char little_buffer[100];
    strcpy(remote_peer_address, temp_ip.c_str());

    makeDestSA(&viewerSocket, remote_peer_address, remote_peer_port);

    // string image_with_full_path = path_full + "/" + selectedImage;
    string msg = this->username + "*" + selectedImage + "*" +
                 to_string(NewNoViews) + "*";
    char *msg_char = new char[msg.length() + 1];
    strcpy(msg_char, msg.c_str());

    Message update_views_message(Request, msg_char, strlen(msg_char), 2006,
                                 ++requestID, 0, 0, 0);
    //  char * marshalled_message = image_request.marshal();

    char marshalled_message[BUFFER_SIZE];

    string x = update_views_message.marshal();
    int n = x.length();
    strncpy(marshalled_message, x.c_str(), n + 1);

    if ((n = sendto(sc->s, marshalled_message,
                    strlen((const char *)marshalled_message), 0,
                    (struct sockaddr *)&viewerSocket,
                    sizeof(struct sockaddr_in))) < 0)
      perror("Send failed\n");

    memset(little_buffer, 0, sizeof(little_buffer));

    // Preparing a temp socket address to store the address of the receiver
    // sending the reply in
    struct sockaddr_in tempSocketAddress;
    socklen_t tempAddrlen = sizeof(tempSocketAddress);

    if ((r = recvfrom(sc->s, little_buffer, sizeof(little_buffer), 0,
                      (struct sockaddr *)&tempSocketAddress, &tempAddrlen)) <
        0) {
      cout << "Receive Failed! " << endl;
      return 3;
    } else {
      // unmarshal the reply
      Message reply_update_request(reinterpret_cast<char *>(little_buffer));

      string msg_view_request_reply =
          reply_update_request.getUnmarshalledMessage();

      // Hassan's resend if lost
      int rpc_id_new = reply_update_request.getRPCId();

      if (rpc_id_new != requestID) // check if the request matches the reply
      {
        // if not resend the request
        int i = update_views_by_owner(viewer, selectedImage, NewNoViews);
      }
      else{
          cout << "msg_received_length " << msg_view_request_reply << endl;
          if (msg_view_request_reply[0] == '1') {
            update_my_img(selectedImage, viewer, NewNoViews);
            return 1;
          } else if (msg_view_request_reply[0] == '0')
            return 0;
          else
            return 2;
      }


    }
          }
        else if(resultUsers == 2){
          return 12;
        }
        else if(resultUsers == 0){
          return 10;
        }
        else{
          return 15;
        }
  }

  int update_views_request_by_viewer(string owner, string selectedImage,
                            int NewNoViews) {
    cout << "Update Views Request by Viewer Invoked" << endl;
    vector<string> images;
    int resultUsers = this->getUsers();
    if(resultUsers == 1){
    images = this->users[owner];
    struct sockaddr_in viewerSocket;

    cout << images[1] << endl;

    string temp_ip = images[1];
    cout << images[2] << endl;
    int remote_peer_port = std::stoi(images[2], nullptr, 0); // Refaay
    cout << "Remote IP " << temp_ip << ", port " << remote_peer_port << endl;
    char remote_peer_address[1024];
    char little_buffer[100];
    strcpy(remote_peer_address, temp_ip.c_str());

    makeDestSA(&viewerSocket, remote_peer_address, remote_peer_port);

    // string image_with_full_path = path_full + "/" + selectedImage;
    string msg = this->username + "*" + selectedImage + "*" +
                 to_string(NewNoViews) + "*";
    char *msg_char = new char[msg.length() + 1];
    strcpy(msg_char, msg.c_str());

    Message update_views_message(Request, msg_char, strlen(msg_char), 2007,
                                 ++requestID, 0, 0, 0);
    //  char * marshalled_message = image_request.marshal();

    char marshalled_message[BUFFER_SIZE];

    string x = update_views_message.marshal();
    int n = x.length();
    strncpy(marshalled_message, x.c_str(), n + 1);

    if ((n = sendto(sc->s, marshalled_message,
                    strlen((const char *)marshalled_message), 0,
                    (struct sockaddr *)&viewerSocket,
                    sizeof(struct sockaddr_in))) < 0)
      perror("Send failed\n");

    memset(little_buffer, 0, sizeof(little_buffer));

    // Preparing a temp socket address to store the address of the receiver
    // sending the reply in
    struct sockaddr_in tempSocketAddress;
    socklen_t tempAddrlen = sizeof(tempSocketAddress);

    if ((r = recvfrom(sc->s, little_buffer, sizeof(little_buffer), 0,
                      (struct sockaddr *)&tempSocketAddress, &tempAddrlen)) <
        0) {
      cout << "Receive Failed! " << endl;
      return 3;
    } else {
      // unmarshal the reply
      Message reply_update_request(reinterpret_cast<char *>(little_buffer));

      string msg_view_request_reply =
          reply_update_request.getUnmarshalledMessage();

      // Hassan's resend if lost
      int rpc_id_new = reply_update_request.getRPCId();

      if (rpc_id_new != requestID) // check if the request matches the reply
      {
        // if not resend the request
        int i = update_views_request_by_viewer(owner, selectedImage, NewNoViews);
      }
      else{
          cout << "msg_received_length " << msg_view_request_reply << endl;
          if (msg_view_request_reply[0] == '1') {
            //update_my_img(selectedImage, owner, NewNoViews);
            return 1;
          } else if (msg_view_request_reply[0] == '0')
            return 0;
          else
            return 2;
      }


    }
    }
  else if(resultUsers == 2){
    return 12;
  }
  else if(resultUsers == 0){
    return 10;
  }
  else{
    return 15;
  }
  }

  int request_image(string selectedUser, string selectedImage, int views) {

    vector<string> images;
    images = this->users[selectedUser];
    struct sockaddr_in ownerSocket;

    string temp_ip = images[1];
    cout << images[1] << endl;
    int remote_peer_port = std::stoi(images[2], nullptr, 0); // Refaay
    cout << "Remote IP " << temp_ip << ", port " << remote_peer_port << endl;
    char remote_peer_address[1024];
    char little_buffer[100];
    strcpy(remote_peer_address, temp_ip.c_str());

    makeDestSA(&ownerSocket, remote_peer_address, remote_peer_port);

    // string image_with_full_path = path_full + "/" + selectedImage;
    string msg = this->username + "*" + selectedImage + "*" + std::to_string(views) + "*";
    char *msg_char = new char[msg.length() + 1];
    strcpy(msg_char, msg.c_str());

    Message image_request(Request, msg_char, strlen(msg_char), 2002,
                          ++requestID, 0, 0, 0);
    //  char * marshalled_message = image_request.marshal();

    char marshalled_message[BUFFER_SIZE];

    string x = image_request.marshal();
    int n = x.length();
    strncpy(marshalled_message, x.c_str(), n + 1);



    if ((n = sendto(sc->s, marshalled_message,
                    strlen((const char *)marshalled_message), 0,
                    (struct sockaddr *)&ownerSocket,
                    sizeof(struct sockaddr_in))) < 0)
      perror("Send failed\n");

    memset(little_buffer, 0, sizeof(little_buffer));

    // Preparing a temp socket address to store the address of the receiver
    // sending the reply in
    struct sockaddr_in tempSocketAddress;
    socklen_t tempAddrlen = sizeof(tempSocketAddress);

    if ((r = recvfrom(sc->s, little_buffer, sizeof(little_buffer), 0,
                      (struct sockaddr *)&tempSocketAddress, &tempAddrlen)) <
        0) {
      cout << "Receive Failed! " << endl;
      return 3;
    } else {
      // unmarshal the reply
      Message reply_view_request(reinterpret_cast<char *>(little_buffer));

      string msg_view_request_reply =
          reply_view_request.getUnmarshalledMessage();

      int rpc_id_new = reply_view_request.getRPCId();

      if (rpc_id_new != requestID) // check if the request matches the reply
      {
        // if not resend the request
        int i = request_image(selectedUser, selectedImage, views);
      }
      else{
          cout << "msg_received_length " << msg_view_request_reply << endl;
          if (msg_view_request_reply[0] == '1')
            return 1;
          else if (msg_view_request_reply[0] == '0')
            return 0;
          else
            return 2;
      }
    }
  }

  void send_image(string viewerName, string selectedImage, int nViews) {
    UDPSocketClient *newSock = new UDPSocketClient();
    vector<string> images;
    const int imageFrag_size = 20000;

    images = this->users[viewerName]; // Saving the vector of the selected
                                      // user image names & socket addresses
                                      // inside the variable images

    pair<string, int> newviewer;
    newviewer.first = viewerName;
    newviewer.second = nViews;
    map<string, vector<pair<string, int>>>::iterator it_2;
    myimages[selectedImage].push_back(newviewer);

    struct sockaddr_in
        receiverSocket; // instantiating the receiver socket address (the user
                        // who has requested my image)

    string remote_peer_ip = images[1]; // retriveing the ip of the receiver
    int remote_peer_port = stoi(images[2], nullptr, 0);
    cout << "Remote IP " << remote_peer_ip << ", port " << remote_peer_port
         << endl;

    // Preparing the receiver (remote peer) socket address
    char remote_peer_address[1024];
    strcpy(remote_peer_address, remote_peer_ip.c_str());
    makeDestSA(&receiverSocket, remote_peer_address, remote_peer_port);

    cout << "Starting to send the image " << endl;
    string steg_image_name = this->username + "_" + selectedImage;
    cout << "Image name: " << selectedImage << ". New name: " << steg_image_name
         << endl;

    // Embedding the views txt file in the image


    string imagenodot;
    for (int j = selectedImage.length() - 1; j > 0; j--) {

      if (selectedImage[j] == '.') {
        imagenodot = selectedImage.substr(0, j);
        break;
      }
    }

    string viewsFilename = viewerName + "_" + imagenodot + "_" + "views.txt";
    ofstream views_os(viewsFilename);

    if (views_os.is_open()) {
      views_os << nViews;
    }

    views_os.close();

    QProcess::execute(QString::fromStdString("steghide embed -cf " +
                                             selectedImage + " -ef " +
                                             viewsFilename + " -p hk "));

    QProcess::execute(QString::fromStdString("steghide embed -cf " +
                                             steg_image_name + " -ef " +
                                             selectedImage + " -p hk "));

    ifstream is(steg_image_name, ifstream::binary);

    if (is) {

      // getting length of file
      is.seekg(0, is.end);
      unsigned int length = is.tellg();
      is.seekg(0, is.beg);

      char *newbuffer = new char[length]; // allocate memory

      is.read(newbuffer, length); // read data as a block
      cout << "buffer length " << length << endl;

      is.close();

      char char_array[10001];
      string x;
      int current_length = 0;
      int current_length_before_marshalling = 0;
      unsigned long fakka = length % imageFrag_size; // last image chunk

      unsigned char little_buffer[2000];
      memset(little_buffer, 0, sizeof(little_buffer));

      requestID++;
      int fragc = 0;
      int MF = 1;
      int fragd = 0;
      if (length > imageFrag_size)
        fragd = 1;

      // Preparing a temp socket address to store the address of the receiver
      // sending the reply in
      struct sockaddr_in tempSocketAddress;
      socklen_t tempAddrlen = sizeof(tempSocketAddress);

      char message[BUFFER_SIZE];
      char message_fakka[BUFFER_SIZE];

      // Start Sending Image
      while (current_length_before_marshalling < length) {
        printf("%s\n", "Loop Start:");
        if (current_length_before_marshalling + imageFrag_size < length) {


          Message imageFrag(
              Request, (newbuffer + current_length_before_marshalling),
              imageFrag_size, 2004, requestID, fragd, ++fragc, MF);
          imageFrag.setImageOwner(this->username);
          imageFrag.setImageName(selectedImage);

          x = imageFrag.marshal();
          int n = x.length();
          strncpy(message, x.c_str(), n + 1);
          current_length += n;
          current_length_before_marshalling += imageFrag_size;

          //  cout<<message<<endl;
          if ((n = sendto(newSock->s, message, strlen(message), 0,
                          (struct sockaddr *)&receiverSocket,
                          sizeof(struct sockaddr_in))) < 0)
            perror("Send failed\n");
          cout << "Current Sent: " << strlen(message) << endl;
          printf("Current Sent Total = %d.\n", current_length);

          memset(little_buffer, 0, sizeof(little_buffer));
          if ((r = recvfrom(newSock->s, little_buffer, sizeof(little_buffer), 0,
                            (struct sockaddr *)&tempSocketAddress,
                            &tempAddrlen)) < 0)
            perror("Receive Failed");
          cout << "received length before unmarshalling: " << little_buffer
               << endl;

          // unmarshal the reply
          Message reply_received_length(
              reinterpret_cast<char *>(little_buffer));

          string msg_received_length =
              reply_received_length.getUnmarshalledMessage();

          cout << "msg_received_length " << msg_received_length << endl;
          // Change to long
          long received_length = stol(msg_received_length);
          cout << "received_length " << received_length << endl;

          // checks if the total length of the packets sent is equal to the
          // received length in the reply

          if (received_length != current_length) {
            // if not reset the current_length_before_marshalling in order to
            // resend the packet
            perror("Dropped packets! received_length != current_length.");
            current_length_before_marshalling -=
                imageFrag_size; // Drop Tolerance
          }

          printf("Current Received Total %d.\n", current_length);

        } else {
          // memcpy (char_array, message+(current_length), fakka);

          // char_array[fakka] = '\0'; // C-String terminate

          MF = 0;
          Message imageLastFrag(Request,
                                (newbuffer + current_length_before_marshalling),
                                fakka, 2004, requestID, fragd, ++fragc, MF);
          x = imageLastFrag.marshal();
          int n = x.length();

          strncpy(message_fakka, x.c_str(), n + 1);

          current_length += n;
          current_length_before_marshalling += fakka;

          //  cout<<message<<endl;
          if ((n = sendto(sc->s, message_fakka, strlen(message_fakka), 0,
                          (struct sockaddr *)&receiverSocket,
                          sizeof(struct sockaddr_in))) < 0)
            perror("Send failed\n");
          cout << "Current Sent: " << strlen(message_fakka) << endl;
          printf("Current Sent Total = %d.\n", current_length);

          memset(little_buffer, 0, sizeof(little_buffer));
          if ((r = recvfrom(sc->s, little_buffer, 2000, 0,
                            (struct sockaddr *)&tempSocketAddress,
                            &tempAddrlen)) < 0)
            perror("Receive Failed");
          cout << "received length before unmarshalling: " << little_buffer
               << endl;

          // unmarshal the reply
          Message reply_received_length(
              reinterpret_cast<char *>(little_buffer));

          string msg_received_length =
              reply_received_length.getUnmarshalledMessage();

          cout << "msg_received_length " << msg_received_length << endl;
          // Change to long
          long received_length = stol(msg_received_length);
          cout << "received_length " << received_length << endl;

          if (received_length != current_length) {
            perror("Dropped packets! received_length != current_length.");
            current_length_before_marshalling -= fakka; // Drop Tolerance
          }

          printf("Current Received Total %d.\n", current_length);
        }
      }
      printf("%s", "Message Sent. ");
      printf("Current %d.\n", current_length);

    } else
      cout << "couldn't open file" << endl;
  }

  int getUsers() {
    char *msg_char = new char[1];
    msg_char[0] = ' ';
    Message view_request(Request, msg_char, strlen(msg_char), 1100, ++requestID,
                         0, 0, 0);
    char marshalled_message[BUFFER_SIZE];

    string x = view_request.marshal();
    int n = x.length();
    strncpy(marshalled_message, x.c_str(), n + 1);

    int r = 1;

    // Timeout Tolerance
    int npoll = 0, no_tries = 0;
    struct pollfd ss;
    ss.fd = sc->s;
    ss.events = POLLIN;
    map<string, vector<string>> temp_m; // for empty return
    while(no_tries <= timeout_loop && (npoll == 0 || npoll == -1)){ // while timeout, resend
        if ((n = sendto(sc->s, marshalled_message,
                        strlen((const char *)marshalled_message), 0,
                        (struct sockaddr *)&(this->dosSocket),
                        sizeof(struct sockaddr_in))) < 0){
            return 0; // send failed
        }
        npoll = poll(&ss, 1, timeout_time_ms);
        no_tries++;
    }

       if (npoll == 0 || npoll == -1) { // return empty map
           map<string, vector<string>> temp_m;
         return 2;
       } else {

    cout << "getusers sent" << endl;
    unsigned char little_buffer[LITTLE_BUFFER_SIZE];
    memset(little_buffer, 0, sizeof(little_buffer));
    struct sockaddr_in tempSocketAddress;
    socklen_t tempAddrlen = sizeof(tempSocketAddress);
    if ((r = recvfrom(sc->s, little_buffer, LITTLE_BUFFER_SIZE, 0,
                      (struct sockaddr *)&tempSocketAddress, &tempAddrlen)) < 0)
      perror("Receive Failed");
    cout << "getusers received" << endl;
    int cc = 0;
    string temp;

    cout << little_buffer << endl;
    while (little_buffer[cc] != 0) {
      temp.append(1, little_buffer[cc]);
      cc++;
    }

    this->users = retmap(temp);
    return 1;
  }
  }

  map<string, vector<string>> retmap(string s) {
    map<string, vector<string>> mymap;
    while (s != "") {
      int len = s.find("*");
      string name = s.substr(0, len);
      s = s.erase(0, len + 1);
      int onlineln = s.find("&");
      string online = s.substr(0, onlineln);
      s = s.erase(0, onlineln + 1);
      int ipln = s.find("&");
      string ip = s.substr(0, ipln);
      s = s.erase(0, ipln + 1);
      int portln = s.find("&");
      string port = s.substr(0, portln);
      s = s.erase(0, portln + 1);
      mymap[name].push_back(online);
      mymap[name].push_back(ip);
      mymap[name].push_back(port);
      while (s[0] != '@') {
        int imgl = s.find("#");
        string img = s.substr(0, imgl);
        s = s.erase(0, imgl + 1);
        mymap[name].push_back(img);
      }
      s = s.erase(0, 1);
    }
    return mymap;
  }

  bool newimg(string img, string owner, int views) {
    // insert a new image in the map
    // if there is a duplicate of the image name and owner name, change the flag
    // to false.
    string imagename, ownername, fullname;
    for (auto const &x : sharedimgs) {
      fullname = x.first;
      for (int j = fullname.length() - 1; j > 0; j--) {
        if (fullname[j] == '_') {
          ownername = fullname.substr(0, j);
          imagename = fullname.substr(j + 1, fullname.length() - 1);
          break;
        }
      }
      if (imagename == img && ownername == owner) {
        return false;
      }
    }
    string newfullname = owner + "_" + img;
    sharedimgs[newfullname] = views;
    return true;
  }

  bool updateimg(string img, string owner, int views) {
    string imagename, ownername, fullname;
    for (auto const &x : sharedimgs) {
      fullname = x.first;
      for (int j = fullname.length() - 1; j > 0; j--) {
        if (fullname[j] == '_') {
          ownername = fullname.substr(0, j);
          imagename = fullname.substr(j + 1, fullname.length() - 1);
          break;
        }
      }
      if (imagename == img && ownername == owner) {
        string newfullname = owner + "_" + img;
        sharedimgs[newfullname] = views;
        return true;
      }
    }
    return false;
  }

  bool update_my_img(string img, string viewer, int views) {
    vector<pair<string, int>> v_vectors;
    v_vectors = myimages[img];
    for (int i = 0; i < v_vectors.size(); i++) {
      if (v_vectors[i].first == viewer)
        v_vectors[i].second = views;
    }

    myimages[img] = v_vectors;
    return false;
  }


  void updatefile() {
        imgfile.close();
      // call this function when the user is signing out, to update the file
      string shared_images_filename = this->username + "_sharedimages.txt";
      imgfile.open(shared_images_filename, fstream::out | fstream::in);

      for (auto const &x : sharedimgs) {
        imgfile << x.first << " " << x.second << endl;
      }
      imgfile.close();
    }

    void update_my_images_file() {
      // call this function when the user is signing out, to update the file
        Myimgsfile.close();
      string filename = this->username + "_images.txt";
      Myimgsfile.open(filename, fstream::out | fstream::in);

      for (auto const &x : myimages) {
        Myimgsfile << x.first << " ";
        for (int i = 0; i < x.second.size(); i++)
          Myimgsfile << x.second[i].first << " " << x.second[i].second << " ";
        Myimgsfile << endl;
      }

      Myimgsfile.close();
    }


    //called when the user logs in to read from their sharedimages.txt
    //to be loaded into our program's map
    void readfile() {

      string shared_images_filename = this->username + "_sharedimages.txt";
      imgfile.open(shared_images_filename, fstream::out | fstream::in | fstream::app);
      imgfile.seekp(0);
      while (!imgfile.eof()) {
        string line;
        getline(imgfile, line);
        if (line != "") {
          int name_len, owner_len, view_len;
          name_len = line.find(" ");
          string fullimagename = line.substr(0, name_len);
          line = line.erase(0, name_len + 1);
          view_len = line.find(" ");
          string views = line.substr(0, view_len);
          line = line.erase(0, view_len + 1);
          int view = stoi(views);
          sharedimgs[fullimagename] = view;
        }
      }
      imgfile.clear();
    }

    //called when the user logs in to read from their images.txt
    //to be loaded into our program's map
    void read_my_images_file() {
      string myimages_filename = this->username + "_images.txt";
      Myimgsfile.open(myimages_filename, fstream::out | fstream::in | fstream::app);
      Myimgsfile.seekp(0);
      pair<string, int> p;

      while (!Myimgsfile.eof()) {
        string line;
        getline(Myimgsfile, line);

        if (line != "") {
          int name_len, viewer_len, view_len;
          name_len = line.find(" ");
          string fullimagename = line.substr(0, name_len);
          line = line.erase(0, name_len + 1);

          if(line != "")
            while (line != "")
            {
                  viewer_len = line.find(" ");
                  string viewer = line.substr(0, viewer_len);
                  line = line.erase(0, viewer_len + 1);
                  view_len = line.find(" ");
                  string views = line.substr(0, view_len);
                  line = line.erase(0, view_len + 1);
                  int view = stoi(views);
                  p.first = viewer;
                  p.second = view;
                  myimages[fullimagename].push_back(p);
              }

            else
              myimages[fullimagename].push_back(p);

        }
      }
      Myimgsfile.clear();

    }

    string getCurrentTime() {
       time_t rawtime;
       struct tm *timeinfo;
       char buffer[80];

       time(&rawtime);
       timeinfo = localtime(&rawtime);

       strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
       std::string str(buffer);
       return str;
     }

};

#endif
