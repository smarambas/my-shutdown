# my-shutdown

**my-shutdown** is a command line shutdown scheduler developed in C for UNIX/POSIX systems.

The objective was to create a more flexible alternative to the "shutdown" utility.

"my-shutdown", in fact, can poweroff/reboot/suspend/hibernate/hybrid-sleep the system after a certain time defined by the user and it does that by using the respective "systemctl" command.

The usage is quite simple:

$ my-shutdown [operation] -[s/m/h/t] [time]

The available operations are:
1) poweroff
2) reboot
3) suspend
4) hibernate
5) hybrid-sleep
6) help

By choosing help you will get an explanation of how to use the utility.
The chosen operation must be written in lowercase.

You can choose between four different format option for the time:
1) -h, indicates hours
2) -m, indicates minutes
3) -s, indicates seconds
4) -t, indicates a human readable format such as HH:MM, where HH can go from 00 to 23 and MM can go from 00 to 59.

After the format option you must type in the amount of hours/minutes/seconds or the precise time in the aforementioned format.

By choice, it was decided not to make it possible to type in an amount such as 0 (it's a shutdown scheduler!).

After setting a timer, it's possible to undo it by sending a SIGINT signal by pressing ctrl+c.

Examples:

$ my-shutdown suspend -m 70             //will suspend the system after 70 minutes

$ my-shutdown hibernate -t 07:35        //will hibernate the system at 07:35

$ my-shutdown help                      //will show you an helper

