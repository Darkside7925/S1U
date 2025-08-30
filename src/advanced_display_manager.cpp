#include "s1u/advanced_display_manager.hpp"
#include "s1u/core.hpp"
#include <vulkan/vulkan.h>
#include <drm/drm.h>
#include <drm/drm_mode.h>
#include <gbm.h>
#include <EGL/egl.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <cmath>
#include <algorithm>
#include <thread>
#include <chrono>

namespace S1U {

class AdvancedDisplayManager::Impl {
public:
    DisplayConfig config_;
    
    int drm_fd_ = -1;
    struct gbm_device* gbm_device_ = nullptr;
    EGLDisplay egl_display_ = EGL_NO_DISPLAY;
    EGLContext egl_context_ = EGL_NO_CONTEXT;
    
    VkInstance vulkan_instance_ = VK_NULL_HANDLE;
    VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
    VkDevice logical_device_ = VK_NULL_HANDLE;
    VkSurfaceKHR surface_ = VK_NULL_HANDLE;
    VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
    VkQueue graphics_queue_ = VK_NULL_HANDLE;
    VkQueue present_queue_ = VK_NULL_HANDLE;
    VkCommandPool command_pool_ = VK_NULL_HANDLE;
    
    Vector<VkImage> swapchain_images_;
    Vector<VkImageView> swapchain_image_views_;
    Vector<VkFramebuffer> swapchain_framebuffers_;
    VkRenderPass render_pass_ = VK_NULL_HANDLE;
    
    Vector<DisplayMode> available_modes_;
    Vector<DisplayOutput> connected_outputs_;
    DisplayMode current_mode_;
    
    drmModeRes* drm_resources_ = nullptr;
    Vector<drmModeConnector*> connectors_;
    Vector<drmModeEncoder*> encoders_;
    Vector<drmModeCrtc*> crtcs_;
    
    std::atomic<bool> vsync_enabled_{true};
    std::atomic<bool> hdr_enabled_{false};
    std::atomic<u32> refresh_rate_{60};
    std::atomic<f64> frame_time_{16.67};
    
    std::atomic<bool> adaptive_sync_enabled_{false};
    std::atomic<bool> variable_refresh_rate_{false};
    std::atomic<u32> min_refresh_rate_{48};
    std::atomic<u32> max_refresh_rate_{240};
    
    std::thread display_thread_;
    std::atomic<bool> display_thread_active_{false};
    
    std::atomic<u64> frames_presented_{0};
    std::atomic<f64> average_fps_{0.0};
    std::atomic<f64> gpu_utilization_{0.0};
    
    bool multi_display_enabled_ = false;
    bool display_scaling_enabled_ = false;
    bool color_correction_enabled_ = false;
    bool brightness_control_enabled_ = false;
    
    f32 gamma_red_ = 1.0f;
    f32 gamma_green_ = 1.0f;
    f32 gamma_blue_ = 1.0f;
    f32 brightness_ = 1.0f;
    f32 contrast_ = 1.0f;
    f32 saturation_ = 1.0f;
    
    Matrix3 color_matrix_;
    bool custom_color_profile_ = false;
    
    Vector<HDRMetadata> hdr_metadata_;
    bool hdr10_enabled_ = false;
    bool dolby_vision_enabled_ = false;
    
    u32 bit_depth_ = 8;
    bool deep_color_enabled_ = false;
    
    Vector<OverlayPlane> overlay_planes_;
    bool hardware_overlay_enabled_ = false;
    
