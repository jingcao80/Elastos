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

#ifndef __ELASTOS_DROID_SYSTEMUI_QS_TILES_CUSERDETAILITEMVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_TILES_CUSERDETAILITEMVIEW_H__

#include "_Elastos_Droid_SystemUI_Qs_Tiles_CUserDetailItemView.h"
#include "Elastos.Droid.Graphics.h"
#include <elastos/droid/widget/LinearLayout.h>

using Elastos::Droid::Graphics::ITypeface;
using Elastos::Droid::SystemUI::StatusBar::Phone::IUserAvatarView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::LinearLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

/**
 * Displays one user in the {@link UserDetailView} view.
 */
CarClass(CUserDetailItemView)
    , public LinearLayout
    , public IUserDetailItemView
{
public:
    CAR_OBJECT_DECL()

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

    static CARAPI_(AutoPtr<IUserDetailItemView>) ConvertOrInflate(
        /* [in] */ IContext* context,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* root);

    CARAPI Bind(
        /* [in] */ const String& name,
        /* [in] */ IBitmap* picture);

    CARAPI Bind(
        /* [in] */ const String& name,
        /* [in] */ IDrawable* picture);

    // @Override
    CARAPI HasOverlappingRendering(
        /* [out] */ Boolean* res);

protected:
    // @Override
    CARAPI OnFinishInflate();

    // @Override
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    // @Override
    CARAPI DrawableStateChanged();

private:
    CARAPI_(void) UpdateTypeface();

private:
    AutoPtr<IUserAvatarView> mAvatar;
    AutoPtr<ITextView> mName;
    AutoPtr<ITypeface> mRegularTypeface;
    AutoPtr<ITypeface> mActivatedTypeface;
};

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_TILES_CUSERDETAILITEMVIEW_H__
