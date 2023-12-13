#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_CLIENTS 10
#define MESSAGE_SIZE 1024

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

char messages[MAX_CLIENTS][MESSAGE_SIZE];
int message_available[MAX_CLIENTS] = {0};

void* handle_client(void* arg) {
    int client_id = *(int*)arg;
    free(arg);

    while (1) {
        pthread_mutex_lock(&mutex);
        while (!message_available[client_id]) {
            pthread_cond_wait(&cond, &mutex);
        }

        printf("클라이언트 %d로부터 메시지 수신: %s", client_id, messages[client_id]);
        message_available[client_id] = 0;

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main() {
    pthread_t client_threads[MAX_CLIENTS];
    int client_ids[MAX_CLIENTS];

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        client_ids[i] = i;
        pthread_create(&client_threads[i], NULL, handle_client, &client_ids[i]);
    }

    while (1) {
        char message[MESSAGE_SIZE];
        printf("메시지 입력: ");
        fgets(message, MESSAGE_SIZE, stdin);

        pthread_mutex_lock(&mutex);

        // 메시지를 모든 클라이언트에게 방송
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            strncpy(messages[i], message, MESSAGE_SIZE);
            message_available[i] = 1;
        }

        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);
    }

    // 쓰레드 종료 대기
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        pthread_join(client_threads[i], NULL);
    }

    return 0;
}

