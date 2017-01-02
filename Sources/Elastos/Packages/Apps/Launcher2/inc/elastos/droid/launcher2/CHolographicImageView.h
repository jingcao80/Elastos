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

#ifndef  __ELASTOS_DROID_LAUNCHER2_CHOLOGRAPHICIMAGEVIEW_H__
#define  __ELASTOS_DROID_LAUNCHER2_CHOLOGRAPHICIMAGEVIEW_H__

#include "_Elastos_Droid_Launcher2_CHolographicImageView.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/HolographicViewHelper.h"
#include "elastos/droid/widget/ImageView.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Widget::ImageView;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CHolographicImageView)
    , public ImageView
    , public IHolographicImageView
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

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
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    AutoPtr<HolographicViewHelper> mHolographicHelper;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CHOLOGRAPHICIMAGEVIEW_H__