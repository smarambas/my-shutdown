/***************************************************************
                            MY-SHUTDOWN
    Version: 2.0
    
    Author: Marco Paesano
    
    Date: 2018_09_24
  
    Description: Script to poweroff/reboot/suspend/hibernate 
                 the computer after a certain time defined by 
                 the user.
***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define SIZE 256

char* command;
long time_sec;
time_t timer;

void helper(void)
{
    printf("\n\t\t\t\t\tShutdown Scheduler\n\n"
           "Script to poweroff/reboot/suspend/hibernate the computer after a certain time defined by the user.\n\n"
           "Usage: my_shutdown [operation] -[s/m/h/t] [time]\n\n"
           "Available operations:\n 1) poweroff\n 2) reboot\n 3) suspend\n 4) hibernate\n 5) hybrid-sleep\n 6) help\n\n"
           "The chosen operation to perform must be written as first argument and in lowercase.\n\n"
           "To define the time, after which the operation will be executed, there are several choices:\n"
           "1) use -s to use seconds as unit of measure and, after that, the amount of seconds;\n"
           "2) use -m to use minutes as unit of measure and, after that, the amount of minutes;\n"
           "3) use -h to use hours as unit of measure and, after that, the amount of hours;\n"
           "4) use -t to use a human redable format (HH:MM), where HH goes from 00 to 23 and MM from 00 to 59.\n\n"
           "\tExample 1:\n\n\t./my-shutdown suspend -m 120\n\nThe aforementioned command will suspend the system after 120 minutes.\n\n"
           "\tExample 2:\n\n\t./my-shutdown hibernate -t 12:00\n\nThe aforementioned command will hibernate the system at 12:00.\n\n"
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

int which_option(char* opt)
{
    int res;

    if(strcmp(opt, "-t") == 0) {
        res = 1;
    } 
    else if(strcmp(opt, "-s") == 0 || strcmp(opt, "-m") == 0 || strcmp(opt, "-h") == 0) {
        res = 0;
    }
    else {
        res = -1;
    }

    return res;
}

char* default_option(char* t_id, long t_input)
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
    }
}

char* allocate_string(int s)
{
    char* dest;
    dest = malloc(s * sizeof(char));
    if(dest == NULL) {
        printf("Error: malloc failed.\n");
        exit(-1);
    }

    return dest;
}

void human_option(char* human_time)
{
    char* hours;
    char* min;
    char* a_hours;
    char* a_min;
    char* a_sec;
    long h, m, a_h, a_m, a_s, h_diff, m_diff;
    char actual_formatted_time[8];
    int s = 3;
    struct tm* tm_info;
    char* endptr;   //to check the input

    time(&timer);
    tm_info = localtime(&timer);
    strftime(actual_formatted_time, 8, "%H:%M:%S", tm_info);  //actual time
    //puts(actual_formatted_time);

    hours = allocate_string(s);
    min = allocate_string(s);
    a_hours = allocate_string(s);
    a_min = allocate_string(s);
    a_sec = allocate_string(s);
        
    memcpy(a_hours, &actual_formatted_time[0], 2);
    a_hours[2] = '\0';
    //printf("%s\n", a_hours);

    memcpy(a_min, &actual_formatted_time[3], 2);
    a_min[2] = '\0';
    //printf("%s\n", a_min);

    memcpy(a_sec, &actual_formatted_time[6], 2);
    a_sec[2] = '\0';
    
    memcpy(hours, &human_time[0], 2);
    hours[2] = '\0';
    //printf("%s\n", hours);

    memcpy(min, &human_time[3], 2);
    min[2] = '\0';
    //printf("%s\n", min);

    h = strtol(hours, &endptr, 10);
    if(*endptr != 0 || h > 23) {
        printf("Error: invalid time format.\n");
        exit(-1);
    }
    m = strtol(min, &endptr, 10);
    if(*endptr != 0 || m > 59) {
        printf("Error: invalid time format.\n");
        exit(-1);
    }
    a_h = strtol(a_hours, NULL, 10);
    a_m = strtol(a_min, NULL, 10);
    a_s = strtol(a_sec, NULL, 10);

    h_diff = h - a_h;
    m_diff = m - a_m;     

    if(h_diff < 0) {
        time_sec = (24 - a_h + h) * 60 * 60 + m_diff * 60 - a_s;  
    }
    else {
        time_sec = h_diff * 3600 + m_diff * 60 - a_s;
    }

    return;
}

int main(int argc, char** argv)
{
    int ret, check;
    long time_input;
    char* option;
    char* time_id;
    double integral;
    double fractional;
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
    
    if(strcmp(argv[1], "poweroff") != 0 && strcmp(argv[1], "reboot") != 0 && strcmp(argv[1], "suspend") != 0 && strcmp(argv[1], "hibernate") != 0 && strcmp(argv[1], "hybrid-sleep") != 0) {
        printf("Error: invalid operation.\n");
        exit(-1);
    }

    command = allocate_string(SIZE);  

    command = strcat(command, "systemctl ");
    
    option = argv[2];
    check = which_option(option);
    switch(check) {
        case 0:
            time_input = strtol(argv[3], NULL, 10);

            if(time_input < 1) {
                printf("Error: invalid time.\n");
                exit(-1);
            }

            time_id = default_option(option, time_input);
            
            break;
        case 1:
            human_option(argv[3]);
            break;
        case -1:
            printf("Error: invalid option.\n");
            exit(-1);
    }

    if(check) {
        fractional = modf((double) time_sec/3600, &integral);
        printf("The system will %s at %s (after %0.1lf hours and %0.1lf minutes)...\n", argv[1], argv[3], integral, fractional*3600/60);
        sleep(1);
    }
    else {
        printf("The system will %s in %ld %s...\n", argv[1], time_input, time_id);
        sleep(1);
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
