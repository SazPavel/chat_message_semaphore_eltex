#ifndef server_saz_h
#define server_saz_h

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <pthread.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>
#include <signal.h>
#include <sys/msg.h>
#define size_chat 21

struct msg_buf
{
    char name[32];
    char buf[32];
};

struct user
{
    char name[32];
    int pid;
    int flag;
};

struct sembuf user_write_lock[2] = {{1, -1, 0}, {0, 1, 0}};
struct sembuf user_read_unlock[1] = {0, -1, 0};
int  id[2], cycle = 1, num = 0, sem_id[2];
struct user *arr_user;
struct msg_buf *chat;
pthread_t tid;

void SigintHandler(int sig);
void *UsersList();

#endif
