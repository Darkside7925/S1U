#include "s1u/quantum_optimization_engine.hpp"
#include "s1u/core.hpp"
#include <vulkan/vulkan.h>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>
#include <atomic>
#include <cmath>
#include <complex>

namespace s1u {

class QuantumOptimizationEngine::Impl {
public:
    struct QuantumState {
        std::vector<std::complex<float>> amplitudes;
        std::vector<std::vector<std::complex<float>>> entanglement_matrix;
        std::vector<float> phase_angles;
        std::vector<float> coherence_factors;
        float superposition_probability;
        float decoherence_rate;
        int quantum_dimension;
    };

    struct OptimizationTarget {
        std::string name;
        std::vector<float> current_values;
        std::vector<float> optimal_values;
        std::vector<float> constraints;
        float optimization_score;
        bool is_optimized;
    };

    struct QuantumAlgorithm {
        std::string name;
        std::vector<QuantumState> states;
        std::vector<std::vector<float>> unitary_matrices;
        std::vector<float> measurement_probabilities;
        float algorithm_fidelity;
        float gate_error_rate;
    };

    VkDevice device_;
    VkPhysicalDevice physical_device_;
    VkCommandPool command_pool_;
    VkDescriptorPool descriptor_pool_;

    VkPipeline quantum_pipeline_;
    VkPipeline optimization_pipeline_;
    VkPipeline prediction_pipeline_;

    VkBuffer quantum_buffer_;
    VkBuffer optimization_buffer_;
    VkBuffer prediction_buffer_;

    VkDeviceMemory quantum_memory_;
    VkDeviceMemory optimization_memory_;
    VkDeviceMemory prediction_memory_;

    std::vector<QuantumState> quantum_states_;
    std::vector<OptimizationTarget> optimization_targets_;
    std::vector<QuantumAlgorithm> quantum_algorithms_;

    std::atomic<bool> active_{true};
    std::thread quantum_thread_;
    std::thread optimization_thread_;
    std::thread prediction_thread_;
    std::thread evolution_thread_;

    float quantum_entanglement_strength_;
    float optimization_convergence_rate_;
    float prediction_accuracy_;
    float evolution_speed_;

    std::vector<std::vector<std::complex<float>>> quantum_circuit_;
    std::vector<std::vector<float>> optimization_landscape_;
    std::vector<std::vector<float>> prediction_matrix_;

    std::mt19937 random_engine_;
    std::normal_distribution<float> noise_dist_;
    std::uniform_real_distribution<float> uniform_dist_;
    std::uniform_real_distribution<float> angle_dist_;

    int max_quantum_states_;
    int max_optimization_targets_;
    int max_quantum_algorithms_;
    int quantum_circuit_depth_;
    int optimization_iterations_;
    int prediction_horizon_;

    float quantum_fidelity_;
    float optimization_efficiency_;
    float prediction_confidence_;
    float evolution_stability_;

public:
    Impl()
        : quantum_entanglement_strength_(0.95f)
        , optimization_convergence_rate_(0.85f)
        , prediction_accuracy_(0.0f)
        , evolution_speed_(0.9f)
        , random_engine_(std::random_device{}())
        , noise_dist_(0.0f, 0.01f)
        , uniform_dist_(0.0f, 1.0f)
        , angle_dist_(0.0f, 2.0f * 3.14159f)
        , max_quantum_states_(32)
        , max_optimization_targets_(16)
        , max_quantum_algorithms_(8)
        , quantum_circuit_depth_(64)
        , optimization_iterations_(1000)
        , prediction_horizon_(10)
        , quantum_fidelity_(0.0f)
        , optimization_efficiency_(0.0f)
        , prediction_confidence_(0.0f)
        , evolution_stability_(0.0f) {

        initialize_quantum_states();
        initialize_optimization_targets();
        initialize_quantum_algorithms();

        quantum_circuit_.resize(quantum_circuit_depth_, std::vector<std::complex<float>>(max_quantum_states_));
        optimization_landscape_.resize(max_optimization_targets_, std::vector<float>(optimization_iterations_));
        prediction_matrix_.resize(max_quantum_states_, std::vector<float>(prediction_horizon_));
    }

