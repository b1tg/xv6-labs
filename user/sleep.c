#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(2, "Usage: sleep <seconds>\n");
		exit(1);
	}

	 char *a = "sleepxxx\n";
	 write(1, a, strlen(a));
	 if(sleep(atoi(argv[0])) <0) {
	   fprintf(2, "sleep: %d==\n", atoi(argv[0]));
	 } else {
	   write(1, "ok\n", 3);
	 }
	 exit(0);
}
