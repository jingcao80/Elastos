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

#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STATUSBARICONVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STATUSBARICONVIEW_H__

#include "elastos/droid/systemui/statusbar/AnimatedImageView.h"
#include "Elastos.Droid.App.h"

using Elastos::Droid::App::INotification;
using Elastos::Droid::Internal::StatusBar::IStatusBarIcon;
using Elastos::Droid::Os::IUserHandle;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

class StatusBarIconView
    : public AnimatedImageView
    , public IStatusBarIconView
{
public:
    CAR_INTERFACE_DECL()

    StatusBarIconView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ const String& slot,
        /* [in] */ INotification* notification);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetNotification(
        /* [in] */ INotification* notification);

    CARAPI Set(
        /* [in] */ IStatusBarIcon* icon,
        /* [out] */ Boolean* result);

    CARAPI UpdateDrawable();

    /**
     * Returns the right icon to use for this item, respecting the iconId and
     * iconPackage (if set)
     *
     * @param context Context to use to get resources if iconPackage is not set
     * @return Drawable for this item, or null if the package or item could not
     *         be found
     */
    static AutoPtr<IDrawable> GetIcon(
        /* [in] */ IContext* context,
        /* [in] */ IStatusBarIcon* icon);

    CARAPI GetStatusBarIcon(
        /* [out] */ IStatusBarIcon** icon);

    virtual CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI ToString(
        /* [out] */ String* str);

    using AnimatedImageView::SetContentDescription;

protected:
    virtual CARAPI OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

    virtual CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    CARAPI_(void) PlaceNumber();

    CARAPI_(Boolean) UpdateDrawable(
        /* [in] */ Boolean withClear);

private:
    static CARAPI_(Boolean) StrEQ(
        /* [in] */ const String& a,
        /* [in] */ const String& b);

    CARAPI_(AutoPtr<IDrawable>) GetIcon(
        /* [in] */ IStatusBarIcon* icon);

    CARAPI_(void) SetContentDescription(
        /* [in] */ INotification* notification);

private:
    static const String TAG;
    static const Boolean DBG;

    AutoPtr<IStatusBarIcon> mIcon;
    String mSlot;
    AutoPtr<IDrawable> mNumberBackground;
    AutoPtr<IPaint> mNumberPain;
    Int32 mNumberX;
    Int32 mNumberY;
    String mNumberText;
    AutoPtr<INotification> mNotification;
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STATUSBARICONVIEW_H__
