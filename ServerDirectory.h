
#ifndef SERVERDIRECTORY_H
#define SERVERDIRECTORY_H
#pragma once

#define MAIN_BUFFER 2500000
#define LITTLE_BUFFER 15000

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


struct data {
	vector<string> imgs;
	bool online = false;
	string currentIP = "";
	string port = "";
};

class DircServ {
private:
	UDPSocketServer* sv;
	fstream verify, users;
	map<string, string> verify_map;
	map<string, data> users_map;
	bool con;
	int port;
	char sender_ip[INET_ADDRSTRLEN];
	uint16_t sender_port;

public:

	int r;
	struct sockaddr_in recievedAddr;
	socklen_t addresslength = sizeof(recievedAddr);

	DircServ() {
		cout << "Enter the port: ";
		cin >> port;

		con = true;
		sv = new UDPSocketServer(port);
		verify.open("verify.txt", fstream::out | fstream::in | fstream::app);
		if (verify.fail())
			cout << "verify file open failed!";

		users.open("users.txt", fstream::out | fstream::in | fstream::app);
		if (users.fail())
			cout << "Users file open failed!";

		storeVerify(verify);
		storeUsers(users);
	}


	void getRequest() {

		unsigned char* buffer = new unsigned char[MAIN_BUFFER];
		unsigned char* little_buffer = new unsigned char[LITTLE_BUFFER];

		printf("Getting requests: \n");
		memset(buffer, 0, sizeof(buffer));

		//receive marshalled message
		r = recvfrom(sv->s, buffer, MAIN_BUFFER, 0,
			(struct sockaddr*) & recievedAddr, &addresslength);

		printf("Received Message = %s.\n", buffer);

		inet_ntop(AF_INET, &(recievedAddr.sin_addr), sender_ip, INET_ADDRSTRLEN);

		sender_port = htons((&recievedAddr)->sin_port);

		printf("Sender IP:%s & port: %d\n", sender_ip, sender_port);

		Message requestMsg(reinterpret_cast<char*>(buffer));
		int OP_ID = requestMsg.getOperation();
		string msg = requestMsg.getUnmarshalledMessage();
		int RPC_ID = requestMsg.getRPCId();

		cout << "Operation ID: " << OP_ID << endl;
		cout << "RPC ID: " << RPC_ID << endl;
		cout << "Received Msg: " << msg << endl;
		switch (OP_ID) {

		//Sign Up
		case 1001:
		{
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
			cout << "User: " << username << endl;
			cout << "Pass: " << password << endl;
			bool didsign = signUp(username, password);
			cout << "Signed up?" << didsign << endl;
			// Sign Up reply
			memset(little_buffer, 0, sizeof(little_buffer));
			if (didsign)
				little_buffer[0] = '1';
			else
				little_buffer[0] = '0';
			// sprintf((char *)(little_buffer), "%d", didsign);
			little_buffer[1] = 0;
			if (sendto(sv->s, little_buffer, strlen((const char*)little_buffer), 0,
				(struct sockaddr*) & recievedAddr, addresslength) < 0) {
				perror("Sign up reply sendto failed");
			}

		}
		break; // end of sign_up

		case 1002: // login
		{
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
			cout << "User: " << username << endl;
			cout << "Pass: " << password << endl;
			int didlogin = login(username, password);

			cout << "Logged in? " << didlogin << endl;
			// Sign Up reply
			memset(little_buffer, 0, sizeof(little_buffer));
			if (didlogin == 1)
				little_buffer[0] = '1';
			else if (didlogin == 0)
				little_buffer[0] = '0';
			else if (didlogin == 5)
				little_buffer[0] = '5';
			else
				little_buffer[0] = '4';
			little_buffer[1] = 0;
			if (sendto(sv->s, little_buffer, strlen((const char*)little_buffer), 0,
				(struct sockaddr*) & recievedAddr, addresslength) < 0) {
				perror("Sign up reply sendto failed");
			}
		}

		break; // end of login

		case 1003: // logout
		{
			int place = 0;
			string username = "";
			while (msg[place] != 0) {
				username.append(1, msg[place]);
				place++;
			}

			cout << "User: " << username << endl;

			bool wasin = logout(username);

			cout << "Logged out? " << wasin << endl;

			// Sign Up reply
			memset(little_buffer, 0, sizeof(little_buffer));
			if (wasin == true)
				little_buffer[0] = '1';
			else
				little_buffer[0] = '0';
			little_buffer[1] = 0;
			if (sendto(sv->s, little_buffer, strlen((const char*)little_buffer), 0,
				(struct sockaddr*) & recievedAddr, addresslength) < 0) {
				perror("Sign up reply sendto failed");
			}

		}
		break; // end of logout

		case 1100: // view
		{

			string res = viewer();

			cout << "View All. Text: " << res << endl;

			// Sign Up reply
			memset(little_buffer, 0, sizeof(little_buffer));

			sprintf((char*)(little_buffer), "%s", res.c_str());

			if (sendto(sv->s, little_buffer, strlen((const char*)little_buffer), 0,
				(struct sockaddr*) & recievedAddr, addresslength) < 0) {
				perror("Sign up reply sendto failed");
			}

		} break; // end of view

		case 2001: // upload
		{
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
			cout << "Imagename: " << imagename << endl;
			int uploaded = upload(username, imagename);

			cout << "Uploaded? " << uploaded << endl;
			// Sign Up reply
			memset(little_buffer, 0, sizeof(little_buffer));
			if (uploaded == 1)
				little_buffer[0] = '1';
			else if (uploaded == 0)
				little_buffer[0] = '0';
			else if (uploaded == 9)
				little_buffer[0] = '9';
			else
				little_buffer[0] = '8'; // if something is wrong
			little_buffer[1] = 0;
			if (sendto(sv->s, little_buffer, strlen((const char*)little_buffer), 0,
				(struct sockaddr*) & recievedAddr, addresslength) < 0) {
				perror("Sign up reply sendto failed");
			}

		} break; // end of upload

		default:
			break;
		}
	}