    bool gpu_composition_ = true;
    bool hardware_cursor_ = true;
    bool page_flipping_ = true;
    bool atomic_modesetting_ = true;
};

AdvancedDisplayManager::AdvancedDisplayManager() : impl_(std::make_unique<Impl>()) {}

AdvancedDisplayManager::~AdvancedDisplayManager() {
    shutdown();
}

bool AdvancedDisplayManager::initialize(const DisplayConfig& config) {
    impl_->config_ = config;
    
    if (!initialize_drm()) {
        return false;
    }
    
    if (!initialize_gbm()) {
        return false;
    }
    
    if (!initialize_egl()) {
        return false;
    }
    
    if (!initialize_vulkan()) {
        return false;
    }
    
    if (!detect_displays()) {
        return false;
    }
    
    if (!configure_displays()) {
        return false;
    }
    
    if (!create_swapchain()) {
        return false;
    }
    
    if (!setup_advanced_features()) {
        return false;
    }
    
    start_display_thread();
    
    return true;
}

void AdvancedDisplayManager::shutdown() {
    stop_display_thread();
    cleanup_vulkan();
    cleanup_egl();
    cleanup_gbm();
    cleanup_drm();
}

bool AdvancedDisplayManager::initialize_drm() {
    impl_->drm_fd_ = open("/dev/dri/card0", O_RDWR);
    if (impl_->drm_fd_ < 0) {
        return false;
    }
    
    if (drmSetMaster(impl_->drm_fd_) != 0) {
        close(impl_->drm_fd_);
        return false;
    }
    
    impl_->drm_resources_ = drmModeGetResources(impl_->drm_fd_);
    if (!impl_->drm_resources_) {
        return false;
    }
    
    for (int i = 0; i < impl_->drm_resources_->count_connectors; i++) {
        drmModeConnector* connector = drmModeGetConnector(impl_->drm_fd_, impl_->drm_resources_->connectors[i]);
        if (connector && connector->connection == DRM_MODE_CONNECTED) {
            impl_->connectors_.push_back(connector);
        }
    }
    
    for (int i = 0; i < impl_->drm_resources_->count_encoders; i++) {
        drmModeEncoder* encoder = drmModeGetEncoder(impl_->drm_fd_, impl_->drm_resources_->encoders[i]);
        if (encoder) {
            impl_->encoders_.push_back(encoder);
        }
    }
    
    for (int i = 0; i < impl_->drm_resources_->count_crtcs; i++) {
        drmModeCrtc* crtc = drmModeGetCrtc(impl_->drm_fd_, impl_->drm_resources_->crtcs[i]);
        if (crtc) {
            impl_->crtcs_.push_back(crtc);
        }
    }
    
    return true;
}

bool AdvancedDisplayManager::initialize_gbm() {
    impl_->gbm_device_ = gbm_create_device(impl_->drm_fd_);
    return impl_->gbm_device_ != nullptr;
}

bool AdvancedDisplayManager::initialize_egl() {
    impl_->egl_display_ = eglGetDisplay(impl_->gbm_device_);
    if (impl_->egl_display_ == EGL_NO_DISPLAY) {
        return false;
    }
    
    EGLint major, minor;
    if (!eglInitialize(impl_->egl_display_, &major, &minor)) {
        return false;
    }
    
    const EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_STENCIL_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };
    
    EGLConfig egl_config;
    EGLint num_configs;
    if (!eglChooseConfig(impl_->egl_display_, config_attribs, &egl_config, 1, &num_configs)) {
        return false;
    }
    
    const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    
    impl_->egl_context_ = eglCreateContext(impl_->egl_display_, egl_config, EGL_NO_CONTEXT, context_attribs);
    return impl_->egl_context_ != EGL_NO_CONTEXT;
}

bool AdvancedDisplayManager::initialize_vulkan() {
    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "S1U Advanced Display Manager";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "S1U Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_3;
    
    Vector<const char*> extensions = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_DISPLAY_EXTENSION_NAME,
        VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME,
        VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME,
        VK_EXT_HDR_METADATA_EXTENSION_NAME
    };
    
    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = static_cast<u32>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();
    
    if (vkCreateInstance(&create_info, nullptr, &impl_->vulkan_instance_) != VK_SUCCESS) {
        return false;
    }
    
    if (!select_physical_device()) {
        return false;
    }
    
    if (!create_logical_device()) {
        return false;
    }
    
    if (!create_command_pool()) {
        return false;
    }
    
    return true;
}

bool AdvancedDisplayManager::select_physical_device() {
    u32 device_count = 0;
    vkEnumeratePhysicalDevices(impl_->vulkan_instance_, &device_count, nullptr);
    
    if (device_count == 0) {
        return false;
    }
    
    Vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(impl_->vulkan_instance_, &device_count, devices.data());
    
    for (const auto& device : devices) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);
        
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            impl_->physical_device_ = device;
            return true;
        }
    }
    
    impl_->physical_device_ = devices[0];
    return true;
}

bool AdvancedDisplayManager::create_logical_device() {
    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(impl_->physical_device_, &queue_family_count, nullptr);
    
    Vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(impl_->physical_device_, &queue_family_count, queue_families.data());
    
    u32 graphics_family = UINT32_MAX;
    for (u32 i = 0; i < queue_families.size(); i++) {
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphics_family = i;
            break;
        }
    }
    
    if (graphics_family == UINT32_MAX) {
        return false;
    }
    
    f32 queue_priority = 1.0f;
    VkDeviceQueueCreateInfo queue_create_info = {};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = graphics_family;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;
    
    VkPhysicalDeviceFeatures device_features = {};
    device_features.samplerAnisotropy = VK_TRUE;
    device_features.fillModeNonSolid = VK_TRUE;
    device_features.wideLines = VK_TRUE;
    
    Vector<const char*> device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_DISPLAY_SWAPCHAIN_EXTENSION_NAME,
        VK_EXT_HDR_METADATA_EXTENSION_NAME,
        VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME
    };
    
    VkDeviceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pQueueCreateInfos = &queue_create_info;
    create_info.queueCreateInfoCount = 1;
    create_info.pEnabledFeatures = &device_features;
    create_info.enabledExtensionCount = static_cast<u32>(device_extensions.size());
    create_info.ppEnabledExtensionNames = device_extensions.data();
    
    if (vkCreateDevice(impl_->physical_device_, &create_info, nullptr, &impl_->logical_device_) != VK_SUCCESS) {
        return false;
    }
    
    vkGetDeviceQueue(impl_->logical_device_, graphics_family, 0, &impl_->graphics_queue_);
    impl_->present_queue_ = impl_->graphics_queue_;
    
    return true;
}

