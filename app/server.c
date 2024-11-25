#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <pthread.h>
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

void *handle_client(void *arg) {
  int fd = *(int *)arg;
  free(arg); // Free the dynamically allocated fd pointer

  char buffer[4096];
  int n = read(fd, buffer, sizeof(buffer) - 1);
  if (n <= 0) {
    close(fd);
    return NULL;
  }
  buffer[n] = '\0';
  printf("Received request:\n%s\n", buffer);

  if (starts_With("GET /echo/", buffer)) {
    // Handle `/echo` request
    char *text = extract_between(buffer, "GET /echo/", " HTTP/1.1");
    if (text) {
      char *msg;
      asprintf(&msg,
               "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nEcho: %s",
               text);
      send(fd, msg, strlen(msg), 0);
      free(msg);
      free(text);
    } else {
      char *reply = "HTTP/1.1 400 Bad Request\r\n\r\nInvalid Echo Request";
      send(fd, reply, strlen(reply), 0);
    }
  } else if (starts_With("GET / HTTP/1.1", buffer)) {
    char *reply = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nWelcome "
                  "to the root path!";
    send(fd, reply, strlen(reply), 0);
  } else if (starts_With("GET /user-agent HTTP/1.1", buffer)) {
    char *text = extract_between(buffer, "User-Agent: ", "\r\n");
    if (text) {
      char *msg;
      asprintf(&msg,
               "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
               "%lu\r\n\r\n%s",
               strlen(text), text);
      send(fd, msg, strlen(msg), 0);
      free(msg);
      free(text);
    } else if (starts_With("GET /files/", buffer)) {
      char *text = extract_between(buffer, "User-Agent: ", "\r\n");
      printf("filename%s\n", text);

    }

    else {
      char *reply = "HTTP/1.1 400 Bad Request\r\n\r\nMissing User-Agent";
      send(fd, reply, strlen(reply), 0);
    }
  } else {
    char *reply = "HTTP/1.1 404 Not Found\r\n\r\n";
    send(fd, reply, strlen(reply), 0);
  }

  close(fd);
  return NULL;
}
int main() {
  setbuf(stdout, NULL);

  int server_fd;
  struct sockaddr_in serv_addr;

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    perror("Socket creation failed");
    return 1;
  }

  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) <
      0) {
    perror("SO_REUSEADDR failed");
    close(server_fd);
    return 1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(4221);
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0) {
    perror("Bind failed");
    close(server_fd);
    return 1;
  }

  if (listen(server_fd, 5) != 0) {
    perror("Listen failed");
    close(server_fd);
    return 1;
  }

  printf("Server is listening on port 4221...\n");

  while (1) {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_fd =
        accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd < 0) {
      perror("Accept failed");
      continue;
    }

    int *pclient = malloc(sizeof(int));
    if (!pclient) {
      perror("Malloc failed");
      close(client_fd);
      continue;
    }
    *pclient = client_fd;

    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, handle_client, pclient) != 0) {
      perror("Thread creation failed");
      close(client_fd);
      free(pclient);
      continue;
    }
    pthread_detach(thread_id);
  }

  close(server_fd);
  return 0;
}
