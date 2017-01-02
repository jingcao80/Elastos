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

#ifndef __ELASTOS_DROID_PREFERENCE_PREFERENCEFRAMELAYOUT_H__
#define __ELASTOS_DROID_PREFERENCE_PREFERENCEFRAMELAYOUT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/FrameLayout.h"

using Elastos::Droid::Graphics::Drawable::IDrawableCallback;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::IKeyEventCallback;
using Elastos::Droid::View::Accessibility::IAccessibilityEventSource;
using Elastos::Droid::Widget::IFrameLayout;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::IFrameLayoutLayoutParams;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Content::Res::ITypedArray;

namespace Elastos {
namespace Droid {
namespace Preference {

class PreferenceFrameLayout
    : public FrameLayout
    , public IPreferenceFrameLayout
{
public:
    class PreferenceFrameLayoutLayoutParams
        : public FrameLayout::FrameLayoutLayoutParams
        , public IPreferenceFrameLayoutParams
    {
    public:
        CAR_INTERFACE_DECL()

        PreferenceFrameLayoutLayoutParams();

        CARAPI constructor(
            /* [in] */ IContext* c,
            /* [in] */ IAttributeSet* attrs);

        CARAPI constructor(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI GetGravity(
            /* [out] */ Int32* gravity);

        CARAPI SetGravity(
            /* [in] */ Int32 gravity);

        CARAPI SetRemoveBorders(
            /* [in] */ Boolean res);

        CARAPI GetRemoveBorders(
            /* [out] */ Boolean* res);

    public:
        Boolean mRemoveBorders;
    };

public:
    CAR_INTERFACE_DECL()

    PreferenceFrameLayout();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** params);

    CARAPI AddView(
        /* [in] */ IView* child);

private:
    static const Int32 DEFAULT_BORDER_TOP = 0;
    static const Int32 DEFAULT_BORDER_BOTTOM = 0;
    static const Int32 DEFAULT_BORDER_LEFT = 0;
    static const Int32 DEFAULT_BORDER_RIGHT = 0;

    Int32 mBorderTop;
    Int32 mBorderBottom;
    Int32 mBorderLeft;
    Int32 mBorderRight;
    Boolean mPaddingApplied;
};

} // Preference
} // Droid
} // Elastos

#endif  // __ELASTOS_DROID_PREFERENCE_PREFERENCEFRAMELAYOUT_H__