bool AdvancedDisplayManager::create_command_pool() {
    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(impl_->physical_device_, &queue_family_count, nullptr);
    
    Vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(impl_->physical_device_, &queue_family_count, queue_families.data());
    
    u32 graphics_family = UINT32_MAX;
    for (u32 i = 0; i < queue_families.size(); i++) {
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphics_family = i;
            break;
        }
    }
    
    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = graphics_family;
    
    return vkCreateCommandPool(impl_->logical_device_, &pool_info, nullptr, &impl_->command_pool_) == VK_SUCCESS;
}

bool AdvancedDisplayManager::detect_displays() {
    impl_->connected_outputs_.clear();
    impl_->available_modes_.clear();
    
    for (auto connector : impl_->connectors_) {
        DisplayOutput output;
        output.connector_id = connector->connector_id;
        output.is_connected = (connector->connection == DRM_MODE_CONNECTED);
        output.width_mm = connector->mmWidth;
        output.height_mm = connector->mmHeight;
        
        switch (connector->connector_type) {
            case DRM_MODE_CONNECTOR_HDMIA:
            case DRM_MODE_CONNECTOR_HDMIB:
                output.connector_type = "HDMI";
                break;
            case DRM_MODE_CONNECTOR_DisplayPort:
                output.connector_type = "DisplayPort";
                break;
            case DRM_MODE_CONNECTOR_eDP:
                output.connector_type = "eDP";
                break;
            case DRM_MODE_CONNECTOR_LVDS:
                output.connector_type = "LVDS";
                break;
            default:
                output.connector_type = "Unknown";
                break;
        }
        
        for (int i = 0; i < connector->count_modes; i++) {
            drmModeModeInfo mode_info = connector->modes[i];
            
            DisplayMode mode;
            mode.width = mode_info.hdisplay;
            mode.height = mode_info.vdisplay;
            mode.refresh_rate = mode_info.vrefresh;
            mode.is_preferred = (mode_info.type & DRM_MODE_TYPE_PREFERRED) != 0;
            mode.pixel_clock = mode_info.clock;
            
            if (mode_info.flags & DRM_MODE_FLAG_INTERLACE) {
                mode.flags |= DISPLAY_MODE_INTERLACED;
            }
            if (mode_info.flags & DRM_MODE_FLAG_DBLSCAN) {
                mode.flags |= DISPLAY_MODE_DOUBLESCAN;
            }
            if (mode_info.flags & DRM_MODE_FLAG_CSYNC) {
                mode.flags |= DISPLAY_MODE_CSYNC;
            }
            
            output.supported_modes.push_back(mode);
            impl_->available_modes_.push_back(mode);
        }
        
        detect_hdr_capabilities(output);
        detect_color_capabilities(output);
        detect_variable_refresh_capabilities(output);
        
        impl_->connected_outputs_.push_back(output);
    }
    
    return !impl_->connected_outputs_.empty();
}

void AdvancedDisplayManager::detect_hdr_capabilities(DisplayOutput& output) {
    drmModePropertyPtr prop;
    drmModeObjectPropertiesPtr props;
    
    for (auto connector : impl_->connectors_) {
        if (connector->connector_id == output.connector_id) {
            props = drmModeObjectGetProperties(impl_->drm_fd_, connector->connector_id, DRM_MODE_OBJECT_CONNECTOR);
            
            for (u32 i = 0; i < props->count_props; i++) {
                prop = drmModeGetProperty(impl_->drm_fd_, props->props[i]);
                
                if (prop && strcmp(prop->name, "HDR_OUTPUT_METADATA") == 0) {
                    output.supports_hdr = true;
                    output.hdr_metadata_types.push_back("HDR10");
                }
                
                if (prop && strcmp(prop->name, "Colorspace") == 0) {
                    output.supports_wide_color = true;
                }
                
                if (prop && strcmp(prop->name, "max_bpc") == 0) {
                    output.max_bpc = static_cast<u32>(props->prop_values[i]);
                }
                
                drmModeFreeProperty(prop);
            }
            
            drmModeFreeObjectProperties(props);
            break;
        }
    }
}

