
#include "DoS.h"
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

int main() {
	//system("hostnamectl set-hostname DoS");

	cout << "ImageSharer's Directory of Service has opened!\n";
	DircServ DoS;
	while (1) {
		DoS.getRequest();
	}

	return 0;
}
