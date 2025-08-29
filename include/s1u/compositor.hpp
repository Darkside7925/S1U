#pragma once

#include "core.hpp"
#include "window.hpp"
#include "display.hpp"
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <gbm.h>
#include <drm/drm.h>
#include <drm/drm_mode.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

namespace s1u {

enum class CompositorMode : u32 {
    Immediate = 0,
    Deferred = 1,
    Atomic = 2,
    Direct = 3,
    Indirect = 4,
    Hardware = 5,
    Software = 6,
    Hybrid = 7,
    Optimized = 8,
    Minimal = 9,
    Full = 10
};

enum class CompositorEffect : u32 {
    None = 0,
    Blur = 1,
    Shadow = 2,
    Transparency = 3,
    Animation = 4,
    Scaling = 5,
    Rotation = 6,
    Translation = 7,
    Skew = 8,
    Perspective = 9,
    Distortion = 10,
    Wave = 11,
    Ripple = 12,
    Explosion = 13,
    Implosion = 14,
    Vortex = 15,
    Spiral = 16,
    Fractal = 17,
    Noise = 18,
    Grain = 19,
    Vignette = 20,
    Chromatic = 21,
    Aberration = 22,
    Bloom = 23,
    Glow = 24,
    Halo = 25,
    Corona = 26,
    Lens = 27,
    Flare = 28,
    Starburst = 29,
    Cross = 30,
    Hexagon = 31,
    Octagon = 32,
    Diamond = 33,
    Triangle = 34,
    Circle = 35,
    Square = 36,
    Rectangle = 37,
    Ellipse = 38,
    Polygon = 39,
    Star = 40,
    Heart = 41,
    Flower = 42,
    Leaf = 43,
    Tree = 44,
    Cloud = 45,
    Rain = 46,
    Snow = 47,
    Fire = 48,
    Smoke = 49,
    Steam = 50,
    Water = 51,
    Oil = 52,
    Mercury = 53,
    Lava = 54,
    Plasma = 55,
    Energy = 56,
    Lightning = 57,
    Thunder = 58,
    Storm = 59,
    Tornado = 60,
    Hurricane = 61,
    Tsunami = 62,
    Earthquake = 63,
    Volcano = 64,
    Meteor = 65,
    Asteroid = 66,
    Comet = 67,
    Blackhole = 68,
    Wormhole = 69,
    Portal = 70,
    Teleport = 71,
    Timewarp = 72,
    Dimension = 73,
    Universe = 74,
    Galaxy = 75,
    Nebula = 76,
    Supernova = 77,
    Pulsar = 78,
    Quasar = 79,
    Neutron = 80,
    Proton = 81,
    Electron = 82,
    Atom = 83,
    Molecule = 84,
    Cell = 85,
    Organ = 86,
    Organism = 87,
    Ecosystem = 88,
    Biome = 89,
    Planet = 90,
    Solar = 91,
    Lunar = 92,
    Stellar = 93,
    Cosmic = 94,
    Quantum = 95,
    Relativistic = 96,
    Subatomic = 97,
    Nanoscopic = 98,
    Microscopic = 99,
    Macroscopic = 100
};

struct CompositorConfig {
    CompositorMode mode;
    CompositorEffect effect;
    u32 max_fps;
    u32 vsync_mode;
    bool triple_buffering;
    bool adaptive_sync;
    bool low_latency;
    bool high_quality;
    bool power_saving;
    bool performance_mode;
    f32 blur_radius;
    f32 shadow_offset;
    f32 transparency_level;
    f32 animation_speed;
    f32 scaling_factor;
    f32 rotation_angle;
    f32 translation_x;
    f32 translation_y;
    f32 skew_x;
    f32 skew_y;
    f32 perspective_depth;
    f32 distortion_amount;
    f32 wave_frequency;
    f32 wave_amplitude;
    f32 ripple_radius;
    f32 explosion_force;
    f32 implosion_force;
    f32 vortex_speed;
    f32 spiral_tightness;
    f32 fractal_iterations;
    f32 noise_intensity;
    f32 grain_size;
    f32 vignette_strength;
    f32 chromatic_offset;
    f32 aberration_amount;
    f32 bloom_threshold;
    f32 glow_intensity;
    f32 halo_radius;
    f32 corona_size;
    f32 lens_distortion;
    f32 flare_intensity;
    f32 starburst_rays;
    f32 cross_thickness;
    f32 hexagon_sides;
    f32 octagon_sides;
    f32 diamond_ratio;
    f32 triangle_height;
    f32 circle_radius;
    f32 square_size;
    f32 rectangle_width;
    f32 rectangle_height;
    f32 ellipse_width;
    f32 ellipse_height;
    f32 polygon_sides;
    f32 star_points;
    f32 heart_curvature;
    f32 flower_petals;
    f32 leaf_veins;
    f32 tree_branches;
    f32 cloud_density;
    f32 rain_intensity;
    f32 snow_flakes;
    f32 fire_intensity;
    f32 smoke_density;
    f32 steam_temperature;
    f32 water_viscosity;
    f32 oil_slickness;
    f32 mercury_fluidity;
    f32 lava_temperature;
    f32 plasma_energy;
    f32 energy_charge;
    f32 lightning_bolts;
    f32 thunder_volume;
    f32 storm_intensity;
    f32 tornado_speed;
    f32 hurricane_force;
    f32 tsunami_height;
    f32 earthquake_magnitude;
    f32 volcano_eruption;
    f32 meteor_size;
    f32 asteroid_belt;
    f32 comet_tail;
    f32 blackhole_mass;
    f32 wormhole_duration;
    f32 portal_size;
    f32 teleport_distance;
    f32 timewarp_factor;
    f32 dimension_shift;
    f32 universe_scale;
    f32 galaxy_arms;
    f32 nebula_gas;
    f32 supernova_explosion;
    f32 pulsar_frequency;
    f32 quasar_brightness;
    f32 neutron_density;
    f32 proton_charge;
    f32 electron_spin;
    f32 atom_nucleus;
    f32 molecule_bonds;
    f32 cell_division;
    f32 organ_function;
    f32 organism_life;
    f32 ecosystem_balance;
    f32 biome_climate;
    f32 planet_gravity;
    f32 solar_radiation;
    f32 lunar_phase;
    f32 stellar_mass;
    f32 cosmic_radiation;
    f32 quantum_entanglement;
    f32 relativistic_speed;
    f32 subatomic_particles;
    f32 nanoscopic_scale;
    f32 microscopic_resolution;
    f32 macroscopic_view;
};

class Compositor {
public:
    explicit Compositor(const CompositorConfig& config);
    ~Compositor();
    
