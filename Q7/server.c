#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

void handle_get(int client_socket, const char* path) {
    char response[BUFFER_SIZE];
    FILE* file = fopen(path, "r");

    if (file == NULL) {
        sprintf(response, "HTTP/1.1 404 Not Found\r\n\r\n");
        send(client_socket, response, strlen(response), 0);
    } else {
        sprintf(response, "HTTP/1.1 200 OK\r\n\r\n");
        send(client_socket, response, strlen(response), 0);

        while (fgets(response, BUFFER_SIZE, file) != NULL) {
            send(client_socket, response, strlen(response), 0);
        }

        fclose(file);
    }
}

void handle_post(int client_socket, const char* path, const char* content) {
    char response[BUFFER_SIZE];
    sprintf(response, "HTTP/1.1 200 OK\r\n\r\nPOST 요청 처리 예제");
    send(client_socket, response, strlen(response), 0);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char request[BUFFER_SIZE];

    
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("서버 소켓 생성 실패");
        exit(EXIT_FAILURE);
    }

    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(8080);  // 사용할 포트 번호

    // 바인딩
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("바인딩 실패");
        exit(EXIT_FAILURE);
    }

    // 리스닝
    if (listen(server_socket, 5) == -1) {
        perror("리스닝 실패");
        exit(EXIT_FAILURE);
    }

    printf("간단한 웹 서버가 시작되었습니다. http://127.0.0.1:8080\n");

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
        if (client_socket == -1) {
            perror("클라이언트 연결 수락 실패");
            continue;
        }
        recv(client_socket, request, BUFFER_SIZE, 0);
        printf("HTTP 요청:\n%s", request);

        char method[10], path[255];
        sscanf(request, "%s %s", method, path);

       
        if (strcmp(method, "GET") == 0) {
          handle_get(client_socket, path + 1);  
       }
        else if (strcmp(method, "POST") == 0) {
            char* content_length_start = strstr(request, "Content-Length:");
            int content_length;
            sscanf(content_length_start, "Content-Length: %d", &content_length);

            char post_data[content_length];
            recv(client_socket, post_data, content_length, 0);

            handle_post(client_socket, path + 1, post_data);
        }
	close(client_socket);
    }

       close(server_socket);

    return 0;
}