	void sendReply(unsigned char* buffer) {
		if (sendto(sv->s, buffer, r, 0, (struct sockaddr*) & recievedAddr,
			addresslength) < 0) {
			perror("sendto failed");
		}
	}


//loads in data from the user file into the user map the first time the server opens
void storeUsers(fstream& users) {

	string line, username, imageName;
	int nameLen, imageLen;

	users.seekp(0);
	while (!users.eof()) {
		getline(users, line);

		//if there is a user, start reading
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

		//gets the passwords associated with each user
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
	bool takenUsername = true;
	string line, takenUsernames;
	int nameLen;

	verify.seekp(0);
	while (!verify.eof()) {
		getline(verify, line);
		if (line != "") {
			nameLen = line.find(" ");
			takenUsernames = line.substr(0, nameLen);
			if (takenUsernames == username)
				takenUsername = false;
		}
	}

	verify.clear();

	//username not taken, so we can complete the sign up
	if (takenUsername) {
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

	//if username taken
	else {
		cout << "Username already exists, please choose another...\n";
		return false;
	}
}


//checks if the username and password match.
//Updates user map with user's ip, port, and login status
bool login(string username, string password) {
	bool flag = false;
	verify.seekp(0);
	while (!verify.eof()) {
		string line;
		getline(verify, line);
		if (line != "") {
			int name_len;
			name_len = line.find(" ");
			string name = line.substr(0, name_len);
			line = line.erase(0, name_len + 1);
			int pass_len = line.find(" ");
			string pass = line.substr(0, pass_len);
			line = line.erase(0, pass_len + 1);
			if (name == username && pass == password) {
				flag = true;
				break;
			}
		}
	}

	verify.clear();

	if (!flag)
		return false;

	char portFromBuffer[LITTLE_BUFFER];
	sprintf((char*)(portFromBuffer), "%u", sender_port);
	users_map[username].currentIP = string(sender_ip);
	users_map[username].port = string(portFromBuffer);

	users_map[username].online = true;
	//users_map[username].currentIP = ip;
	users_map[username].port = port;
	return true;
}

bool logout(string username) {
	bool wasOnline = users_map[username].online;
	users_map[username].online = false;
	users_map[username].currentIP = "";
	users_map[username].port = "";

	return wasOnline;
}

string viewer() {

	string temp = "";
	for (auto const& x : users_map) {
		temp += x.first + "*";

		if (x.second.online) {
			temp += "Login status: 1\tUser's IP: ";
			temp += x.second.currentIP + "\tUser's Port: ";
			temp += x.second.port + "\tUser's Images: ";
		}

		else
			temp += "OFFLINE.\t User's Images: ";


		for (int i = 0; i < x.second.imgs.size(); i++) {
			temp += x.second.imgs[i] + "#";
		}
		temp += "----------------";
	}
	return temp;
}

//uploads images into existing user profiles
int upload(string username, string img_name) {

	bool exists = false;
	string output = "";
	int count = 0;

	for (auto const& x : users_map) {
		count++;						//keeps track of placement of username
		if (x.first == username) {
			exists = true;
			break;
		}
	}

	//if user exists
	if (exists) {
		string line, orig, name;
		int nameLen;

		users_map[username].imgs.push_back(img_name);

		//to quickly find username
		for (int i = 0; i < count; i++) {
			getline(users, line);
		}

		while (!users.eof()) {
			getline(users, line);

			if (line == "")
				break;

			orig = line;
			nameLen = line.find(" ");
			name = line.substr(0, nameLen);
			line = line.erase(0, nameLen + 1);

			if (name == username) {
				output += username + " ";
				for (int i = 0; i < users_map[username].imgs.size(); i++)
					output += users_map[username].imgs[i] + " ";
				output += "\n";
			}

			else {
				output += orig + "\n";
			}
		}
	}

	else {
		cout << "Username not found\n";
	}

	users.close();
	users.open("users.txt", fstream::out | fstream::in);
	users << output;
}

~DircServ() { users.close(); }

};
#endif
