#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // 클라이언트 소켓 생성
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("클라이언트 소켓 생성 실패");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // 서버 IP 주소
    server_addr.sin_port = htons(5555);  // 서버 포트 번호

    // 서버에 연결
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("서버 연결 실패");
        exit(EXIT_FAILURE);
    }

    printf("채팅 서버에 연결되었습니다.\n");

    // 메시지 전송 루프
    while (1) {
        printf("메시지 입력: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        // 입력받은 메시지를 서버에 전송
        if (send(client_socket, buffer, strlen(buffer), 0) == -1) {
            perror("메시지 전송 실패");
            exit(EXIT_FAILURE);
        }
    }

    // 클라이언트 소켓 닫기
    close(client_socket);

    return 0;
}

