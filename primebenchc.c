/*
All primebenchc code is Copyright 2013 by Nyiro Zoltan-Csaba.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program as the file LICENSE.txt; if not, please see
http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>
#include <getopt.h>

#define _PRIMEBENCHC_VERSION "v0.4"


#define handle_error_en(en, msg) \
do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

unsigned int sum, numbersPerProcess;
pthread_mutex_t sum_mutex;

static void usage(char *prog_name, char *msg)
{
   if (msg != NULL)
       fputs(msg, stderr);

   fprintf(stderr, "Usage: %s [options]\n", prog_name);
   fprintf(stderr, "Options are:\n");
   #define fpe(msg) fprintf(stderr, "\t%s", msg); /* Shorter */
   fpe("--im <numeric>      Set intervcal maximum\n");
   fpe("--tmin <numeric>    Set the minimum number of threads\n");
   fpe("--tmax <numeric>    Set the maximum number of threads\n");
   fpe("--showlicense       List license message\n");
   fpe("--hidelicense       Don't list license message\n");
   fprintf(stderr, "Defaults: %s --im 100000 --tmin 1 --tmax 8 --showlicense\n", prog_name);
   exit(EXIT_FAILURE);
}

int isPrime(unsigned int num) {
    if (num<3) {
        return 0;
    }
    unsigned int i;
    for(i=2;i<num;i++)
    {
        if(num%i==0)
            return 0;
    }
    return 1;
}

unsigned int primeCountInterval(unsigned int from, unsigned int to) {
    unsigned int primecount = 0;
    unsigned int i=0;
    for(i=from; i<=to; i++) {
        if (isPrime(i)>0) {
            primecount++;
        }
    }
    return primecount;
}

void *primeCountPrint(void *arg) {
    unsigned int i = (unsigned int)arg;
    unsigned int pc = primeCountInterval(numbersPerProcess*i,numbersPerProcess*(i+1));
    pthread_mutex_lock(&sum_mutex);
    sum += pc;
    pthread_mutex_unlock(&sum_mutex);
    pthread_exit(NULL);
}


void calculateThreaded(unsigned int threadcount, unsigned int until) {
    sum = 0;
    numbersPerProcess = ceill((float)until/(float)threadcount);
    unsigned int i;

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
    printf("%fs - %u threads [%u,%u] = %u primes found\n", time_spent, threadcount, 0, numbersPerProcess*threadcount,sum+1);
    free(threads);
}


int main(int argc, char *argv[])
{
    unsigned int until = 100000;
    int threadstart = 1;
    int threadcount = 8;
    static int listlicense = 1;

    static struct option long_options[] = {
                   {"showlicense", no_argument, &listlicense, 1},
                   {"hidelicense", no_argument, &listlicense, 0},
                   {"im", required_argument, 0, 'i'},
                   {"tmin", required_argument, 0, 'c'},
                   {"tmax", required_argument, 0, 'f'}
    };
    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "i:c:f:",long_options, &option_index))!=-1) {
        switch (opt) {
            case 0: break;
            case 'i':
                until = atol(optarg);
                if (until<2) {
                    usage(argv[0], "\nInvalid --im value!\n\n");
                }
                break;
            case 'c':
                threadstart = atol(optarg);
                if (threadstart<1) {
                    usage(argv[0], "\nInvalid --tmin value!\n\n");
                }
                break;
            case 'f':
                threadcount = atol(optarg);
                if (threadcount<1) {
                    usage(argv[0], "\nInvalid --tmax value!\n\n");
                }
                break;
            default: usage(argv[0], NULL);
        }
    }

    if (threadcount<threadstart) {
        usage(argv[0], "\nInvalid --tmax value!\n--tmax should be greater or equal then --tmin\n\n");
    }

    printf("PrimeBenchc %s\n", _PRIMEBENCHC_VERSION);
    if (listlicense) {
        printf("All primebenchc code is Copyright 2013 by Nyiro Zoltan-Csaba.\n\nThis program is free software; you can redistribute it and/or modify\nit under the terms of the GNU General Public License as published by\nthe Free Software Foundation; either version 2 of the License, or (at\nyour option) any later version.\n\nThis program is distributed in the hope that it will be useful, but\nWITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY\nor FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License\nfor more details.\n\n");
        printf("You should have received a copy of the GNU General Public License\nalong with this program as the file LICENSE.txt; if not, please see\nhttp://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.\n\n\n");
    }
    printf("The calculations are made up to %d thread(s).\n", threadcount);
    int i;
    for (i=threadstart; i<=threadcount; i++) {
        sum = 0;
        calculateThreaded(i,until);
    }
    return 0;
}
