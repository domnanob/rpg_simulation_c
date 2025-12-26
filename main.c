#include <stdio.h> // printf()
#include <stdlib.h> // atoi()
#include <sys/stat.h> // stat struktúra
#include <time.h> // ctime()
#include <sys/wait.h> // wait()
#include <pwd.h> // passwd struktúra
#include <dirent.h> // könyvtárkezelés
#include <semaphore.h> // nevesített szemafor
#include <sys/shm.h> // osztott memória
#include <signal.h>
#include <unistd.h>

#include "entity.h"


void signal_kezelo(int sig)
{
    //printf("Jelzés beérkezett! (%d)\n", sig);
}

sem_t* szemafor_letrehozas(char* nev, int ertek)
{
    sem_t* szem_id = sem_open(nev, O_CREAT, S_IRUSR|S_IWUSR, ertek);    
    if(szem_id == SEM_FAILED)
    {
        perror("Hiba a szemafor létrehozásakor.\n");
    }
    return szem_id;
}

void szemafor_torles(char* nev)
{
    sem_unlink(nev);
}

int main()
{   
    key_t kulcs = ftok(0, 1);
    int sh_mem_id = shmget(kulcs, 1024, IPC_CREAT|S_IRUSR|S_IWUSR);
    char *s = shmat(sh_mem_id, NULL, 0);

    char* szem_nev = "szemafor";
    sem_t* szem_id = szemafor_letrehozas(szem_nev, 0);

    signal(SIGUSR1, signal_kezelo);
    
    int playerpipe[2]; 
    int attackpipe[2];
    int enemypipe[2];
    if(pipe(playerpipe) < 0 || pipe(attackpipe) < 0 || pipe(enemypipe) < 0)
    {
        printf("Hiba a cső létrehozásakor.\n");
        return 1;
    }

    pid_t pid = fork();
    if(pid == -1) 
    {
        printf("Hiba a folyamatkezelésben.\n"); // hibakezelés
        exit(1);
    }
    if(pid > 0) // szülő
    {
        struct CHARACTER meele_character = CreateMeeleCharacter();
        struct CHARACTER ranged_character = CreateRangedCharacter();
        int game_rounds = 0;
        while (game_rounds < 5 && (alive(&meele_character) || alive(&ranged_character)))
        {
            struct ENEMY enemy = CreateDummyEnemy();
            int parent_rounds = 0;
            while (alive(&enemy) && (alive(&meele_character) || alive(&ranged_character)))
            {
                printf("----- %d. Gane %d. Round! -----\n",game_rounds+1, parent_rounds+1);
                int parent_turn = 0;
                while (parent_turn < 3) //egy kör ahol mindenki támad
                {
                    if (parent_turn == 0 && parent_rounds == 0) {
                        printf("----- Game master started the round. -----\n");
                        printf("\t~ Generating a random enemy.\n");
                        switch (random_number_between(3,5))
                        {
                            case 3:
                                enemy = CreateTrollEnemy();
                                printf("\t~ Troll created.\n");
                                break;
                            case 4:
                                enemy = CreateDragonEnemy();
                                printf("\t~ Dragon created.\n");
                                break;
                            case 5:
                                enemy = CreateBanditEnemy();
                                printf("\t~ Bandit created.\n");
                                break;
                        }
                        printEntity(&ranged_character);
                        printEntity(&meele_character);
                        printEntity(&enemy);
                    }
                    sleep(1);

                    struct CHARACTER players[2] = {ranged_character, meele_character};
                    close(playerpipe[0]); 
                    write(playerpipe[1], &players, sizeof(players));

                    close(enemypipe[0]); 
                    write(enemypipe[1], &enemy, sizeof(enemy));
                
                    kill(0, SIGUSR1);
                
                    sem_wait(szem_id);
                    close(attackpipe[1]); 
                    int attack_state;
                    read(attackpipe[0], &attack_state, sizeof(attack_state)); 
                    if (attack_state == 1)
                    {
                        printf("\t~ Successfull attack!\n");
                        if (parent_turn == 0)
                        {
                            attack(&ranged_character, &enemy);
                        } else if (parent_turn == 1) {
                            attack(&meele_character, &enemy);
                        } else {
                            if (alive(&meele_character) && alive(&ranged_character))
                            {
                                switch (random_number_between(1,2))
                                {
                                    case 1:
                                        attack(&enemy, &ranged_character);
                                        break;
                                    
                                    case 2:
                                        attack(&enemy, &meele_character);
                                        break;
                                }
                            } else if (alive(&meele_character)) {
                                attack(&enemy, &meele_character);
                            } else {
                                attack(&enemy, &ranged_character);
                            }
                        }
                    } else if (attack_state == -1) {
                        printf("\t~ Skipped attack!\n");
                    } else 
                    {
                        printf("\t~ Missed attack!\n");
                        if (parent_turn == 0)
                        {
                            resetCharacterAccuracy(&ranged_character);
                        } else if (parent_turn == 1) {
                            resetCharacterAccuracy(&meele_character);
                        }
                    }
                    parent_turn++;
                    sleep(2);
                }
                printf("----- Round End -----\n");
                printEntity(&ranged_character);
                printEntity(&meele_character);
                printEntity(&enemy);
                parent_rounds++;
            }
            if (alive(&meele_character)) {
                healCharacter(&meele_character,25);
            }
            if (alive(&ranged_character)) {
                healCharacter(&ranged_character,25);
            }
            game_rounds++;
        }
        printf("----- The Game Master closed the game! -----\n");
        if (alive(&meele_character) || alive(&ranged_character))
        {
            printf("\t~ Players won the Game!\n");
        } else {
            printf("\t~ Players lost the Game!\n");
        }
        
        kill(0, SIGTERM);
    }
    else // gyerek
    {
        int child_turn = 0;
        for (;;)
        {
            pause();
            struct CHARACTER players[2]; 
            struct ENEMY enemy; 
            close(playerpipe[1]); 
            close(enemypipe[1]); 
            read(playerpipe[0], &players, sizeof(players));
            read(enemypipe[0], &enemy, sizeof(enemy));
            if (child_turn == 0)
            {
                int attack_state = -1;
                if (alive(&players[0]))
                {
                    printf("---- The First player prepared. ----\n");
                    printf("\t~ First player trying to attack.\n");
                    attack_state = aim(&players[0]);
                } else {
                    printf("---- The Dirst player is dead. ----\n");
                    printf("\t~ Skipping First player attack.\n");
                }
                close(attackpipe[0]); 
                write(attackpipe[1], &attack_state, sizeof(attack_state));
                sem_post(szem_id);
            } else if (child_turn == 1) {
                int attack_state = -1;
                if (alive(&players[1]))
                {
                    printf("---- The Second player prepared. ----\n");
                    printf("\t~ Second player trying to attack.\n");
                    attack_state = aim(&players[1]);
                } else {
                    printf("---- The Second player is dead. ----\n");
                    printf("\t~ Skipping Second player attack.\n");
                }
                close(attackpipe[0]); 
                write(attackpipe[1], &attack_state, sizeof(attack_state));
                sem_post(szem_id);
            } else {
                int attack_state = -1;
                if (alive(&enemy))
                {
                    printf("---- The Enemy prepared. ----\n");
                    printf("\t~ Enemy trying to attack.\n");
                    attack_state = aim(&enemy);
                } else {
                    printf("---- The Enemy died in the round. ----\n");
                    printf("\t~ Skipping Enemy attack.\n");
                }
                close(attackpipe[0]); 
                write(attackpipe[1], &attack_state, sizeof(attack_state));
                sem_post(szem_id);
            }
            if (child_turn == 2)
            {
                 child_turn = 0;
            } else {
                 child_turn++;
            }
        }

    }
}
