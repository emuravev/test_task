#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <thread>
#include <future>

#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>

using namespace std;

string extract_ints(string str)
{
	vector<int> v;

	stringstream ss;
	ss << str;

	#ifdef DEBUG
	cout << str << endl << endl;
	#endif

	string temp;
	int found;
	for (; !ss.eof(); ss >> temp) 
	{

		if (stringstream(temp) >> found)
		{
			v.push_back(found);
		}

	}

	#ifdef DEBUG
	cout << "unsorted: " << endl;
	for (size_t i = 0; i < v.size(); i++)
	{
		cout << v[i] << " ";
	}
	cout << endl;
	#endif

	sort(v.begin(), v.end());

	#ifdef DEBUG
	cout << "sorted: " << endl;
	for (size_t i = 0; i < v.size(); i++)
	{
		cout << v[i] << " ";
	}
	cout << endl;
	#endif
	
	int sum = 0;
	stringstream resstream;
	for (size_t i = 0; i < v.size(); i++)
	{
		resstream << v[i] << " ";
		sum += v[i];
	}
	resstream << endl << sum << endl;

	return resstream.str();
}

void tcp_request_handler(int sockfd)
{
	char buff[1024] = {0};

	read(sockfd, buff, 1024);

	string out = extract_ints(string(buff));

	memset(buff, 0, 1024);
	strncpy(buff, out.c_str(), 1024);

	send(sockfd, buff, strnlen(buff, 1024), 0);
	close(sockfd);
}

void tcp_server_handler()
{
	int server_fd, new_socket;
	struct sockaddr_in address;
	int port = 6666;
	int addrlen = sizeof(address);
	vector <thread> threads;
	
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	
	server_fd = socket(AF_INET, SOCK_STREAM, 0);

	bind(server_fd, (struct sockaddr *) &address, sizeof(address));
	listen(server_fd, 10);
	for (;;)
	{
		new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen);
		thread tcp_request(tcp_request_handler, new_socket);
		tcp_request.join();
		//threads.push_back(tcp_request);
	}
/*
	for (size_t i = 0; i < threads.size(); i++)
	{
		threads[i].join();
	}
*/
}

void udp_server_handler()
{
	int sockfd, port = 6666;
	char buff[1024];
	struct sockaddr_in servaddr, cliaddr;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(port);
	bzero(&(servaddr.sin_zero), 8);

	bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	
	for (;;)
	{
		int len = sizeof(struct sockaddr), n;
		n = recvfrom(sockfd, buff, 1024, 0, (struct sockaddr *) &cliaddr, (socklen_t *) &len);
		buff[n] = 0;

		string out = extract_ints(string(buff));

		memset(buff, 0, 1024);
		strncpy(buff, out.c_str(), 1024);

		sendto(sockfd, buff, strnlen(buff, 1024), 0, (const struct sockaddr *) &cliaddr, len);
	}
}


int main()
{
	thread tcp_server(tcp_server_handler);
	thread udp_server(udp_server_handler);

	tcp_server.join();
	udp_server.join();

	return 0;
}

