#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];
int in = 0, out = 0;

sem_t mutex, empty, full;

void *producer(void *arg) {
    int item;

    for (int i = 0; i < 10; ++i) {
        item = rand() % 100; // 생산할 아이템 생성

        sem_wait(&empty); // 빈 자리가 있는지 확인
        sem_wait(&mutex); // 버퍼에 접근하는 동안 다른 쓰레드들이 접근하지 못하도록

        // 생산자가 버퍼에 아이템을 추가
        buffer[in] = item;
        printf("Produced: %d at position %d\n", item, in);
        in = (in + 1) % BUFFER_SIZE;

        sem_post(&mutex); // 버퍼 접근 종료
        sem_post(&full); // 소비자에게 알림
        sleep(1);
    }

    pthread_exit(NULL);
}

void *consumer(void *arg) {
    int item;

    for (int i = 0; i < 10; ++i) {
        sem_wait(&full); // 버퍼에 데이터가 있는지 확인
        sem_wait(&mutex); // 버퍼에 접근하는 동안 다른 쓰레드들이 접근하지 못하도록

        // 소비자가 버퍼에서 아이템을 가져옴
        item = buffer[out];
        printf("Consumed: %d from position %d\n", item, out);
        out = (out + 1) % BUFFER_SIZE;

        sem_post(&mutex); // 버퍼 접근 종료
        sem_post(&empty); // 생산자에게 알림
        sleep(2);
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t producer_threads[2];
    pthread_t consumer_threads[2];

    // Initialize semaphores
    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);

    // Create producer threads
    for (int i = 0; i < 2; ++i) {
        pthread_create(&producer_threads[i], NULL, producer, NULL);
    }

    // Create consumer threads
    for (int i = 0; i < 2; ++i) {
        pthread_create(&consumer_threads[i], NULL, consumer, NULL);
    }

    // Join threads
    for (int i = 0; i < 2; ++i) {
        pthread_join(producer_threads[i], NULL);
        pthread_join(consumer_threads[i], NULL);
    }

    // Destroy semaphores
    sem_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
}

