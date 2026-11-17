#include <iostream>
#include <string>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <semaphore.h>

struct PriceEntry {
    char commodity[11];
    double price;
};

struct SharedData {
    sem_t empty;
    sem_t full;
    sem_t mutex;

    int in;
    int out;
    int bufferSize;
};

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cerr << "Usage: ./consumer <buffer_size>" << std::endl;
        return 1;
    }

    int bufferSize = std::stoi(argv[1]);

    if (bufferSize <= 0) {
        std::cerr << "Buffer size must be positive" << std::endl;
        return 1;
    }
    int shm_fd = shm_open("/price_shm", O_CREAT | O_RDWR, 0666);

    if (shm_fd == -1) {
        perror("Error creating shared memory");
        return 1;
    }
  size_t shmSize = sizeof(SharedData) + bufferSize * sizeof(PriceEntry);
    if (ftruncate(shm_fd, shmSize) == -1) {
        perror("Error setting shared memory size");
        close(shm_fd);
        shm_unlink("/price_shm");
        return 1;
    }
    void* sharedMemory = mmap(
        nullptr,
        shmSize,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        shm_fd,
        0
    );
 if (sharedMemory == MAP_FAILED) {
        perror("Error mapping shared memory");
        close(shm_fd);
        shm_unlink("/price_shm");
        return 1;
    }
SharedData* sharedData = (SharedData*) sharedMemory;

PriceEntry* buffer = (PriceEntry*)((char*)sharedMemory + sizeof(SharedData));

sharedData->in = 0;
sharedData->out = 0;
sharedData->bufferSize = bufferSize;


sem_init(&sharedData->empty, 1, bufferSize);
sem_init(&sharedData->full, 1, 0);
sem_init(&sharedData->mutex, 1, 1);

close(shm_fd);

    return 0;
}
