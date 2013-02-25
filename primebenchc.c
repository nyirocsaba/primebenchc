#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>

long sum, numersPerProcess;
pthread_mutex_t sum_mutex;


int isPrime(long num) {
    if (num<3) {
        return 0;
    }
    long i;
    for(i=2;i<num;i++)
    {
        if(num%i==0)
            return 0;
    }
    return 1;
}

long primeCountInterval(long from, long to) {
    long primecount = 0;
    long i=0;
    for(i=from; i<=to; i++) {
        if (isPrime(i)>0) {
            primecount++;
        }
    }
    return primecount;
}

void *primeCountPrint(void *arg) {
    long i = (long)arg;
    long pc = primeCountInterval(numersPerProcess*i,numersPerProcess*(i+1));
    pthread_mutex_lock(&sum_mutex);
    sum += pc;
    pthread_mutex_unlock(&sum_mutex);
    pthread_exit(NULL);
}


void calculateThreaded(long threadcount, long until) {
    sum = 0;
    numersPerProcess = ceill((float)until/(float)threadcount);
    long i;

    pthread_t* threads;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int rc;
    threads = (pthread_t*)malloc(sizeof(pthread_t)*(threadcount));

    struct timeval  tv1, tv2;
    gettimeofday(&tv1, NULL);

    for (i=0; i<threadcount; i++) {
        rc = pthread_create(&threads[i], &attr, primeCountPrint, (void*)i);
        if (rc!=0) {
            printf("Couldn't create thread!\n");
            exit(-1);
        }
    }
    void *status;
    pthread_attr_destroy(&attr);
    for(i=0; i<threadcount; i++) {
      pthread_join(threads[i], &status);
    }
    gettimeofday(&tv2, NULL);
    double time_spent = (double) (tv2.tv_usec - tv1.tv_usec)/1000000 +(double) (tv2.tv_sec - tv1.tv_sec);
    printf("%fs - %ld threads [%ld,%ld] = %ld primes found\n", time_spent, threadcount, 0L, numersPerProcess*threadcount,sum+1);
    free(threads);
}


int main(int argc, char *argv[])
{
    if (argc<3) {
        printf("Wrong arguments!\nUseage: primebenchc [interval top value] [max number of threads]");
        return 0;
    }
    long until = atol(argv[1]);
    int threadcount = atoi(argv[2]);

    if (until<2) {
        printf("Wrong interval top value!");
        return 0;
    }
    printf("The calculations are made up to %d thread(s).\n", threadcount);
    int i;
    for (i=1; i<=threadcount; i++) {
        calculateThreaded(i,until);
    }
    return 0;
}
