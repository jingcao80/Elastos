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

#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSTATUSVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSTATUSVIEW_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardStatusView.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/droid/widget/GridLayout.h>
#include "elastos/droid/systemui/keyguard/KeyguardUpdateMonitorCallback.h"

using Elastos::Droid::App::IAlarmClockInfo;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Widget::GridLayout;
using Elastos::Droid::Widget::ITextClock;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CKeyguardStatusView)
    , public GridLayout
    , public IKeyguardStatusView
{
private:
    // DateFormat.getBestDateTimePattern is extremely expensive, and refresh is called often.
    // This is an optimization to ensure we only recompute the patterns when the inputs change.
    class Patterns: public Object
    {
    public:
        static CARAPI_(void) Update(
            /* [in] */ IContext* context,
            /* [in] */ Boolean hasAlarm);

    public:
        static String mDateView;
        static String mClockView12;
        static String mClockView24;
        static String mCacheKey;
    };

    class MyKeyguardUpdateMonitorCallback
        : public KeyguardUpdateMonitorCallback
    {
    public:
        MyKeyguardUpdateMonitorCallback(
            /* [in] */ CKeyguardStatusView* host);

        CARAPI OnTimeChanged();

        CARAPI OnKeyguardVisibilityChanged(
            /* [in] */ Boolean showing);

        CARAPI OnScreenTurnedOn();

        CARAPI OnScreenTurnedOff(
            /* [in] */ Int32 why);

        CARAPI OnUserSwitchComplete(
            /* [in] */ Int32 userId);
    private:
        CKeyguardStatusView* mHost;
    };

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
        /* [in] */ Int32 defStyle);

    CARAPI RefreshTime();

    CARAPI_(void) RefreshAlarmStatus(
        /* [in] */ IAlarmClockInfo* nextAlarm);

    static CARAPI_(String) FormatNextAlarm(
        /* [in] */ IContext* context,
        /* [in] */ IAlarmClockInfo* info);

    CARAPI GetAppWidgetId(
        /* [out] */ Int32* id);

    // @Override
    CARAPI HasOverlappingRendering(
        /* [out] */ Boolean* result);

protected:
    // @Override
    CARAPI OnFinishInflate();

    // @Override
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    // @Override
    CARAPI OnAttachedToWindow();

    // @Override
    CARAPI OnDetachedFromWindow();

private:
    CARAPI_(void) SetEnableMarquee(
        /* [in] */ Boolean enabled);

    CARAPI_(void) Refresh();

    CARAPI_(void) UpdateOwnerInfo();

    CARAPI_(String) GetOwnerInfo();

private:
    static const Boolean DEBUG;

    AutoPtr<ILockPatternUtils> mLockPatternUtils;

    AutoPtr<ITextView> mAlarmStatusView;
    AutoPtr<ITextClock> mDateView;
    AutoPtr<ITextClock> mClockView;
    AutoPtr<ITextView> mOwnerInfo;
    AutoPtr<IKeyguardUpdateMonitorCallback> mInfoCallback;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSTATUSVIEW_H__
