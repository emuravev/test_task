#include "client.h"

int main(int argc, char* argv[])
{
	char proto[4], ip[16];
	int port;
	int is_tcp = 0;

	if (argc != 2)
	{
error_args:
		printf("usage: %s [tcp|udp]://ip:port\n", argv[0]);
		exit(-1);
	}
	else
	{
		#ifdef DEBUG
		printf("%s\n", argv[1]);
		#endif
		//no sec
		sscanf(argv[1], "%[^:]://%[^:]:%d", proto, ip, &port);
		#ifdef DEBUG
		printf("proto:\t%s\nip:\t%s\nport:\t%d\n", proto, ip, port);
		#endif

		if (strncmp(proto, "tcp", 3) && strncmp(proto, "udp", 3))
		{
			goto error_args;
		}

		if (!strncmp(proto, "tcp", 3))
		{
			is_tcp = 1;
		}
	}

	int sock = 0;
	struct sockaddr_in serv_addr;

	if (is_tcp)
	{
		sock = socket(AF_INET, SOCK_STREAM, 0);
	}
	else
	{
		sock = socket(AF_INET, SOCK_DGRAM, 0);
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	inet_pton(AF_INET, ip, &serv_addr.sin_addr);

	if (is_tcp)
	{
		if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		{
			fprintf(stderr, "\nConnection failed\n");
			exit(-1);
		}
		else
		{
			printf("Connection established with %s://%s:%d\nCtrl-C to exit\n\n", proto, ip, port);
			char buff[1024] = {0};
			char * out_buff = NULL;
			size_t len = 0;

			printf("Enter message: ");
				
			getline(&out_buff, &len, stdin);

			send(sock, out_buff, strnlen(out_buff, 1024), 0);
			read(sock, buff, 1024);
			
			printf("Response: %s\n", buff);
			
			free(out_buff);

			close(sock);
		}
	}
	else
	{
		bzero(&(serv_addr.sin_zero), 8);
		
		char buff[1024] = {0};
		char * out_buff = NULL;
		size_t len = 0;
		int sock_len = sizeof(struct sockaddr), n;

		printf("Enter message: ");
				
		getline(&out_buff, &len, stdin);

		printf("%s\n", out_buff);

		sendto(sock, out_buff, strnlen(out_buff, 1024), 0, (struct sockaddr *) &serv_addr,  (socklen_t) sock_len);
		n = recvfrom(sock, buff, 1024, 0, (struct sockaddr *) &serv_addr, (socklen_t *) &sock_len);
		buff[n] = 0;
			
		printf("Response: %s\n", buff);
			
		free(out_buff);
	}


	return 0;
}

