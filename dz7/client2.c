#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>

#define SHARED_MEM_SIZE  sizeof(int)
#define RAND_RANGE 100

volatile sig_atomic_t exit_flag = 0;

void sig_handler(int signo)
{
  if (signo == SIGINT || signo == SIGTERM) {
    exit_flag = 1;
  }
}

int main()
{
  int fd;
  int *shared_mem_ptr;
  int rand_num;
  srand(time(NULL));

  // открытие сегмента разделяемой памяти
  if ((fd = shm_open("/my_shared_mem", O_CREAT | O_RDWR, 0666)) == -1) {
    perror("shm_open");
    exit(1);
  }

  // отображение сегмента разделяемой памяти в адресное пространство процесса
  if ((shared_mem_ptr = mmap(NULL, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }

  // установка обработчика сигналов
  if (signal(SIGINT, sig_handler) == SIG_ERR) {
    perror("signal");
    exit(1);
  }

  // запись данных в сегмент разделяемой памяти
  while (!exit_flag) {
    rand_num = rand() % RAND_RANGE + 1;
    *shared_mem_ptr = rand_num;
    printf("Client: generated number %d\n", rand_num);
    sleep(1);
  }

  // отображение сегмента разделяемой памяти
  if (munmap(shared_mem_ptr, SHARED_MEM_SIZE) == -1) {
    perror("munmap");
    exit(1);
  }

  printf("Client terminated\n");

  return 0;
}

