/*
Single producer Single Consumer ring buffer project

*/

#include "main.hpp"

//Creating a global variable for ring buffer
constexpr size_t NUM_ITER = 1'000'000;
constexpr size_t BUFFER_SIZE = 1 << 9; //usuall ring buffer queues in microcontroller are of the size 512
ring_buffer<uint64_t, BUFFER_SIZE> ring;


void normalizeTimespec(timespec &ts) {
    while (ts.tv_nsec >= 1'000'000'000) {
        ts.tv_nsec -= 1'000'000'000;
        ts.tv_sec += 1;
    }
}

void producer(uint64_t loop_hz=1000){

    timespec next_wake;
    uint64_t time_period_ns = 1e9/loop_hz;
    std::cout << time_period_ns;
    clock_gettime(CLOCK_MONOTONIC, &next_wake);

    for(uint64_t i = 0; i < NUM_ITER; ++i){
        //clock_gettime(CLOCK_MONOTONIC, &next_wake);
        next_wake.tv_nsec += time_period_ns;
        normalizeTimespec(next_wake);

        // Pause producer if the ring buffer is full to allow consumer to catch up
        while(!ring.push(i))
            std::this_thread::yield();

        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_wake, nullptr);
    }   
}

void consumer(uint64_t loop_hz=1000){

    timespec next_wake;
    uint64_t time_period_ns = 1e9/loop_hz;
    clock_gettime(CLOCK_MONOTONIC, &next_wake);
    uint64_t expected = 0;
    uint64_t temp;

    while (expected < NUM_ITER)
    {
        //clock_gettime(CLOCK_MONOTONIC, &next_wake);
        next_wake.tv_nsec += time_period_ns;
        normalizeTimespec(next_wake);
        
        if(ring.pop(temp)){
            if (temp != expected++){
                std::cerr << "Data corrupted!\n";
                std::abort();
            }
            else{
                std::cout << temp << std::endl;
            }
        }
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_wake, nullptr);
    }
}


int main(){

    int prod_loop_hz, cons_loop_hz;
    prod_loop_hz = cons_loop_hz = 100000;


    std::thread producer_thread(producer, prod_loop_hz);
    std::thread consumer_thread(consumer, cons_loop_hz);

    producer_thread.join();
    consumer_thread.join();

    return 0;
}