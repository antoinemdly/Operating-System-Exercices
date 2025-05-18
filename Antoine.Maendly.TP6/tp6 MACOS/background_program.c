#include <stdio.h>
#include <unistd.h>

int main(void){
    for(int i = 0; i < 10; i++){
        printf("This program is running in the background... (%d)\n", getpid());
        sleep(2);
    }

    printf("exiting program...\n");
    return 0;
}