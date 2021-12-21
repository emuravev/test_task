#include "server.h"

int main()
{
	Server tcp_server(TCP_SERVER, 6666);
	Server udp_server(UDP_SERVER, 6666);

	tcp_server.start();
	udp_server.start();

	return 0;
}

