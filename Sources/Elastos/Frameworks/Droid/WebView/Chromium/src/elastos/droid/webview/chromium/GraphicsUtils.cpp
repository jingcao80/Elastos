//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/webkit/webview/chromium/GraphicsUtils.h"
#include "elastos/droid/webkit/webview/chromium/GraphicBufferImpl.h"
#include "elastos/droid/webkit/webview/chromium/PixelInfo.h"
#include <Elastos.Droid.Graphics.h>
//#include "elastos/droid/graphics/GraphicsNative.h"

using Elastos::Droid::Graphics::ICanvas;
//TODO using Elastos::Droid::Graphics::GraphicsNative;

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
    SkCanvas* canvas = NULL; //TOOD GraphicsNative::GetNativeCanvas(ca);
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


