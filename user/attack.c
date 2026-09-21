#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

#define SCAN_SIZE 1024
#define PAGE_SIZE 4096

const char pattern[] = "This may help.";

int
main(int argc, char *argv[])
{
  char *scan = sbrk(SCAN_SIZE * PAGE_SIZE);
  if (scan == ((char *)-1)) exit(1);
  int lp = strlen(pattern);
  scan[SCAN_SIZE * PAGE_SIZE - 1] = 0;
  for (int i = 0; i + 16 < SCAN_SIZE * PAGE_SIZE; i++) {
    int flag = 1;
    for (int j = 0; j < lp; j++) {
      if (scan[i + j] != pattern[j]) { flag = 0; break; }
    }
    if (flag) {
      printf("%s\n", scan + i + 16);
      exit(0);
    }
  }

  exit(0);
}
