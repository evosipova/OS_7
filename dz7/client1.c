#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define SHARED_MEM_NAME "/my_shared_memory"
#define SHARED_MEM_SIZE sizeof(int)

int main() {
  srand(time(NULL)); // Инициализация генератора случайных чисел

  int fd = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0666);
  if (fd == -1) {
    perror("shm_open failed");
    return EXIT_FAILURE;
  }

  int *shared_mem_ptr =
      mmap(NULL, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shared_mem_ptr == MAP_FAILED) {
    perror("mmap failed");
    return EXIT_FAILURE;
  }

  close(fd);

  while (1) {
    int random_number =
        rand() % 100 + 1; // Генерируем случайное число в диапазоне от 1 до 100
    *shared_mem_ptr = random_number;
    printf("Client: generated number %d\n", random_number);
    sleep(1);
  }
}
