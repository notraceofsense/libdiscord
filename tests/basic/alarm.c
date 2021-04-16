#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>

#define INTERVAL 5000

void handle_alarm(int sig) {

    printf("test\n");

}

int main() {
    struct itimerval it_val;
    signal(SIGALRM, handle_alarm);
    it_val.it_value.tv_sec = INTERVAL / 1000;
    it_val.it_value.tv_usec = (INTERVAL*1000) % 1000000;
    it_val.it_interval = it_val.it_value;
    if(setitimer(ITIMER_REAL, &it_val, NULL) == -1) {
        perror("setitimer");
        exit(EXIT_FAILURE);
    }

    while(1) {
        pause();
    }

    return 0;
}