#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
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
  /* Message Details */
  MessageType message_type;   // Either request or Reply
  char *message;              
  size_t message_size;       
  string unmarshalledmessage; 

  /* Fragmentation Details */
  bool didfrag;               // A flag to indicate if the message is fragmented or not
  int countFrags;             // A unique id for each fragment
  int moreFrags;              // A variable indicating if there are more fragments

  /* OP Details */
  int operation;              // The OPCode basically
  int rpc_id;                 // Message ID, so that users aren't confused per operation

  string image_owner = " ";
  string image_name = " ";

public:
  static string base64_encode(unsigned char const *bytes_to_encode,
                                   unsigned int in_len) {
    string ret;
    int i = 0;
    int j = 0;
    unsigned char Size3[3];
    unsigned char Size4[4];

    while (in_len--) {
      Size3[i++] = *(bytes_to_encode++);
      if (i == 3) {

        //8'b
        Size4[0] = (Size3[0] & 0xfc) >> 2;
        Size4[1] = ((Size3[0] & 0x03) << 4) + ((Size3[1] & 0xf0) >> 4);
        Size4[2] =
            ((Size3[1] & 0x0f) << 2) + ((Size3[2] & 0xc0) >> 6);
        Size4[3] = Size3[2] & 0x3f;

        for (i = 0; (i < 4); i++)
          ret += base64_chars[Size4[i]];
        i = 0;
      }
    }

    if (i) {
      for (j = i; j < 3; j++)
        Size3[j] = '\0';

      Size4[0] = (Size3[0] & 0xfc) >> 2;
      Size4[1] =
          ((Size3[0] & 0x03) << 4) + ((Size3[1] & 0xf0) >> 4);
      Size4[2] =
          ((Size3[1] & 0x0f) << 2) + ((Size3[2] & 0xc0) >> 6);

      for (j = 0; (j < i + 1); j++)
        ret += base64_chars[Size4[j]];

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
    unsigned char Size4[4], Size3[3];
    string ret;

    while (in_len-- && (encoded_string[in_] != '=') &&
           is_base64(encoded_string[in_])) {
      Size4[i++] = encoded_string[in_];
      in_++;
      if (i == 4) {
        for (i = 0; i < 4; i++)
          Size4[i] = base64_chars.find(Size4[i]);

        Size3[0] =
            (Size4[0] << 2) + ((Size4[1] & 0x30) >> 4);
        Size3[1] =
            ((Size4[1] & 0xf) << 4) + ((Size4[2] & 0x3c) >> 2);
        Size3[2] = ((Size4[2] & 0x3) << 6) + Size4[3];

        for (i = 0; (i < 3); i++)
          ret += Size3[i];
        i = 0;
      }
    }

    if (i) {
      for (j = 0; j < i; j++)
        Size4[j] = base64_chars.find(Size4[j]);

      Size3[0] =
          (Size4[0] << 2) + ((Size4[1] & 0x30) >> 4);
      Size3[1] =
          ((Size4[1] & 0xf) << 4) + ((Size4[2] & 0x3c) >> 2);

      for (j = 0; (j < i - 1); j++)
        ret += Size3[j];
    }

    return ret;
  }

  Message(MessageType p_message_type, char *p_message, size_t p_message_size,
          int operation, int p_rpc_id, bool p_didfrag, int p_countFrags,
          int p_moreFrags) {
    this->message_type = p_message_type;
    this->message = p_message;
    this->message_size = p_message_size;
    this->operation = operation;
    this->rpc_id = p_rpc_id;
    this->didfrag = p_didfrag;
    this->countFrags = p_countFrags;
    this->moreFrags = p_moreFrags;
  }


  Message(char *marshalled_base64) {
    string encoded_header = "";
    string encoded_msg = "";
    int i = 0;
    
    cout<<"marshalled_base64 "<<endl;
    
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
    //  cout<<"decoded_msg "<<decoded_msg<<endl;

    string decoded_msg = base64_decode(encoded_msg);
    int len = decoded_msg.length();

    this->unmarshalledmessage = decoded_msg;
    this->message_size = len;
  }


  void parseMsgHeader(string decoded_header) {

    string msgtype, op, rpcid, msgsize, fragd, fragc, mfrag;
    string owner, filename;
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
    while (decoded_header[i] != '-') {
      mfrag.append(1, decoded_header[i]);
      i++;
    }

    i++;
    while (decoded_header[i] != '-') {
      owner.append(1, decoded_header[i]);
      i++;
    }

    i++;
    while (decoded_header[i] != '*') {
      filename.append(1, decoded_header[i]);
      i++;
    }

    if (msgtype == "0")
      this->message_type = Request;
    else
      this->message_type = Reply;

    this->operation = stoi(op);
    this->rpc_id = stoi(rpcid);
    this->message_size = stoi(msgsize);
    this->didfrag = stoi(fragd);
    this->countFrags = stoi(fragc);
    this->moreFrags = stoi(mfrag);
    this->image_owner = owner;
    this->image_name = filename;
  }

  string marshal() {
    string encoded_msg = "";
    string headerinfo =
        to_string(message_type) + "-" + to_string(operation) + "-" +
        to_string(rpc_id) + "-" + to_string(message_size) + "-" +
        to_string(didfrag) + "-" + to_string(countFrags) + "-" +
        to_string(moreFrags) + "-" + image_owner + "-" + image_name + "*";

    int n = headerinfo.length();
    unsigned int len = n + 1;
    char header_char[len];
    strcpy(header_char, headerinfo.c_str());

    // encoding message header
    encoded_msg = base64_encode((const unsigned char *)header_char, len);

    encoded_msg += " ";

    // encoding message payload
    encoded_msg += base64_encode((const unsigned char *)message, message_size);

    return encoded_msg;
  }

  int getOperation() { return operation; }
  int getRPCId() { return rpc_id; }
  char *getMessage() { return message; }
  string getUnmarshalledMessage() { return unmarshalledmessage; }
  size_t getMessageSize() { return message_size; }
  MessageType getMessageType() { return message_type; }
  int getDidFrag() { return didfrag; }
  int getFragCount() { return countFrags; }
  int getMoreFrag() { return moreFrags; }
  string getImageOwner() { return image_owner; }
  string getImageName() { return image_name; }

  void setOperation(int _operation) { this->operation = _operation; }
  void setMessage(char *message, size_t message_size) {
    this->message = message;
    this->message_size = message_size;
  }
  void setMessageType(MessageType message_type) {
    this->message_type = message_type;
  }
  void setDidFrag(bool p_didfrag) { this->didfrag = p_didfrag; }
  void setFragCount(int p_countFrags) { this->countFrags = p_countFrags; }
  void setMoreFrag(int p_moreFrags) { this->moreFrags = p_moreFrags; }
  void setImageOwner(string p_image_owner) {
    this->image_owner = p_image_owner;
  }
  void setImageName(string p_image_name) { this->image_name = p_image_name; }
  ~Message() {}
};
#endif
