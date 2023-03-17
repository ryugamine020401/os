#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Start sleeping...\n");

    sleep(5); // delay 5 seconds
    printf("Done sleeping.\n");
    return 0;

}