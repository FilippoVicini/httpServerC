#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

bool starts_With(const char *pre, const char *str) {
  size_t lenpre = strlen(pre), lenstr = strlen(str);
  return lenstr < lenpre ? false : memcmp(pre, str, lenpre) == 0;
}

char *extract_between(const char *str, const char *p1, const char *p2) {
  const char *i1 = strstr(str, p1);
  if (i1 != NULL) {
    const size_t pl1 = strlen(p1);
    const char *i2 = strstr(i1 + pl1, p2);
    if (p2 != NULL) {
      /* Found both markers, extract text. */
      const size_t mlen = i2 - (i1 + pl1);
      char *ret = malloc(mlen + 1);
      if (ret != NULL) {
        memcpy(ret, i1 + pl1, mlen);
        ret[mlen] = '\0';
        return ret;
      }
    }
  }
}
int main() {
  // Disable output buffering
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  // You can use print statements as follows for debugging, they'll be visible
  printf("Logs from your program will appear here!\n");

  //
  int server_fd, client_addr_len;
  struct sockaddr_in client_addr;

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    printf("Socket creation failed: %s...\n", strerror(errno));
    return 1;
  }

  // Since the tester restarts your program quite often, setting SO_REUSEADDR
  // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) <
      0) {
    printf("SO_REUSEADDR failed: %s \n", strerror(errno));
    return 1;
  }

  struct sockaddr_in serv_addr = {
      .sin_family = AF_INET,
      .sin_port = htons(4221),
      .sin_addr = {htonl(INADDR_ANY)},
  };

  if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0) {
    printf("Bind failed: %s \n", strerror(errno));
    return 1;
  }

  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    printf("Listen failed: %s \n", strerror(errno));
    return 1;
  }

  printf("Waiting for a client to connect...\n");
  client_addr_len = sizeof(client_addr);

  char *response = "HTTP/1.1 200 OK\r\n\r\n";
  int fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
  printf("Client connected\n");
  send(fd, response, strlen(response), 0);

  char buffer[4096];
  int n = read(fd, buffer, sizeof(buffer));

  printf("messgae %s\n", buffer);

  if (starts_With("GET /echo/", buffer)) {
    // Handle `/echo` request
    // "GET /echo/abababab HTTP/1.1"

    char *text = extract_between(buffer, "GET /echo/", " HTTP/1.1");
    char *reply;
    printf("output %s\n", text);
    printf("size %lu\n", strlen(text));

    char *msg;
    asprintf(&msg,
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/plain\r\n\r\n"
             "Echo: %s",
             text);

    // Send the response
    send(fd, msg, strlen(msg), 0);
    printf("output %s", text);

  } else if (starts_With("GET / HTTP/1.1", buffer)) {
    // Handle `/` request
    char *reply = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nWelcome "
                  "to the root path!";
    send(fd, reply, strlen(reply), 0);
    printf("Handled / request\n");

  } else {
    char *reply = "HTTP/1.1 404 NOT FOUND\r\n\r\n";
    int bytes_send = send(fd, reply, strlen(reply), 0);
  }

  close(server_fd);

  return 0;
}