    ~Impl() {
        active_ = false;
        if (quantum_thread_.joinable()) quantum_thread_.join();
        if (optimization_thread_.joinable()) optimization_thread_.join();
        if (prediction_thread_.joinable()) prediction_thread_.join();
        if (evolution_thread_.joinable()) evolution_thread_.join();
    }

    void initialize_quantum_states() {
        quantum_states_.resize(max_quantum_states_);
        for (auto& state : quantum_states_) {
            int dimension = 2 + (random_engine_() % 6); // 2 to 8 dimensional quantum states
            state.quantum_dimension = dimension;
            state.amplitudes.resize(dimension);
            state.phase_angles.resize(dimension);
            state.coherence_factors.resize(dimension);
            state.entanglement_matrix.resize(dimension, std::vector<std::complex<float>>(dimension));

            // Initialize with random quantum amplitudes
            for (int i = 0; i < dimension; ++i) {
                float magnitude = uniform_dist_(random_engine_);
                float phase = angle_dist_(random_engine_);
                state.amplitudes[i] = std::complex<float>(magnitude * std::cos(phase), magnitude * std::sin(phase));
                state.phase_angles[i] = phase;
                state.coherence_factors[i] = uniform_dist_(random_engine_);
            }

            // Normalize the quantum state
            float norm = 0.0f;
            for (const auto& amp : state.amplitudes) {
                norm += std::norm(amp);
            }
            norm = std::sqrt(norm);
            for (auto& amp : state.amplitudes) {
                amp /= norm;
            }

            // Initialize entanglement matrix
            for (int i = 0; i < dimension; ++i) {
                for (int j = 0; j < dimension; ++j) {
                    float real = uniform_dist_(random_engine_) * 2.0f - 1.0f;
                    float imag = uniform_dist_(random_engine_) * 2.0f - 1.0f;
                    state.entanglement_matrix[i][j] = std::complex<float>(real, imag);
                }
            }

            state.superposition_probability = uniform_dist_(random_engine_);
            state.decoherence_rate = uniform_dist_(random_engine_) * 0.1f;
        }
    }

    void initialize_optimization_targets() {
        optimization_targets_.resize(max_optimization_targets_);
        for (int i = 0; i < max_optimization_targets_; ++i) {
            auto& target = optimization_targets_[i];
            target.name = "OptimizationTarget_" + std::to_string(i);
            target.current_values.resize(8);
            target.optimal_values.resize(8);
            target.constraints.resize(8);

            for (size_t j = 0; j < target.current_values.size(); ++j) {
                target.current_values[j] = uniform_dist_(random_engine_) * 100.0f;
                target.optimal_values[j] = target.current_values[j] + (uniform_dist_(random_engine_) - 0.5f) * 20.0f;
                target.constraints[j] = uniform_dist_(random_engine_) * 10.0f + 5.0f;
            }

            target.optimization_score = 0.0f;
            target.is_optimized = false;
        }
    }

    void initialize_quantum_algorithms() {
        quantum_algorithms_.resize(max_quantum_algorithms_);
        for (auto& algorithm : quantum_algorithms_) {
            algorithm.name = "QuantumAlgorithm_" + std::to_string(&algorithm - &quantum_algorithms_[0]);
            algorithm.states.resize(max_quantum_states_);
            algorithm.unitary_matrices.resize(quantum_circuit_depth_);
            algorithm.measurement_probabilities.resize(max_quantum_states_);

            for (auto& state : algorithm.states) {
                state = quantum_states_[random_engine_() % quantum_states_.size()];
            }

            for (auto& matrix : algorithm.unitary_matrices) {
                matrix.resize(max_quantum_states_ * max_quantum_states_);
                for (auto& element : matrix) {
                    float real = uniform_dist_(random_engine_) * 2.0f - 1.0f;
                    float imag = uniform_dist_(random_engine_) * 2.0f - 1.0f;
                    element = real + imag * 1.0if;
                }
            }

            for (auto& prob : algorithm.measurement_probabilities) {
                prob = uniform_dist_(random_engine_);
            }

            algorithm.algorithm_fidelity = uniform_dist_(random_engine_);
            algorithm.gate_error_rate = uniform_dist_(random_engine_) * 0.01f;
        }
    }

