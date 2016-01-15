
#include "elastos/droid/webkit/webview/chromium/GraphicsUtils.h"
#include "elastos/droid/webkit/webview/chromium/GraphicBufferImpl.h"
#include "elastos/droid/webkit/webview/chromium/PixelInfo.h"
#include "elastos/droid/graphics/GraphicsNative.h"
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::GraphicsNative;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

//=====================================================================
//                            GraphicsUtils
//=====================================================================
Int64 GraphicsUtils::GetDrawSWFunctionTable()
{
    // ==================before translated======================
    // return nativeGetDrawSWFunctionTable();

    return NativeGetDrawSWFunctionTable();
}

Int64 GraphicsUtils::GetDrawGLFunctionTable()
{
    // ==================before translated======================
    // return nativeGetDrawGLFunctionTable();

    return NativeGetDrawGLFunctionTable();
}

AwPixelInfo* GetPixels(IInterface* _canvas) {
    AutoPtr<ICanvas> ca = ICanvas::Probe(_canvas);
    SkCanvas* canvas = GraphicsNative::GetNativeCanvas(ca);
    if (!canvas)
        return NULL;

    // Workarounds for http://crbug.com/271096: SW draw only supports
    // translate & scale transforms, and a simple rectangular clip.
    // (This also avoids significant wasted time in calling
    // SkCanvasStateUtils::CaptureCanvasState when the clip is complex).
    if (!canvas->isClipRect() ||
            (canvas->getTotalMatrix().getType() &
             ~(SkMatrix::kTranslate_Mask | SkMatrix::kScale_Mask))) {
        return NULL;
    }

    PixelInfo* pixels = new PixelInfo(canvas);
    if (!pixels->state) {
        delete pixels;
        pixels = NULL;
    }
    return pixels;
}

void ReleasePixels(AwPixelInfo* pixels) {
      delete static_cast<PixelInfo*>(pixels);
}

Int64 GraphicsUtils::NativeGetDrawSWFunctionTable()
{
    static AwDrawSWFunctionTable function_table;
    function_table.version = kAwDrawSWFunctionTableVersion;
    function_table.access_pixels = &GetPixels;
    function_table.release_pixels = &ReleasePixels;
    return reinterpret_cast<intptr_t>(&function_table);
}

Int64 GraphicsUtils::NativeGetDrawGLFunctionTable()
{
    static AwDrawGLFunctionTable function_table;
    function_table.version = kAwDrawGLFunctionTableVersion;
    function_table.create_graphic_buffer = &GraphicBufferImpl::Create;
    function_table.release_graphic_buffer = &GraphicBufferImpl::Release;
    function_table.map = &GraphicBufferImpl::MapStatic;
    function_table.unmap = &GraphicBufferImpl::UnmapStatic;
    function_table.get_native_buffer = &GraphicBufferImpl::GetNativeBufferStatic;
    function_table.get_stride = &GraphicBufferImpl::GetStrideStatic;
    return reinterpret_cast<intptr_t>(&function_table);
}

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


