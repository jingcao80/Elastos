// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_UI_DROPDOWNDIVIDERDRAWABLE_H_
#define _ELASTOS_DROID_WEBKIT_UI_DROPDOWNDIVIDERDRAWABLE_H_

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/drawable/Drawable.h"

// package org.chromium.ui;
// import android.graphics.Canvas;
// import android.graphics.ColorFilter;
// import android.graphics.Paint;
// import android.graphics.PixelFormat;
// import android.graphics.Rect;
// import android.graphics.drawable.Drawable;

using Elastos::Droid::Graphics::Drawable::Drawable;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IRect;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Ui {

class DropdownDividerDrawable
    : public Drawable
{
public:
    DropdownDividerDrawable();

    // @Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    // @Override
    // consistent with the parent class
    CARAPI_(void) OnBoundsChange(
        /* [in] */ IRect* bounds);

    virtual CARAPI SetHeight(
        /* [in] */ Int32 height);

    virtual CARAPI SetColor(
        /* [in] */ Int32 color);

    // @Override
    CARAPI SetAlpha(
        /* [in] */ Int32 alpha);

    // @Override
    CARAPI SetColorFilter(
        /* [in] */ IColorFilter* cf);

    // @Override
    CARAPI_(Int32) GetOpacity();

    // it is a pure function in parent class Drawable
    CARAPI GetOpacity(
        /* [out] */ Int32* opacity);

private:
    AutoPtr<IPaint> mPaint;
    AutoPtr<IRect> mDividerRect;
};

} // namespace Ui
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_UI_DROPDOWNDIVIDERDRAWABLE_H_