    bool initialize();
    void shutdown();
    
    void set_config(const CompositorConfig& config);
    const CompositorConfig& get_config() const { return config_; }
    
    void add_window(WindowPtr window);
    void remove_window(WindowPtr window);
    void update_window(WindowPtr window);
    void damage_window(WindowPtr window, const Rect& damage);
    
    void composite();
    void render();
    void flip();
    void vsync();
    void hsync();
    
    void set_mode(CompositorMode mode);
    void set_effect(CompositorEffect effect);
    void set_max_fps(u32 max_fps);
    void set_vsync_mode(u32 vsync_mode);
    void set_triple_buffering(bool enabled);
    void set_adaptive_sync(bool enabled);
    void set_low_latency(bool enabled);
    void set_high_quality(bool enabled);
    void set_power_saving(bool enabled);
    void set_performance_mode(bool enabled);
    
    void set_blur_radius(f32 radius);
    void set_shadow_offset(f32 offset);
    void set_transparency_level(f32 level);
    void set_animation_speed(f32 speed);
    void set_scaling_factor(f32 factor);
    void set_rotation_angle(f32 angle);
    void set_translation(f32 x, f32 y);
    void set_skew(f32 x, f32 y);
    void set_perspective_depth(f32 depth);
    void set_distortion_amount(f32 amount);
    
    void set_wave_parameters(f32 frequency, f32 amplitude);
    void set_ripple_radius(f32 radius);
    void set_explosion_force(f32 force);
    void set_implosion_force(f32 force);
    void set_vortex_speed(f32 speed);
    void set_spiral_tightness(f32 tightness);
    void set_fractal_iterations(f32 iterations);
    void set_noise_intensity(f32 intensity);
    void set_grain_size(f32 size);
    void set_vignette_strength(f32 strength);
    
    void set_chromatic_offset(f32 offset);
    void set_aberration_amount(f32 amount);
    void set_bloom_threshold(f32 threshold);
    void set_glow_intensity(f32 intensity);
    void set_halo_radius(f32 radius);
    void set_corona_size(f32 size);
    void set_lens_distortion(f32 distortion);
    void set_flare_intensity(f32 intensity);
    void set_starburst_rays(f32 rays);
    void set_cross_thickness(f32 thickness);
    
