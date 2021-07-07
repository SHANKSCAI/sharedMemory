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
    int semid=semget(0x123,3,IPC_CREAT|IPC_EXCL|0666);
    if(semid==-1){
        cout<<"semid error"<<endl;
        exit(0);
    }
    union Semun semun;
    semun.val=1;
    if(semctl(semid,0,SETVAL,semun)==-1){
        cout<<"smctl mutex error"<<endl;
    }
    semun.val=3;
    if(semctl(semid,1,SETVAL,semun)==-1){
        cout<<"smctl empty error"<<endl;
    }
    semun.val=0;
    if(semctl(semid,2,SETVAL,semun)==-1){
        cout<<"smctl full error"<<endl;
    }
    /*-------------------------shared memory-----------------------------*/
    int shm_id=shmget(0x111,BLOCK_NUM*sizeof(struct  data)+sizeof(Shm_head),IPC_CREAT|IPC_EXCL|0666);
    if(shm_id==-1){
        cout<<"shm_id error\n";
    }
    void *shared_m=shmat(shm_id,NULL,0);
    Shm_head* shm_head=(Shm_head*)shared_m;
    shm_head->head=0;
    shm_head->tail=0;
    shm_head->lastNode=BLOCK_NUM-1;
/*-------------------------shared memory end-----------------------------*/
    struct data* p=NULL;
    char str_buf[100]={0};
    while(1){
        cin>>str_buf;
        P(semid,1);//empty
        P(semid,0);//mutex
        p=(struct data*)((char*)shared_m+sizeof(*shm_head)+shm_head->tail*sizeof(struct data));
        strcpy(p->text,str_buf);
        if(shm_head->tail<BLOCK_NUM-1){
            shm_head->tail++;
        }
        else{
            shm_head->tail=0;
        }
        V(semid,0);//mutex
        V(semid,2);//full
        if(strncmp(str_buf,"end",3)==0){
            break;
        }
    };
    if(shmdt(shared_m)==-1){
        cout<<"shmdt error\n"<<endl;
    }


    cout << "test git pull" << endl;

    //cout << "test" << endl;
	//cout << "test20210707" << endl;


 
    //cout << "test" << endl;
    //cout<<"merge"<<endl;

    return 0;
}
