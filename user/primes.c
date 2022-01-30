#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"



int prime_func(int fds[2], int is_root) {
	int pid = fork();
	if (pid<0) {
		printf("fork error\n");
		exit(-1);	
	} else if (pid >0) {
		close(fds[0]); // unused read end
		//if (is_root == 0) {
		//	wait(0);
		//	exit(0);
		//}
	} else if (pid == 0) {
		close(fds[1]);
//		printf("new child: %d\n", getpid());
		int recv = 0;
		int prime = 0;
		int fds1[2];
		int read_n = read(fds[0], &recv, 4);
		if (read_n == 0) {
			//printf("pipe closed1\n");
			close(fds[0]);
			return -1;
		} else if(read_n != 4) {
			printf("read error1\n");
			close(fds[0]);
			return -1;
		}
		prime = recv;
		printf("\rprime %d\n", prime);
		int p_created = 0;
		while(1) {
			int read_n = read(fds[0], &recv, 4);
			if (read_n == 0) {
				//printf("pipe closed2\n");
				break;
			} else if(read_n != 4) {
				printf("read error2\n");
				break;
			}
			if (recv % prime != 0) {
				// create process and pass
				if (p_created == 0) {
					p_created = 1;
					pipe(fds1);
					prime_func(fds1, 0);
				}
				int write_n = write(fds1[1], &recv, 4);
				if (write_n == 0) {
				//	printf("pipe closed3\n");
					break;
				} else if (write_n != 4) {
					// TODO: why many fails (-1)
					// printf("write error3 %d\n", write_n);
					break;
				}
			}
		}
		//close(fds[0]);
		//close(fds[1]);
		//close(fds1[1]);
		//close(fds1[0]);

		close(fds[0]); // r
		exit(0);
	} 
	return 0;
}

int
main(int argc, char *argv[])
{

	// 1,2,3,4,5,6,7,8,
	//    2 3   5   7
	int fds[2];
	pipe(fds);
	//for (int i=2; i<=35; i++) {
	prime_func(fds, 1);
	for (int i=2; i<=35; i++) {
		if(write(fds[1], &i, 4)!=4) {
		//	printf("wrtie error\n");
		//	exit(-1);
		 break;
		}
	}
	close(fds[1]);
	wait(0);
	close(fds[0]);
	//printf("end main\n");
//	while(1) {}
	//exit(0);
	return 0;
}
