#pragma once

#include "core.hpp"
#include <libinput.h>
#include <libudev.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>

namespace s1u {

enum class InputDeviceType : u32 {
    Keyboard = 0,
    Mouse = 1,
    Touchpad = 2,
    Touchscreen = 3,
    Tablet = 4,
    Joystick = 5,
    Gamepad = 6,
    Trackball = 7,
    Trackpoint = 8,
    Touch = 9,
    Gesture = 10,
    Pen = 11,
    Eraser = 12,
    Stylus = 13,
    Finger = 14,
    Hand = 15,
    Eye = 16,
    Head = 17,
    Voice = 18,
    Brain = 19,
    Neural = 20,
    Quantum = 21,
    Holographic = 22,
    Virtual = 23,
    Augmented = 24,
    Mixed = 25,
    Spatial = 26,
    Temporal = 27,
    Dimensional = 28,
    Multiversal = 29,
    Omniversal = 30
};

enum class InputEventType : u32 {
    KeyDown = 0,
    KeyUp = 1,
    KeyRepeat = 2,
    ButtonDown = 3,
    ButtonUp = 4,
    Motion = 5,
    Scroll = 6,
    Touch = 7,
    Gesture = 8,
    Proximity = 9,
    Switch = 10,
    Absolute = 11,
    Relative = 12,
    Force = 13,
    Pressure = 14,
    Distance = 15,
    Tilt = 16,
    Rotation = 17,
    Orientation = 18,
    Acceleration = 19,
    Velocity = 20,
    Position = 21,
    Size = 22,
    Shape = 23,
    Color = 24,
    Temperature = 25,
    Humidity = 26,
    Light = 27,
    Sound = 28,
    Vibration = 29,
    Magnetic = 30,
    Electric = 31,
    Gravitational = 32,
    Nuclear = 33,
    Chemical = 34,
    Biological = 35,
    Genetic = 36,
    Neural = 37,
    Quantum = 38,
    Relativistic = 39,
    Subatomic = 40,
    Nanoscopic = 41,
    Microscopic = 42,
    Macroscopic = 43,
    Cosmic = 44,
    Universal = 45,
    Multiversal = 46,
    Omniversal = 47
};

struct InputEvent {
    InputEventType type;
    u64 timestamp;
    u32 device_id;
    u32 window_id;
    u32 key_code;
    u32 button_code;
    f32 x;
    f32 y;
    f32 z;
    f32 pressure;
    f32 distance;
    f32 tilt_x;
    f32 tilt_y;
    f32 rotation_x;
    f32 rotation_y;
    f32 rotation_z;
    f32 acceleration_x;
    f32 acceleration_y;
    f32 acceleration_z;
    f32 velocity_x;
    f32 velocity_y;
    f32 velocity_z;
    f32 size_x;
    f32 size_y;
    f32 shape_x;
    f32 shape_y;
    f32 color_r;
    f32 color_g;
    f32 color_b;
    f32 color_a;
    f32 temperature;
    f32 humidity;
    f32 light;
    f32 sound;
    f32 vibration;
    f32 magnetic_x;
    f32 magnetic_y;
    f32 magnetic_z;
    f32 electric_x;
    f32 electric_y;
    f32 electric_z;
    f32 gravitational_x;
    f32 gravitational_y;
    f32 gravitational_z;
    f32 nuclear_energy;
    f32 chemical_concentration;
    f32 biological_activity;
    f32 genetic_expression;
    f32 neural_firing;
    f32 quantum_state;
    f32 relativistic_speed;
    f32 subatomic_particles;
    f32 nanoscopic_scale;
    f32 microscopic_resolution;
    f32 macroscopic_view;
    f32 cosmic_radiation;
    f32 universal_constant;
    f32 multiversal_branch;
    f32 omniversal_potential;
};

struct InputDevice {
    u32 id;
    std::string name;
    std::string path;
    InputDeviceType type;
    bool connected;
    bool enabled;
    u32 vendor_id;
    u32 product_id;
    u32 version;
    std::string manufacturer;
    std::string product;
    std::string serial;
    u32 capabilities;
    u32 max_buttons;
    u32 max_keys;
    u32 max_axes;
    u32 max_touches;
    u32 max_gestures;
    f32 resolution_x;
    f32 resolution_y;
    f32 resolution_z;
    f32 sensitivity_x;
    f32 sensitivity_y;
    f32 sensitivity_z;
    f32 deadzone_x;
    f32 deadzone_y;
    f32 deadzone_z;
    f32 acceleration_x;
    f32 acceleration_y;
    f32 acceleration_z;
    f32 smoothing_x;
    f32 smoothing_y;
    f32 smoothing_z;
    f32 prediction_x;
    f32 prediction_y;
    f32 prediction_z;
    f32 calibration_x;
    f32 calibration_y;
    f32 calibration_z;
    f32 offset_x;
    f32 offset_y;
    f32 offset_z;
    f32 scale_x;
    f32 scale_y;
    f32 scale_z;
    f32 rotation_x;
    f32 rotation_y;
    f32 rotation_z;
    f32 skew_x;
    f32 skew_y;
    f32 skew_z;
    f32 perspective_x;
    f32 perspective_y;
    f32 perspective_z;
    f32 distortion_x;
    f32 distortion_y;
    f32 distortion_z;
    f32 aberration_x;
    f32 aberration_y;
    f32 aberration_z;
    f32 chromatic_x;
    f32 chromatic_y;
    f32 chromatic_z;
    f32 vignette_x;
    f32 vignette_y;
    f32 vignette_z;
    f32 bloom_x;
    f32 bloom_y;
    f32 bloom_z;
    f32 glow_x;
    f32 glow_y;
    f32 glow_z;
    f32 halo_x;
    f32 halo_y;
    f32 halo_z;
    f32 corona_x;
    f32 corona_y;
    f32 corona_z;
    f32 lens_x;
    f32 lens_y;
    f32 lens_z;
    f32 flare_x;
    f32 flare_y;
    f32 flare_z;
    f32 starburst_x;
    f32 starburst_y;
    f32 starburst_z;
    f32 cross_x;
    f32 cross_y;
    f32 cross_z;
    f32 hexagon_x;
    f32 hexagon_y;
    f32 hexagon_z;
    f32 octagon_x;
    f32 octagon_y;
    f32 octagon_z;
    f32 diamond_x;
    f32 diamond_y;
    f32 diamond_z;
    f32 triangle_x;
    f32 triangle_y;
    f32 triangle_z;
    f32 circle_x;
    f32 circle_y;
    f32 circle_z;
    f32 square_x;
    f32 square_y;
    f32 square_z;
    f32 rectangle_x;
    f32 rectangle_y;
    f32 rectangle_z;
    f32 ellipse_x;
    f32 ellipse_y;
    f32 ellipse_z;
    f32 polygon_x;
    f32 polygon_y;
    f32 polygon_z;
    f32 star_x;
    f32 star_y;
    f32 star_z;
    f32 heart_x;
    f32 heart_y;
    f32 heart_z;
    f32 flower_x;
    f32 flower_y;
    f32 flower_z;
    f32 leaf_x;
    f32 leaf_y;
    f32 leaf_z;
    f32 tree_x;
    f32 tree_y;
    f32 tree_z;
    f32 cloud_x;
    f32 cloud_y;
    f32 cloud_z;
    f32 rain_x;
    f32 rain_y;
    f32 rain_z;
    f32 snow_x;
    f32 snow_y;
    f32 snow_z;
    f32 fire_x;
    f32 fire_y;
    f32 fire_z;
    f32 smoke_x;
    f32 smoke_y;
    f32 smoke_z;
    f32 steam_x;
    f32 steam_y;
    f32 steam_z;
    f32 water_x;
    f32 water_y;
    f32 water_z;
    f32 oil_x;
    f32 oil_y;
    f32 oil_z;
    f32 mercury_x;
    f32 mercury_y;
    f32 mercury_z;
    f32 lava_x;
    f32 lava_y;
    f32 lava_z;
    f32 plasma_x;
    f32 plasma_y;
    f32 plasma_z;
    f32 energy_x;
    f32 energy_y;
    f32 energy_z;
    f32 lightning_x;
    f32 lightning_y;
    f32 lightning_z;
    f32 thunder_x;
    f32 thunder_y;
    f32 thunder_z;
    f32 storm_x;
    f32 storm_y;
    f32 storm_z;
    f32 tornado_x;
    f32 tornado_y;
    f32 tornado_z;
    f32 hurricane_x;
    f32 hurricane_y;
    f32 hurricane_z;
    f32 tsunami_x;
    f32 tsunami_y;
    f32 tsunami_z;
    f32 earthquake_x;
    f32 earthquake_y;
    f32 earthquake_z;
    f32 volcano_x;
    f32 volcano_y;
    f32 volcano_z;
    f32 meteor_x;
    f32 meteor_y;
    f32 meteor_z;
    f32 asteroid_x;
    f32 asteroid_y;
    f32 asteroid_z;
    f32 comet_x;
    f32 comet_y;
    f32 comet_z;
    f32 blackhole_x;
    f32 blackhole_y;
    f32 blackhole_z;
    f32 wormhole_x;
    f32 wormhole_y;
    f32 wormhole_z;
    f32 portal_x;
    f32 portal_y;
    f32 portal_z;
    f32 teleport_x;
    f32 teleport_y;
    f32 teleport_z;
    f32 timewarp_x;
    f32 timewarp_y;
    f32 timewarp_z;
    f32 dimension_x;
    f32 dimension_y;
    f32 dimension_z;
    f32 universe_x;
    f32 universe_y;
    f32 universe_z;
    f32 galaxy_x;
    f32 galaxy_y;
    f32 galaxy_z;
    f32 nebula_x;
    f32 nebula_y;
    f32 nebula_z;
    f32 supernova_x;
    f32 supernova_y;
    f32 supernova_z;
    f32 pulsar_x;
    f32 pulsar_y;
    f32 pulsar_z;
    f32 quasar_x;
    f32 quasar_y;
    f32 quasar_z;
    f32 neutron_x;
    f32 neutron_y;
    f32 neutron_z;
    f32 proton_x;
    f32 proton_y;
    f32 proton_z;
    f32 electron_x;
    f32 electron_y;
    f32 electron_z;
    f32 atom_x;
    f32 atom_y;
    f32 atom_z;
    f32 molecule_x;
    f32 molecule_y;
    f32 molecule_z;
    f32 cell_x;
    f32 cell_y;
    f32 cell_z;
    f32 organ_x;
    f32 organ_y;
    f32 organ_z;
    f32 organism_x;
    f32 organism_y;
    f32 organism_z;
    f32 ecosystem_x;
    f32 ecosystem_y;
    f32 ecosystem_z;
    f32 biome_x;
    f32 biome_y;
    f32 biome_z;
    f32 planet_x;
    f32 planet_y;
    f32 planet_z;
    f32 solar_x;
    f32 solar_y;
    f32 solar_z;
    f32 lunar_x;
    f32 lunar_y;
    f32 lunar_z;
    f32 stellar_x;
    f32 stellar_y;
    f32 stellar_z;
    f32 cosmic_x;
    f32 cosmic_y;
    f32 cosmic_z;
    f32 quantum_x;
    f32 quantum_y;
    f32 quantum_z;
    f32 relativistic_x;
    f32 relativistic_y;
    f32 relativistic_z;
    f32 subatomic_x;
    f32 subatomic_y;
    f32 subatomic_z;
    f32 nanoscopic_x;
    f32 nanoscopic_y;
    f32 nanoscopic_z;
    f32 microscopic_x;
    f32 microscopic_y;
    f32 microscopic_z;
    f32 macroscopic_x;
    f32 macroscopic_y;
    f32 macroscopic_z;
};

class InputManager {
public:
    InputManager();
    ~InputManager();
    
