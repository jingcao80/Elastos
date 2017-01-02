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

#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_ICONUTILITIES_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_ICONUTILITIES_H__

#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Utility::IDisplayMetrics;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

class IconUtilities
    : public Object
{
public:
    IconUtilities(
        /* [in] */ IContext* context);

    AutoPtr<IDrawable> CreateIconDrawable(
        /* [in] */ IDrawable* src);

    /**
     * Returns a bitmap suitable for the all apps view.  The bitmap will be a power
     * of two sized ARGB_8888 bitmap that can be used as a gl texture.
     */
    AutoPtr<IBitmap> CreateIconBitmap(
        /* [in] */ IDrawable* icon);

    AutoPtr<IBitmap> CreateSelectedBitmap(
        /* [in] */ IBitmap* src,
        /* [in] */ Boolean pressed);

private:
    static const String TAG;// = "IconUtilities";

    static const UInt32 sColors[3];// = { 0xffff0000, 0xff00ff00, 0xff0000ff };

    Int32 mIconWidth;// = -1;
    Int32 mIconHeight;// = -1;
    Int32 mIconTextureWidth;// = -1;
    Int32 mIconTextureHeight;// = -1;

    AutoPtr<IPaint> mPaint;// = new Paint();
    AutoPtr<IPaint> mBlurPaint;// = new Paint();
    AutoPtr<IPaint> mGlowColorPressedPaint;// = new Paint();
    AutoPtr<IPaint> mGlowColorFocusedPaint;// = new Paint();
    AutoPtr<IRect> mOldBounds;// = new Rect();
    AutoPtr<ICanvas> mCanvas;// = new Canvas();
    AutoPtr<IDisplayMetrics> mDisplayMetrics;

    Int32 mColorIndex;// = 0;

};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_ICONUTILITIES_H__
