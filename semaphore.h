#include <iostream>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <iomanip>
#include <chrono>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define SEMPERM 0600



int initsem(key_t semkey, int initval)
{
 	int status = 0, semid;
 	union semun {
 	int val;
        struct semid_ds *stat;
 	ushort *array;
 } ctl_arg;

 if ((semid = semget(semkey, 1, SEMPERM | IPC_CREAT | IPC_EXCL)) == -1)
 {
     semid = semget(semkey, 1, 0);
 }
 else
 {
 	ctl_arg.val = initval;
 	status = semctl(semid, 0, SETVAL, ctl_arg);
 }
 
 if (semid == -1 || status == -1)
 {
 	perror("initsem failed");
 	return (-1);
 }
 else
 {
     return semid;
 }
}

int down(int semid)
{
 	struct sembuf p_buf;
 	p_buf.sem_num = 0;
 	p_buf.sem_op = -1;
 // p_buf.sem_flg = SEM_UNDO;

 if (semop(semid, &p_buf, 1) == -1)
 {
 	perror("p(semid) failed");
 	exit(1);
 }
 else return 0 ; 
}

int up(int semid)
{
 	struct sembuf v_buf;
 	v_buf.sem_num = 0;
 	v_buf.sem_op = 1;
 // v_buf.sem_flg = SEM_UNDO;
 if (semop(semid, &v_buf, 1) == -1)
 {
 	perror("v(semid) failed");
 	exit(1);
 }
 else return 0 ; 
}


