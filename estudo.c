#include "codexion.h"
#include "time.h"


int emails = 7;
pthread_mutex_t mutex;

void *roll_dice()
{
    int *result = malloc(sizeof(int));
    *result = (rand() % 6) + 1;
    return (void *) result;
}

void *race_condition()
{
    for (int i = 0; i < 1000000; i++)
        emails++;
    return NULL;
}

void *race_condition_prevented()
{
    for (int i = 0; i < 1000000; i++)
    {
        pthread_mutex_lock(&mutex);
        emails++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}


/*
int main()
{
    pthread_t *thread;
    size_t size;

    size = 20;

    thread = malloc(sizeof(pthread_t) * size);

    pthread_mutex_init(&mutex, NULL);

    for (size_t i = 0; i < size; i++)
    {
        if (pthread_create(&thread[i], NULL, &race_condition_prevented, NULL) != 0)
        {
            printf("Something went wrong...\n");
        }
        printf("Thread %ld started...\n", i+1);
    }
    for (size_t i = 0; i < size; i++)
    {
        sleep(1);
        pthread_join(thread[i], NULL);
        printf("Thread %ld ended...\n", i+1);
    }

    pthread_mutex_destroy(&mutex);
    printf("Number of E-Mails: %i\n", emails);
    free(thread);
}
*/

int main()
{
    pthread_t *thread;
    size_t size;
    int *result;
    size = 20;

    thread = malloc(sizeof(pthread_t) * size);

    pthread_mutex_init(&mutex, NULL);

    for (size_t i = 0; i < size; i++)
    {
        if (pthread_create(&thread[i], NULL, &roll_dice, NULL) != 0)
        {
            printf("Something went wrong...\n");
        }
        printf("Thread %ld started...\n", i+1);
    }
    for (size_t i = 0; i < size; i++)
    {
        sleep(1);
        pthread_join(thread[i], (void **)&result);
        printf("Thread %ld rolled the dice and got %d ...\n", i+1, *result);
    }

    pthread_mutex_destroy(&mutex);
    free(thread);
}