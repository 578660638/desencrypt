//
// Created by tom on 4/20/20.
//

#ifndef MULTIREGISTERNATIVE_ANTIFRIDA_H
#define MULTIREGISTERNATIVE_ANTIFRIDA_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <jni.h>
#include <sys/types.h>
#include <cstring>
#include <cstdio>
#include <android/log.h>
#include <unistd.h>
#include <android/log.h>
#include <pthread.h>
#include <cstdlib>
#include <elf.h>
#include <link.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_LINE 512
#define MAX_LENGTH 256
static const char *FRIDA_THREAD_GUM_JS_LOOP = "gum-js-loop";
static const char *FRIDA_THREAD_GMAIN = "gmain";
static const char *FRIDA_NAMEDPIPE_LINJECTOR = "linjector";
static const char *PROC_MAPS = "/proc/self/maps";
static const char *PROC_STATUS = "/proc/self/task/%s/status";
static const char *PROC_FD = "/proc/self/fd";
static const char *PROC_TASK = "/proc/self/task";
#define LIBC "libc.so"
#define BUFFER_LEN 512

#define TAG "fridadetection"

int
wrap_memcmp(const unsigned char *s1, const unsigned char *s2, size_t n) {
    if (n != 0) {
        const unsigned char *p1 = s1;
        const unsigned char *p2 = s2;

        do {
            if (*p1++ != *p2++)
                return (*--p1 - *--p2);
        } while (--n != 0);
    }
    return (0);
}


int
find_mem_string(unsigned long start, unsigned long end, unsigned char *bytes, unsigned int len) {
    unsigned char *pmem = ( unsigned char * ) start;
    int matched = 0;
    while (( unsigned long ) pmem < (end - len)) {
        if (*pmem == bytes[0]) {
            matched = 1;
            unsigned char *p = pmem + 1;
            while (*p == bytes[matched] && ( unsigned long ) p < end) {
                matched++;
                p++;
            }
            if (matched >= len) {
                return 1;
            }
        }
        pmem++;

    }
    return 0;
}

int read_line(int fd, char *ptr, unsigned int maxlen) {
    int n;
    int rc;
    char c;

    for (n = 1; n < maxlen; n++) {
        if ((rc = read(fd, &c, 1)) == 1) {
            *ptr++ = c;
            if (c == '\n')
                break;
        } else if (rc == 0) {
            return 0;    /* EOF no data read */
        } else
            return (-1);    /* error */
    }
    *ptr = 0;
    return (n);
}

int elf_check_header(uintptr_t base_addr) {
    ElfW(Ehdr) *ehdr = ( ElfW(Ehdr) * )
            base_addr;
    if (0 != memcmp(ehdr->e_ident, ELFMAG, SELFMAG)) return 0;
#if defined(__LP64__)
    if(ELFCLASS64 != ehdr->e_ident[EI_CLASS]) return 0;
#else
    if (ELFCLASS32 != ehdr->e_ident[EI_CLASS]) return 0;
#endif
    if (ELFDATA2LSB != ehdr->e_ident[EI_DATA]) return 0;
    if (EV_CURRENT != ehdr->e_ident[EI_VERSION]) return 0;
    if (ET_EXEC != ehdr->e_type && ET_DYN != ehdr->e_type) return 0;
    if (EV_CURRENT != ehdr->e_version) return 0;
    return 1;
}

int wrap_endsWith(const char *str, const char *suffix) {
    if (!str || !suffix)
        return 0;
    size_t lenA = strlen(str);
    size_t lenB = strlen(suffix);
    if (lenB > lenA)
        return 0;
    return strncmp(str + lenA - lenB, suffix, lenB) == 0;
}


bool detect_frida_string() {
    int fd;
    char path[256];
    char perm[5];
    unsigned long offset;
    unsigned long base;
    unsigned long end;
    char buffer[BUFFER_LEN];
    int loop = 0;

    //"frida:rpc"
    const char *fridarpc = "frida:rpc";
    unsigned int length = strlen(fridarpc);
//打开进程映射表
    fd = openat(AT_FDCWD, "/proc/self/maps", O_RDONLY, 0);

    if (fd > 0) {
        //读取内存中的maps
        while ((read_line(fd, buffer, BUFFER_LEN)) > 0) {
            __android_log_print(6, "maps", "%s", buffer);
//字符串匹配frida
            if (strstr(buffer, "frida") != nullptr &&
                strstr(buffer, "/data/app/com.frida.fridadetection") ==
                nullptr) {

                __android_log_print(6, "hanbingle", "%s", "found frida.so in memory");
            }
            memset(path, 0, 256);
            memset(perm, 0, 5);
            if (sscanf(buffer, "%lx-%lx %4s %lx %*s %*s %s", &base, &end, perm, &offset, path) !=
                5) {
                continue;
            }
            if (perm[0] != 'r') continue;
            if (perm[3] != 'p') continue; //do not touch the shared memory
            if (0 != offset) continue;
            if (strlen(path) == 0) continue;
            if ('[' == path[0]) continue;
            if (end - base <= 1000000) continue;
            if (wrap_endsWith(path, ".oat")) continue;
            if (strstr(path, "/dev/") == path) continue;
            //__android_log_print(6, "frida", "start find fridarpc in %s", path);
            //if (elf_check_header(base) != 1) continue;
            if (find_mem_string(base, end, ( unsigned char * ) fridarpc, length) == 1) {
                __android_log_print(6, "frida", "find fridarpc in %s", buffer);
               // mylog("find fridarpc in memory");
                break;
            }
        }
    } else {
    }
    return false;
}

