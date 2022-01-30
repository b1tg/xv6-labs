#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
ls(char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    printf("%s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);
}

void find(char* dir, char* fname, char* pattern) {
  int fd;
  char buf[512], *p;
  struct dirent de;
  struct stat st;

  //printf("In find: dir=%s, pattern=%s\n", dir, pattern);

  if((fd = open(dir, 0)) <0 ) {
	//fprintf(2, "find: cannot open %s\n", dir);
	printf("find: cannot open %s\n", dir);
	return;
  }
  if(fstat(fd, &st) <0) {
	  //fprintf(2, "find: cannot stat %s\n", dir);
	  printf("find: cannot stat %s\n", dir);
	  close(fd);
	  return;
  }
  switch(st.type){
	case T_FILE:
		if(strcmp(fname, pattern) == 0) {
			//printf("file match %s\n",dir);
			printf("%s\n", dir);
		} else {
//			printf("file miss  %s\n",fmtname(dir));
		}
		break;
	case T_DIR:
//		printf("dir %s\n", dir);
		strcpy(buf, dir);
		p = buf+strlen(buf);
		*p++ = '/';
		while(read(fd, &de, sizeof(de)) == sizeof(de)){
		//	if(de.inum == 0)
		//		continue;
			memmove(p, de.name, strlen(de.name));
			p[strlen(de.name)] = 0;
		//	printf("p %s\n", p);
			// TODO: Why this exist?
			if (strlen(de.name) == 0) {
				continue;
			}
			if (strcmp(de.name, ".") == 0) {
				continue;
			}
			if (strcmp(de.name, "..") == 0) {
				continue;
			}
			find(buf, p, pattern);
			//printf("found in dir(%s): %s(%d)\n", dir, de.name, strlen(de.name));
		}
		break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  //int i;

  if(argc < 3){
    printf("usage: find <dir> <pattern>\n");
    exit(0);
  }
  char* dir = argv[1];
  char* pattern = argv[2];
  find(dir,"", pattern);
  exit(0);
}