    std::vector<std::complex<float>> quantum_superposition_evolution(const std::vector<std::complex<float>>& initial_state) {
        std::vector<std::complex<float>> evolved_state = initial_state;

        for (const auto& state : quantum_states_) {
            std::vector<std::complex<float>> new_state(evolved_state.size());

            for (size_t i = 0; i < evolved_state.size(); ++i) {
                std::complex<float> sum = 0.0f;

                for (size_t j = 0; j < state.entanglement_matrix.size() && j < evolved_state.size(); ++j) {
                    sum += state.entanglement_matrix[i][j] * evolved_state[j] * state.amplitudes[j % state.amplitudes.size()];
                }

                // Apply quantum phase
                float phase = state.phase_angles[i % state.phase_angles.size()];
                std::complex<float> phase_factor = std::complex<float>(std::cos(phase), std::sin(phase));

                new_state[i] = sum * phase_factor * state.superposition_probability;
                new_state[i] *= (1.0f - state.decoherence_rate);
            }

            evolved_state = new_state;
        }

        // Normalize the evolved state
        float norm = 0.0f;
        for (const auto& amp : evolved_state) {
            norm += std::norm(amp);
        }
        if (norm > 0.0f) {
            norm = std::sqrt(norm);
            for (auto& amp : evolved_state) {
                amp /= norm;
            }
        }

        return evolved_state;
    }

    std::vector<float> quantum_optimization_search(const std::vector<float>& current_parameters) {
        std::vector<float> optimized_parameters = current_parameters;

        for (auto& target : optimization_targets_) {
            if (!target.is_optimized) {
                std::vector<std::complex<float>> quantum_params;
                for (size_t i = 0; i < optimized_parameters.size() && i < target.current_values.size(); ++i) {
                    quantum_params.push_back(std::complex<float>(optimized_parameters[i], target.optimal_values[i]));
                }

                auto evolved_params = quantum_superposition_evolution(quantum_params);

                for (size_t i = 0; i < optimized_parameters.size() && i < evolved_params.size(); ++i) {
                    optimized_parameters[i] = std::real(evolved_params[i]) * (1.0f - optimization_convergence_rate_) +
                                            target.optimal_values[i % target.optimal_values.size()] * optimization_convergence_rate_;
                }

                // Check optimization constraints
                for (size_t i = 0; i < optimized_parameters.size() && i < target.constraints.size(); ++i) {
                    if (optimized_parameters[i] > target.constraints[i]) {
                        optimized_parameters[i] = target.constraints[i];
                    }
                }

                target.optimization_score = calculate_optimization_score(optimized_parameters, target);
                target.is_optimized = target.optimization_score > 0.95f;
            }
        }

        return optimized_parameters;
    }

    std::vector<float> quantum_prediction_forecast(const std::vector<float>& historical_data) {
        std::vector<float> predictions(prediction_horizon_);

        std::vector<std::complex<float>> quantum_history;
        for (size_t i = 0; i < historical_data.size() && i < max_quantum_states_; ++i) {
            quantum_history.push_back(std::complex<float>(historical_data[i], 0.0f));
        }

        auto evolved_history = quantum_superposition_evolution(quantum_history);

        for (int t = 0; t < prediction_horizon_; ++t) {
            std::complex<float> prediction_sum = 0.0f;
            for (size_t i = 0; i < evolved_history.size(); ++i) {
                float phase = 2.0f * 3.14159f * t * (i + 1) / prediction_horizon_;
                std::complex<float> phase_factor = std::complex<float>(std::cos(phase), std::sin(phase));
                prediction_sum += evolved_history[i] * phase_factor;
            }
            predictions[t] = std::real(prediction_sum) / evolved_history.size();
        }

        return predictions;
    }

