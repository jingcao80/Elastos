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

#ifndef __ELASTOS_DROID_APP_FASTBITMAPDRAWABLE_H__
#define __ELASTOS_DROID_APP_FASTBITMAPDRAWABLE_H__

#include "Elastos.Droid.App.h"
#include "elastos/droid/graphics/drawable/Drawable.h"

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Graphics::Drawable::Drawable;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Special drawable that draws a wallpaper as fast as possible.  Assumes
 * no scaling or placement off (0,0) of the wallpaper (this should be done
 * at the time the bitmap is loaded).
 */
class FastBitmapDrawable
    : public Elastos::Droid::Graphics::Drawable::Drawable
    , public IFastBitmapDrawable
{
public:
    CAR_INTERFACE_DECL()

    FastBitmapDrawable();

    CARAPI constructor(
        /* [in] */ IBitmap* bitmap);

    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    CARAPI GetOpacity(
        /* [out] */ Int32* result);

    CARAPI SetBounds(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    CARAPI SetAlpha(
        /* [in] */ Int32 alpha)
    {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    CARAPI SetColorFilter(
        /* [in] */ IColorFilter* cf)
    {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    CARAPI SetDither(
        /* [in] */ Boolean dither)
    {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    CARAPI SetFilterBitmap(
        /* [in] */ Boolean filter)
    {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    CARAPI GetIntrinsicWidth(
        /* [out] */ Int32* result);

    CARAPI GetIntrinsicHeight(
        /* [out] */ Int32* result);

    CARAPI GetMinimumWidth(
        /* [out] */ Int32* result);

    CARAPI GetMinimumHeight(
        /* [out] */ Int32* result);

private:
    AutoPtr<IBitmap> mBitmap;
    Int32 mWidth;
    Int32 mHeight;
    Int32 mDrawLeft;
    Int32 mDrawTop;
    AutoPtr<IPaint> mPaint;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_FASTBITMAPDRAWABLE_H__
