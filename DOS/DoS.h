
#ifndef SERVERDIRECTORY_H
#define SERVERDIRECTORY_H
#pragma once

#define MAIN_BUFFER 3000000
#define LITTLE_BUFFER 20000

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

#include "Message.h"
#include "UDPSocketServer.h"
#include <arpa/inet.h>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

using namespace std;


struct userInfo {
	bool online = false;
	string userIP = "";
	string userPort = "";
	vector<string> imgs;
};

class DircServ {
private:
	UDPSocketServer* sv;
	fstream verify, users;
	map<string, string> verify_map;
	map<string, userInfo> users_map;
	bool connect;
	int srvrPort;
	char sender_ip[INET_ADDRSTRLEN];
	uint16_t sender_port;

public:

	int r;
	struct sockaddr_in recievedAddr;
	socklen_t addresslength = sizeof(recievedAddr);

	DircServ() {
		cout << "Enter your port: ";
		cin >> srvrPort;

		connect = true;
		sv = new UDPSocketServer(srvrPort);
		verify.open("verify.txt", fstream::out | fstream::in | fstream::app);
		if (verify.fail())
			cout << "Verify file open failed!";

		users.open("users.txt", fstream::out | fstream::in | fstream::app);
		if (users.fail())
			cout << "Users file open failed!";

		storeVerify(verify);
		storeUsers(users);
	}