    float calculate_optimization_score(const std::vector<float>& parameters, const OptimizationTarget& target) {
        float score = 0.0f;
        float total_weight = 0.0f;

        for (size_t i = 0; i < parameters.size() && i < target.optimal_values.size(); ++i) {
            float diff = std::abs(parameters[i] - target.optimal_values[i]);
            float max_diff = std::abs(target.optimal_values[i]) + 1.0f;
            float normalized_diff = diff / max_diff;

            float weight = 1.0f / (1.0f + normalized_diff);
            score += weight;
            total_weight += 1.0f;
        }

        return total_weight > 0.0f ? score / total_weight : 0.0f;
    }

    void quantum_algorithm_evolution() {
        for (auto& algorithm : quantum_algorithms_) {
            // Evolve quantum states
            for (auto& state : algorithm.states) {
                auto evolved = quantum_superposition_evolution(state.amplitudes);
                state.amplitudes = evolved;

                // Update phase angles based on evolution
                for (size_t i = 0; i < state.phase_angles.size(); ++i) {
                    state.phase_angles[i] += evolution_speed_ * uniform_dist_(random_engine_) * 0.1f;
                    state.phase_angles[i] = std::fmod(state.phase_angles[i], 2.0f * 3.14159f);
                }

                // Update coherence factors
                for (auto& coherence : state.coherence_factors) {
                    coherence *= (1.0f - state.decoherence_rate);
                    coherence += evolution_speed_ * uniform_dist_(random_engine_) * 0.01f;
                    coherence = std::max(0.0f, std::min(1.0f, coherence));
                }
            }

            // Evolve unitary matrices
            for (auto& matrix : algorithm.unitary_matrices) {
                for (auto& element : matrix) {
                    element += evolution_speed_ * std::complex<float>(
                        noise_dist_(random_engine_), noise_dist_(random_engine_)
                    ) * 0.01f;
                }
            }

            // Update measurement probabilities
            float prob_sum = 0.0f;
            for (auto& prob : algorithm.measurement_probabilities) {
                prob += evolution_speed_ * noise_dist_(random_engine_) * 0.01f;
                prob = std::max(0.0f, prob);
                prob_sum += prob;
            }

            // Normalize probabilities
            if (prob_sum > 0.0f) {
                for (auto& prob : algorithm.measurement_probabilities) {
                    prob /= prob_sum;
                }
            }

            // Update algorithm metrics
            algorithm.algorithm_fidelity *= (1.0f - evolution_speed_ * 0.001f);
            algorithm.algorithm_fidelity += evolution_speed_ * uniform_dist_(random_engine_) * 0.01f;
            algorithm.algorithm_fidelity = std::max(0.0f, std::min(1.0f, algorithm.algorithm_fidelity));

            algorithm.gate_error_rate *= (1.0f - evolution_speed_ * 0.01f);
            algorithm.gate_error_rate += evolution_speed_ * uniform_dist_(random_engine_) * 0.001f;
            algorithm.gate_error_rate = std::max(0.0f, std::min(0.1f, algorithm.gate_error_rate));
        }
    }

    void update_quantum_fidelity() {
        float total_fidelity = 0.0f;
        for (const auto& algorithm : quantum_algorithms_) {
            total_fidelity += algorithm.algorithm_fidelity;
        }
        quantum_fidelity_ = total_fidelity / quantum_algorithms_.size();
    }

    void update_optimization_efficiency() {
        int optimized_count = 0;
        for (const auto& target : optimization_targets_) {
            if (target.is_optimized) {
                optimized_count++;
            }
        }
        optimization_efficiency_ = static_cast<float>(optimized_count) / optimization_targets_.size();
    }

    void update_prediction_confidence() {
        prediction_confidence_ = uniform_dist_(random_engine_) * 0.1f + 0.85f;
    }

