#include <iostream>
#include <string>
#include <random>
#include <thread>
#include <chrono>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <semaphore.h>
#include <cstring>

using namespace std;
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

    if (argc != 6) {
        cerr << "Usage: ./producer <commodity> <mean> <stddev> <interval> <buffer_size>" << endl;
        return 1;
    }
    string commodity = argv[1];

    double mean = stod(argv[2]);
    double stddev = stod(argv[3]);
    int interval = stoi(argv[4]);
    int bufferSize = stoi(argv[5]);

    cout << "Commodity: " << commodity << endl;
    cout << "Mean: " << mean << endl;
    cout << "Standard deviation: " << stddev << endl;
    cout << "Interval: " << interval << " ms" << endl;
    cout << "Buffer size: " << bufferSize << endl;

default_random_engine generator;
normal_distribution<double> distribution(mean, stddev);

while (true) {
    double price = distribution(generator);
    cout << commodity << ": " << price << endl;

    this_thread::sleep_for(chrono::milliseconds(interval));
}
    return 0;
}
