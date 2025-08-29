#include "s1u/core.hpp"
#include <chrono>
#include <thread>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

namespace s1u {

u64 get_timestamp() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
}

void set_thread_priority(int priority) {
    sched_param param;
    param.sched_priority = priority;
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
}

void set_process_priority(int priority) {
    setpriority(PRIO_PROCESS, 0, priority);
}

void set_cpu_affinity(int cpu_id) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_id, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
}

void lock_memory() {
    mlockall(MCL_CURRENT | MCL_FUTURE);
}

void unlock_memory() {
    munlockall();
}

void prefault_memory(size_t size) {
    void* ptr = malloc(size);
    if (ptr) {
        memset(ptr, 0, size);
        free(ptr);
    }
}

void set_realtime_scheduling() {
    set_thread_priority(99);
    set_process_priority(-20);
    lock_memory();
}

void disable_cpu_scaling() {
    system("echo performance | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor");
}

void enable_cpu_scaling() {
    system("echo ondemand | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor");
}

void set_cpu_frequency(int cpu_id, int frequency_khz) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "echo %d | sudo tee /sys/devices/system/cpu/cpu%d/cpufreq/scaling_setspeed", frequency_khz, cpu_id);
    system(cmd);
}

void set_gpu_frequency(int gpu_id, int frequency_mhz) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "echo %d | sudo tee /sys/class/devfreq/gpu%d/cur_freq", frequency_mhz * 1000000, gpu_id);
    system(cmd);
}

void set_memory_frequency(int frequency_mhz) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "echo %d | sudo tee /sys/class/devfreq/memory/cur_freq", frequency_mhz * 1000000);
    system(cmd);
}

void set_network_priority(const std::string& interface, int priority) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "sudo tc qdisc add dev %s root handle 1: prio bands 3", interface.c_str());
    system(cmd);
    snprintf(cmd, sizeof(cmd), "sudo tc filter add dev %s protocol ip parent 1: prio %d u32 match ip tos 0x10 0xfc flowid 1:1", interface.c_str(), priority);
    system(cmd);
}

void set_disk_priority(const std::string& device, int priority) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "echo %d | sudo tee /sys/block/%s/queue/iosched/prio", priority, device.c_str());
    system(cmd);
}

void set_interrupt_affinity(int irq, int cpu_id) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "echo %d | sudo tee /proc/irq/%d/smp_affinity_list", cpu_id, irq);
    system(cmd);
}

void set_network_affinity(const std::string& interface, int cpu_id) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "echo %d | sudo tee /sys/class/net/%s/queues/rx-0/rps_cpus", 1 << cpu_id, interface.c_str());
    system(cmd);
    snprintf(cmd, sizeof(cmd), "echo %d | sudo tee /sys/class/net/%s/queues/tx-0/xps_cpus", 1 << cpu_id, interface.c_str());
    system(cmd);
}

void set_disk_affinity(const std::string& device, int cpu_id) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "echo %d | sudo tee /sys/block/%s/queue/cpu", cpu_id, device.c_str());
    system(cmd);
}

void set_gpu_affinity(int gpu_id, int cpu_id) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "echo %d | sudo tee /sys/class/drm/card%d/device/local_cpulist", cpu_id, gpu_id);
    system(cmd);
}

void set_memory_affinity(int node_id) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "echo %d | sudo tee /proc/self/numa_maps", node_id);
    system(cmd);
}

void set_cache_affinity(int cache_id) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "echo %d | sudo tee /proc/self/cpuset", cache_id);
    system(cmd);
}

void set_scheduler_affinity(int scheduler_id) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "echo %d | sudo tee /proc/self/sched_affinity", scheduler_id);
    system(cmd);
}

void set_timer_affinity(int timer_id, int cpu_id) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "echo %d | sudo tee /proc/timer_list", cpu_id);
    system(cmd);
}

void set_clock_affinity(int clock_id, int cpu_id) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "echo %d | sudo tee /proc/clock_list", cpu_id);
    system(cmd);
}

void set_power_affinity(int power_id, int cpu_id) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "echo %d | sudo tee /proc/power_list", cpu_id);
    system(cmd);
}

void set_thermal_affinity(int thermal_id, int cpu_id) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "echo %d | sudo tee /proc/thermal_list", cpu_id);
    system(cmd);
}

void set_frequency_affinity(int freq_id, int cpu_id) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "echo %d | sudo tee /proc/freq_list", cpu_id);
    system(cmd);
}

void set_voltage_affinity(int volt_id, int cpu_id) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "echo %d | sudo tee /proc/voltage_list", cpu_id);
    system(cmd);
}

void set_current_affinity(int curr_id, int cpu_id) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "echo %d | sudo tee /proc/current_list", cpu_id);
    system(cmd);
}

void set_power_affinity(int power_id, int cpu_id) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "echo %d | sudo tee /proc/power_list", cpu_id);
    system(cmd);
}

void set_thermal_affinity(int thermal_id, int cpu_id) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "echo %d | sudo tee /proc/thermal_list", cpu_id);
    system(cmd);
}

void set_frequency_affinity(int freq_id, int cpu_id) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "echo %d | sudo tee /proc/freq_list", cpu_id);
    system(cmd);
}

void set_voltage_affinity(int volt_id, int cpu_id) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "echo %d | sudo tee /proc/voltage_list", cpu_id);
    system(cmd);
}

void set_current_affinity(int curr_id, int cpu_id) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "echo %d | sudo tee /proc/current_list", cpu_id);
    system(cmd);
}

}
