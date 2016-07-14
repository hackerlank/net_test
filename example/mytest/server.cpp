
#include <stdio.h>
#include <unistd.h>
#include <event.h>

void accept_cb(int fd, short event, void* arg);
void socket_read_cb(int fd, short event, void* arg);

int tcp_server_init(int port, int listen_num);


int main(int argc, char** argv)
{
	int listener = tcp_server_init(9999, 10);
	if(listener == -1)
	{
		printf("tcp_server_init error");
		return -1;
	}
	
	event_base* base = event_base_new();
	
	event* ev_listen = event_new(base, listener, EV_READ | EV_PERSIST,
								accept_cb, base);
								
	event_add(ev_listen, NULL);
	
	event_base_dispatch(base);
	
	return 0;
}

void accept_cb(int fd, short event, void* arg)
{
	evutil_socket_t sockfd;
	
	sockaddr_in client;
	socklen_t len;
		
	sockfd = ::accept(fd, (sockaddr*)&client, &len);
	evutil_make_socket_nonblocking(sockfd);
	
	printf("accpet a client %d \n", sockfd);
	
	event_base* base = (event_base*)arg;
	
	event *ev = event_new(NULL, -1, 0, NULL, NULL);
	event_assign(ev, base, sockfd, EV_READ | EV_PERSIST, 
				socket_read_cb, (void*)ev);
	
	event_add(ev, NULL);
	
	
}

void socket_read_cb(int fd, short event, void* arg)
{
	
}

int tcp_server_init(int port, int listen_num)
{
	
}