
#include <stdio.h>
#include <time.h>

int main() {
    time_t current_time = time(NULL);
    printf("Current time: %ld\n", current_time);
    return 0;
}