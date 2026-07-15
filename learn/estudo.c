#include "codexion.h"
#include "time.h"


int emails = 7;
pthread_mutex_t mutex;

void *roll_dice(void *arg)
{
    int *result = malloc(sizeof(int));
    char *name = *(char **)arg;
    *result = (rand() % 6) + 1;
    printf("Player %s rolled the dice...\n", name);
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
    char **names;

    size = 3;
    names = malloc(size * sizeof(char *));

    names[0] = "Diogo";
    names[1] = "Sofia";
    names[2] = "Cris";



    thread = malloc(sizeof(pthread_t) * size);

    pthread_mutex_init(&mutex, NULL);

    for (size_t i = 0; i < size; i++)
    {
        if (pthread_create(&thread[i], NULL, &roll_dice, &names[i]) != 0)
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