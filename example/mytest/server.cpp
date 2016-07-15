
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <event.h>
#include <event2/util.h>

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

void accept_cb(int fd, short events, void* arg)
{
	evutil_socket_t sockfd;
	
	sockaddr_in client;
	socklen_t len;
		
	sockfd = ::accept(fd, (sockaddr*)&client, &len);
	evutil_make_socket_nonblocking(sockfd);
	
	printf("accpet a client %d \n", sockfd);
	
	event_base* base = (event_base*)arg;
	
	struct event *ev = event_new(NULL, -1, 0, NULL, NULL);
	event_assign(ev, base, sockfd, EV_READ | EV_PERSIST, 
				socket_read_cb, (void*)ev);
	
	event_add(ev, NULL);
	
	
}

void socket_read_cb(int fd, short events, void* arg)
{
	char msg[4096] = {0};
	
	event* ev = (event*)arg;
	int len = read(fd, msg, sizeof(msg) - 1);
	
	if(len <= 0) 
	{
		printf("some error happen when read \n");
		close(event_get_fd(ev));
		event_free(ev);
		return;
	}
	
	printf("recv the client msg: %s", msg);
	char reply_msg[4096] = "I have recvied the msg: ";
	strcat(reply_msg + strlen(reply_msg), msg);
	
	write(fd, reply_msg, strlen(reply_msg));
	
	
	
}

int tcp_server_init(int port, int listen_num)
{
	evutil_socket_t listener;
	
	listener = ::socket(AF_INET, SOCK_STREAM, 0);
	if(listener == -1) 
	{
		return -1;
	}
	
	evutil_make_listen_socket_reuseable(listener);
	
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0;
	sin.sin_port = htons(port);
	
	if(::bind(listener, (sockaddr*)&sin, sizeof(sin)) < 0 )
	{
		//errno_save = errno;
		evutil_closesocket(listener);
		//errno = errno_save;
		return -1;
	}
	
	if(::listen(listener, listen_num) < 0)
	{
		evutil_closesocket(listener);
		return -1;
	}
	
	evutil_make_socket_nonblocking(listener);
	
	return listener;
	
}








