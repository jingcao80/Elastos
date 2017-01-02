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

#ifndef  __ELASTOS_DROID_LAUNCHER2_CHOLOGRAPHICLINEARLAYOUT_H__
#define  __ELASTOS_DROID_LAUNCHER2_CHOLOGRAPHICLINEARLAYOUT_H__

#include "_Elastos_Droid_Launcher2_CHolographicLinearLayout.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/HolographicViewHelper.h"
#include "elastos/droid/widget/LinearLayout.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::Launcher2::HolographicViewHelper;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::LinearLayout;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CHolographicLinearLayout)
    , public LinearLayout
    , public IHolographicLinearLayout
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL();

    CHolographicLinearLayout();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI InvalidatePressedFocusedStates();

protected:
    //@Override
    CARAPI DrawableStateChanged();

    //@Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    AutoPtr<HolographicViewHelper> mHolographicHelper;
    AutoPtr<IImageView> mImageView;
    Int32 mImageViewId;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CHOLOGRAPHICLINEARLAYOUT_H__