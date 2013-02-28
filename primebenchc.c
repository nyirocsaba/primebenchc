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
alongArchFix with this program as the file LICENSE.txt; if not, please see
http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>
#include <getopt.h>
#include <sched.h>
#include <unistd.h>

#define _PRIMEBENCHC_VERSION "v0.2"


#define handle_error_en(en, msg) \
       do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#ifdef __i386__
 typedef long longArchFix;
#endif

#ifdef __x86_64__
 typedef int longArchFix;
#endif

longArchFix sum, numbersPerProcess;
pthread_mutex_t sum_mutex;

struct schedulerParams {
    int enabled, priority;
} sc;

static void usage(char *prog_name, char *msg)
{
   if (msg != NULL)
       fputs(msg, stderr);

   fprintf(stderr, "Usage: %s [options]\n", prog_name);
   fprintf(stderr, "Options are:\n");
   #define fpe(msg) fprintf(stderr, "\t%s", msg);          /* Shorter */
   fpe("--im <numeric>         Set intervcal maximum\n");
   fpe("--tmin <numeric>       Set the minimum number of threads\n");
   fpe("--tmax <numeric>       Set the maximum number of threads\n");
   fpe("--showlicense          List license message\n");
   fpe("--hidelicense          Don't list license message\n");
   fpe("--default-scheduler    Use the system default scheduler\n");
   fpe("                       Only 0 priority is accepted\n");
   fpe("--fifo-scheduler       Use the real time scheduler\n");
   fpe("                       Priority from 0 to 90\n");
   fpe("--rr-scheduler         Use the round robin scheduler\n");
   fpe("                       Priority from 0 to 90\n");
   fpe("--scheduler-priority   Set the threads priority\n");
   fpe("                       to -1 and highest possible\n");
   fpe("                       priority will be used\n");
   fprintf(stderr, "Defaults: %s --im 100000 --tmin 1 --tmax 8 --showlicense --default-scheduler\n", prog_name);
   exit(EXIT_FAILURE);
}

int isPrime(longArchFix num) {
    if (num<3) {
        return 0;
    }
    longArchFix i;
    for(i=2;i<num;i++)
    {
        if(num%i==0)
            return 0;
    }
    return 1;
}

longArchFix primeCountInterval(longArchFix from, longArchFix to) {
    longArchFix primecount = 0;
    longArchFix i=0;
    for(i=from; i<=to; i++) {
        if (isPrime(i)>0) {
            primecount++;
        }
    }
    return primecount;
}

void *primeCountPrint(void *arg) {
    longArchFix i = (longArchFix)arg;
    longArchFix pc = primeCountInterval(numbersPerProcess*i,numbersPerProcess*(i+1));
    pthread_mutex_lock(&sum_mutex);
    sum += pc;
    pthread_mutex_unlock(&sum_mutex);
    pthread_exit(NULL);
}


void calculateThreaded(longArchFix threadcount, longArchFix until) {
    numbersPerProcess = ceill((float)until/(float)threadcount);
    longArchFix i;

    pthread_t* threads;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int rc;
    threads = (pthread_t*)malloc(sizeof(pthread_t)*(threadcount));

    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);

    for (i=0; i<threadcount; i++) {
        rc = pthread_create(&threads[i], &attr, primeCountPrint, (void*)i);
        if (rc!=0) {
            printf("Couldn't create thread!\n");
            exit(-1);
        }
    }

    if (sc.enabled) {
        struct sched_param param;
        param.sched_priority = sc.priority;
        for (i=0; i<threadcount; i++) {
            pthread_setschedparam(threads[i], sc.enabled, &param);
        }
    }

    pthread_attr_destroy(&attr);

    void *status;
    for(i=0; i<threadcount; i++) {
      pthread_join(threads[i], &status);
    }

    gettimeofday(&tv2, NULL);
    double time_spent = (double) (tv2.tv_usec - tv1.tv_usec)/1000000 +(double) (tv2.tv_sec - tv1.tv_sec);

    printf("%fs - %ld threads [%ld,%ld] = %ld primes found\n", time_spent, threadcount, 0L, numbersPerProcess*threadcount,sum+1);
    free(threads);
}


int main(int argc, char *argv[])
{
    longArchFix until = 100000;
    int threadstart = 1;
    int threadcount = 8;
    static int listlicense = 1;
    sc.enabled = 0;
    sc.priority = -1;

    static struct option longArchFix_options[] = {
                   {"showlicense", no_argument, &listlicense, 1},
                   {"hidelicense", no_argument, &listlicense, 0},
                   {"default-scheduler", no_argument, &sc.enabled, 0},
                   {"fifo-scheduler",    no_argument, &sc.enabled, SCHED_FIFO},
                   {"rr-scheduler",      no_argument, &sc.enabled, SCHED_RR},
                   {"im",       required_argument, 0, 'i'},
                   {"tmin",     required_argument, 0, 'c'},
                   {"tmax",     required_argument, 0, 'f'},
                   {"priority", required_argument, 0, 'p'}
    };
    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "i:c:f:",longArchFix_options, &option_index))!=-1) {
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
            case 'p':
                sc.priority = atol(optarg);
                if (sc.priority<1) {
                    usage(argv[0], "\nInvalid --priority value!\n\n");
                }
                break;
            default:  usage(argv[0], NULL);
        }
    }

    if (threadcount<threadstart) {
        usage(argv[0], "\nInvalid --tmax value!\n--tmax should be greater or equal then --tmin\n\n");
    }

    if (sc.enabled) {
        if(geteuid() != 0) {
            usage(argv[0], "\nThe selected type of scheduler can be used only with root privilidges\n\n");
        }
        struct sched_param param;
        if (sc.priority==-1) {
            sc.priority = sched_get_priority_max(sc.enabled);
        }
        param.sched_priority = sc.priority;
        pthread_setschedparam(pthread_self(), sc.enabled, &param);
        if (sc.priority>0) {
            sc.priority--;
        }
    }

    printf("PrimeBenchc %s\n", _PRIMEBENCHC_VERSION);
    if (listlicense) {
        printf("All primebenchc code is Copyright 2013 by Nyiro Zoltan-Csaba.\n\nThis program is free software; you can redistribute it and/or modify\nit under the terms of the GNU General Public License as published by\nthe Free Software Foundation; either version 2 of the License, or (at\nyour option) any later version.\n\nThis program is distributed in the hope that it will be useful, but\nWITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY\nor FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License\nfor more details.\n\n");
        printf("You should have received a copy of the GNU General Public License\nalongArchFix with this program as the file LICENSE.txt; if not, please see\nhttp://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.\n\n\n");
    }
    printf("The calculations are made up to %d thread(s).\n", threadcount);
    int i;
    for (i=threadstart; i<=threadcount; i++) {
        sum = 0;
        calculateThreaded(i,until);
    }
    return 0;
}
