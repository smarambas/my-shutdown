/***************************************************************
                            MY-SHUTDOWN
    Version: 1.0
    
    Author: Marco Paesano
    
    Date: 2018_09_15
  
    Description: Script to shutdown/reboot/suspend/hibernate 
                 the computer after a certain time defined by 
                 the user.
***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>

#define SIZE 256

char* command;
char formatted_time[16];
long time_sec;
time_t timer;
struct tm* tm_info;

void helper(void)
{
    printf("\n\t\t\t\t\tShutdown Scheduler\n\n"
           "Script to shutdown/reboot/suspend/hibernate the computer after a certain time defined by the user.\n\n"
           "Usage: my_shutdown <operation> -[s/m/h] <timer>\n\n"
           "Available operations:\n 1) poweroff\n 2) reboot\n 3) suspend\n 4) hibernate\n 5) hybrid-sleep\n\n"
           "The chosen operation to perform must be written as first argument and in lowercase.\n\n"
           "To define the time, after which the operation will be executed, there are several choices:\n"
           "1) use -s to use seconds as unit of measure and, after that, the amount of seconds;\n"
           "2) use -m to use minutes as unit of measure and, after that, the amount of minutes;\n"
           "3) use -h to use hours as unit of measure and, after that, the amount of hours.\n\n"
           "\tFor example:\n\n\t./shutdown_scheduler.out suspend -m 120\n\nThe aforementioned command will suspend the computer after 120 minutes.\n\n"
           "It's important to notice that the possibility to set the timer to 0 would make this program useless, that's why is not possible.\n\n"
           "After setting a timer, it's possible to undo it by sending a SIGINT signal (ctrl+c).\n\n");
    return;
}

void alarm_handler(int dummy)
{
    printf("\n");
    system(command);
    //return;
}

void sigint_handler(int dummy)
{
    printf("\nCancelling pending operation...\n");
    exit(0);
}

char* select_time_id(char* t_id, long t_input)
{
    if(strcmp(t_id, "-s") == 0) {
        time_sec = t_input;
        return "seconds";
    }
    else if(strcmp(t_id, "-m") == 0) {
        time_sec = t_input * 60;
        return "minutes";
    }
    else if(strcmp(t_id, "-h") == 0) {
        time_sec = t_input * 60 * 60;
        return "hours";
    }/*
    else if(strcmp(t_id, "-t") == 0) {
        time(&timer);
        tm_info = localtime(&timer);
        strftime(formatted_time, 16, "%d %H:%M:%S", tm_info);
        //puts(formatted_time);
        
        //function for time formatting
    }*/
    else {
        printf("Error: invalid time id.\n");
        exit(-1);
    }
}

int main(int argc, char** argv)
{
    int ret;
    long time_input;
    char* time_id;
    struct sigaction act;
    sigset_t set1, set2;

    if(argc < 4) {
        if(argc < 2) {
            printf("Usage: my_shutdown <operation> -[s/m/h] <timer>\n\n"
                   "Try 'my_shutdown help' for more information.\n\n");
            exit(-1);
        }
        else if(strcmp(argv[1], "help") == 0) {
            helper();
            exit(0);
        }
        else {
            printf("Usage: my_shutdown <operation> -[s/m/h] <timer>\n\n"
                   "Try 'my_shutdown help' for more information.\n\n");
            exit(-1);
        }
    }

    command = malloc(SIZE * sizeof(char));
    if(!command) {
        printf("Error: malloc failed.\n");
        exit(-1); 
    }    

    command = strcat(command, "systemctl ");

    time_input = strtol(argv[3], NULL, 10);

    if(time_input < 1) {
        printf("Error: invalid time.\n");
        exit(-1);
    }

    time_id = select_time_id(argv[2], time_input);

    if(strcmp(argv[1], "poweroff") == 0) {
        printf("The computer will shutdown and poweroff in %ld %s...\n", time_input, time_id);
        sleep(1);
    }
    else if(strcmp(argv[1], "reboot") == 0) {
        printf("The computer will reboot in %ld %s...\n", time_input, time_id);
        sleep(1);
    }
    else if(strcmp(argv[1], "suspend") == 0) {
        printf("The computer will suspend in %ld %s...\n", time_input, time_id);
        sleep(1);
    }
    else if(strcmp(argv[1], "hibernate") == 0) {
        printf("The computer will hibernate in %ld %s...\n", time_input, time_id);
        sleep(1);
    }
    else if(strcmp(argv[1], "hybrid-sleep") == 0) {
        printf("The computer will hibernate and suspend in %ld %s...\n", time_input, time_id);
        sleep(1);
    }
    else {
        printf("Error: invalid operation.\n");
        exit(-1);
    }
        
    command = strcat(command, argv[1]);

    sigfillset(&set1);
    act.sa_handler = sigint_handler;
    act.sa_mask = set1;
    act.sa_flags = 0;
    ret = sigaction(SIGINT, &act, NULL);
    if(ret == -1) {
        printf("Error: sigaction failed.\n");
        exit(-1);
    }

    sigfillset(&set2);
    act.sa_handler = alarm_handler;
    act.sa_mask = set2;
    act.sa_flags = 0;
    ret = sigaction(SIGALRM, &act, NULL);
    if(ret == -1) {
        printf("Error: sigaction failed.\n");
        exit(-1);
    }

    alarm(time_sec);

    pause();    

    return 0;
}