ssize_t read_one_line(int fd, char *buf, unsigned int max_len) {
    char b;
    ssize_t ret;
    ssize_t bytes_read = 0;

    memset(buf, 0, max_len);

    do {
        ret = read(fd, &b, 1);

        if (ret != 1) {
            if (bytes_read == 0) {
                // error or EOF
                return -1;
            } else {
                return bytes_read;
            }
        }

        if (b == '\n') {
            return bytes_read;
        }

        *(buf++) = b;
        bytes_read += 1;

    } while (bytes_read < max_len - 1);

    return bytes_read;
}

void detect_frida_threads() {
    DIR *dir = opendir(PROC_TASK);
    if (dir != NULL) {
        struct dirent *entry = NULL;
        while ((entry = readdir(dir)) != NULL) {
            char filePath[MAX_LENGTH] = "";
            if (0 == strcmp(entry->d_name, ".") || 0 == strcmp(entry->d_name, "..")) {
                continue;
            }
            snprintf(filePath, sizeof(filePath), PROC_STATUS, entry->d_name);
            int fd = openat(AT_FDCWD, filePath, O_RDONLY | O_CLOEXEC, 0);
            if (fd != 0) {
                char buf[MAX_LENGTH] = "";
                read_one_line(fd, buf, MAX_LENGTH);
                if (strstr(buf, FRIDA_THREAD_GUM_JS_LOOP) ||
                    strstr(buf, FRIDA_THREAD_GMAIN)) {
                    //Kill the thread. This freezes the app. Check if it is an anticpated behaviour
                    //int tid = my_atoi(entry->d_name);
                    //int ret = my_tgkill(getpid(), tid, SIGSTOP);
                    __android_log_print(6, "hanbingle", "%s", "found frida thread");
                    //kill(getpid(), SIGKILL);
                }
                close(fd);
            }

        }
        closedir(dir);

    }

}

void detect_frida_namedpipe() {
    DIR *dir = opendir(PROC_FD);
    if (dir != NULL) {
        struct dirent *entry = NULL;
        while ((entry = readdir(dir)) != NULL) {
            struct stat thisfilestat;
            char buf[MAX_LENGTH] = "";
            char filePath[MAX_LENGTH] = "";
            snprintf(filePath, sizeof(filePath), "/proc/self/fd/%s", entry->d_name);
            lstat(filePath, &thisfilestat);
            if ((thisfilestat.st_mode & S_IFMT) == S_IFLNK) {
                //TODO: Another way is to check if filepath belongs to a path not related to system or the app
                readlinkat(AT_FDCWD, filePath, buf, MAX_LENGTH);
                if (NULL != strstr(buf, FRIDA_NAMEDPIPE_LINJECTOR)) {
                    __android_log_print(6, "frida", "find fridanamedpipe:%s", buf);
                    __android_log_print(6, "hanbingle", "%s", "find frida named pipe");
                }
            }

        }
    }
    closedir(dir);
}

void detect_frida_byport() {
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    inet_aton("127.0.0.1", &(sa.sin_addr));
    int sock;
    /*
     * 1: Frida Server Detection.
     */
    int port = 27042;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    sa.sin_port = htons(port);
    if (connect(sock, ( struct sockaddr * ) &sa, sizeof sa) != -1) {
        __android_log_print(6, TAG,
                            "frida server running on port %d!", port);
    }
    close(sock);

}

void *check_loop(void *) {
    while (1) {
        __android_log_print(6, "hanbingle", "%s", "go into check_loop");
        detect_frida_string();
        detect_frida_threads();
        detect_frida_namedpipe();
        detect_frida_byport();
        sleep(200);
    }

    return nullptr;
}

void anti_frida_thread() {
    pthread_t t;
    if (pthread_create(&t, nullptr, check_loop, ( void * ) nullptr) != 0) {
        exit(-1);
    };
    pthread_detach(t);
}


void startCheck() {
    __android_log_print(6, "qqq", "%s", "go into startCheck()");
    anti_frida_thread();
}

#endif //MULTIREGISTERNATIVE_ANTIFRIDA_H
