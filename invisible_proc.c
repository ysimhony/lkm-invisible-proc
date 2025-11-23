#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define PORT 3000
#define SERVER_IP "10.0.2.2"
#define MESSAGE "Invisible process is alive"

int main() {
  int sockfd;
  struct sockaddr_in servaddr = {0};
  char message[100] = {0};
  time_t now;
  int pid = getpid();

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    return EXIT_FAILURE;
  }

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(PORT);

  if (inet_pton(AF_INET, SERVER_IP, &servaddr.sin_addr) <= 0) {
    perror("Invalid server IP address");
    close(sockfd);
    return EXIT_FAILURE;
  }

  int count = 0;
  while (1) {
    count++;
    now = time(NULL);
    memset(message, 0, sizeof(message));
    sprintf(message, "[%d][%d] " MESSAGE ": %d\n", now, pid, count);

    if (sendto(sockfd, (const char *)message, strlen(message), 0,
               (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
      perror("sendto failed");
      close(sockfd);
      return EXIT_FAILURE;
    }

    sleep(5);
  }

  close(sockfd);
  return EXIT_SUCCESS;
}
