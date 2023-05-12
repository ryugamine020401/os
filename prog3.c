#include<stdio.h>
#include<stdlib.h>
#include<time.h> 
#include <stdbool.h>

int page_fault_arr[3] = {0};
int main(){
    /* 生成要求的頁數 範圍是0~9 */

    // srand(time(NULL));                  // 使用時間作為種子，使每次生成的隨機數不同
    // int page_reference_string[20];
    // for(int i=0 ; i<20 ; i++){
    //     int random_integer = rand() % 10;// 生成介於0和9之間的隨機整數
    //     page_reference_string[i] = random_integer;
    // }    

    // for(int i=0 ; i<20 ; i++){
    //     printf("%d, ", page_reference_string[i]);

    // }
    // printf("\n");
    /* ------------------------------------------------------FIFO------------------------------------------------- */
    int page_reference_string[20] = {7, 2, 3, 1, 2, 5, 3, 4, 6, 7, 7, 1, 0, 5, 4, 6, 2, 3, 0 , 1};
    int frames_degree = 3;      /* 選擇frame的數量 */
    int frames[frames_degree];
    for (int i = 0;i < frames_degree ; i++){
        frames[i] = -1;
    }

    int cnt = 0;        // 計數總共填入多少資料 共20筆
    bool check = false; // 確認目前frame內是否已經存在需要的page
    int page_fault = 0; // pagefault計數
    while(cnt < 20){

        /* 檢查是否已經有 page faults了 */
        for(int i = 0; i < frames_degree; i++){
            if(frames[i] == page_reference_string[cnt]){
                check=true;
                break;
            }else{
                check=false;
            }
        }

        /*------------------------------------若發發生page fault--------------------------------------*/
        if(check == false){
            for(int i = frames_degree; i > 0; i--){
                frames[i] = frames[i-1];
            }
            frames[0] = page_reference_string[cnt];
            page_fault++;
        }
        /*------------------------------------若發發生page fault--------------------------------------*/
        
        /*----------------------輸出-------------------------*/
        for (int i =0; i< frames_degree;i++){
            printf("%d ", frames[i]);
        }
        printf("\n");
        /*----------------------輸出-------------------------*/

        cnt++;
    }
    printf("page faults = %d\n LRU:\n",page_fault);
        
    page_fault_arr[0] = page_fault;
    /* ------------------------------------------------------FIFO------------------------------------------------- */

    /* ------------------------------------------------------LRU------------------------------------------------- */
    /* -------------------------------初始化--------------------------------------- */
    for (int i = 0;i < frames_degree ; i++){
        frames[i] = -1;
    }
    cnt = 0;
    page_fault = 0;
    /* -------------------------------初始化--------------------------------------- */
    while(cnt <20){
        int tmp = 0;

        /*--------------------------檢查frame內是否存在現在要用的page---------------------*/
        for(int i = 0; i < frames_degree; i++){
            if(frames[i] == page_reference_string[cnt]){
                check=true;
                tmp = i;    //確認第幾個位置有該資料，要以他為基準往右移。
                break;
            }else{
                check=false;
            }
        }
        /*--------------------------檢查frame內是否存在現在要用的page---------------------*/

        /*------------------------------------若發發生page fault--------------------------------------*/
        if(check == false){ // 發生page_fault
            for(int i = frames_degree; i > 0; i--){
                frames[i] = frames[i-1];
            }
            frames[0] = page_reference_string[cnt];
            page_fault++;
        }else{              // frame內已經存在，沒有發生page_fault但將數值往前提代表近期才被呼叫
            int frame_tmp = frames[tmp];
            for(int i = tmp; i > 0; i--){
                frames[i] = frames[i-1];
            }
            frames[0] = frame_tmp;
        }
        /*------------------------------------若發發生page fault--------------------------------------*/


        /*----------------------輸出-------------------------*/
        for (int i =0; i< frames_degree;i++){
            printf("%d ", frames[i]);
        }
        printf("\n");
        /*----------------------輸出-------------------------*/
        cnt++;
    }
    printf("LRU page fault=%d\nOptimal replacement\n\n",page_fault);
    page_fault_arr[1] = page_fault;
    /* ------------------------------------------------------LRU------------------------------------------------- */


    /* -------------------------------------------Optimal replacement----------------------------------------------------- */

    /* -------------------------------初始化--------------------------------------- */
    for (int i = 0;i < frames_degree ; i++){
        frames[i] = -1;
    }
    cnt = 0;
    page_fault = 0;
    bool page_fault_check=false;    // page_fault的次數
    /* -------------------------------初始化--------------------------------------- */


    while(cnt <20){

        bool check = false;     // 用來確認未來是否都有輸入
        int tmp = 0;                // 當前frame內的index再度被調用是幾格後
        int max_tmp = 0;            // 當前frame內哪一個page最晚會再度被使用
        int need_replace_page = 0;  // 需要更改的page在flames內的索引
        int have_future_cnt = 0;    // 當每個frame未來都還有輸入


        /* ------------------------檢查每個frame內的page要多久才會再遇到---------------------------- */
        for(int i = 0; i<frames_degree;i++){
            for(int k = cnt; k<20; k++){    // 檢測未來的輸入
                if(frames[i] == page_reference_string[k]){
                    tmp = k-cnt;
                    have_future_cnt++;
                    check = true;
                    break;
                }else{
                    if(have_future_cnt != frames_degree && k==19){
                        need_replace_page = i;
                        check = false;
                        // printf("後面沒有ㄌ = %d ;need_replace_page = %d\n", need_replace_page, frames[i]);
                        break;
                    }
                }
            }
            if(max_tmp < tmp && check == true){
                max_tmp = tmp;
                need_replace_page = i;
            }else if(check == false){
                break;
            }
        }
        /* ------------------------檢查每個frame內的page要多久才會再遇到---------------------------- */


        /* --------------------------------檢查frame內是否已經存在page------------------------------------ */
        for(int i = 0; i < frames_degree; i++){
            if(frames[i] == page_reference_string[cnt]){
                page_fault_check=false;
                break;
            }else{
                page_fault_check=true;  //代表page fault
            }
        }
        /* --------------------------------檢查frame內是否已經存在page------------------------------------ */


        /*---------------------------------當發生page_fault----------------------------------------*/

        if(page_fault_check == true && cnt > frames_degree){
            // printf("need_replace_page = %d\n", need_replace_page);
            frames[need_replace_page] = page_reference_string[cnt];
            page_fault++;
        }

        /*---------------------------------當發生page_fault----------------------------------------*/

        /*----------當frame還有空位時直接填入-------------*/
        if(cnt <= frames_degree){
            
            for(int i = frames_degree; i > 0; i--){
                frames[i] = frames[i-1];
                //printf("frame[%d]=%d\n",i,frames[i]);
            }
            frames[0] = page_reference_string[cnt];
            page_fault++;
   
        }
        /*-----------當frame還有空位時直接填入------------*/

        /*----------------------輸出-------------------------*/

        for (int i =0; i< frames_degree;i++){
            printf("%d ", frames[i]);
        }
        printf("\n");
        /*----------------------輸出-------------------------*/
        cnt++;
    }
    printf("page fault=%d\n",page_fault);
    page_fault_arr[2] = page_fault;

    printf("FIFO page faults:%d\nLRU  page faults:%d\nOptimal replacement page faults:%d\n",page_fault_arr[0],page_fault_arr[1],page_fault_arr[2]);
    return 0;
}

    /* -------------------------------------------Optimal replacement----------------------------------------------------- */