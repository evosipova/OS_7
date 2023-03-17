#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHARED_MEM_SIZE sizeof(int)

volatile sig_atomic_t exit_flag = 0;

void sig_handler(int signo) {
  if (signo == SIGINT || signo == SIGTERM) {
    exit_flag = 1;
  }
}

int main() {
  int fd;
  int *shared_mem_ptr;

  // открытие сегмента разделяемой памяти
  if ((fd = shm_open("/my_shared_mem", O_CREAT | O_RDWR, 0666)) == -1) {
    perror("shm_open");
    exit(1);
  }

  // изменение размера сегмента разделяемой памяти
  if (ftruncate(fd, SHARED_MEM_SIZE) == -1) {
    perror("ftruncate");
    exit(1);
  }

  // отображение сегмента разделяемой памяти в адресное пространство процесса
  if ((shared_mem_ptr = mmap(NULL, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE,
                             MAP_SHARED, fd, 0)) == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }

  // установка обработчика сигналов
  if (signal(SIGINT, sig_handler) == SIG_ERR) {
    perror("signal");
    exit(1);
  }

  // чтение данных из сегмента разделяемой памяти
  while (!exit_flag) {
    if (*shared_mem_ptr != 0) {
      printf("Server: received number %d\n", *shared_mem_ptr);
      *shared_mem_ptr = 0; // обнуление данных в сегменте разделяемой памяти
    }
  }

  // отображение сегмента разделяемой памяти
  if (munmap(shared_mem_ptr, SHARED_MEM_SIZE) == -1) {
    perror("munmap");
    exit(1);
  }

  // удаление сегмента разделяемой памяти
  if (shm_unlink("/my_shared_mem") == -1) {
    perror("shm_unlink");
    exit(1);
  }

  printf("Server terminated\n");

  return 0;
}
