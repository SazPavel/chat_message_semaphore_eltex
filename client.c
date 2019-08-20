#include "client.h"

int main()
{
    key_t key[2];
    pid_t my_pid = getpid();
    pthread_t tid;
    int i, cycle = 1;
    char my_name[32];
    char my_text[32];
    key[0] = ftok("server", 'a');
    key[1] = ftok("server", 'b');
    if(key[0] == -1 || key[1] == -1)
    {
        perror("ftok");
        exit(-1);
    }
    id[0] = shmget(key[0], sizeof(struct user) * 16, 0);
    id[1] = shmget(key[1], sizeof(struct msg_buf) * size_chat, 0);
    if(id[0] == -1 || id[1] == -1)
    {
        perror("shmget");
        exit(-1);
    }
    sem_id[0] = semget(key[0], 2, 0);
    sem_id[1] = semget(key[1], 2, 0);
    if((all_user = (struct user*)shmat(id[0], NULL, 0)) == NULL)
    {
        perror("shmat");
        exit(-1);
    }
    if((chat = (struct msg_buf *)shmat(id[1], NULL, 0)) == NULL)
    {
        perror("shmat");
        exit(-1);
    }
    printf("Enter name:\n");
    fgets(my_name, sizeof(my_name), stdin);
    strtok(my_name, "\n");
    semop(sem_id[0], user_read_lock, 2);
    all_user[15].flag = 1;
    all_user[15].pid = my_pid;
    strcpy(all_user[15].name, my_name);
    semop(sem_id[0], user_read_unlock, 2);
    initscr();
    cbreak();
    curs_set(0);
    getmaxyx(stdscr, rows, cols);
    wnd = newwin(rows, cols, 0, 0);
    listwnd = newwin(rows - 2, cols/3 + 1, 0, 2*cols/3);
    chatwnd = newwin(rows - 4, 2*cols/3-1, 1, 1);
    chatboxwnd = newwin(rows - 2, 2*cols/3 + 1, 0, 0);
    pthread_create(&tid, NULL, AcceptMessage, NULL);
    while(cycle)
    {
        print(1);
        getnstr(my_text, sizeof(my_text));
        strtok(my_text, "\n");
        semop(sem_id[1], user_read_lock, 2);
        strcpy(chat[size_chat - 1].buf, my_text);
        strcpy(chat[size_chat - 1].name, my_name);
        semop(sem_id[1], user_read_unlock, 2);
        if(!strcmp(my_text, "exit"))
        {
            cycle = 0;
            pthread_cancel(tid);
            pthread_join(tid, NULL);
            semop(sem_id[0], user_read_lock, 2);
            all_user[15].flag = 0;
            all_user[15].pid = my_pid;
            strcpy(all_user[15].name, my_name);
            semop(sem_id[0], user_read_unlock, 2);
            if(listwnd != NULL)
                delwin(listwnd);
            if(chatboxwnd != NULL)
                delwin(chatboxwnd);
            if(chatwnd != NULL)
                delwin(chatwnd);
            if(wnd != NULL)
                delwin(wnd);
            endwin();
        }
    }
    exit(0);
}

void print(int n)
{
    int i;
    clear();
    wclear(listwnd);
    wclear(chatwnd);
    box(wnd, '|', '-');
    box(listwnd, '|', '-');
    box(chatboxwnd, '|', '-');
    semop(sem_id[0], user_read, 1);
    for(i = 0; i < 15; i++)
        mvwprintw(listwnd, i + 1, 1, "%s", all_user[i].name);
    semop(sem_id[1], user_read, 1);
    for(i = 0; i < size_chat - 1; i++)
        mvwprintw(chatwnd, i, 0, "%s: %s", chat[i].name, chat[i].buf);
    mvwprintw(wnd, rows - 2, 2, "%s", "Enter text: ");
    move(rows - 2, 14);
    refresh();
    wrefresh(wnd);
    wrefresh(listwnd);
    wrefresh(chatboxwnd);
    wrefresh(chatwnd);
}

void *AcceptMessage()
{
    while(1)
    {
        sleep(1);
        print(0);
    }
}