void AdvancedDisplayManager::detect_color_capabilities(DisplayOutput& output) {
    output.color_depth = 8;
    output.supports_10bit = false;
    output.supports_12bit = false;
    
    if (output.max_bpc >= 10) {
        output.supports_10bit = true;
        output.color_depth = 10;
    }
    if (output.max_bpc >= 12) {
        output.supports_12bit = true;
        output.color_depth = 12;
    }
    
    output.color_spaces.push_back("sRGB");
    if (output.supports_wide_color) {
        output.color_spaces.push_back("DCI-P3");
        output.color_spaces.push_back("Rec.2020");
    }
}

void AdvancedDisplayManager::detect_variable_refresh_capabilities(DisplayOutput& output) {
    drmModePropertyPtr prop;
    drmModeObjectPropertiesPtr props;
    
    for (auto connector : impl_->connectors_) {
        if (connector->connector_id == output.connector_id) {
            props = drmModeObjectGetProperties(impl_->drm_fd_, connector->connector_id, DRM_MODE_OBJECT_CONNECTOR);
            
            for (u32 i = 0; i < props->count_props; i++) {
                prop = drmModeGetProperty(impl_->drm_fd_, props->props[i]);
                
                if (prop && (strcmp(prop->name, "vrr_capable") == 0 || strcmp(prop->name, "VRR_ENABLED") == 0)) {
                    output.supports_variable_refresh = true;
                }
                
                if (prop && strcmp(prop->name, "adaptive_sync") == 0) {
                    output.supports_adaptive_sync = true;
                }
                
                drmModeFreeProperty(prop);
            }
            
            drmModeFreeObjectProperties(props);
            break;
        }
    }
}

bool AdvancedDisplayManager::configure_displays() {
    if (impl_->connected_outputs_.empty()) {
        return false;
    }
    
    DisplayOutput& primary_output = impl_->connected_outputs_[0];
    
    DisplayMode best_mode;
    bool found_preferred = false;
    
    for (const auto& mode : primary_output.supported_modes) {
        if (mode.is_preferred) {
            best_mode = mode;
            found_preferred = true;
            break;
        }
    }
    
    if (!found_preferred && !primary_output.supported_modes.empty()) {
        best_mode = primary_output.supported_modes[0];
        for (const auto& mode : primary_output.supported_modes) {
            if (mode.width * mode.height > best_mode.width * best_mode.height) {
                best_mode = mode;
            }
        }
    }
    
    impl_->current_mode_ = best_mode;
    impl_->refresh_rate_ = best_mode.refresh_rate;
    
    if (impl_->config_.enable_hdr && primary_output.supports_hdr) {
        enable_hdr(true);
    }
    
    if (impl_->config_.enable_variable_refresh && primary_output.supports_variable_refresh) {
        enable_variable_refresh_rate(true);
    }
    
    configure_color_management();
    
    return true;
}

bool AdvancedDisplayManager::create_swapchain() {
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(impl_->physical_device_, impl_->surface_, &capabilities);
    
    u32 format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(impl_->physical_device_, impl_->surface_, &format_count, nullptr);
    
    Vector<VkSurfaceFormatKHR> formats(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(impl_->physical_device_, impl_->surface_, &format_count, formats.data());
    
    u32 present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(impl_->physical_device_, impl_->surface_, &present_mode_count, nullptr);
    
    Vector<VkPresentModeKHR> present_modes(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(impl_->physical_device_, impl_->surface_, &present_mode_count, present_modes.data());
    
    VkSurfaceFormatKHR surface_format = choose_swap_surface_format(formats);
    VkPresentModeKHR present_mode = choose_swap_present_mode(present_modes);
    VkExtent2D extent = choose_swap_extent(capabilities);
    
    u32 image_count = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount) {
        image_count = capabilities.maxImageCount;
    }
    
    VkSwapchainCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = impl_->surface_;
    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.preTransform = capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = VK_NULL_HANDLE;
    
    if (vkCreateSwapchainKHR(impl_->logical_device_, &create_info, nullptr, &impl_->swapchain_) != VK_SUCCESS) {
        return false;
    }
    
    vkGetSwapchainImagesKHR(impl_->logical_device_, impl_->swapchain_, &image_count, nullptr);
    impl_->swapchain_images_.resize(image_count);
    vkGetSwapchainImagesKHR(impl_->logical_device_, impl_->swapchain_, &image_count, impl_->swapchain_images_.data());
    
    return create_image_views(surface_format.format);
}

VkSurfaceFormatKHR AdvancedDisplayManager::choose_swap_surface_format(const Vector<VkSurfaceFormatKHR>& available_formats) {
    for (const auto& format : available_formats) {
        if (impl_->hdr_enabled_) {
            if (format.format == VK_FORMAT_A2B10G10R10_UNORM_PACK32 && 
                format.colorSpace == VK_COLOR_SPACE_HDR10_ST2084_EXT) {
                return format;
            }
        } else {
            if (format.format == VK_FORMAT_B8G8R8A8_SRGB && 
                format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return format;
            }
        }
    }
    
    return available_formats[0];
}

VkPresentModeKHR AdvancedDisplayManager::choose_swap_present_mode(const Vector<VkPresentModeKHR>& available_modes) {
    if (!impl_->vsync_enabled_) {
        for (const auto& mode : available_modes) {
            if (mode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
                return mode;
            }
        }
    }
    
    if (impl_->variable_refresh_rate_) {
        for (const auto& mode : available_modes) {
            if (mode == VK_PRESENT_MODE_FIFO_RELAXED_KHR) {
                return mode;
            }
        }
    }
    
    for (const auto& mode : available_modes) {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return mode;
        }
    }
    
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D AdvancedDisplayManager::choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    }
    
    VkExtent2D actual_extent = {
        static_cast<u32>(impl_->current_mode_.width),
        static_cast<u32>(impl_->current_mode_.height)
    };
    
    actual_extent.width = std::clamp(actual_extent.width, 
                                   capabilities.minImageExtent.width, 
                                   capabilities.maxImageExtent.width);
    actual_extent.height = std::clamp(actual_extent.height, 
                                    capabilities.minImageExtent.height, 
                                    capabilities.maxImageExtent.height);
    
    return actual_extent;
}

