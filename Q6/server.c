#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

void handle_client(int client_socket, int *client_sockets) {
    char buffer[BUFFER_SIZE];
    int bytes_received;

    while (1) {
        bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            printf("클라이언트 연결 종료\n");
            break;
        }

        buffer[bytes_received] = '\0';
        printf("클라이언트로부터 메시지 수신: %s", buffer);

        // 수신한 메시지를 다른 클라이언트에게 브로드캐스트
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] != 0 && client_sockets[i] != client_socket) {
                send(client_sockets[i], buffer, strlen(buffer), 0);
            }
        }
    }

    // 클라이언트 소켓 닫기
    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // 클라이언트 소켓 배열
    int client_sockets[MAX_CLIENTS] = {0};

    // 서버 소켓 생성
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("서버 소켓 생성 실패");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(5555);

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

    printf("채팅 서버가 시작되었습니다.\n");

    while (1) {
        // 클라이언트 연결 대기
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
        if (client_socket == -1) {
            perror("클라이언트 연결 수락 실패");
            continue;
        }

        printf("클라이언트가 연결되었습니다.\n");

        // 클라이언트 소켓 배열에 추가
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] == 0) {
                client_sockets[i] = client_socket;

                // 자식 프로세스 생성
                if (fork() == 0) {
                    close(server_socket);
                    handle_client(client_socket, client_sockets);
                    exit(EXIT_SUCCESS);
                }

                break;
            }
        }
    }

    // 부모 프로세스에서는 서버 소켓 닫기
    close(server_socket);

    return 0;
}