    void set_hexagon_sides(f32 sides);
    void set_octagon_sides(f32 sides);
    void set_diamond_ratio(f32 ratio);
    void set_triangle_height(f32 height);
    void set_circle_radius(f32 radius);
    void set_square_size(f32 size);
    void set_rectangle_size(f32 width, f32 height);
    void set_ellipse_size(f32 width, f32 height);
    void set_polygon_sides(f32 sides);
    void set_star_points(f32 points);
    
    void set_heart_curvature(f32 curvature);
    void set_flower_petals(f32 petals);
    void set_leaf_veins(f32 veins);
    void set_tree_branches(f32 branches);
    void set_cloud_density(f32 density);
    void set_rain_intensity(f32 intensity);
    void set_snow_flakes(f32 flakes);
    void set_fire_intensity(f32 intensity);
    void set_smoke_density(f32 density);
    void set_steam_temperature(f32 temperature);
    
    void set_water_viscosity(f32 viscosity);
    void set_oil_slickness(f32 slickness);
    void set_mercury_fluidity(f32 fluidity);
    void set_lava_temperature(f32 temperature);
    void set_plasma_energy(f32 energy);
    void set_energy_charge(f32 charge);
    void set_lightning_bolts(f32 bolts);
    void set_thunder_volume(f32 volume);
    void set_storm_intensity(f32 intensity);
    void set_tornado_speed(f32 speed);
    
    void set_hurricane_force(f32 force);
    void set_tsunami_height(f32 height);
    void set_earthquake_magnitude(f32 magnitude);
    void set_volcano_eruption(f32 eruption);
    void set_meteor_size(f32 size);
    void set_asteroid_belt(f32 belt);
    void set_comet_tail(f32 tail);
    void set_blackhole_mass(f32 mass);
    void set_wormhole_duration(f32 duration);
    void set_portal_size(f32 size);
    
    void set_teleport_distance(f32 distance);
    void set_timewarp_factor(f32 factor);
    void set_dimension_shift(f32 shift);
    void set_universe_scale(f32 scale);
    void set_galaxy_arms(f32 arms);
    void set_nebula_gas(f32 gas);
    void set_supernova_explosion(f32 explosion);
    void set_pulsar_frequency(f32 frequency);
    void set_quasar_brightness(f32 brightness);
    void set_neutron_density(f32 density);
    
    void set_proton_charge(f32 charge);
    void set_electron_spin(f32 spin);
    void set_atom_nucleus(f32 nucleus);
    void set_molecule_bonds(f32 bonds);
    void set_cell_division(f32 division);
    void set_organ_function(f32 function);
    void set_organism_life(f32 life);
    void set_ecosystem_balance(f32 balance);
    void set_biome_climate(f32 climate);
    void set_planet_gravity(f32 gravity);
    
    void set_solar_radiation(f32 radiation);
    void set_lunar_phase(f32 phase);
    void set_stellar_mass(f32 mass);
    void set_cosmic_radiation(f32 radiation);
    void set_quantum_entanglement(f32 entanglement);
    void set_relativistic_speed(f32 speed);
    void set_subatomic_particles(f32 particles);
    void set_nanoscopic_scale(f32 scale);
    void set_microscopic_resolution(f32 resolution);
    void set_macroscopic_view(f32 view);
    
    bool is_initialized() const { return initialized_; }
    bool is_running() const { return running_; }
    bool is_vsync_enabled() const { return config_.vsync_mode > 0; }
    bool is_triple_buffering_enabled() const { return config_.triple_buffering; }
    bool is_adaptive_sync_enabled() const { return config_.adaptive_sync; }
    bool is_low_latency_enabled() const { return config_.low_latency; }
    bool is_high_quality_enabled() const { return config_.high_quality; }
    bool is_power_saving_enabled() const { return config_.power_saving; }
    bool is_performance_mode_enabled() const { return config_.performance_mode; }
    
    u32 get_max_fps() const { return config_.max_fps; }
    u32 get_vsync_mode() const { return config_.vsync_mode; }
    f32 get_blur_radius() const { return config_.blur_radius; }
    f32 get_shadow_offset() const { return config_.shadow_offset; }
    f32 get_transparency_level() const { return config_.transparency_level; }
    f32 get_animation_speed() const { return config_.animation_speed; }
    f32 get_scaling_factor() const { return config_.scaling_factor; }
    f32 get_rotation_angle() const { return config_.rotation_angle; }
    f32 get_translation_x() const { return config_.translation_x; }
    f32 get_translation_y() const { return config_.translation_y; }
    