bool AdvancedDisplayManager::create_image_views(VkFormat format) {
    impl_->swapchain_image_views_.resize(impl_->swapchain_images_.size());
    
    for (size_t i = 0; i < impl_->swapchain_images_.size(); i++) {
        VkImageViewCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        create_info.image = impl_->swapchain_images_[i];
        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.format = format;
        create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount = 1;
        
        if (vkCreateImageView(impl_->logical_device_, &create_info, nullptr, 
                            &impl_->swapchain_image_views_[i]) != VK_SUCCESS) {
            return false;
        }
    }
    
    return true;
}

bool AdvancedDisplayManager::setup_advanced_features() {
    setup_hdr_metadata();
    setup_color_management();
    setup_overlay_planes();
    setup_variable_refresh_rate();
    setup_adaptive_sync();
    
    return true;
}

void AdvancedDisplayManager::setup_hdr_metadata() {
    if (!impl_->hdr_enabled_) return;
    
    HDRMetadata metadata;
    metadata.display_primaries_x[0] = 0.708f;
    metadata.display_primaries_y[0] = 0.292f;
    metadata.display_primaries_x[1] = 0.170f;
    metadata.display_primaries_y[1] = 0.797f;
    metadata.display_primaries_x[2] = 0.131f;
    metadata.display_primaries_y[2] = 0.046f;
    metadata.white_point_x = 0.3127f;
    metadata.white_point_y = 0.3290f;
    metadata.max_display_mastering_luminance = 1000.0f;
    metadata.min_display_mastering_luminance = 0.01f;
    metadata.max_cll = 1000.0f;
    metadata.max_fall = 400.0f;
    
    impl_->hdr_metadata_.push_back(metadata);
}

void AdvancedDisplayManager::setup_color_management() {
    impl_->color_matrix_ = Matrix3::identity();
    
    if (impl_->config_.enable_color_correction) {
        apply_color_correction();
    }
    
    if (impl_->config_.enable_gamma_correction) {
        apply_gamma_correction(impl_->gamma_red_, impl_->gamma_green_, impl_->gamma_blue_);
    }
}

void AdvancedDisplayManager::setup_overlay_planes() {
    if (!impl_->config_.enable_hardware_overlay) return;
    
    drmModePlaneRes* plane_res = drmModeGetPlaneResources(impl_->drm_fd_);
    if (!plane_res) return;
    
    for (u32 i = 0; i < plane_res->count_planes; i++) {
        drmModePlane* plane = drmModeGetPlane(impl_->drm_fd_, plane_res->planes[i]);
        if (!plane) continue;
        
        OverlayPlane overlay;
        overlay.plane_id = plane->plane_id;
        overlay.possible_crtcs = plane->possible_crtcs;
        overlay.gamma_size = plane->gamma_size;
        
        drmModeObjectPropertiesPtr props = drmModeObjectGetProperties(impl_->drm_fd_, 
                                                                    plane->plane_id, 
                                                                    DRM_MODE_OBJECT_PLANE);
        if (props) {
            for (u32 j = 0; j < props->count_props; j++) {
                drmModePropertyPtr prop = drmModeGetProperty(impl_->drm_fd_, props->props[j]);
                if (prop && strcmp(prop->name, "type") == 0) {
                    if (props->prop_values[j] == DRM_PLANE_TYPE_OVERLAY) {
                        overlay.is_overlay = true;
                    } else if (props->prop_values[j] == DRM_PLANE_TYPE_CURSOR) {
                        overlay.is_cursor = true;
                    }
                }
                drmModeFreeProperty(prop);
            }
            drmModeFreeObjectProperties(props);
        }
        
        impl_->overlay_planes_.push_back(overlay);
        drmModeFreePlane(plane);
    }
    
    drmModeFreePlaneResources(plane_res);
    impl_->hardware_overlay_enabled_ = !impl_->overlay_planes_.empty();
}

