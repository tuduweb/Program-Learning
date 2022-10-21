#include <pthread.h>
#include <queue>
#include <stdio.h>
#include <unistd.h>
 
// 注意pthread_*函数返回的异常值
 
pthread_mutex_t mutex;
pthread_cond_t condvar;
 
std::queue<int> msgQueue;
struct Produce_range {
    int start;
    int end;
};
 
void *producer(void *args)
{
    int start = static_cast<Produce_range *>(args)->start;
    int end = static_cast<Produce_range *>(args)->end;
    for (int x = start; x < end; x++) {
        usleep(200 * 1000);
        pthread_mutex_lock(&mutex);
        msgQueue.push(x);
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&condvar);
        printf("Produce create %d\n", x);
    }
    pthread_exit((void *)0);
    return NULL;
}
 
void *consumer(void *args)
{
    int demand = *static_cast<int *>(args);
    while (true) {
        pthread_mutex_lock(&mutex);
        while (msgQueue.size() <= 0) {
            pthread_cond_wait(&condvar, &mutex);
        }
        if (msgQueue.size() > 0) {
            printf("Consume message %d\n", msgQueue.front());
            msgQueue.pop();
            --demand;
        }
        pthread_mutex_unlock(&mutex);
        if (!demand) break;
    }
    pthread_exit((void *)0);
    return NULL;
}
 
 
int main()
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condvar, NULL);
 
    pthread_t producer1, producer2, producer3, consumer1, consumer2;
 
    Produce_range range1 = {0, 10};
    pthread_create(&producer1, &attr, producer, static_cast<void *>(&range1));
    Produce_range range2 = {range1.end, range1.end + 10};
    pthread_create(&producer2, &attr, producer, static_cast<void *>(&range2));
    Produce_range range3 = {range2.end, range2.end + 10};
    pthread_create(&producer3, &attr, producer, static_cast<void *>(&range3));
 
    int consume_demand1 = 20;
    int consume_demand2 = 10;
    pthread_create(&consumer1, &attr, consumer, 
            static_cast<void *>(&consume_demand1));
    pthread_create(&consumer2, &attr, consumer, 
            static_cast<void *>(&consume_demand2));
 
    pthread_join(producer1, NULL);
    pthread_join(producer2, NULL);
    pthread_join(producer3, NULL);
    pthread_join(consumer1, NULL);
    pthread_join(consumer2, NULL);
} 