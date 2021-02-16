#include <libreborn/libreborn.h>

#include "../screenshot/screenshot.h"
#include "../init/init.h"

#include <libreborn/minecraft.h>

// Take Screenshot Using TripodCamera
static void AppPlatform_linux_saveScreenshot_injection(__attribute__((unused)) unsigned char *app_platform, __attribute__((unused)) std::string const& param1, __attribute__((unused)) std::string const& param_2) {
    take_screenshot();
}

// Enable TripodCameraRenderer
static unsigned char *EntityRenderDispatcher_injection(unsigned char *dispatcher) {
    // Call Original Method
    (*EntityRenderDispatcher)(dispatcher);

    // Register TripodCameraRenderer
    unsigned char *renderer = (unsigned char *) ::operator new(TRIPOD_CAMERA_RENDERER_SIZE);
    ALLOC_CHECK(renderer);
    (*TripodCameraRenderer)(renderer);
    (*EntityRenderDispatcher_assign)(dispatcher, (unsigned char) 0x5, renderer);

    return dispatcher;
}

// Display Smoke From TripodCamera Higher
static void TripodCamera_tick_Level_addParticle_call_injection(unsigned char *level, std::string const& particle, float x, float y, float z, float deltaX, float deltaY, float deltaZ, int count) {
    // Call Original Method
    (*Level_addParticle)(level, particle, x, y + 0.5, z, deltaX, deltaY, deltaZ, count);
}

void init_camera() {
    // Implement AppPlatform_linux::saveScreenshot So Cameras Work
    patch_address(AppPlatform_linux_saveScreenshot_vtable_addr, (void *) AppPlatform_linux_saveScreenshot_injection);

    // Enable TripodCameraRenderer
    overwrite_calls((void *) EntityRenderDispatcher, (void *) EntityRenderDispatcher_injection);
    // Display Smoke From TripodCamera Higher
    overwrite_call((void *) 0x87dc4, (void *) TripodCamera_tick_Level_addParticle_call_injection);
}