void AdvancedDisplayManager::setup_variable_refresh_rate() {
    if (!impl_->config_.enable_variable_refresh) return;
    
    for (const auto& output : impl_->connected_outputs_) {
        if (output.supports_variable_refresh) {
            impl_->variable_refresh_rate_ = true;
            break;
        }
    }
}

void AdvancedDisplayManager::setup_adaptive_sync() {
    if (!impl_->config_.enable_adaptive_sync) return;
    
    for (const auto& output : impl_->connected_outputs_) {
        if (output.supports_adaptive_sync) {
            impl_->adaptive_sync_enabled_ = true;
            break;
        }
    }
}

void AdvancedDisplayManager::start_display_thread() {
    impl_->display_thread_active_ = true;
    impl_->display_thread_ = std::thread([this]() {
        display_thread_loop();
    });
}

void AdvancedDisplayManager::stop_display_thread() {
    impl_->display_thread_active_ = false;
    if (impl_->display_thread_.joinable()) {
        impl_->display_thread_.join();
    }
}

void AdvancedDisplayManager::display_thread_loop() {
    auto last_frame_time = std::chrono::high_resolution_clock::now();
    u64 frame_count = 0;
    
    while (impl_->display_thread_active_) {
        auto current_time = std::chrono::high_resolution_clock::now();
        auto delta_time = std::chrono::duration<f64, std::milli>(current_time - last_frame_time);
        
        impl_->frame_time_ = delta_time.count();
        
        if (frame_count % 60 == 0) {
            f64 fps = 1000.0 / impl_->frame_time_;
            impl_->average_fps_ = fps;
        }
        
        update_brightness_control();
        update_adaptive_sync();
        monitor_display_health();
        
        frame_count++;
        impl_->frames_presented_ = frame_count;
        last_frame_time = current_time;
        
        auto target_frame_time = std::chrono::microseconds(1000000 / impl_->refresh_rate_.load());
        auto sleep_time = target_frame_time - std::chrono::duration_cast<std::chrono::microseconds>(delta_time);
        
        if (sleep_time > std::chrono::microseconds(0)) {
            std::this_thread::sleep_for(sleep_time);
        }
    }
}

void AdvancedDisplayManager::update_brightness_control() {
    if (!impl_->brightness_control_enabled_) return;
    
    auto current_time = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(current_time);
    auto* local_time = std::localtime(&time_t);
    
    int hour = local_time->tm_hour;
    
    f32 target_brightness = 1.0f;
    if (hour >= 22 || hour <= 6) {
        target_brightness = 0.6f;
    } else if (hour >= 20 || hour <= 8) {
        target_brightness = 0.8f;
    }
    
    if (std::abs(impl_->brightness_ - target_brightness) > 0.01f) {
        f32 step = (target_brightness > impl_->brightness_) ? 0.01f : -0.01f;
        impl_->brightness_ = std::clamp(impl_->brightness_ + step, 0.1f, 1.0f);
        apply_brightness_change();
    }
}

void AdvancedDisplayManager::update_adaptive_sync() {
    if (!impl_->adaptive_sync_enabled_) return;
    
    f64 current_fps = impl_->average_fps_;
    
    if (current_fps < impl_->min_refresh_rate_) {
        impl_->refresh_rate_ = impl_->min_refresh_rate_;
    } else if (current_fps > impl_->max_refresh_rate_) {
        impl_->refresh_rate_ = impl_->max_refresh_rate_;
    } else {
        impl_->refresh_rate_ = static_cast<u32>(current_fps);
    }
}

void AdvancedDisplayManager::monitor_display_health() {
    for (auto& output : impl_->connected_outputs_) {
        drmModeConnector* connector = drmModeGetConnector(impl_->drm_fd_, output.connector_id);
        if (connector) {
            bool was_connected = output.is_connected;
            output.is_connected = (connector->connection == DRM_MODE_CONNECTED);
            
            if (was_connected && !output.is_connected) {
                handle_display_disconnect(output);
            } else if (!was_connected && output.is_connected) {
                handle_display_connect(output);
            }
            
            drmModeFreeConnector(connector);
        }
    }
}

void AdvancedDisplayManager::handle_display_connect(DisplayOutput& output) {
    detect_hdr_capabilities(output);
    detect_color_capabilities(output);
    detect_variable_refresh_capabilities(output);
    
    if (impl_->config_.auto_configure_displays) {
        configure_displays();
    }
}

void AdvancedDisplayManager::handle_display_disconnect(DisplayOutput& output) {
    if (impl_->connected_outputs_.size() > 1) {
        reconfigure_remaining_displays();
    }
}

