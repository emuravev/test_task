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

#define TCP_SERVER 1
#define UDP_SERVER 2

class Server
{
private:
	int type, port;

	int sockfd;
	char buff[1024];

	struct sockaddr_in servaddr;
	
	//UDP
	void udp_server_handler()
	{
		struct sockaddr_in cliaddr;
		bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	
		for (;;)
		{
			int len = sizeof(struct sockaddr), n;
			n = recvfrom(sockfd, buff, 1024, 0, (struct sockaddr *) &cliaddr, (socklen_t *) &len);
			buff[n] = 0;

			std::string out(buff);
			out = extract_ints(out);

			memset(buff, 0, 1024);
			strncpy(buff, out.c_str(), 1024);

			sendto(sockfd, buff, strnlen(buff, 1024), 0, (const struct sockaddr *) &cliaddr, len);
		}
	}

	//TCP
	void tcp_server_handler()
	{
		int new_socket;
		
		int addrlen = sizeof(servaddr);
		std::vector <std::thread> threads;

		bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
		listen(sockfd, 10);

		for (;;)
		{
			new_socket = accept(sockfd, (struct sockaddr *) &servaddr, (socklen_t *) &addrlen);
			std::thread tcp_request(tcp_request_handler, new_socket);
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

	static void tcp_request_handler(int sockfd)
	{
		char buff[1024] = {0};

		read(sockfd, buff, 1024);

		std::string out(buff);
		out = extract_ints(out);

		memset(buff, 0, 1024);
		strncpy(buff, out.c_str(), 1024);

		send(sockfd, buff, strnlen(buff, 1024), 0);

		close(sockfd);
	}


	static std::string extract_ints(std::string str)
	{
		std::vector<int> v;

		std::stringstream ss;
		ss << str;

		#ifdef DEBUG
		std::cout << str << std::endl << std::endl;
		#endif

		std::string temp;
		int found;
		for (; !ss.eof(); ss >> temp) 
		{

			if (std::stringstream(temp) >> found)
			{
				v.push_back(found);
			}

		}

		#ifdef DEBUG
		std::cout << "unsorted: " << std::endl;
		for (size_t i = 0; i < v.size(); i++)
		{
			std::cout << v[i] << " ";
		}
		std::cout << std::endl;
		#endif

		sort(v.begin(), v.end());

		#ifdef DEBUG
		std::cout << "sorted: " << std::endl;
		for (size_t i = 0; i < v.size(); i++)
		{
			std::cout << v[i] << " ";
		}
		std::cout << std::endl;
		#endif
	
		int sum = 0;
		std::stringstream resstream;
		for (size_t i = 0; i < v.size(); i++)
		{
			resstream << v[i] << " ";
			sum += v[i];
		}
		resstream << std::endl << sum << std::endl;

		return resstream.str();
	}

public:
	void start()
	{
		if (type == TCP_SERVER) 
		{
			sockfd = socket(AF_INET, SOCK_STREAM, 0);
			
			tcp_server_handler();
		}
		else if (type == UDP_SERVER)
		{
		
			sockfd = socket(AF_INET, SOCK_DGRAM, 0);
			bzero(&(servaddr.sin_zero), 8);

			udp_server_handler();
		}
	}

	Server(int type, int port)
	{
		this->type = type;
		this->port = port;
		
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = INADDR_ANY;
		servaddr.sin_port = htons(port);
	}
};

