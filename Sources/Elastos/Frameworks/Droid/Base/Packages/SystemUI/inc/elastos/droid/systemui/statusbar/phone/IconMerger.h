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

#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_ICONMERGER_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_ICONMERGER_H__

#include "_Elastos.Droid.SystemUI.h"
#include <elastos/core/Runnable.h>
#include <elastos/droid/widget/LinearLayout.h>

using Elastos::Core::Runnable;
using Elastos::Droid::Widget::LinearLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

class IconMerger
    : public LinearLayout
    , public IIconMerger
{
private:
    class IconRunnable : public Runnable
    {
    public:
        IconRunnable(
            /* [in] */ IconMerger* host,
            /* [in] */ Boolean moreRequired);

        // @Override
        CARAPI Run();

    private:
        IconMerger* mHost;
        Boolean mMoreRequired;
    };

public:
    CAR_INTERFACE_DECL()

    IconMerger();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetOverflowIndicator(
        /* [in] */ IView* v);

protected:
    // @Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

private:
    CARAPI_(void) CheckOverflow(
        /* [in] */ Int32 width);

private:
    static const String TAG;
    static const Boolean DEBUG;

    Int32 mIconSize;
    AutoPtr<IView> mMoreView;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_ICONMERGER_H__
