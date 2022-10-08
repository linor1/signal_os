// Linor Salhov 206389553
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <stdbool.h>

int secondCounter=0;
int timeout=0;
int TIMEOUT(){
    return timeout;
}

void signalHandlerSigAlarm(){
    secondCounter+=1;
    if(secondCounter==30){
        printf("Client closed because no response was received from the server for 30 second\n");
        timeout=1;
        exit(1);
    }
    else{
        alarm(1);
    }
    
}

int  TIMER(){
    secondCounter=0;
    int static flag=0;
    if(! flag){
        if(signal(SIGALRM, signalHandlerSigAlarm)){
        printf("ERROR_FROM_EX4\n");
        exit(1);
        };
    }
    flag=1;
    alarm(1);
    
}


int file_exists(char* filename) 
{
    struct stat buffer;
    if(stat(filename, &buffer) == 0){
        return true;
    }
    return false;
}

int isPosNumber(char number[])
{

    int i = 0;

    for (; number[i] != 0; i++)
    {
        if (!isdigit(number[i]))
            return false;
    }
    return true;
}
int isPosOrNegNumber(char number[])
{
   int i = 0;
    //checking for negative numbers
    if (number[0] == '-')
        i = 1;
    for (; number[i] != 0; i++)
    {
        //if (number[i] > '9' || number[i] < '0')
        if (!isdigit(number[i]))
            return false;
    }
    return true;
}

int gotSignal=0;
void signalhandlerFromServer(){
    gotSignal=1;
    signal(SIGUSR2, signalhandlerFromServer);
    int my_pid = getpid();
    char pid_str[6],file_name[15],to_client[15]="";

    // file to_clien
    sprintf(pid_str, "%d", my_pid);
    strcat(strcpy(to_client, "to_client_"),pid_str);

    // file reading
    char tmp[5];
    FILE* result=NULL;
    result = fopen(to_client, "r");
    int res_for_print;
    while(fscanf(result,"%s",tmp)!=EOF){
        res_for_print = atoi(tmp);
        printf("%d\n" , res_for_print);
        remove(to_client);
    }
}

int main(int argc, char* argv[]) {
    
        //num of arguments valid
    	if (argc != 5) { 
            printf("ERROR_FROM_EX4\n");
		    return 0;
	    }
        //casting to arguments to integers
        int pidServer = atoi(argv[1]);
        int firstNum = atoi(argv[2]);
        int op = atoi(argv[3]);
        int secondNum = atoi(argv[4]);

        //validation chacks
        if((op!=1)&&(op!=2)&&(op!=3)&&(op!=4)){
            printf("ERROR_FROM_EX4\n");
		    return 0;
        }
        if ((op == 4) && (secondNum == 0)) {
            printf("CANNOT_DIVIDE_BY_ZERO\n");
		    return 0;
	    }
        
        if(!(isPosNumber(argv[1])&& isPosOrNegNumber(argv[2]) && isPosNumber(argv[3]) && isPosOrNegNumber(argv[4]))){
            printf("ERROR_FROM_EX4\n");
		    return 0;
        }
        FILE* f = NULL;
        //while loop check if "to_srv" exist and counter smaller than 10
        int counter=0,randTime;
        while((f = fopen("to_srv","wx"))==NULL && (counter<10)){
            randTime = rand()%5 + 1;
            sleep(randTime);
            counter++;
        }
        if(counter>=10){
            printf("ERROR_FROM_EX4\n");
            return 0;
        }


        //if "to_srv" exist write parameters to file
        else{
            signal(SIGUSR2, signalhandlerFromServer);
            int x = getpid();
            fprintf(f, "%d", x);
            fprintf(f, "%c",' ');
            fprintf(f, "%d",firstNum);
            fprintf(f, "%c",' ');
            fprintf(f, "%d",op);
            fprintf(f, "%c",' ');
            fprintf(f, "%d",secondNum);
            fclose(f);


            pid_t pidS =(pid_t)pidServer;
            int ret_val_kill = kill(pidS, SIGUSR1);
            if(ret_val_kill==-1){
                int exist = file_exists("to_srv");
                if(exist==true){
                    remove("to_srv");
                }
                printf("ERROR_FROM_EX4\n");
                return 0;
            }
            TIMER();
            while(!gotSignal && !TIMEOUT()){
                pause();
            }
            
        }
        
    return 0;
}