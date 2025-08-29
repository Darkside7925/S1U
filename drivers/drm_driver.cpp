#include "s1u/core.hpp"
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <drm/drm.h>
#include <drm/drm_mode.h>
#include <gbm.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

namespace s1u {

class DRMDriver {
public:
    DRMDriver() : fd_(-1), resources_(nullptr), connector_(nullptr), encoder_(nullptr), crtc_(nullptr), mode_(nullptr) {}
    
    bool initialize(const std::string& device_path = "/dev/dri/card0") {
        fd_ = open(device_path.c_str(), O_RDWR | O_CLOEXEC);
        if (fd_ < 0) return false;
        
        if (drmSetClientCap(fd_, DRM_CLIENT_CAP_UNIVERSAL_PLANES, 1) < 0) return false;
        if (drmSetClientCap(fd_, DRM_CLIENT_CAP_ATOMIC, 1) < 0) return false;
        
        resources_ = drmModeGetResources(fd_);
        if (!resources_) return false;
        
        for (int i = 0; i < resources_->count_connectors; i++) {
            connector_ = drmModeGetConnector(fd_, resources_->connectors[i]);
            if (connector_ && connector_->connection == DRM_MODE_CONNECTED) break;
        }
        
        if (!connector_) return false;
        
        encoder_ = drmModeGetEncoder(fd_, connector_->encoder_id);
        if (!encoder_) return false;
        
        crtc_ = drmModeGetCrtc(fd_, encoder_->crtc_id);
        if (!crtc_) return false;
        
        mode_ = &connector_->modes[0];
        
        return true;
    }
    
    void shutdown() {
        if (crtc_) drmModeFreeCrtc(crtc_);
        if (encoder_) drmModeFreeEncoder(encoder_);
        if (connector_) drmModeFreeConnector(connector_);
        if (resources_) drmModeFreeResources(resources_);
        if (fd_ >= 0) close(fd_);
    }
    
    int get_fd() const { return fd_; }
    drmModeRes* get_resources() const { return resources_; }
    drmModeConnector* get_connector() const { return connector_; }
    drmModeEncoder* get_encoder() const { return encoder_; }
    drmModeCrtc* get_crtc() const { return crtc_; }
    drmModeModeInfo* get_mode() const { return mode_; }
    
    bool set_mode(drmModeModeInfo* mode) {
        if (!crtc_ || !mode) return false;
        return drmModeSetCrtc(fd_, crtc_->crtc_id, 0, 0, 0, nullptr, 1, mode) == 0;
    }
    
    bool page_flip(u32 fb_id) {
        if (!crtc_) return false;
        return drmModePageFlip(fd_, crtc_->crtc_id, fb_id, DRM_MODE_PAGE_FLIP_EVENT, nullptr) == 0;
    }
    
    bool atomic_commit(drmModeAtomicReq* req) {
        return drmModeAtomicCommit(fd_, req, DRM_MODE_ATOMIC_ALLOW_MODESET, nullptr) == 0;
    }
    
    u32 create_fb(u32 width, u32 height, u32 format, u32 handles[4], u32 pitches[4], u32 offsets[4]) {
        u32 fb_id;
        if (drmModeAddFB2(fd_, width, height, format, handles, pitches, offsets, &fb_id, 0) == 0) {
            return fb_id;
        }
        return 0;
    }
    
    bool destroy_fb(u32 fb_id) {
        return drmModeRmFB(fd_, fb_id) == 0;
    }
    
    bool set_gamma(u32 crtc_id, u32 size, u16* red, u16* green, u16* blue) {
        return drmModeCrtcSetGamma(fd_, crtc_id, size, red, green, blue) == 0;
    }
    
    bool get_gamma(u32 crtc_id, u32 size, u16* red, u16* green, u16* blue) {
        return drmModeCrtcGetGamma(fd_, crtc_id, size, red, green, blue) == 0;
    }
    
    bool set_property(u32 obj_id, u32 obj_type, u32 prop_id, u64 value) {
        return drmModeObjectSetProperty(fd_, obj_id, obj_type, prop_id, value) == 0;
    }
    
    bool get_property(u32 obj_id, u32 obj_type, u32 prop_id, u64* value) {
        return drmModeObjectGetProperty(fd_, obj_id, obj_type, prop_id, value) == 0;
    }
    
    bool set_plane(u32 plane_id, u32 fb_id, u32 crtc_id, u32 x, u32 y, u32 src_x, u32 src_y, u32 src_w, u32 src_h) {
        return drmModeSetPlane(fd_, plane_id, crtc_id, fb_id, 0, x, y, src_w, src_h, src_x << 16, src_y << 16, src_w << 16, src_h << 16) == 0;
    }
    
    bool get_plane(u32 plane_id, drmModePlane** plane) {
        *plane = drmModeGetPlane(fd_, plane_id);
        return *plane != nullptr;
    }
    
    bool get_plane_resources(drmModePlaneRes** plane_resources) {
        *plane_resources = drmModeGetPlaneResources(fd_);
        return *plane_resources != nullptr;
    }
    
    bool get_property_blob(u32 blob_id, drmModePropertyBlob** blob) {
        *blob = drmModeGetPropertyBlob(fd_, blob_id);
        return *blob != nullptr;
    }
    
    bool create_property_blob(u32 size, void* data, u32* blob_id) {
        return drmModeCreatePropertyBlob(fd_, data, size, blob_id) == 0;
    }
    
    bool destroy_property_blob(u32 blob_id) {
        return drmModeDestroyPropertyBlob(fd_, blob_id) == 0;
    }
    
    bool get_capability(u64 capability, u64* value) {
        return drmGetCap(fd_, capability, value) == 0;
    }
    
    bool set_client_cap(u64 capability, u64 value) {
        return drmSetClientCap(fd_, capability, value) == 0;
    }
    
    bool get_client_cap(u64 capability, u64* value) {
        return drmGetClientCap(fd_, capability, value) == 0;
    }
    
    bool get_driver_name(char* name) {
        return drmGetDriverName(fd_, name) == 0;
    }
    
    bool get_bus_id(char* bus_id) {
        return drmGetBusid(fd_, bus_id) == 0;
    }
    
    bool get_magic(int* magic) {
        return drmGetMagic(fd_, magic) == 0;
    }
    
    bool auth_magic(int magic) {
        return drmAuthMagic(fd_, magic) == 0;
    }
    
    bool drop_master() {
        return drmDropMaster(fd_) == 0;
    }
    
    bool set_master() {
        return drmSetMaster(fd_) == 0;
    }
    
    bool is_master() {
        return drmIsMaster(fd_) == 1;
    }
    
    bool get_auth_magic(int* magic) {
        return drmGetMagic(fd_, magic) == 0;
    }
    
    bool auth_magic(int magic) {
        return drmAuthMagic(fd_, magic) == 0;
    }
    
    bool drop_master() {
        return drmDropMaster(fd_) == 0;
    }
    
    bool set_master() {
        return drmSetMaster(fd_) == 0;
    }
    
    bool is_master() {
        return drmIsMaster(fd_) == 1;
    }
    
private:
    int fd_;
    drmModeRes* resources_;
    drmModeConnector* connector_;
    drmModeEncoder* encoder_;
    drmModeCrtc* crtc_;
    drmModeModeInfo* mode_;
};

}
