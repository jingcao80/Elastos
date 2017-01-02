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

#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_MULTIWAVEVIEW_TARGETDRAWABLE_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_MULTIWAVEVIEW_TARGETDRAWABLE_H__

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/ext/frameworkext.h"

#include <elastos/core/Object.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::Drawable::IDrawable;

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {
namespace Multiwaveview {

class TargetDrawable
    : public Object
    , public ITargetDrawable
{
public:
    CAR_INTERFACE_DECL()

    TargetDrawable();

    TargetDrawable(
        /* [in] */ ITargetDrawable* other);

    CARAPI constructor(
        /* [in] */ IResources* res,
        /* [in] */ Int32 resId);

    CARAPI SetDrawable(
        /* [in] */ IResources* res,
        /* [in] */ Int32 resId);

    CARAPI SetState(
        /* [in] */ ArrayOf<Int32>* state);

    CARAPI HasState(
        /* [in] */ ArrayOf<Int32>* state,
        /* [out] */ Boolean* result);

    /**
     * Returns true if the drawable is a StateListDrawable and is in the focused state.
     *
     * @return
     */
    CARAPI IsActive(
        /* [out] */ Boolean* result);

    /**
     * Returns true if this target is enabled. Typically an enabled target contains a valid
     * drawable in a valid state. Currently all targets with valid drawables are valid.
     *
     * @return
     */
    CARAPI IsEnabled(
        /* [out] */ Boolean* result);

    CARAPI SetX(
        /* [in] */ Float x);

    CARAPI SetY(
        /* [in] */ Float y);

    CARAPI SetScaleX(
        /* [in] */ Float x);

    CARAPI SetScaleY(
        /* [in] */ Float y);

    CARAPI SetAlpha(
        /* [in] */ Float alpha);

    CARAPI GetX(
        /* [out] */ Float* result);

    CARAPI GetY(
        /* [out] */ Float* result);

    CARAPI GetScaleX(
        /* [out] */ Float* result);

    CARAPI GetScaleY(
        /* [out] */ Float* result);

    CARAPI GetAlpha(
        /* [out] */ Float* result);

    CARAPI SetPositionX(
        /* [in] */ Float x);

    CARAPI SetPositionY(
        /* [in] */ Float y);

    CARAPI GetPositionX(
        /* [out] */ Float* result);

    CARAPI GetPositionY(
        /* [out] */ Float* result);

    CARAPI GetWidth(
        /* [out] */ Int32* result);

    CARAPI GetHeight(
        /* [out] */ Int32* result);

    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    CARAPI GetResourceId(
        /* [out] */ Int32* result);

private:
    /**
     * Makes drawables in a StateListDrawable all the same dimensions.
     * If not a StateListDrawable, then justs sets the bounds to the intrinsic size of the
     * drawable.
     */
    CARAPI_(void) ResizeDrawables();

public:
    static AutoPtr<ArrayOf<Int32> > STATE_ACTIVE;
    static AutoPtr<ArrayOf<Int32> > STATE_INACTIVE;
    static AutoPtr<ArrayOf<Int32> > STATE_FOCUSED;

private:
    static const String TAG;
    static const Boolean DEBUG;
    Float mTranslationX;
    Float mTranslationY;
    Float mPositionX;
    Float mPositionY;
    Float mScaleX;
    Float mScaleY;
    Float mAlpha;
    AutoPtr<IDrawable> mDrawable;
    Boolean mEnabled;
    Int32 mResourceId;
};

} // namespace Multiwaveview
} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_MULTIWAVEVIEW_TARGETDRAWABLE_H__