void AdvancedDisplayManager::reconfigure_remaining_displays() {
    Vector<DisplayOutput> active_outputs;
    for (const auto& output : impl_->connected_outputs_) {
        if (output.is_connected) {
            active_outputs.push_back(output);
        }
    }
    
    if (!active_outputs.empty()) {
        impl_->connected_outputs_ = active_outputs;
        configure_displays();
    }
}

bool AdvancedDisplayManager::set_display_mode(u32 width, u32 height, u32 refresh_rate) {
    for (const auto& mode : impl_->available_modes_) {
        if (mode.width == width && mode.height == height && mode.refresh_rate == refresh_rate) {
            impl_->current_mode_ = mode;
            impl_->refresh_rate_ = refresh_rate;
            return recreate_swapchain();
        }
    }
    return false;
}

bool AdvancedDisplayManager::recreate_swapchain() {
    vkDeviceWaitIdle(impl_->logical_device_);
    
    cleanup_swapchain();
    
    return create_swapchain();
}

void AdvancedDisplayManager::cleanup_swapchain() {
    for (auto& framebuffer : impl_->swapchain_framebuffers_) {
        vkDestroyFramebuffer(impl_->logical_device_, framebuffer, nullptr);
    }
    impl_->swapchain_framebuffers_.clear();
    
    for (auto& image_view : impl_->swapchain_image_views_) {
        vkDestroyImageView(impl_->logical_device_, image_view, nullptr);
    }
    impl_->swapchain_image_views_.clear();
    
    if (impl_->swapchain_ != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(impl_->logical_device_, impl_->swapchain_, nullptr);
        impl_->swapchain_ = VK_NULL_HANDLE;
    }
}

void AdvancedDisplayManager::enable_hdr(bool enabled) {
    impl_->hdr_enabled_ = enabled;
    
    if (enabled) {
        impl_->hdr10_enabled_ = true;
        setup_hdr_metadata();
    } else {
        impl_->hdr10_enabled_ = false;
        impl_->dolby_vision_enabled_ = false;
    }
    
    recreate_swapchain();
}

void AdvancedDisplayManager::enable_variable_refresh_rate(bool enabled) {
    impl_->variable_refresh_rate_ = enabled;
    
    if (enabled) {
        impl_->min_refresh_rate_ = 48;
        impl_->max_refresh_rate_ = 240;
    }
}

void AdvancedDisplayManager::set_refresh_rate_range(u32 min_rate, u32 max_rate) {
    impl_->min_refresh_rate_ = min_rate;
    impl_->max_refresh_rate_ = max_rate;
}

void AdvancedDisplayManager::apply_color_correction() {
    impl_->color_correction_enabled_ = true;
    
    impl_->color_matrix_ = Matrix3(
        impl_->contrast_, 0.0f, 0.0f,
        0.0f, impl_->contrast_, 0.0f,
        0.0f, 0.0f, impl_->contrast_
    );
    
    impl_->color_matrix_ = impl_->color_matrix_ * Matrix3(
        impl_->saturation_, 0.0f, 0.0f,
        0.0f, impl_->saturation_, 0.0f,
        0.0f, 0.0f, impl_->saturation_
    );
}

void AdvancedDisplayManager::apply_gamma_correction(f32 red, f32 green, f32 blue) {
    impl_->gamma_red_ = red;
    impl_->gamma_green_ = green;
    impl_->gamma_blue_ = blue;
    
    for (auto& crtc : impl_->crtcs_) {
        if (crtc->gamma_size > 0) {
            Vector<u16> gamma_red(crtc->gamma_size);
            Vector<u16> gamma_green(crtc->gamma_size);
            Vector<u16> gamma_blue(crtc->gamma_size);
            
            for (int i = 0; i < crtc->gamma_size; i++) {
                f32 normalized = static_cast<f32>(i) / (crtc->gamma_size - 1);
                
                gamma_red[i] = static_cast<u16>(std::pow(normalized, 1.0f / red) * 65535);
                gamma_green[i] = static_cast<u16>(std::pow(normalized, 1.0f / green) * 65535);
                gamma_blue[i] = static_cast<u16>(std::pow(normalized, 1.0f / blue) * 65535);
            }
            
            drmModeCrtcSetGamma(impl_->drm_fd_, crtc->crtc_id, crtc->gamma_size,
                              gamma_red.data(), gamma_green.data(), gamma_blue.data());
        }
    }
}

void AdvancedDisplayManager::set_brightness(f32 brightness) {
    impl_->brightness_ = std::clamp(brightness, 0.0f, 1.0f);
    apply_brightness_change();
}

