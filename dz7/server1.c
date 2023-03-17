#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define SHARED_MEM_NAME "/my_shared_memory"
#define SHARED_MEM_SIZE sizeof(int)

int main() {
  int fd = shm_open(SHARED_MEM_NAME, O_RDONLY, 0666);
  if (fd == -1) {
    perror("shm_open failed");
    return EXIT_FAILURE;
  }

  int *shared_mem_ptr =
      mmap(NULL, SHARED_MEM_SIZE, PROT_READ, MAP_SHARED, fd, 0);
  if (shared_mem_ptr == MAP_FAILED) {
    perror("mmap failed");
    return EXIT_FAILURE;
  }

  close(fd);

  while (1) {
    sleep(1);
    printf("Server: received number %d\n", *shared_mem_ptr);
  }
}
