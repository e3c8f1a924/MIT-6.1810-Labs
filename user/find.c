#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define BUFSIZ 512
#define MAXARGS 10

char buf[BUFSIZ];

void find(char *buf, int bufsiz, const void *pattern, int (*match)(const void *, const char *), void (*callback)(const char *)) {
    int fd = open(buf, O_RDONLY);
    if (fd < 0) {
        fprintf(2, "find: cannot open %s\n", buf);
        exit(1);
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", buf);
        exit(1);
    }

    if (st.type == T_DIR) {
        if (strlen(buf) + 1 + DIRSIZ + 1 > bufsiz) {
            fprintf(2, "find: path too long\n");
            exit(1);
        }
        char *p = buf + strlen(buf);
        *(p++) = '/';
        struct dirent de;
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (de.inum == 0) continue;
            if (strcmp(de.name, ".") == 0) continue;
            if (strcmp(de.name, "..") == 0) continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            find(buf, bufsiz, pattern, match, callback);
        }
    } else if (match(pattern, buf)) callback(buf);
    close(fd);
}

void print(const char *buf) {
    printf("%s\n", buf);
}

int match(const void *pattern, const char *buf) {
    const char *p = buf + strlen(buf);
    while (*p != '/' && p >= buf) p--;
    return !strcmp(pattern, p + 1);
}

char *cmd[MAXARGS]; int nargs;
void exec_cmd(const char *buf) {
    cmd[nargs] = (char *)buf;
    int pid = fork();
    if (pid < 0) {
        fprintf(2, "find: failed to fork\n");
        exit(1);
    }
    if (pid == 0) {
        exec(cmd[0], cmd);
    }
    wait(0);
}

int main(int argc, char *argv[]) {
    if ((argc != 3 && argc < 5) || (argc >= 5 && strcmp(argv[3], "-exec"))) {
        fprintf(2, "Usage: find dir pattern [-exec ...]\n");
        exit(1);
    }
    if (strlen(argv[1]) + 1 > BUFSIZ) {
        fprintf(2, "find: path too long\n");
        exit(1);
    }
    memmove(buf, argv[1], strlen(argv[1]));
    if (argc >= 5) {
        nargs = argc - 4;
        for (int i = 4; i < argc; i++) cmd[i - 4] = argv[i];
        find(buf, BUFSIZ, argv[2], match, exec_cmd);
    } else {
        find(buf, BUFSIZ, argv[2], match, print);
    }
    exit(0);
}
