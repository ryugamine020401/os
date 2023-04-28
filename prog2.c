#include <stdio.h>
#include<stdlib.h>
#include <pthread.h>
#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3

int total[NUMBER_OF_RESOURCES]={0};                             //資源總數
int available[NUMBER_OF_RESOURCES];                             //允許使用的資源數量
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];          //process最大的資源需求量
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES]={0};   //process目前已經取得的資源數量
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];             //max-allo
int Bavailable[NUMBER_OF_RESOURCES];                            //用於保存還原現場的資源量
int Bmaximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];         //用於保存還原現場的最大需求量
int Ballocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];      //用於保存還原現場的已分配量
int Bneed[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];            //用於保存還原現場的需要量
int Finish[NUMBER_OF_CUSTOMERS]={0};                            //確認process是否結束

int release_resources(int customer_num);                    //釋放資源
int request_resources(int customer_num, int request[]);     //要求資源
void *thread_func(void* customer_numt);                     //thread
int bankerAlgorithm(int customer_num,int request[]);        //實現banker函數
void printState();                                          //輸出目前狀態
pthread_mutex_t mutex;                                      //保護資源狀態，避免競爭
char string[NUMBER_OF_RESOURCES*2]={0};                     //用於輸出每個資源的名稱和數量
int safeSequence[NUMBER_OF_CUSTOMERS]={0};                  // 用於保存安全序列




int main(int argc, const char * argv[]) {
//********************* 初始化矩陣 ************************************************
    for(int i=0 ;i<argc-1;i++){
        available[i]=atoi(argv[i+1]);   // 從命令行參數中讀取當前可用的資源數量
        total[i]=available[i];          // 一開始可用的資源等於總資源
    }
    for(int i=0;i< NUMBER_OF_RESOURCES;i++){
        for(int j=0;j<NUMBER_OF_CUSTOMERS;j++){
            maximum[j][i]=rand()%(total[i]+1);     //maximum should less than total
            need[j][i]=maximum[j][i];               // need=max-allo  (allocation=0)
        }
    }

//********************* 初始化矩陣 *************************************************
    
    //****************** 顯現出max跟aviable *********************
    for (int i=0; i<NUMBER_OF_RESOURCES; i++) {
        string[i*2]=i+'A';
        string[i*2+1]=' ';
    }
    printf("Total system resources are:\n");
    printf("%s\n",string);
    for (int i=0; i<NUMBER_OF_RESOURCES; i++) {
        printf("%d ",total[i]);
    }
    printf("\n\nProcesses (maximum resources):\n");
    printf("   %s\n",string);
    for(int i=0; i<NUMBER_OF_CUSTOMERS;i++){
        printf("P%d ",i+1);
        for(int j=0;j<NUMBER_OF_RESOURCES;j++){
            printf("%d ",maximum[i][j]);
        }
        printf("\n");
    }

    printState();
   //****************** 顯現出max跟aviable *********************
    

    pthread_mutex_init(&mutex, NULL);    //initialize mutex
    pthread_t p1,p2,p3,p4,p5;

    int a=0,b=1,c=2,d=3,e=4;

    pthread_create(&p1,NULL,thread_func,&a);
    pthread_create(&p2,NULL,thread_func,&b);
    pthread_create(&p3,NULL,thread_func,&c);
    pthread_create(&p4,NULL,thread_func,&d);
    pthread_create(&p5,NULL,thread_func,&e);     //create 5 threads
    char *returnV;
        
    pthread_join(p1,(void**)&returnV);
    pthread_join(p2,(void**)&returnV);
    pthread_join(p3,(void**)&returnV);
    pthread_join(p4,(void**)&returnV);
    pthread_join(p5,(void**)&returnV); // wait for all the 5 threads to terminate

    return 0;
}


void *thread_func(void* Tcustomer_num){
    int *c=(int*)Tcustomer_num;     //宣告一個指向 int 的指標 c，存放傳入 thread function 的參數
    int customer_num= *c;           //從指標 c 取出參數，存放在 customer_num
    
    int requestSum=0;       //宣告一個 int 變數 requestSum，用來存放 request 的總和
    
    while(!Finish[customer_num]){   //當 thread 尚未完成工作（Finish[customer_num] 為 0）時執行 while loop
        requestSum=0;   
        int request[NUMBER_OF_RESOURCES]={0};   //宣告一個長度為 NUMBER_OF_RESOURCES 的 int array request，並將每個元素初始化為 0
        for(int i=0;i<NUMBER_OF_RESOURCES;i++){ //產生每種資源的 request，並將它們的總和存放在 requestSum
            request[i]=rand()%(need[customer_num][i]+1); //隨機生成低於其需要的請求
            requestSum=requestSum+request[i];
        }
        
        if(requestSum!=0)  //如果 requestSum 不為 0，表示有資源被 request 
            while(request_resources(customer_num,request)==-1); 
            /*只有當 request 成功被 granted 時，while loop 才會結束，否則一直 request。*/
    }
    return 0;
}


