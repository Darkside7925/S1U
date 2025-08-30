#pragma once

#include "s1u/core.hpp"
#include <vulkan/vulkan.h>
#include <vector>
#include <complex>
#include <memory>
#include <thread>
#include <atomic>

namespace s1u {

class QuantumFieldProcessor {
public:
    QuantumFieldProcessor();
    ~QuantumFieldProcessor();

    bool initialize(VkDevice device, VkPhysicalDevice physical_device);
    void shutdown();

    double get_field_coherence() const;
    double get_operator_fidelity() const;
    double get_circuit_performance() const;
    double get_simulation_accuracy() const;
    double get_quantum_volume() const;
    double get_information_density() const;

    void set_planck_constant(double h);
    void set_speed_of_light(double c);
    void set_field_strength(double strength);
    void set_coupling_constant(double coupling);

    void enable_quantum_field_evolution(bool enable);
    void enable_quantum_operator_evolution(bool enable);
    void enable_quantum_circuit_simulation(bool enable);
    void enable_monte_carlo_simulation(bool enable);

    void set_lattice_dimension(int dimension);
    void set_max_quantum_fields(int max_fields);
    void set_max_quantum_operators(int max_operators);
    void set_max_quantum_circuits(int max_circuits);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;

    void quantum_field_evolution_loop();
    void quantum_operator_loop();
    void quantum_circuit_loop();
    void quantum_simulation_loop();

    bool create_vulkan_resources();
    bool create_quantum_buffers();
    bool create_buffer(size_t size, VkBufferUsageFlags usage, VkBuffer* buffer, VkDeviceMemory* memory);
    void cleanup_vulkan_resources();
};

} // namespace s1u

