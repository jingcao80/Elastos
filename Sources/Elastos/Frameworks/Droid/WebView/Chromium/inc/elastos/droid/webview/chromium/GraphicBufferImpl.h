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

// Provides the implementation of the GraphicBuffer interface in
// renderer compostior

#ifndef __ELASTOS_DROID_WEBVIEW_CHROMIUM_GRAPHIC_BUFFER_IMPL_H__
#define __ELASTOS_DROID_WEBVIEW_CHROMIUM_GRAPHIC_BUFFER_IMPL_H__

#include "elastos/droid/ext/frameworkext.h"
#include <ui/GraphicBuffer.h>
#include "android_webview/public/browser/draw_gl.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {


class GraphicBufferImpl {
public:
    ~GraphicBufferImpl();

    static CARAPI_(long) Create(Int32 w, Int32 h);
    static CARAPI_(void) Release(long buffer_id);
    static CARAPI_(Int32) MapStatic(long buffer_id, AwMapMode mode, void** vaddr);
    static CARAPI_(Int32) UnmapStatic(long buffer_id);
    static CARAPI_(void*) GetNativeBufferStatic(long buffer_id);
    static CARAPI_(UInt32) GetStrideStatic(long buffer_id);

private:
    CARAPI_(android::status_t) Map(AwMapMode mode, void** vaddr);
    CARAPI_(android::status_t) Unmap();
    CARAPI_(android::status_t) InitCheck() const;
    CARAPI_(void*) GetNativeBuffer() const;
    CARAPI_(UInt32) GetStride() const;
    GraphicBufferImpl(UInt32 w, UInt32 h);

    android::sp<android::GraphicBuffer> mBuffer;
};

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_WEBVIEW_CHROMIUM_GRAPHIC_BUFFER_IMPL_H__