void AdvancedDisplayManager::apply_brightness_change() {
    for (auto& connector : impl_->connectors_) {
        drmModeObjectPropertiesPtr props = drmModeObjectGetProperties(impl_->drm_fd_, 
                                                                    connector->connector_id, 
                                                                    DRM_MODE_OBJECT_CONNECTOR);
        if (props) {
            for (u32 i = 0; i < props->count_props; i++) {
                drmModePropertyPtr prop = drmModeGetProperty(impl_->drm_fd_, props->props[i]);
                if (prop && strcmp(prop->name, "Backlight") == 0) {
                    u64 brightness_value = static_cast<u64>(impl_->brightness_ * prop->values[1]);
                    drmModeObjectSetProperty(impl_->drm_fd_, connector->connector_id, 
                                           DRM_MODE_OBJECT_CONNECTOR, props->props[i], brightness_value);
                }
                drmModeFreeProperty(prop);
            }
            drmModeFreeObjectProperties(props);
        }
    }
}

void AdvancedDisplayManager::set_contrast(f32 contrast) {
    impl_->contrast_ = std::clamp(contrast, 0.1f, 2.0f);
    apply_color_correction();
}

void AdvancedDisplayManager::set_saturation(f32 saturation) {
    impl_->saturation_ = std::clamp(saturation, 0.0f, 2.0f);
    apply_color_correction();
}

Vector<DisplayMode> AdvancedDisplayManager::get_available_modes() const {
    return impl_->available_modes_;
}

Vector<DisplayOutput> AdvancedDisplayManager::get_connected_outputs() const {
    return impl_->connected_outputs_;
}

DisplayMode AdvancedDisplayManager::get_current_mode() const {
    return impl_->current_mode_;
}

DisplayStats AdvancedDisplayManager::get_display_stats() const {
    DisplayStats stats;
    stats.frames_presented = impl_->frames_presented_;
    stats.average_fps = impl_->average_fps_;
    stats.frame_time_ms = impl_->frame_time_;
    stats.gpu_utilization = impl_->gpu_utilization_;
    stats.vsync_enabled = impl_->vsync_enabled_;
    stats.hdr_enabled = impl_->hdr_enabled_;
    stats.variable_refresh_enabled = impl_->variable_refresh_rate_;
    stats.adaptive_sync_enabled = impl_->adaptive_sync_enabled_;
    stats.current_refresh_rate = impl_->refresh_rate_;
    stats.color_depth = impl_->bit_depth_;
    stats.brightness = impl_->brightness_;
    stats.contrast = impl_->contrast_;
    stats.saturation = impl_->saturation_;
    return stats;
}

void AdvancedDisplayManager::cleanup_drm() {
    for (auto& crtc : impl_->crtcs_) {
        drmModeFreeCrtc(crtc);
    }
    impl_->crtcs_.clear();
    
    for (auto& encoder : impl_->encoders_) {
        drmModeFreeEncoder(encoder);
    }
    impl_->encoders_.clear();
    
    for (auto& connector : impl_->connectors_) {
        drmModeFreeConnector(connector);
    }
    impl_->connectors_.clear();
    
    if (impl_->drm_resources_) {
        drmModeFreeResources(impl_->drm_resources_);
        impl_->drm_resources_ = nullptr;
    }
    
    if (impl_->drm_fd_ >= 0) {
        drmDropMaster(impl_->drm_fd_);
        close(impl_->drm_fd_);
        impl_->drm_fd_ = -1;
    }
}

void AdvancedDisplayManager::cleanup_gbm() {
    if (impl_->gbm_device_) {
        gbm_device_destroy(impl_->gbm_device_);
        impl_->gbm_device_ = nullptr;
    }
}

void AdvancedDisplayManager::cleanup_egl() {
    if (impl_->egl_context_ != EGL_NO_CONTEXT) {
        eglDestroyContext(impl_->egl_display_, impl_->egl_context_);
        impl_->egl_context_ = EGL_NO_CONTEXT;
    }
    
    if (impl_->egl_display_ != EGL_NO_DISPLAY) {
        eglTerminate(impl_->egl_display_);
        impl_->egl_display_ = EGL_NO_DISPLAY;
    }
}

void AdvancedDisplayManager::cleanup_vulkan() {
    if (impl_->logical_device_ != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(impl_->logical_device_);
    }
    
    cleanup_swapchain();
    
    if (impl_->render_pass_ != VK_NULL_HANDLE) {
        vkDestroyRenderPass(impl_->logical_device_, impl_->render_pass_, nullptr);
    }
    
    if (impl_->command_pool_ != VK_NULL_HANDLE) {
        vkDestroyCommandPool(impl_->logical_device_, impl_->command_pool_, nullptr);
    }
    
    if (impl_->logical_device_ != VK_NULL_HANDLE) {
        vkDestroyDevice(impl_->logical_device_, nullptr);
    }
    
    if (impl_->surface_ != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(impl_->vulkan_instance_, impl_->surface_, nullptr);
    }
    
    if (impl_->vulkan_instance_ != VK_NULL_HANDLE) {
        vkDestroyInstance(impl_->vulkan_instance_, nullptr);
    }
}

} // namespace S1U
