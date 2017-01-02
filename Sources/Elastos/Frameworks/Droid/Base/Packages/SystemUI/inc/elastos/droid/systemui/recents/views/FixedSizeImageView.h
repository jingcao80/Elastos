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

#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_FIXEDSIZEIMAGEVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_FIXEDSIZEIMAGEVIEW_H__

#include "_Elastos.Droid.SystemUI.h"
#include <elastos/droid/widget/ImageView.h>

using Elastos::Droid::Widget::ImageView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

/**
 * This is an optimized ImageView that does not trigger a requestLayout() or invalidate() when
 * setting the image to Null.
 */
class FixedSizeImageView
    : public ImageView
    , public IFixedSizeImageView
{
public:
    FixedSizeImageView();

    CAR_INTERFACE_DECL()

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

    // @Override
    CARAPI RequestLayout();

    // @Override
    CARAPI Invalidate();

    // @Override
    CARAPI SetImageDrawable(
        /* [in] */ IDrawable* drawable);

public:
    Boolean mAllowRelayout;
    Boolean mAllowInvalidate;
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_FIXEDSIZEIMAGEVIEW_H__
