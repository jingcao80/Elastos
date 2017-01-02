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

#ifndef __ELASTOS_DROID_VIEW_HARDWARECANVAS_H__
#define __ELASTOS_DROID_VIEW_HARDWARECANVAS_H__

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/graphics/Canvas.h"

using Elastos::Droid::Graphics::Canvas;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IRect;

namespace Elastos {
namespace Droid {
namespace View {

class HardwareCanvas
    : public Canvas
    , public IHardwareCanvas
{
public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("HardwareCanvas")

    CARAPI constructor();

    CARAPI IsHardwareAccelerated(
        /* [out] */ Boolean* isAccelerated);

    CARAPI SetBitmap(
        /* [in] */ IBitmap* bitmap);

    /**
     * Draws the specified display list onto this canvas. The display list can only
     * be drawn if {@link android.view.RenderNode#isValid()} returns true.
     *
     * @param renderNode The RenderNode to replay.
     */
    CARAPI DrawRenderNode(
        /* [in] */ IRenderNode* renderNode);

    virtual CARAPI DrawRenderNode(
        /* [in] */ IRenderNode* renderNode,
        /* [in] */ IRect* dirty,
        /* [in] */ Int32 flags,
        /* [out] */ Int32* result) = 0;

    /**
     * Calls the function specified with the drawGLFunction function pointer. This is
     * functionality used by webkit for calling into their renderer from our display lists.
     * This function may return true if an invalidation is needed after the call.
     *
     * @param drawGLFunction A native function pointer
     *
     * @return {@link RenderNode#STATUS_DONE}
     *
     * @hide
     */
    CARAPI CallDrawGLFunction(
        /* [in] */ Int64 drawGLFunction,
        /* [in] */ Int32* res);

    static CARAPI SetProperty(
        /* [in] */ const String& name,
        /* [in] */ const String& value);
};

} // namespace Elastos
} // namespace Droid
} // namespace View

#endif