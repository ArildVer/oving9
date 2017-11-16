#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/dispatch.h>
#include <sys/mman.h>

struct pid_data{
	pthread_mutex_t pid_mutex;
	pid_t pid;
};

struct test{
	int t1;
	int t2;
};

int set_priority(int priority){
	int policy;
	struct sched_param param;
	// check priority in range
	if (priority < 1 || priority > 63) return -1;
	// set priority
	pthread_getschedparam(pthread_self(), &policy, &param);
	param.sched_priority = priority;
	return pthread_setschedparam(pthread_self(), policy, &param);
}

int get_priority(){
	int policy;
	struct sched_param param;
	// get priority
	pthread_getschedparam(pthread_self(), &policy, &param);
	return param.sched_curpriority;
}

void *send_msg(){
	int tid = gettid();
	if(tid == 2 || tid == 3) {
		set_priority(1);
	} else {
		set_priority(63);
	}


	int file_desc = shm_open("/sharedpid", O_RDWR,S_IRWXU);
	void * mmap_test = mmap(0, sizeof(struct pid_data),PROT_READ|PROT_WRITE, MAP_SHARED, file_desc, 0);
	struct pid_data * nmb = mmap_test;
	printf("pid = %i\n", nmb->pid);

	int ch_id = ConnectAttach(0, nmb->pid, 1, 0 ,0);
	int msg = 2;
	int buffer = 0;
	int status = MsgSend(ch_id, &msg, sizeof(int), &buffer, sizeof(int));
	ConnectDetach(ch_id);
	printf("Status = %i, msg = %i\n", status, buffer);
}

int main(int argc, char *argv[]) {
	printf("Client\n");

	pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3;
	pthread_t thread4;
	pthread_create(&thread1, NULL, send_msg, NULL);
	pthread_create(&thread2, NULL, send_msg, NULL);
	pthread_create(&thread3, NULL, send_msg, NULL);
	pthread_create(&thread4, NULL, send_msg, NULL);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);
	pthread_join(thread4, NULL);


	return EXIT_SUCCESS;
}