    void update_evolution_stability() {
        float total_stability = 0.0f;
        for (const auto& state : quantum_states_) {
            float state_stability = 1.0f;
            for (float coherence : state.coherence_factors) {
                state_stability *= coherence;
            }
            total_stability += state_stability;
        }
        evolution_stability_ = total_stability / quantum_states_.size();
    }
};

QuantumOptimizationEngine::QuantumOptimizationEngine() : impl_(std::make_unique<Impl>()) {}

QuantumOptimizationEngine::~QuantumOptimizationEngine() = default;

bool QuantumOptimizationEngine::initialize(VkDevice device, VkPhysicalDevice physical_device) {
    impl_->device_ = device;
    impl_->physical_device_ = physical_device;

    if (!create_vulkan_resources()) {
        return false;
    }

    impl_->quantum_thread_ = std::thread([this]() {
        quantum_evolution_loop();
    });

    impl_->optimization_thread_ = std::thread([this]() {
        optimization_loop();
    });

    impl_->prediction_thread_ = std::thread([this]() {
        prediction_loop();
    });

    impl_->evolution_thread_ = std::thread([this]() {
        algorithm_evolution_loop();
    });

    return true;
}

void QuantumOptimizationEngine::shutdown() {
    impl_->active_ = false;
    if (impl_->quantum_thread_.joinable()) impl_->quantum_thread_.join();
    if (impl_->optimization_thread_.joinable()) impl_->optimization_thread_.join();
    if (impl_->prediction_thread_.joinable()) impl_->prediction_thread_.join();
    if (impl_->evolution_thread_.joinable()) impl_->evolution_thread_.join();

    cleanup_vulkan_resources();
}

bool QuantumOptimizationEngine::create_vulkan_resources() {
    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = 0;

    if (vkCreateCommandPool(impl_->device_, &pool_info, nullptr, &impl_->command_pool_) != VK_SUCCESS) {
        return false;
    }

    VkDescriptorPoolCreateInfo descriptor_pool_info = {};
    descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_pool_info.maxSets = 100;
    descriptor_pool_info.poolSizeCount = 0;

    if (vkCreateDescriptorPool(impl_->device_, &descriptor_pool_info, nullptr, &impl_->descriptor_pool_) != VK_SUCCESS) {
        return false;
    }

    return create_quantum_buffers();
}

bool QuantumOptimizationEngine::create_quantum_buffers() {
    size_t quantum_buffer_size = impl_->max_quantum_states_ * 1024;
    size_t optimization_buffer_size = impl_->max_optimization_targets_ * 512;
    size_t prediction_buffer_size = impl_->max_quantum_states_ * impl_->prediction_horizon_ * 4;

    if (!create_buffer(quantum_buffer_size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                      &impl_->quantum_buffer_, &impl_->quantum_memory_)) {
        return false;
    }

    if (!create_buffer(optimization_buffer_size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                      &impl_->optimization_buffer_, &impl_->optimization_memory_)) {
        return false;
    }

    if (!create_buffer(prediction_buffer_size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                      &impl_->prediction_buffer_, &impl_->prediction_memory_)) {
        return false;
    }

    return true;
}

bool QuantumOptimizationEngine::create_buffer(size_t size, VkBufferUsageFlags usage, VkBuffer* buffer, VkDeviceMemory* memory) {
    VkBufferCreateInfo buffer_info = {};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(impl_->device_, &buffer_info, nullptr, buffer) != VK_SUCCESS) {
        return false;
    }

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(impl_->device_, *buffer, &mem_requirements);

    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(impl_->physical_device_, &mem_properties);

    u32 memory_type = UINT32_MAX;
    for (u32 i = 0; i < mem_properties.memoryTypeCount; i++) {
        if ((mem_requirements.memoryTypeBits & (1 << i)) &&
            (mem_properties.memoryTypes[i].propertyFlags &
             (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))) {
            memory_type = i;
            break;
        }
    }

    if (memory_type == UINT32_MAX) {
        return false;
    }

    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = memory_type;

    if (vkAllocateMemory(impl_->device_, &alloc_info, nullptr, memory) != VK_SUCCESS) {
        return false;
    }

    return vkBindBufferMemory(impl_->device_, *buffer, *memory, 0) == VK_SUCCESS;
}

