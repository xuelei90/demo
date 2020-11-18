#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <ctype.h>

#define MAXLEN 1024
#define SERV_PORT 8001
#define MAX_OPEN_FD 10240

int main(int argc, char *argv[])
{
  int listenfd,connfd,efd,ret;
  char buf[MAXLEN];
  struct sockaddr_in cliaddr,servaddr;
  socklen_t clilen = sizeof(cliaddr);
  struct epoll_event tmpevent, events[MAX_OPEN_FD];

  listenfd = socket(AF_INET,SOCK_STREAM,0);

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERV_PORT);
  bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

  listen(listenfd, 20);

  //create
  efd = epoll_create(MAX_OPEN_FD);
  tmpevent.events = EPOLLIN;
  tmpevent.data.fd = listenfd;
  //add listen socket
  ret = epoll_ctl(efd, EPOLL_CTL_ADD, listenfd, &tmpevent);
  printf("server start\n");
  //loop wait
  for (;;)
  {
    size_t nready = epoll_wait(efd, events, MAX_OPEN_FD, -1); //-1 forever
    //loop proc events
    for (int i=0; i<nready; ++i)
    {
      if (events[i].data.fd == listenfd)
      {
        //new connect
        connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen);
        printf("new connect[%d]\n", connfd);
        tmpevent.events = EPOLLIN;
        tmpevent.data.fd = connfd;
        ret = epoll_ctl(efd, EPOLL_CTL_ADD, connfd, &tmpevent);
      }
      else
      {
        //read data
        connfd = events[i].data.fd;
        int bytes = read(connfd, buf, MAXLEN);
        if (0 == bytes) //client close
        {
          ret = epoll_ctl(efd,EPOLL_CTL_DEL,connfd,NULL);
          close(connfd);
          printf("connect[%d] closed\n", connfd);
        }
        else
        {
          for (int j=0; j<bytes; ++j)
          {
            buf[j] = toupper(buf[j]);
          }
          write(connfd,buf,bytes);
        }
      }
    }
  }
  close(efd);
  return 0;
}
