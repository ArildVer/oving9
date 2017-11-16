#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/dispatch.h>
#include <sys/mman.h>


int main(int argc, char *argv[]) {
	printf("Welcome to the QNX Momentics IDE\n");
	int file_desc = shm_open("/sharedpid", O_RDWR|O_CREAT,S_IRWXU);
	ftruncate(file_desc, sizeof(int));
	void * mmap_test = mmap(0, sizeof(int),PROT_READ|PROT_WRITE, MAP_SHARED, file_desc, 0);
	int * nmb = mmap_test;

	*nmb = 2;

	printf("nmb = %i", *nmb);

	return EXIT_SUCCESS;
}
