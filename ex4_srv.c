// Linor Salhov 206389553
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/prctl.h>

int secondCounter=0;
int timeout=0;
int TIMEOUT(){
    return timeout;
}

void signalHandlerSigAlarm(){
    secondCounter+=1;
    if(secondCounter==60){
        while(wait(NULL)!=-1);  
        printf("The server was closed because no service request was received for the last 60 seconds\n");
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

//caculate result
int caculateFunc(int first, int op, int second){
    int result;
    if (op==1){
        result = first+second;
        return result;
    }
    if (op==2){
        result = first-second;
        return result;
    }
    if (op==3){
        result = first*second;
        return result;
    }
    if (op==4){
        result = first/second;
        return result;
    }
}


int file_exists(char* filename) 
{
    struct stat buffer;
    if(stat(filename, &buffer) == 0){
        return true;
    }
    return false;
}

void handleClient() {
    TIMER();
    signal(SIGUSR1, handleClient);
    int status;
    pid_t sonpid;
    int pid = fork();
    if(pid==-1){ 
        //failed
        printf("ERROR_FROM_EX4\n");
		exit(1);
    }
    if(pid==0){
        //child
        char ch;
        char to_client_txt[100];
        char tmp[5];
        int srv_file, read_srv;
        FILE* file;

        //open file for reading
        file = fopen("to_srv", "r");

        //return error if failed
        if(!file){
            printf("ERROR_FROM_EX4\n");
            exit(1);
        }
        
        // variables
        int flag=0;
        int i=0;
        char *pid_client="";
        int first, op, second;
        int client_pid;
        char client_pid_str[5]="",tmpFirst[20] = "",tmpSecond[20]="";

        //read from file parameters: pid_client num1 op num2
        while(fscanf(file,"%s",tmp)!=EOF){
            if(i==0){
                client_pid = atoi(tmp);
                sprintf(client_pid_str, "%d", client_pid);
            }   
            if(i==1){
                
                first = atoi(tmp);
            }  
            if(i==2){
                op = atoi(tmp);
            }
            if(i==3){
                second = atoi(tmp);
            }
            i++;
        }
        fclose(file);
        // to check remove
        remove("to_srv");

        //result
        int result = caculateFunc(first,op,second);

        //create "to_client" file
        char client_file_name[20]="";
        char to_client[20]="";
        strcat(strcpy(to_client, "to_client_"),client_pid_str);

        //write the result to "to_client" file
        FILE* pf1 = NULL;
        pf1 = fopen(to_client, "w");
        fprintf(pf1, "%d", result);
        fclose(pf1);
        pid_t pid_client_pid = (pid_t)client_pid;
        int ret_val_kill = kill(pid_client_pid, SIGUSR2);
        if (ret_val_kill == -1) { 
            printf("ERROR_FROM_EX4\n");
            exit(1);
        }
    }
    else{
       
    }
}

int main(){

   int s; 
    if (file_exists("to_srv")){  
       remove("to_srv");
    }
    
    //wait for signal
    signal(SIGUSR1, handleClient);
    signal(SIGCHLD, SIG_IGN);
    TIMER();
    while(1){
        
       pause(); 
      
       if(TIMEOUT()){
            break;
       }  
    }
    if(file_exists("to_srv")){
        remove("to_srv");
    }
}