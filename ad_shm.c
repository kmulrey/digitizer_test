/***
 DAQ shared memory
 Version:0.2
 Date: 19/2/2013
 Author: Charles Timmermans, Nikhef/Radboud University
 
 Altering the code without explicit consent of the author is forbidden
 ***/
#include <string.h>
#include "Adaq.h"
//#include "ad_shm.h"

int ad_shm_create(shm_struct *ptr,int nbuf,int size)
{
    int sz_int = sizeof(int);
    size_t isize = (size+1)*nbuf*sizeof(uint16_t)+5*sz_int;
    key_t key = IPC_PRIVATE;
    
    ptr->shmid = 0;
    ptr->buf = NULL;
    ptr->Ubuf = NULL;
    ptr->next_write = NULL;
    ptr->next_read = NULL;
    ptr->next_readb = NULL;
    ptr->nbuf = NULL;
    ptr->size = NULL;
    ptr->shmid = shmget(key,isize,IPC_CREAT|0666);
    if(ptr->shmid < 0) return(ERROR);
    ptr->buf = shmat(ptr->shmid,NULL,0600);
    memset((void *)ptr->buf,0,isize);
    ptr->Ubuf = (uint16_t *)(&(ptr->buf[5*sz_int]));
    ptr->next_write = (int *)&(ptr->buf[0]);
    ptr->next_read = (int *)&(ptr->buf[sz_int]);
    ptr->next_readb = (int *)&(ptr->buf[2*sz_int]);
    ptr->nbuf = (int *)&(ptr->buf[3*sz_int]);
    ptr->size = (int *)&(ptr->buf[4*sz_int]);
    *(ptr->next_write) = 0;
    *(ptr->next_read) = 0;
    *(ptr->next_readb) = 0;
    *(ptr->nbuf) = nbuf;
    *(ptr->size) = size;
    return(NORMAL);
}

void ad_shm_delete(shm_struct *ptr)
{
    shmdt(ptr->buf);
    shmctl(ptr->shmid, IPC_RMID, NULL);
    ptr->shmid = 0;
    ptr->buf = NULL;
    ptr->Ubuf = NULL;
    ptr->next_write = NULL;
    ptr->next_read = NULL;
    ptr->next_readb = NULL;
    ptr->nbuf = NULL;
    ptr->size = NULL;
}

