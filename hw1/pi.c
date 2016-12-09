#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <float.h>
#include <math.h>
#include <assert.h>

long toss;
long thread_num;
long* thread_sum;
int cache_bit = 8;

struct arg_struct {
	long rank;
	unsigned seed1;
};

struct arg_struct *args;

void* DoToss(void* argss);


int main(int argc, char* argv[]){
	//read number of core first
	int nprocs = -1;
	int nprocs_max = -1;
#ifdef _SC_NPROCESSORS_ONLN
	nprocs = sysconf(_SC_NPROCESSORS_ONLN);
	if (nprocs < 1)
	{
//		fprintf(stderr, "Could not determine number of CPUs online:\n%s\n", 
//				strerror (errno));
		thread_num = 4;
	}
	nprocs_max = sysconf(_SC_NPROCESSORS_CONF);
	if (nprocs_max < 1)
	{
//		fprintf(stderr, "Could not determine number of CPUs configured:\n%s\n", 
//				strerror (errno));
		thread_num = 4;
	}
//	printf ("%d of %d processors online\n",nprocs, nprocs_max);
	thread_num=nprocs;
#else
//	fprintf(stderr, "Could not determine number of CPUs");
	thread_num = 4;
#endif




//	clock_t start, end;
//	double cpu_time_used;
	          
//	start = clock();

	long thread;	
	long ans = 0;
	pthread_t* thread_handles;

	toss = strtol(argv[1], NULL, 12);

	thread_sum = malloc (thread_num*cache_bit*sizeof(long));
	args = malloc (sizeof(struct arg_stsruct*)*thread_num*cache_bit);
	thread_handles = malloc (thread_num*sizeof(pthread_t));
	unsigned int seed1;
	for (thread = 0; thread < thread_num; thread++){
		thread_sum[thread*cache_bit]=0;
		args[thread*cache_bit].rank = thread;
		seed1=time(NULL);
		args[thread*cache_bit].seed1 = (unsigned int)rand_r(&seed1);
		pthread_create(&thread_handles[thread], NULL, DoToss,(void*)  &args[thread*cache_bit]);
	}

	for (thread = 0; thread < thread_num; thread++){
		pthread_join(thread_handles[thread], NULL);
		ans += thread_sum[thread*cache_bit];
	}
	
	double final = 4 * ans/(double) toss;
	printf("%f\n", final);
	
//	end = clock();
//	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	
//	printf("%f \n", cpu_time_used);

	return 0;
}


void* DoToss(void* argss){
	struct arg_struct* arg = argss;
	long my_rank = arg->rank;
	unsigned int seed1 = arg->seed1;
	long check;
	long end;
	if ( (toss%thread_num) >= (my_rank+1)){
		check = 1;
	}
	else check = 0;
//	if ( my_rank == 0){
//		end = toss - thread_num + 1;
//	}else{
//		end = 1;
//	}

	long localtoss;
	for ( localtoss=0; localtoss <= (toss/thread_num + check) ; localtoss ++){
//	for ( localtoss=0; localtoss <= end ; localtoss ++){
		double x = (double)rand_r(&seed1)/(RAND_MAX/2) - 1 ;
		double y = (double)rand_r(&seed1)/(RAND_MAX/2) - 1 ;
		double ds = x * x + y * y ;
//		printf("%ld re: %ld x: %lf y: %lf d: %lf  ",my_rank, (check), x, y, ds);
		if (ds <= 1){
//			printf("++");
			thread_sum[my_rank*cache_bit] ++;
		}
//		else printf("--");

//		printf(" sum: %ld\n", thread_sum[my_rank*cache_bit]);
	}
	return NULL;
}
