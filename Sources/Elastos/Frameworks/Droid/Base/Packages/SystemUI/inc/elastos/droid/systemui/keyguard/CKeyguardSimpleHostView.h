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

#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSIMPLEHOSTVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSIMPLEHOSTVIEW_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardSimpleHostView.h"
#include "elastos/droid/systemui/keyguard/KeyguardViewBase.h"
#include "elastos/droid/systemui/keyguard/KeyguardUpdateMonitorCallback.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CKeyguardSimpleHostView)
    , public KeyguardViewBase
{
private:
    class MyKeyguardUpdateMonitorCallback
        : public KeyguardUpdateMonitorCallback
    {
    public:
        TO_STRING_IMPL("CKeyguardSimpleHostView::MyKeyguardUpdateMonitorCallback")

        MyKeyguardUpdateMonitorCallback(
            /* [in] */ CKeyguardSimpleHostView* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnUserSwitchComplete(
            /* [in] */ Int32 userId);

        //@Override
        CARAPI OnTrustInitiatedByUser(
            /* [in] */ Int32 userId);

    private:
        CKeyguardSimpleHostView* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CKeyguardSimpleHostView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    //@Override
    CARAPI CleanUp();

    //@Override
    CARAPI GetUserActivityTimeout(
        /* [out] */ Int64* time);

protected:
    //@Override
    CARAPI ShowBouncer(
        /* [in] */ Boolean show);

    //@Override
    CARAPI OnUserSwitching(
        /* [in] */ Boolean switching);

    //@Override
    CARAPI OnCreateOptions(
        /* [in] */ IBundle* options);

    //@Override
    CARAPI OnExternalMotionEvent(
        /* [in] */ IMotionEvent* event);

private:
    AutoPtr<IKeyguardUpdateMonitorCallback> mUpdateCallback;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSIMPLEHOSTVIEW_H__
