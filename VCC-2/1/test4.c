#include <stdlib.h>
#include <stdio.h>

int main() {
    int i;
    size_t sizes[] = {4096,12288,16384,24576,32768,40960,61440,73728,81920};
    printf("PID of the current process is : %d\n", getpid());
    for (i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++) {
        void *ptr = malloc(sizes[i]);
        if (ptr == NULL) {
            fprintf(stderr, "Failed to allocate memory of size %lu\n", sizes[i]);
            return 1;
        }
        printf("Allocating  %lu bytes of memory\n", sizes[i]);
        sleep(10);
    }

    return 0;
}