	void getRequest() {

		unsigned char* buffer = new unsigned char[MAIN_BUFFER];
		unsigned char* sendBuffer = new unsigned char[LITTLE_BUFFER];

		printf("\nGetting requests: \n");
		memset(buffer, 0, sizeof(buffer));

		//receive marshalled message
		r = recvfrom(sv->s, buffer, MAIN_BUFFER, 0,
			(struct sockaddr*) & recievedAddr, &addresslength);

		inet_ntop(AF_INET, &(recievedAddr.sin_addr), sender_ip, INET_ADDRSTRLEN);
		sender_port = htons((&recievedAddr)->sin_port);
		printf("Sender IP: %s. Sender's port: %i\n", sender_ip, sender_port);

		Message requestMsg(reinterpret_cast<char*>(buffer));
		int OPCODE = requestMsg.getOperation();
		int RPC_ID = requestMsg.getRPCId();
		string msg = requestMsg.getUnmarshalledMessage();

		cout << "Operation Code: " << OPCODE;
		
		switch (OPCODE) {

		//Sign Up
		case 1001:
		{
			cout << ", Signing Up...\n";
			int place = 0;
			string username = "", password = "";
			while (msg[place] != '*') {
				username.append(1, msg[place]);
				place++;
			}

			place++;
			while (msg[place] != 0) {
				password.append(1, msg[place]);
				place++;
			}
			bool didsign = signUp(username, password);

			//Reply back
			memset(sendBuffer, 0, sizeof(sendBuffer));
			if (didsign)
				sendBuffer[0] = '1';
			else
				sendBuffer[0] = '0';

			sendBuffer[1] = 0;

			if (sendto(sv->s, sendBuffer, strlen((const char*)sendBuffer), 0,
				(struct sockaddr*) & recievedAddr, addresslength) < 0) {
				perror("Sign up reply sendto failed");
			}

		}
		break;


		//Login
		case 1002:
		{
			cout << ", Logging in...\n";
			int place = 0;
			string username = "", password = "";
			while (msg[place] != '*') {
				username.append(1, msg[place]);
				place++;
			}
			place++;

			while (msg[place] != 0) {
				password.append(1, msg[place]);
				place++;
			}
			int didLogin = login(username, password);

			//Login reply
			memset(sendBuffer, 0, sizeof(sendBuffer));
			if (didLogin == 1)
				sendBuffer[0] = '1';
			else if (didLogin == 0)
				sendBuffer[0] = '0';
			else if (didLogin == 5)
				sendBuffer[0] = '5';
			else
				sendBuffer[0] = '4';

			sendBuffer[1] = 0;
			if (sendto(sv->s, sendBuffer, strlen((const char*)sendBuffer), 0,
				(struct sockaddr*) & recievedAddr, addresslength) < 0) {
				perror("Logging In reply failed");
			}
		}
		break; 


		//Logout
		case 1003:
		{
			cout << ", Logging Out...\n";
			int place = 0;
			string username = "";
			while (msg[place] != 0) {
				username.append(1, msg[place]);
				place++;
			}

			cout << "User " << username << " is logging out...";
			bool didLogout = logout(username);

			//Logout reply
			memset(sendBuffer, 0, sizeof(sendBuffer));

			if (didLogout == true)
				sendBuffer[0] = '1';
			else
				sendBuffer[0] = '0';

			sendBuffer[1] = 0;

			if (sendto(sv->s, sendBuffer, strlen((const char*)sendBuffer), 0,
				(struct sockaddr*) & recievedAddr, addresslength) < 0) {
				perror("Logging out reply failed");
			}

		}
		break;


		//View Users
		case 1100: 
		{
			cout << ", Viewing Users...\n";
			string lister = view();

			//View Reply
			memset(sendBuffer, 0, sizeof(sendBuffer));
			sprintf((char*)(sendBuffer), "%s", lister.c_str());
			if (sendto(sv->s, sendBuffer, strlen((const char*)sendBuffer), 0,
				(struct sockaddr*) & recievedAddr, addresslength) < 0) {
				perror("View Users failed to send");
			}
		} 
		break;


		//Upload
		case 2001:
		{
			cout << ", Uploading Image...\n";
			int place = 0;
			string username = "", imagename = "";

			while (msg[place] != '*') {
				username.append(1, msg[place]);
				place++;
			}
			place++;
			while (msg[place] != 0) {
				imagename.append(1, msg[place]);
				place++;
			}

			cout << "User: " << username << endl;
			cout << "Image Name: " << imagename << endl;
			int uploaded = upload(username, imagename);

			//Upload reply
			memset(sendBuffer, 0, sizeof(sendBuffer));
			if (uploaded == 1)
				sendBuffer[0] = '1';
			else if (uploaded == 0)
				sendBuffer[0] = '0';
			else if (uploaded == 9)
				sendBuffer[0] = '9';
			else
				sendBuffer[0] = '8';
			sendBuffer[1] = 0;
			if (sendto(sv->s, sendBuffer, strlen((const char*)sendBuffer), 0,
				(struct sockaddr*) & recievedAddr, addresslength) < 0) {
				perror("Upload reply failed");
			}

		} 
		break;

		default:
			break;
		}
	}


bool serveRequests() { getRequest(); }


//just sends a reply with the buffer
void sendReply(unsigned char* buffer) {
	if (sendto(sv->s, buffer, r, 0, (struct sockaddr*) & recievedAddr,
		addresslength) < 0) {
		perror("Failed to 'sendto'");
	}
}


//loads in data from the user file into the user map the first time the server opens
void storeUsers(fstream& users) {

	string line, username, imageName;
	int nameLen, imageLen;

	users.seekp(0);
	while (!users.eof()) {
		getline(users, line);

		//if there is a user, get their username
		if (line != "") {
			nameLen = line.find(" ");
			username = line.substr(0, nameLen);
			line = line.erase(0, nameLen + 1);
			users_map[username];

			//getting all the images uploaded by the user
			while (line != "") {
				imageLen = line.find(" ");
				imageName = line.substr(0, imageLen);
				line = line.erase(0, imageLen + 1);
				users_map[username].imgs.push_back(imageName);
			}
		}

		else {

		}
	}

	users.clear();
}


//loads in data from the verify file into the verify map the first time the server opens
void storeVerify(fstream& verify) {

	string line, username, pass;
	int nameLen, passLen;

	verify.seekp(0);
	while (!verify.eof()) {

		getline(verify, line);

		//gets the usernames and their respective passwords
		if (line != "") {
			nameLen = line.find(" ");
			username = line.substr(0, nameLen);
			line = line.erase(0, nameLen + 1);

			passLen = line.find(" ");
			pass = line.substr(0, passLen);
			line = line.erase(0, passLen + 1);
			verify_map[username] = pass;
		}
	}
	verify.clear();
}


//attempts to sign user up, and put data into respective files
bool signUp(string username, string password) {
	bool validUsername = true;
	string line, takenUsernames;
	int nameLen;

	verify.seekp(0);
	while (!verify.eof()) {
		getline(verify, line);
		
		//if there's a user
		if (line != "") {
			nameLen = line.find(" ");
			takenUsernames = line.substr(0, nameLen);
			if (takenUsernames == username)
				validUsername = false;
		}
	}

	verify.clear();

	//username is valid, so we can complete the sign up
	if (validUsername) {
		verify_map[username] = password;
		users_map[username];
		int ind = verify.tellg();
		verify.seekp(ind);
		verify << username << " " << password << endl;
		int inds = users.tellg();
		users.seekp(inds);
		users << username << " " << endl;
		return true;
	}

	//if username taken aka invalid
	else {
		cout << "Username already exists, please choose another...\n";
		return false;
	}
}


//checks if the username and password match.
//Updates user map with user's ip, port, and login status
int login(string username, string password) {
	bool nameFlag = false, passFlag = false;
	int nameLen, passLen;
	string line, currName, pass;

	verify.seekp(0);
	while (!verify.eof()) {

		getline(verify, line);
		if (line != "") {
			nameLen = line.find(" ");
			currName = line.substr(0, nameLen);
			line = line.erase(0, nameLen + 1);

			passLen = line.find(" ");
			pass = line.substr(0, passLen);
			line = line.erase(0, passLen + 1);

			//if we've found the user
			if (currName == username) 
				nameFlag = true;
			
			//if the password is also true to this username
			if (currName == username && pass == password) { 
				passFlag = true;
				break;
			}
		}
	}

	verify.clear();

	//entered the username doesn't even exist
	if (!nameFlag)
		return 0;

	//password doesn't match
	if (!passFlag)
		return 5;	

	//user was already logged in
	if (users_map[username].online == true)	{
		return 4;
	}

	char portFromSender[LITTLE_BUFFER];
	sprintf((char*)(portFromSender), "%u", sender_port);
	users_map[username].userIP = string(sender_ip);
	users_map[username].userPort = string(portFromSender);
	users_map[username].online = true;
	return 1;
}


//logs user out by reseting all variables back
bool logout(string username) {
	bool wasOnline = users_map[username].online;
	users_map[username].online = false;
	users_map[username].userIP = "";
	users_map[username].userPort = "";

	return wasOnline;
}


string viewer() {

	string temp = "";
	for (auto const& x : users_map) {
		temp += x.first + "*";

		if (x.second.online) {
			temp += "Login status: ONLINE.\tUser's IP: ";
			temp += x.second.userIP + ".\tUser's port: ";
			temp += x.second.userPort + ".\tUser's Images: ";
		}

		else
			temp += "Login Status: OFFLINE.\tUser's Images: ";


		for (int i = 0; i < x.second.imgs.size(); i++) {
			temp += x.second.imgs[i] + "#";
		}
		temp += "----------------";
	}
	return temp;
}

string view() {
    string ret = "";
    for (auto const &x : users_map) {
      ret += x.first + "*";
      if (x.second.online) {
        ret += "1&";
        ret += x.second.userIP + "&";
        ret += x.second.userPort + "&";
      } else {
        ret += "0&&&";
      }
      for (int i = 0; i < x.second.imgs.size(); i++) {
        ret += x.second.imgs[i] + "#";
      }
      ret += "@";
    }
    return ret;
  }

/*
//uploads images into existing user profiles
int upload1(string username, string imgName) {

	bool exists = false;
	string updated = "";
	int count = 0;

	for (auto const& x : users_map) {
		//count++;
		if (x.first == username) {
			exists = true;
			break;
		}
	}

	//if user exists
	if (exists) {
		bool iFind = false;
		
		for (int f = 0; f < users_map[username].imgs.size() && !iFind; f++) {
        	iFind = (users_map[username].imgs[f] == imgName);
      	}
		
		//if the image wasn't found, so we should add it in
		if (!iFind) {
			users_map[username].imgs.push_back(imgName);

			//to quickly find username
			//for (int i = 0; i < count - 1; i++) {
			//	getline(users, line);
			//}

			while (!users.eof()) {
				string line;
				getline(users, line);

				if (line == "")
					break;

				string main = line;
				int nameLen = line.find(" ");
				string name = line.substr(0, nameLen);
				line = line.erase(0, nameLen + 1);

				if (name == username) {
					updated += username + " ";
					for (int i = 0; i < users_map[username].imgs.size(); i++)
						updated += users_map[username].imgs[i] + " ";
					updated += "\n";
				}

				else {
					updated += main + "\n";
				}
			}
		}

		//the image was found
		else {
			return 9;
		}
	}

	//username wasn't found
	else {
		return 0;
	}

	//if it all went successfully
	users.close();
    users.open("users.txt", fstream::out | fstream::in);
    users << updated;
    users.close();
    users.open("users.txt", fstream::out | fstream::in | fstream::app);
	return 1;
}*/


//uploads images into existing user profiles
int upload(string username, string img_name) { 
	
	bool exists = false; 	//check if user exists or not
	bool findImage = false;	//check if user already has the image
	string updated = "";

	for (auto const &x : users_map) {
		if (x.first == username) {
			exists = true;
			break;
		}
	}

	if (exists) {
		for (int e = 0; e < users_map[username].imgs.size() && !findImage; e++) {
			findImage = users_map[username].imgs[e] == img_name;
		}

		if (!findImage) {
			users_map[username].imgs.push_back(img_name);
			users.seekg(0);
			while (!users.eof()) {
				string line;
				getline(users, line);

				if (line == "")
					break;

				string first = line;
				int nameLen;
				nameLen = line.find(" ");
				string name = line.substr(0, nameLen);
				line = line.erase(0, nameLen + 1);

				if (name == username) {
				updated += username + " ";

				for (int i = 0; i < users_map[username].imgs.size(); i++)
					updated += users_map[username].imgs[i] + " ";

				updated += "\n";
				} 

				else {
				updated += first + "\n";
				}
			}
		} 
		
		//image was found
		else {
		users.close();
		users.open("users.txt", fstream::out | fstream::in);
		users << updated;
		users.close();
		users.open("users.txt", fstream::out | fstream::in | fstream::app);
		return 9;
		}
	} 

	//username not found
	else {
		users.close();
		users.open("users.txt", fstream::out | fstream::in);
		users << updated;
		users.close();
		users.open("users.txt", fstream::out | fstream::in | fstream::app);
		return 0;
	}

	//if all went successfully
	users.close();
	users.open("users.txt", fstream::out | fstream::in);
	users << updated;
	users.close();
	users.open("users.txt", fstream::out | fstream::in | fstream::app);
	return 1;
}


~DircServ() { users.close(); }

};
#endif
