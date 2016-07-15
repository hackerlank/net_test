
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include <unistd.h>

#include <event.h>
#Include <event2/util.h>

int tcp_connect_server(const char* server_ip, int port);

void cmd_msg_cb(int fd, short events, void* arg);
void socket_read_cb(int fd, short events, void* arg);


int main(int argc, char** argv)
{
	char * server_ip= "192.168.16.44";
	const int port = 12345;
	
	int sockfd = tcp_connect_server(server_ip, port);
	if(sockfd == -1)
	{
		return -1;
	}
	
	printf("connect to server successful \n");
	
	event_base* base = event_base_new();
	event * ev_sockfd = event_new(base, sockfd, EV_READ | EV_PERSIST,
								socket_read_cb, NULL);
								
	event_add(ev_sockfd, NULL);
	
	event* ev_cmd = event_new(base, STDIN_FILENO, EV_READ | EV_PERSIST,
								cmd_msg_cb, (void*)&sockfd);
								
	event_add(ev_cmd, NULL);
	
	event_base_dispatch(base);
	
	return 0;
}


int tcp_connect_server(const char* server_ip, int port)
{
	int sockfd = 0;
	int status = 0;

	sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	status = inet_aton(server_ip, &server_addr.sin_addr);
	
	if(status == 0)
	{
		return -1;
	}
	
	int sockfd = ::socket(PF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{
		return sockfd;
	}
	
	status = ::connect(sockfd, (sockaddr)&server_addr, sizeof(server_addr));
	if(status == -1)
	{
		::close(sockfd);
		return -1;
	}
	
	evutil_make_socket_nonblocking(sockfd);
	
	return sockfd;
}

void cmd_msg_cb(int fd, short events, void* arg)
{
	char msg[4096] = {0};
	
	int ret = read(fd, msg, sizeof(msg));
	if(ret <= 0)
	{
		printf("read fail \n");
		exit(1);
	}
	
	int sockfd = *((int*)arg);
	
	write(sockfd, msg, ret);
}

void socket_read_cb(int fd, short events, void* arg)
{
	char msg[4096] = {0};
	
	int ret = read(fd, msg, sizeof(msg)-1);
	if(ret <= 0)
	{
		printf("read fail \n");
		exit(1);
	}

	printf("recv %s from server \n", msg);
}