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

#ifndef  __ELASTOS_DROID_LAUNCHER2_FASTBITMAPDRAWABLE_H__
#define  __ELASTOS_DROID_LAUNCHER2_FASTBITMAPDRAWABLE_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/drawable/Drawable.h"
#include "Elastos.Droid.Graphics.h"

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Graphics::Drawable::Drawable;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class FastBitmapDrawable
    : public Drawable
    , public IFastBitmapDrawable
{
public:
    TO_STRING_IMPL("FastBitmapDrawable")

    CAR_INTERFACE_DECL();

    FastBitmapDrawable();

    CARAPI constructor(
        /* [in] */ IBitmap* b);

    //@Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    //@Override
    CARAPI SetColorFilter(
        /* [in] */ IColorFilter* cf);

    //@Override
    CARAPI GetOpacity(
        /* [out] */ Int32* opacity);

    //@Override
    CARAPI SetAlpha(
        /* [in] */ Int32 alpha);

    CARAPI SetFilterBitmap(
        /* [in] */ Boolean filterBitmap);

    CARAPI GetAlpha(
        /* [out] */ Int32* alpha);

    //@Override
    CARAPI GetIntrinsicWidth(
        /* [out] */ Int32* width);

    //@Override
    CARAPI GetIntrinsicHeight(
        /* [out] */ Int32* height);

    //@Override
    CARAPI GetMinimumWidth(
        /* [out] */ Int32* width);

    //@Override
    CARAPI GetMinimumHeight(
        /* [out] */ Int32* height);

    CARAPI SetBitmap(
        /* [in] */ IBitmap* b);

    CARAPI GetBitmap(
        /* [out] */ IBitmap** map);

private:
    AutoPtr<IBitmap> mBitmap;
    Int32 mAlpha;
    Int32 mWidth;
    Int32 mHeight;
    AutoPtr<IPaint> mPaint;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_FASTBITMAPDRAWABLE_H__