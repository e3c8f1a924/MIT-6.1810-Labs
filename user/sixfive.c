#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

const char seps[] = "-\r\t\n./,";
char buf[1];

void sixfive(int fd) {
    int u = 0, valid = 1, n, emp = 1;

    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        if (strchr(seps, buf[0])) {
            if (!emp && valid && (u % 6 == 0 || u % 5 == 0)) fprintf(1, "%d\n", u);
            emp = valid = 1, u = 0;
        } else if (buf[0] >= '0' && buf[0] <= '9') {
            if (valid) emp = 0, u = u * 10 + buf[0] - '0';
        } else valid = 0;
    }
    
    if (n < 0) fprintf(2, "sixfive: read error\n"), exit(1);
    if (!emp && valid && (u % 6 == 0 || u % 5 == 0)) fprintf(1, "%d\n", u);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "Usage: sixfive files...\n");
        exit(1);
    }

    for (int i = 1; i < argc; i++) {
        int fd = open(argv[i], O_RDONLY);
        if (fd < 0) fprintf(2, "sixfive: cannot open %s\n", argv[i]), exit(1);
        sixfive(fd);
    }

    exit(0);
}