void QuantumOptimizationEngine::quantum_evolution_loop() {
    while (impl_->active_) {
        std::vector<std::complex<float>> initial_state(impl_->max_quantum_states_);
        for (auto& state : initial_state) {
            state = std::complex<float>(impl_->uniform_dist_(impl_->random_engine_),
                                      impl_->uniform_dist_(impl_->random_engine_));
        }

        auto evolved_state = impl_->quantum_superposition_evolution(initial_state);

        for (size_t i = 0; i < evolved_state.size() && i < impl_->quantum_circuit_.size(); ++i) {
            for (size_t j = 0; j < evolved_state.size() && j < impl_->quantum_circuit_[i].size(); ++j) {
                impl_->quantum_circuit_[i][j] = evolved_state[j] * impl_->quantum_entanglement_strength_;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

void QuantumOptimizationEngine::optimization_loop() {
    while (impl_->active_) {
        std::vector<float> current_params(8);
        for (auto& param : current_params) {
            param = impl_->uniform_dist_(impl_->random_engine_) * 100.0f;
        }

        auto optimized_params = impl_->quantum_optimization_search(current_params);

        for (size_t i = 0; i < optimized_params.size() && i < impl_->optimization_landscape_.size(); ++i) {
            for (size_t j = 0; j < optimized_params.size() && j < impl_->optimization_landscape_[i].size(); ++j) {
                impl_->optimization_landscape_[i][j] = optimized_params[j] * impl_->optimization_convergence_rate_;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void QuantumOptimizationEngine::prediction_loop() {
    while (impl_->active_) {
        std::vector<float> historical_data(impl_->max_quantum_states_);
        for (auto& data : historical_data) {
            data = impl_->uniform_dist_(impl_->random_engine_) * 50.0f + 25.0f;
        }

        auto predictions = impl_->quantum_prediction_forecast(historical_data);

        for (size_t i = 0; i < predictions.size() && i < impl_->prediction_matrix_.size(); ++i) {
            for (size_t j = 0; j < predictions.size() && j < impl_->prediction_matrix_[i].size(); ++j) {
                impl_->prediction_matrix_[i][j] = predictions[j] * impl_->prediction_accuracy_;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void QuantumOptimizationEngine::algorithm_evolution_loop() {
    while (impl_->active_) {
        impl_->quantum_algorithm_evolution();
        impl_->update_quantum_fidelity();
        impl_->update_optimization_efficiency();
        impl_->update_prediction_confidence();
        impl_->update_evolution_stability();

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void QuantumOptimizationEngine::cleanup_vulkan_resources() {
    if (impl_->device_ != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(impl_->device_);

        if (impl_->quantum_buffer_ != VK_NULL_HANDLE) {
            vkDestroyBuffer(impl_->device_, impl_->quantum_buffer_, nullptr);
        }
        if (impl_->optimization_buffer_ != VK_NULL_HANDLE) {
            vkDestroyBuffer(impl_->device_, impl_->optimization_buffer_, nullptr);
        }
        if (impl_->prediction_buffer_ != VK_NULL_HANDLE) {
            vkDestroyBuffer(impl_->device_, impl_->prediction_buffer_, nullptr);
        }

        if (impl_->quantum_memory_ != VK_NULL_HANDLE) {
            vkFreeMemory(impl_->device_, impl_->quantum_memory_, nullptr);
        }
        if (impl_->optimization_memory_ != VK_NULL_HANDLE) {
            vkFreeMemory(impl_->device_, impl_->optimization_memory_, nullptr);
        }
        if (impl_->prediction_memory_ != VK_NULL_HANDLE) {
            vkFreeMemory(impl_->device_, impl_->prediction_memory_, nullptr);
        }

        if (impl_->descriptor_pool_ != VK_NULL_HANDLE) {
            vkDestroyDescriptorPool(impl_->device_, impl_->descriptor_pool_, nullptr);
        }

        if (impl_->command_pool_ != VK_NULL_HANDLE) {
            vkDestroyCommandPool(impl_->device_, impl_->command_pool_, nullptr);
        }
    }
}

} // namespace s1u

