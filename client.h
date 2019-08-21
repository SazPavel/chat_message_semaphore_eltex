#ifndef client_saz_h
#define client_saz_h

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <curses.h>
#include <sys/sem.h>
#include <sys/shm.h>
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

struct sembuf user_read_lock[2] = {{0, 0, 0}, {0, 1, 0}};
struct sembuf user_read_unlock[2] = {{0, -1, 0}, {1, 1, 0}};
struct sembuf user_read[1] = {0, 0, 0};
struct msg_buf *chat;
int id[2], rows, cols, sem_id[2];
struct user *all_user;
WINDOW *wnd, *listwnd, *chatwnd, *chatboxwnd, *writewnd;
void print();
void *AcceptMessage();

#endif
