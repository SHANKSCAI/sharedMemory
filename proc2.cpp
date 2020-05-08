#include<iostream>
#include<sys/sem.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<cstring>
#include<sys/types.h>
using namespace std;
#define BLOCK_NUM 3
struct  data
{
    char text[100];
};
struct Shm_head
{
    int lastNode;
    int head;
    int tail;
};
union Semun
{
    int val;
    struct  semid_ds* buf;
    unsigned short* array;
    struct seminfo* _buf;
};

int P(int semid ,int semnum){
    struct  sembuf sops={semnum,-1,SEM_UNDO};
    return (semop(semid,&sops,1));
}
int V(int semid,int semnum){
    struct sembuf sops={semnum,+1,SEM_UNDO};
    return (semop(semid,&sops,1));
}
int main(){
    int semid=semget(0x123,3,IPC_CREAT|0666);
    if(semid==-1){
        cout<<"semid error"<<endl;
        exit(0);
    }
    /*-------------------------shared memory-----------------------------*/
    int shm_id=shmget(0x111,BLOCK_NUM*sizeof(struct  data)+sizeof(Shm_head),IPC_CREAT|0666);
    if(shm_id==-1){
        cout<<"shm_id error\n";
    }
    void *shared_m=shmat(shm_id,NULL,0);
    Shm_head* shm_head=(Shm_head*)shared_m;

/*-------------------------shared memory end-----------------------------*/
    struct data* p=NULL;
    char str_buf[100]={0};
    while(1){
        P(semid,2);//full
        P(semid,0);//mutex
        p=(struct data*)((char*)shared_m+sizeof(*shm_head)+shm_head->head*sizeof(struct data));
        strcpy(str_buf,p->text);
        if(shm_head->head<BLOCK_NUM-1){
            shm_head->head++;
        }
        else{
            shm_head->head=0;
        }
        V(semid,0);//mutex
        V(semid,1);//empty
        cout<<str_buf<<endl;
        if(strncmp(str_buf,"end",3)==0){
            break;
        }
    };
    if(shmdt(shared_m)==-1){
        cout<<"shmdt error\n"<<endl;
    }
    shmctl(shm_id,IPC_RMID,NULL);
    union Semun semun;
    semctl(semid,0,IPC_RMID,semun);
    return 0;
}