    bool initialize();
    void shutdown();
    
    void poll_events();
    void process_event(const InputEvent& event);
    void dispatch_event(const InputEvent& event);
    
    void add_device(const InputDevice& device);
    void remove_device(u32 device_id);
    void enable_device(u32 device_id);
    void disable_device(u32 device_id);
    
    void set_device_sensitivity(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_deadzone(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_acceleration(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_smoothing(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_prediction(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_calibration(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_offset(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_scale(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_rotation(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_skew(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_perspective(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_distortion(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_aberration(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_chromatic(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_vignette(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_bloom(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_glow(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_halo(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_corona(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_lens(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_flare(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_starburst(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_cross(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_hexagon(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_octagon(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_diamond(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_triangle(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_circle(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_square(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_rectangle(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_ellipse(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_polygon(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_star(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_heart(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_flower(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_leaf(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_tree(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_cloud(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_rain(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_snow(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_fire(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_smoke(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_steam(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_water(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_oil(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_mercury(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_lava(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_plasma(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_energy(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_lightning(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_thunder(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_storm(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_tornado(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_hurricane(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_tsunami(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_earthquake(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_volcano(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_meteor(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_asteroid(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_comet(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_blackhole(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_wormhole(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_portal(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_teleport(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_timewarp(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_dimension(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_universe(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_galaxy(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_nebula(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_supernova(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_pulsar(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_quasar(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_neutron(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_proton(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_electron(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_atom(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_molecule(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_cell(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_organ(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_organism(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_ecosystem(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_biome(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_planet(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_solar(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_lunar(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_stellar(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_cosmic(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_quantum(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_relativistic(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_subatomic(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_nanoscopic(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_microscopic(u32 device_id, f32 x, f32 y, f32 z);
    void set_device_macroscopic(u32 device_id, f32 x, f32 y, f32 z);
    
    const InputDevice* get_device(u32 device_id) const;
    const std::vector<InputDevice>& get_devices() const { return devices_; }
    
    void set_event_callback(std::function<void(const InputEvent&)> callback);
    void set_device_callback(std::function<void(const InputDevice&)> callback);
    
    bool is_initialized() const { return initialized_; }
    bool is_running() const { return running_; }
    
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
    struct libinput* libinput_;
    struct udev* udev_;
    std::vector<InputDevice> devices_;
    std::unordered_map<u32, InputDevice> device_map_;
    std::function<void(const InputEvent&)> event_callback_;
    std::function<void(const InputDevice&)> device_callback_;
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
    
    void handle_device_added(struct libinput_device* device);
    void handle_device_removed(struct libinput_device* device);
    void handle_key_event(struct libinput_event* event);
    void handle_pointer_event(struct libinput_event* event);
    void handle_touch_event(struct libinput_event* event);
    void handle_gesture_event(struct libinput_event* event);
    void handle_switch_event(struct libinput_event* event);
    void handle_tablet_event(struct libinput_event* event);
    void handle_tablet_pad_event(struct libinput_event* event);
    
    InputDevice create_device(struct libinput_device* device);
    void update_device(InputDevice& device, struct libinput_device* libinput_device);
    void process_device_capabilities(InputDevice& device, struct libinput_device* libinput_device);
    void process_device_properties(InputDevice& device, struct libinput_device* libinput_device);
    void process_device_axes(InputDevice& device, struct libinput_device* libinput_device);
    void process_device_buttons(InputDevice& device, struct libinput_device* libinput_device);
    void process_device_keys(InputDevice& device, struct libinput_device* libinput_device);
    void process_device_touches(InputDevice& device, struct libinput_device* libinput_device);
    void process_device_gestures(InputDevice& device, struct libinput_device* libinput_device);
    
    void optimize_device(InputDevice& device);
    void profile_device(InputDevice& device);
    void benchmark_device(InputDevice& device);
    void calibrate_device(InputDevice& device);
    void tune_device(InputDevice& device);
    void adjust_device(InputDevice& device);
    void fine_tune_device(InputDevice& device);
    void micro_tune_device(InputDevice& device);
    void nano_tune_device(InputDevice& device);
    void pico_tune_device(InputDevice& device);
};

}