int request_resources(int customer_num, int request[]){
    int returnValue=-1;
    pthread_mutex_lock(&mutex); //使用pthread庫函數，申請鎖定mutex變數，保護並發訪問變數的獨佔權。
    
    printf("\nP%d requests for ",customer_num+1);
    for(int i=0;i<NUMBER_OF_RESOURCES;i++){
        printf("%d ",request[i]);
    }
    printf("\n");

        /*檢查申請的資源是否超過了系統中目前可用的資源，如果超過了，就輸出提示訊息，
        並且解鎖mutex變數，返回-1，表示申請失敗，顧客需要等待更多的資源。*/
        for(int i=0;i<NUMBER_OF_RESOURCES;i++){   
            if(request[i]>available[i]){
                printf("P%d is waiting for the reaources...\n",customer_num+1);
                pthread_mutex_unlock(&mutex);   
                return -1;
            }
        }
    returnValue=bankerAlgorithm(customer_num,request);  
    
    if(returnValue==0){ //函式的返回值為0，代表系統可以滿足客戶的需求。
        int needIsZero=1;
        printf("a safe sequence is found: ");
        for(int i=0;i<NUMBER_OF_CUSTOMERS;i++){
            printf("P%d ",safeSequence[i]+1 );
        }
        printf("\nP%d's request has been granted\n",customer_num+1);
       
        for(int j=0;j<NUMBER_OF_RESOURCES;j++){ 
            allocation[customer_num][j]=allocation[customer_num][j]+request[j];
            available[j]=available[j]-request[j];
            need[customer_num][j]=need[customer_num][j]-request[j];
            if(need[customer_num][j]!=0){
                needIsZero=0;
            }
        }
        
        if(needIsZero){ //如果該客戶所有需求均已滿足，則將該客戶的Finish狀態設置為1，並釋放該客戶已經分配的資源。
            Finish[customer_num]=1;     
            release_resources(customer_num); 
        }   
        printState();   
    }
    else{
        printf("cannot find a safe sequence\n");
    }  
    pthread_mutex_unlock(&mutex); 
    return returnValue;
}


int release_resources(int customer_num){
    printf("P%d releases all the resources\n",customer_num+1);
    for(int j=0;j<NUMBER_OF_RESOURCES;j++){
        available[j]=available[j]+allocation[customer_num][j]; // release the resources
        allocation[customer_num][j]=0;       
    }
    return 0;
}

int bankerAlgorithm(int customer_num,int request[]){
    int finish[NUMBER_OF_CUSTOMERS]={0};
    
    for(int i=0;i<NUMBER_OF_RESOURCES;i++){ // 複製矩陣主要為allo、max、need、Avai
        Bavailable[i]=available[i];
        for(int j=0;j<NUMBER_OF_CUSTOMERS;j++){
            Ballocation[j][i]=allocation[j][i];  
            Bmaximum[j][i]=maximum[j][i];
            Bneed[j][i]=need[j][i];
        }
    }
    for(int i=0;i<NUMBER_OF_RESOURCES;i++){ //假裝將資源給執行緒
        Bavailable[i]=Bavailable[i]-request[i];
        Ballocation[customer_num][i]=Ballocation[customer_num][i]+request[i];
        Bneed[customer_num][i]=Bneed[customer_num][i]-request[i];
    }
//*************************safety Algorithm***************************
    int count=0;// 記錄找到的執行緒數量
    while(1){   
        int I=-1;   //紀錄找到的可以運行的執行緒編號
        for(int i=0;i<NUMBER_OF_CUSTOMERS;i++){ //找到需要資源數量小於等於可用資源的執行緒
            int nLessThanA=1;                           //假設目前找到的執行緒的需要資源量皆小於等於可用資源量
            for(int j=0; j<NUMBER_OF_RESOURCES;j++){    //判斷是否有資源不足或是已經完成的情況
                if(Bneed[i][j]>Bavailable[j] || finish[i]==1){
                    nLessThanA=0;       //若有不足資源或是已完成的狀況，就無法運行該執行緒，將 nLessThanA 設為 0
                    break;
                }
            }
            if(nLessThanA){ //若 nLessThanA 仍為 1，表示可以運行該執行緒
                I=i;
                break;
            }
        }
        if(I!=-1){                  //如果找到可以運行的執行緒
            safeSequence[count]=I;  //紀錄該執行緒編號到安全序列中
            count++;                //安全序列長度+1
            finish[I]=1;            //將該執行緒標記為已完成
            for(int k=0;k<NUMBER_OF_RESOURCES;k++){  //假裝給予該執行緒所需要的資源
                Bavailable[k]=Bavailable[k]+Ballocation[I][k];
            }
        }else{              //如果找不到可以運行的執行緒
            for(int i=0;i<NUMBER_OF_CUSTOMERS;i++){
                if(finish[i]==0){ //檢查是否還有未完成的執行緒，如果有就無法找到安全序列
                    return -1;      //檢查是否還有未完成的執行緒，如果有就無法找到安全序列
                }  
            }
            return 0;  //有找到
        }
    }
    
//*************************safety Algorithm***************************
    
    
}

//-------------------------印出need alloc和avail-----------------------
void printState(){
    printf("Processes (currently allocated resources):\n   %s\n",string);
    for(int i=0; i<NUMBER_OF_CUSTOMERS;i++){
        printf("P%d ",i+1);
        for(int j=0;j<NUMBER_OF_RESOURCES;j++){
            printf("%d ",allocation[i][j]);
        }
        printf("\n");
    }
    printf("Processes (possibly needed resources):\n   %s\n",string);
    for(int i=0; i<NUMBER_OF_CUSTOMERS;i++){
        printf("P%d ",i+1);
        for(int j=0;j<NUMBER_OF_RESOURCES;j++){
            printf("%d ",need[i][j]);
        }       
        printf("\n");
    }
    printf("Available system resources are:\n%s\n",string);
    for (int i=0; i<NUMBER_OF_RESOURCES; i++) {
        printf("%d ",available[i]);
    } 
    printf("\n");
}
//-------------------------印出need alloc和avail-----------------------