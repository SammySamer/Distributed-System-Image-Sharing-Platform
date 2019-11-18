#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

using namespace std;


struct data {
	vector<string> imgs;
	bool online = false;
	string currentIP = "";
	string port = "";
};


map<string, string> verify_map;
map<string, data> users_map;

//loads in data from the user file into the user map the first time the server opens
void storeUsers (fstream& users) {

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

//loads in data from the auth file into the auth map the first time the server opens
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
void signUp(string username, string password, fstream& verify, fstream& users) {
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
	}
	else
		cout << "Username already exists, please choose another...\n";
}

//checks if the username and password match.
//Updates user map with user's ip, port, and login status
bool login(string username, string password, string ip, string port, fstream& verify) {
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
	users_map[username].online = true;
	users_map[username].currentIP = ip;
	users_map[username].port = port;
	return true;
}

void logout(string username) {
	users_map[username].online = false;
	users_map[username].currentIP = "";
	users_map[username].port = "";
}

void viewer() {
	for (auto const& temp : users_map) {
		cout << "Username: " << temp.first << "\nImages:";
		for (int i = 0; i < temp.second.imgs.size(); i++) {
			cout << temp.second.imgs[i] << " ";
		}
		cout << endl;
	}
	cout << "----------------" << endl;
}

//uploads images into existing user profiles
void upload(string username, string img_name, fstream& users) {

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

int main(int argc, const char* argv[]) {

	fstream verify, users;
	verify.open("verify.txt", fstream::out | fstream::in | fstream::app);
	if (verify.fail())
		cout << "Unable to open a\n";
	else
		cout << "Opened a\n" << endl;

	users.open("users.txt", fstream::out | fstream::in | fstream::app);
	if (users.fail())
		cout << "Failed to open u\n";
	else
		cout << "Opened u\n" << endl;

	//signUp("Samer", "samer123", verify, users);
	//signUp("Mariam", "mariam123", verify, users);
	//signUp("Maram", "maram123", verify, users);
	//signUp("Ruby", "ruby123", verify, users);

	storeVerify(verify);
	storeUsers(users);
	cout << "Samer login status: " << login("Samer", "samer123", "127.0.0.1", "8080", verify) << endl;
	upload("Samer", "img.txt", users);

	logout("Samer");
	cout << users_map["Samer"].online << " " << users_map["Samer"].port << endl;

	//show_online();
	// logout("Yasser");
	// cout<<users_map["Yasser"].online<<" "<<users_map["Yasser"].port<<endl;
	// view();
	return 0;
}