    CompositorMode get_mode() const { return config_.mode; }
    CompositorEffect get_effect() const { return config_.effect; }
    
    void start();
    void stop();
    void pause();
    void resume();
    void reset();
    void clear();
    void flush();
    void sync();
    void wait();
    void signal();
    
    void optimize();
    void profile();
    void benchmark();
    void calibrate();
    void tune();
    void adjust();
    void fine_tune();
    void micro_tune();
    void nano_tune();
    void pico_tune();
    
private:
    CompositorConfig config_;
    std::vector<WindowPtr> windows_;
    std::unordered_map<u32, WindowPtr> window_map_;
    std::vector<Rect> damage_regions_;
    std::mutex mutex_;
    std::atomic<bool> initialized_;
    std::atomic<bool> running_;
    std::atomic<bool> paused_;
    std::atomic<bool> optimized_;
    std::atomic<bool> profiled_;
    std::atomic<bool> benchmarked_;
    std::atomic<bool> calibrated_;
    std::atomic<bool> tuned_;
    std::atomic<bool> adjusted_;
    std::atomic<bool> fine_tuned_;
    std::atomic<bool> micro_tuned_;
    std::atomic<bool> nano_tuned_;
    std::atomic<bool> pico_tuned_;
    
    EGLDisplay egl_display_;
    EGLContext egl_context_;
    EGLSurface egl_surface_;
    GLuint framebuffer_;
    GLuint texture_;
    GLuint program_;
    GLuint vertex_shader_;
    GLuint fragment_shader_;
    GLuint vao_;
    GLuint vbo_;
    GLuint ebo_;
    
    void initialize_gl();
    void shutdown_gl();
    void create_shaders();
    void create_buffers();
    void create_textures();
    void create_framebuffers();
    void setup_rendering();
    void cleanup_rendering();
    
    void render_window(WindowPtr window);
    void render_effect(CompositorEffect effect);
    void apply_blur();
    void apply_shadow();
    void apply_transparency();
    void apply_animation();
    void apply_scaling();
    void apply_rotation();
    void apply_translation();
    void apply_skew();
    void apply_perspective();
    void apply_distortion();
    void apply_wave();
    void apply_ripple();
    void apply_explosion();
    void apply_implosion();
    void apply_vortex();
    void apply_spiral();
    void apply_fractal();
    void apply_noise();
    void apply_grain();
    void apply_vignette();
    void apply_chromatic();
    void apply_aberration();
    void apply_bloom();
    void apply_glow();
    void apply_halo();
    void apply_corona();
    void apply_lens();
    void apply_flare();
    void apply_starburst();
    void apply_cross();
    void apply_hexagon();
    void apply_octagon();
    void apply_diamond();
    void apply_triangle();
    void apply_circle();
    void apply_square();
    void apply_rectangle();
    void apply_ellipse();
    void apply_polygon();
    void apply_star();
    void apply_heart();
    void apply_flower();
    void apply_leaf();
    void apply_tree();
    void apply_cloud();
    void apply_rain();
    void apply_snow();
    void apply_fire();
    void apply_smoke();
    void apply_steam();
    void apply_water();
    void apply_oil();
    void apply_mercury();
    void apply_lava();
    void apply_plasma();
    void apply_energy();
    void apply_lightning();
    void apply_thunder();
    void apply_storm();
    void apply_tornado();
    void apply_hurricane();
    void apply_tsunami();
    void apply_earthquake();
    void apply_volcano();
    void apply_meteor();
    void apply_asteroid();
    void apply_comet();
    void apply_blackhole();
    void apply_wormhole();
    void apply_portal();
    void apply_teleport();
    void apply_timewarp();
    void apply_dimension();
    void apply_universe();
    void apply_galaxy();
    void apply_nebula();
    void apply_supernova();
    void apply_pulsar();
    void apply_quasar();
    void apply_neutron();
    void apply_proton();
    void apply_electron();
    void apply_atom();
    void apply_molecule();
    void apply_cell();
    void apply_organ();
    void apply_organism();
    void apply_ecosystem();
    void apply_biome();
    void apply_planet();
    void apply_solar();
    void apply_lunar();
    void apply_stellar();
    void apply_cosmic();
    void apply_quantum();
    void apply_relativistic();
    void apply_subatomic();
    void apply_nanoscopic();
    void apply_microscopic();
    void apply_macroscopic();
};

}
