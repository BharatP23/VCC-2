#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define DEVICE_FILE "/dev/my_ioctl"

//Defining  the ioctl call 
#define IOCTL_GET_PHYSICAL_ADDRESS _IOWR('k', 1, struct GetInfo)
#define IOCTL_WRITE_PHYSICAL_ADDRESS _IOWR('k', 2, struct WriteInfo)

struct GetInfo {
    unsigned long virtual_address;
    unsigned long physical_address;
};

struct WriteInfo {
    unsigned long physical_address;
    char value;
};

int main() {
    int fd;
    struct WriteInfo write_info;
    struct GetInfo  get_info;
    
    fd = open(DEVICE_FILE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device file\n");
        return -1;
    }
    //Allocating one bytes of memory
   char *m = (char *)malloc(sizeof(char));
    if (!m) {
        perror("Failed to allocate dynamic memory\n");
        close(fd);
        return -1;
    }
    *m = 6;

   unsigned long virtual_address = (unsigned long)m;
   get_info.virtual_address=virtual_address;

    printf("Virtual address sending is 0x%lx\n",get_info.virtual_address);

    //Call ioctl to get the physical address of the passed virtual address 
    if (ioctl(fd, IOCTL_GET_PHYSICAL_ADDRESS, &get_info) < 0) {
        perror("IOCTL_GET_PHYSICAL_ADDRESS failed");
        return -1;
    }
    //Storing the received physical address
    write_info.physical_address = get_info.physical_address;
    write_info.value = 5; 
    printf("Physical address received is 0x%lx\n",write_info.physical_address);

    //Calling the ioctl to update the value 
    if (ioctl(fd, IOCTL_WRITE_PHYSICAL_ADDRESS, &write_info) < 0) {
        perror("IOCTL_WRITE_PHYSICAL_ADDRESS failed");
        free(m);
        close(fd);
        return -1;
    }
    
    printf("Value %d written to the Physical Address %lx\n", *m, write_info.physical_address);

    
    free(m);
    close(fd);

    return 0;
}
