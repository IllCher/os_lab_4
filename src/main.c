#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include "c_queue.h"
typedef struct ans ans;
struct ans {
    int cmd;
    int val;
    char path[32];
};
typedef struct node node,*pnode;
struct node {
    pnode s;
    pnode b;
    int val;
    bool is_root;
};
pnode node_create(int val) {
    pnode new_node = (pnode)malloc(sizeof(node));
    if (new_node) {
        new_node -> val = val;
        new_node -> s = NULL;
        new_node -> b = NULL;
        new_node -> is_root = false;
    }
    return new_node;
}
pnode* search(pnode* t, queue *path) {
    if (!(*t) && !q_is_empty(path)) {
        return NULL;
    }
    if (!q_is_empty(path)) {
        char c = q_front(path);
        pop(path);
        if (c == 's') {
            return search(&(*t) -> s, path);
        } else if (c == 'b') {
            return search(&(*t) -> b, path);
        }
        return NULL;
    }
    return t;
}
bool add(pnode* t, int val, queue *path) {
    if (!(*t) && q_is_empty(path)) {
        (*t) = node_create(val);
        return true;
    }
    pnode* pr = search(t, path);
    if (!pr) {
        return false;
    }
    pnode new_node = node_create(val);
    if (!new_node) {
        return false;
    }
    new_node -> b = (*pr);
    (*pr) = new_node;
    return true;
}
void rmv(pnode* t) {
    while((*t) -> s != NULL){
        rmv(&((*t) -> s));
    }
    pnode tmp = *t;
    *t = (*t) -> b;
    free(tmp);
}
bool valid_numb(char* numb) {
    if (numb == NULL) {
        return false;
    }
    bool flag = true;
    int i = 0;
    if (numb[i] != '-' && !(numb[i] >= '0' && numb[i] <= '9')) {
        flag = false;
    }
    i++;
    while (i < 11) {
        if (numb[i] == '\0') {
            break;
        }
        if (!(numb[i] >= '0' && numb[i] <= '9')) {
            flag = false;
            break;
        }
        i++;
    }
    return flag;
}
bool valid_path(char* path) {
    if (path == NULL) {
        return false;
    }
    if (path[0] == '@' && path[1] == '\0') {
        return true;
    }
    for (int i = 0; i < 32; i++) {
        if (path[i] == '\0') {
            break;
        } else if (path[i] != 's' && path[i] != 'b') {
            return false;
        }
    }
    return true;
}
ans* parser(char* cmd) {
    ans* parsed = (ans*)malloc(sizeof(ans));
    char* pch = strtok(cmd," \n");
    while (pch != NULL) {
        if (strcmp(pch, "prt") == 0) {
            parsed->cmd = 0;
            break;
        } else if (strcmp(pch, "rmv") == 0) {
            pch = strtok(NULL, " \n");
            if (valid_path(pch)) {
                parsed->cmd = 1;
                strcpy(parsed->path, pch);
                if (parsed->path[0] == 'b') {
                    parsed->cmd = -1;
                }
                break;
            } else {
                parsed->cmd = -1;
                break;
            }
        } else if (strcmp(pch, "add") == 0) {
            pch = strtok(NULL, " \n");
            if (valid_path(pch)) {
                strcpy(parsed->path, pch);
                pch = strtok(NULL, " \n");
                if (parsed->path[0] == 'b') {
                    parsed->cmd = -1;
                    break;
                }
                if (valid_numb(pch)) {
                    parsed->cmd = 2;
                    parsed->val = atoi(pch);
                    break;
                } else {
                    parsed->cmd = -2;
                    break;
                }
            } else {
                parsed->cmd = -1;
                break;
            }
        } else if (strcmp(pch, "ext") == 0) {
            parsed->cmd = 3;
            break;
        } else {
            parsed->cmd = -777;
            break;
        }
    }
    return parsed;
}
void tree_print(pnode t, int depth) {
    if (t) {
        for (int i = 0; i < depth; i++) {
            write(1, "\t", 1);
        }
        char numb[11] = {'\0'};
        sprintf(numb, "%d", t->val);
        int i = 0;
        while (numb[i] != '\0') {
            i++;
        }
        write(1, numb, i);
        write(1,"\n", 1);
        tree_print(t -> s, depth + 1);
        tree_print(t -> b, depth);
    }
}
int create_tmp() {
    char *fn = strdup("/tmp/tmpf.XXXXXX");
    int fd = mkstemp(fn);
    unlink(fn);
    free(fn);
    write(fd, "                                                                                                     ", 100);
    return fd;
}

int main(int argc, char* argv[]) {
    setvbuf(stdout, (char *) NULL, _IONBF, 0);
    pnode test = NULL;
    char cmd[100] = {'\0'};
    ans *parsed = (ans *) malloc(sizeof(ans));
    int fd = create_tmp();
    lseek(fd, 100, SEEK_END);
    write(fd, "", 1);
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("can't get file size\n");
    }
    int fsize = sb.st_size;
    char* f_in_m = mmap(NULL, fsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    sem_t* sem_calc = sem_open("/calc", O_CREAT, 777, 0);
    if (sem_calc == SEM_FAILED) {
        perror("Semaphores doesn't create");
        exit(1);
    }
    sem_unlink("/calc");
    pid_t pr = -1;
    pr = fork();
    if (pr < 0) {
        write(1, "Can't create process\n", 22);
    } else if (pr > 0) {
        while (read(0, cmd, 100)) {
            parsed = parser(cmd);
            sprintf(f_in_m, "%d %d %s", parsed->cmd, parsed->val, parsed->path);
            if (parsed->cmd == 3) {
                return 0;
            }
            for (int i = 0; i < 100; i++) {
                cmd[i] = '\0';
            }
            sem_post(sem_calc);
        }
        sem_post(sem_calc);
        close(fd);
    } else {

        while (1) {
            sem_wait(sem_calc);
            queue *q = q_create();
            sscanf(f_in_m, "%d %d %32s", &parsed->cmd, &parsed->val, parsed->path);
            int k = 0;
            while (parsed->path[k] != '\0') {
                push(q, parsed->path[k]);
                k++;
            }
            if (q_size(q) == 0) {
                push(q, '\0');
            }
            if (parsed->cmd == 3) {
                return 0;
            } else if (parsed->cmd == 2) {
                if (test == NULL) {
                    while (q_size(q) != 0) {
                        pop(q);
                    }
                    test = node_create(parsed->val);
                    test->is_root = true;
                } else {
                    add(&test, parsed->val, q);
                }
            } else if (parsed->cmd == 1) {
                pnode* f = search(&test, q);
                if (test == NULL) {
                    write(1, "empty tree\n", 11);
                } else if ((*f) == NULL) {
                        write(1, "its root\n", 9);
                        rmv(&test);
                } else {
                        rmv(f);
                }
            } else if (parsed->cmd == 0) {
                if (test == NULL) {
                    write(1, "empty tree\n", 11);
                } else {
                    tree_print(test, 0);
                }
            } else if (parsed->cmd == -2){
                write(1, "invalid value\n", 14);
            } else if (parsed->cmd == -1) {
                write(1, "invalid path\n", 13);
            } else if (parsed->cmd == -777) {
                write(1, "invalid command\n", 16);
            }
            q_destroy(q);
            lseek(fd, 0, SEEK_SET);
            write(fd, "", 100);
        }
        sem_close(sem_calc);
        munmap(f_in_m, 100);
        lseek(fd, 0, SEEK_SET);
        write(fd, "", 100);
        close(fd);
    }
    return 0;
}
