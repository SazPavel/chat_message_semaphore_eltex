#include "server.h"

int main()
{
    struct sigaction sigint;
    sigint.sa_handler = SigintHandler;
    sigint.sa_flags = 0;
    sigemptyset(&sigint.sa_mask);
    sigaddset(&sigint.sa_mask, SIGINT);
    sigaction(SIGINT, &sigint, 0);
    key_t key[2];
    long type;
    int size, i, num_tmp, chat_num = 0;
    struct msg_buf msg;
    key[0] = ftok("server", 'a');
    key[1] = ftok("server", 'b');
    if(key[0] == -1 || key[1] == -1)
    {
        perror("ftok");
        exit(-1);
    }
    id[0] = shmget(key[0], sizeof(struct user) * 16, IPC_CREAT|0600);
    id[1] = shmget(key[1], sizeof(struct msg_buf) * size_chat, IPC_CREAT|0600);
    if(id[0] == -1 || id[1] == -1)
    {
        perror("shmget");
        exit(-1);
    }
    sem_id[0] = semget(key[0], 2, IPC_CREAT|0600);
    sem_id[1] = semget(key[1], 2, IPC_CREAT|0600);
    if((arr_user = (struct user*)shmat(id[0], NULL, 0)) == NULL)
    {
        perror("shmat");
        exit(-1);
    }
    if((chat = (struct msg_buf*)shmat(id[1], NULL, 0)) == NULL)
    {
        perror("shmat");
        exit(-1);
    }
    pthread_create(&tid, NULL, UsersList, NULL);
    while(cycle)
    {
        semop(sem_id[1], user_write_lock, 2);
        if(chat_num == size_chat - 1)
        {
            chat_num--;
            for(i = 0; i < chat_num; i++)
            {
                strcpy(chat[i].name, chat[i+1].name);
                strcpy(chat[i].buf, chat[i+1].buf);
            }
        }
        strcpy(chat[chat_num].name, chat[size_chat - 1].name);
        strcpy(chat[chat_num].buf, chat[size_chat - 1].buf);
        chat_num ++;
        semop(sem_id[1], user_read_unlock, 1);
    }
    exit(0);
}

void SigintHandler(int sig)
{
    cycle = 0;
    pthread_cancel(tid);
    if(shmctl(id[0], IPC_RMID, 0) == -1)
    {
        perror("shmctl");
        exit(-1);
    }
    if(shmctl(id[1], IPC_RMID, 0) == -1)
    {
        perror("shmctl");
        exit(-1);
    }
    if(semctl(sem_id[0], 0, IPC_RMID) == -1)
    {
        perror("semctl");
        exit(-1);
    }
    if(semctl(sem_id[1], 0, IPC_RMID) == -1)
    {
        perror("semctl");
        exit(-1);
    }
    pthread_join(tid, NULL);
    printf("\nServer out\n");
    exit(0);
}

void *UsersList()
{
    int i;
    while(cycle)
    {
        semop(sem_id[0], user_write_lock, 2);
    
        if(arr_user[15].flag == 1)
        {
            arr_user[num].pid = arr_user[15].pid;
            strcpy(arr_user[num].name, arr_user[15].name);
            num ++;
        }else{
            for(i = 0; i < num; i++)
            {
                if(arr_user[i].pid == arr_user[15].pid)
                {
                    for(; i < num - 1; i++)
                    {
                        arr_user[i].pid = arr_user[i + 1].pid;
                        strcpy(arr_user[i].name, arr_user[i + 1].name);
                    }
                    memset(&arr_user[i].name[0], 0, sizeof(arr_user[i].name));
                    num --;
                    break;
                }
            }
        }
        semop(sem_id[0], user_read_unlock, 1);
    }
}
