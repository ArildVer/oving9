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

int main(int argc, char *argv[]) {
	printf("Server\n");
	set_priority(32);

	int file_desc = shm_open("/sharedpid", O_RDWR|O_CREAT,S_IRWXU);
	ftruncate(file_desc, sizeof(struct pid_data));
	void * mmap_test = mmap(0, sizeof(struct pid_data),PROT_READ|PROT_WRITE, MAP_SHARED, file_desc, 0);
	struct pid_data * nmb = mmap_test;

	pthread_mutexattr_t myattr;
	pthread_mutexattr_init(&myattr);
	pthread_mutexattr_setpshared(&myattr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(&nmb->pid_mutex, &myattr );

	nmb->pid = getpid();
	printf("PID = %i\n", nmb->pid);

	int ch_id = ChannelCreate(0 );
	printf("ch_id = %i\n", ch_id);

	int buffer = 0;
	struct _msg_info msg_info;

	while(1) {
		printf("My priority is %i\n", get_priority());
		int msg_id = MsgReceive(ch_id, &buffer,sizeof(int), &msg_info);
		printf("Got message: Process ID: %i, Thread ID: %i, Thread priority: %i\n", msg_info.pid, msg_info.tid, msg_info.priority);
		buffer += 1;
		MsgReply(msg_id, 0, &buffer, sizeof(int));
	}


	return EXIT_SUCCESS;
}
