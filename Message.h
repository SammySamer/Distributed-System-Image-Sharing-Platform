#ifndef MESSAGE_H
#define MESSAGE_H

#pragma once

#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

using namespace std;
enum MessageType { Request, Reply };

static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                        "abcdefghijklmnopqrstuvwxyz"
                                        "0123456789+/";

static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

class Message {
private:
  MessageType message_type;		//Either request or reply
  char *message;				      //Message payload
  string unmarshalledmessage;	//The unmarshalled message
  size_t messageSize;			    //Total length of the message
  int OPCode;				          //Number of the required operation
  int RPC_ID;					        //Message's ID
  bool didfrag;					      //A flag to indicate if the message is fragmented or not (1 when fragmented)
  int countFrag;				      //Unique id for each fragment
  int moreFrag;				        //A variable indicating if there are more fragments

public:


  static string base64_encode(unsigned char const *bytes_to_encode, unsigned int in_len) {
    string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (in_len--) {
      char_array_3[i++] = *(bytes_to_encode++);
      if (i == 3) {
        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] =
            ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] =
            ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (i = 0; i < 4; i++)
          ret += base64_chars[char_array_4[i]];
        i = 0;
      }
    }

    if (i) {
      for (j = i; j < 3; j++)
        char_array_3[j] = '\0';

      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] =
          ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] =
          ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

      for (j = 0; (j < i + 1); j++)
        ret += base64_chars[char_array_4[j]];

      while ((i++ < 3))
        ret += '=';
    }

    return ret;
  }

  static string base64_decode(string const &encoded_string) {
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    string ret;

    while (in_len-- && (encoded_string[in_] != '=') &&
           is_base64(encoded_string[in_])) {

      char_array_4[i++] = encoded_string[in_];
      in_++;
      if (i == 4) {
        for (i = 0; i < 4; i++)
          char_array_4[i] = base64_chars.find(char_array_4[i]);

        char_array_3[0] =
            (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] =
            ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (i = 0; (i < 3); i++)
          ret += char_array_3[i];
        i = 0;
      }
    }

    if (i) {
      for (j = 0; j < i; j++)
        char_array_4[j] = base64_chars.find(char_array_4[j]);

      char_array_3[0] =
          (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] =
          ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

      for (j = 0; (j < i - 1); j++)
        ret += char_array_3[j];
    }

    return ret;
  }

  Message(MessageType p_messageType, char *p_message, size_t p_messageSize,
          int operation, int p_RPCID, int p_didfrag, int p_fragCount,
          int p_moreFrag) {
    this->message_type = p_messageType;
    this->message = p_message;
    this->messageSize = p_messageSize;
    this->OPCode = operation;
    this->RPC_ID = p_RPCID;
    this->didfrag = p_didfrag;
    this->countFrag = p_fragCount;
    this->moreFrag = p_moreFrag;
  }

  
  Message(char *marshalled_base64) {
    string encoded_header = "";
    string encoded_msg = "";
    //  cout<<"marshalled_base64 "<<endl;
    int i = 0;
    while (marshalled_base64[i] != ' ') {
      encoded_header.append(1, marshalled_base64[i]);
      i++;
    }
    i++;
    while (marshalled_base64[i] != '\0') {
      encoded_msg.append(1, marshalled_base64[i]);
      i++;
    }
    // cout<<"encoded_header "<<encoded_header<<endl;
    string decoded_header = base64_decode(encoded_header);
    // cout<<"decoded_header "<<decoded_header<<endl;
    this->parseMsgHeader(decoded_header);
    //  cout<<"HERE!!!"<<endl;
    //  cout<<"encoded_msg "<<encoded_msg<<endl;
    string decoded_msg = base64_decode(encoded_msg);
    //  cout<<"decoded_msg "<<decoded_msg<<endl;

    int len = decoded_msg.length();

    this->unmarshalledmessage = decoded_msg;
    this->messageSize = len;
  }
  void parseMsgHeader(string decoded_header) {

    string msgtype, op, rpcid, msgsize, fragd, fragc, mfrag;
    int i = 0;
    while (decoded_header[i] != '-') {
      msgtype.append(1, decoded_header[i]);
      i++;
    }
    i++;
    while (decoded_header[i] != '-') {
      op.append(1, decoded_header[i]);
      i++;
    }
    i++;
    while (decoded_header[i] != '-') {
      rpcid.append(1, decoded_header[i]);
      i++;
    }
    i++;
    while (decoded_header[i] != '-') {
      msgsize.append(1, decoded_header[i]);
      i++;
    }
    i++;
    while (decoded_header[i] != '-') {
      fragd.append(1, decoded_header[i]);
      i++;
    }
    i++;
    while (decoded_header[i] != '-') {
      fragc.append(1, decoded_header[i]);
      i++;
    }
    i++;
    while (decoded_header[i] != '*') {
      mfrag.append(1, decoded_header[i]);
      i++;
    }

    if (msgtype == "0")
      this->message_type = Request;
    else
      this->message_type = Reply;

    this->OPCode = stoi(op);
    this->RPC_ID = stoi(rpcid);
    this->messageSize = stoi(msgsize);
    this->didfrag = stoi(fragd);
    this->countFrag = stoi(fragc);
    this->moreFrag = stoi(mfrag);
  }

  string marshal() {
    string encoded_msg = "";
    string headerinfo = to_string(message_type) + "-" + to_string(OPCode) +
                        "-" + to_string(RPC_ID) + "-" +
                        to_string(messageSize) + "-" + to_string(didfrag) +
                        "-" + to_string(countFrag) + "-" +
                        to_string(moreFrag) + "*";

    int n = headerinfo.length();
    unsigned int len = n + 1;
    char header_char[len];
    strcpy(header_char, headerinfo.c_str());

    // encoding message header
    encoded_msg = base64_encode((const unsigned char *)header_char, len);

    encoded_msg += " ";

    // encoding message payload
    encoded_msg += base64_encode((const unsigned char *)message, messageSize);

    return encoded_msg;
  }

  int getOperation() { return OPCode; }
  int getRPCId() { return RPC_ID; }
  char *getMessage() { return message; }
  string getUnmarshalledMessage() { return unmarshalledmessage; }
  size_t getMessageSize() { return messageSize; }
  MessageType getMessageType() { return message_type; }

  int getDidFrag() { return didfrag; }
  int getFragCount() { return countFrag; }
  int getMoreFrag() { return moreFrag; }
  void setOperation(int _operation) { this->OPCode = _operation; }
  void setMessage(char *message, size_t message_size) {
    this->message = message;
    this->messageSize = message_size;
  }
  void setMessageType(MessageType message_type) {
    this->message_type = message_type;
  }
  void setDidFrag(int p_didfrag) { this->didfrag = p_didfrag; }
  void setFragCount(int p_frag_count) { this->countFrag = p_frag_count; }
  void setMoreFrag(int p_more_frag) { this->moreFrag = p_more_frag; }
  ~Message() {}
};
#endif
