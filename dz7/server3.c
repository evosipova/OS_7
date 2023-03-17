#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>

#define SHARED_MEM_SIZE sizeof(int)

void handler(int signum) {
  if (signum == SIGINT) {
    printf("Server received SIGINT signal\n");
    exit(EXIT_SUCCESS);
  }
}

int main() {
  int fd = shm_open("/myshm", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    perror("shm_open");
    exit(EXIT_FAILURE);
  }

  int *shm_ptr = mmap(NULL, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shm_ptr == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }

  signal(SIGINT, handler);

  while (1) {
    printf("Server received number: %d\n", *shm_ptr);
    sleep(1); // wait for 1 second
  }

  return 0;
}
