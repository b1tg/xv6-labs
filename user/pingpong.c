#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{

	int fds[2];
	pipe(fds);
	int pid = fork();
	if (pid==0){
		char c;
		 if(read(fds[0], &c, 1)!=1) {
			printf("child pipe read failed\n");
			exit(0);
		 } else {
			 printf("child pipe read %c\n", c);
		 }
		 int pid = getpid();
		 printf("%d: received ping\n", pid);
		 if (write(fds[1], "y", 1) != 1) {
			printf("child pipe write failed\n");
		 }
		exit(0);
	} else if (pid <0) {
		printf("fork failed\n");
	} else {
		char c;
		 if (write(fds[1], "x", 1) != 1) {
			printf("parent pipe write failed\n");
		 }
		 if(read(fds[0], &c, 1)!=1) {
			printf("parent pipe read failed\n");
			exit(0);
		 } else {
			 printf("parent pipe read %c\n", c);
		 }
		 int pid = getpid();
		 printf("%d: received pong\n", pid);
		exit(0);

	}
	 char *a = "pingpong...\n";
	 write(1, a, strlen(a));

	 exit(0);
}
