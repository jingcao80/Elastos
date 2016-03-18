#include "elastos/droid/webkit/webview/chromium/NativeDrawGLFunctor.h"

//#define LOG_TAG "webviewchromium_plat_support"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

AwDrawGLFunction* NativeDrawGLFunctor::g_aw_drawgl_function = NULL;

NativeDrawGLFunctor::NativeDrawGLFunctor(
    /* [in] */ Int64 view_context)
    : view_context_(view_context)
{
}

NativeDrawGLFunctor::~NativeDrawGLFunctor() {
}

android::status_t NativeDrawGLFunctor::operator()(
    /* [in] */ Int32 what,
    /* [in] */ void* data)
{
    using android::uirenderer::DrawGlInfo;
    if (!g_aw_drawgl_function) {
        ALOGE("Cannot draw: no DrawGL Function installed");
        return DrawGlInfo::kStatusDone;
    }

    AwDrawGLInfo aw_info;
    aw_info.version = kAwDrawGLInfoVersion;
    switch (what) {
    case DrawGlInfo::kModeDraw: {
        aw_info.mode = AwDrawGLInfo::kModeDraw;
        DrawGlInfo* gl_info = reinterpret_cast<DrawGlInfo*>(data);

        // Map across the input values.
        aw_info.clip_left = gl_info->clipLeft;
        aw_info.clip_top = gl_info->clipTop;
        aw_info.clip_right = gl_info->clipRight;
        aw_info.clip_bottom = gl_info->clipBottom;
        aw_info.width = gl_info->width;
        aw_info.height = gl_info->height;
        aw_info.is_layer = gl_info->isLayer;
        //COMPILE_ASSERT(NELEM(aw_info.transform) == NELEM(gl_info->transform),
        //mismatched_transform_matrix_sizes);
        for (int i = 0; i < NELEM(aw_info.transform); ++i) {
            aw_info.transform[i] = gl_info->transform[i];
        }
        break;
        }
        case DrawGlInfo::kModeProcess:
            aw_info.mode = AwDrawGLInfo::kModeProcess;
            break;
        case DrawGlInfo::kModeProcessNoContext:
            aw_info.mode = AwDrawGLInfo::kModeProcessNoContext;
            break;
        case DrawGlInfo::kModeSync:
            aw_info.mode = AwDrawGLInfo::kModeSync;
            break;
        default:
            ALOGE("Unexpected DrawGLInfo type %d", what);
            return DrawGlInfo::kStatusDone;
    }
    // Invoke the DrawGL method.
    g_aw_drawgl_function(view_context_, &aw_info, NULL);

    return DrawGlInfo::kStatusDone;
}

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
