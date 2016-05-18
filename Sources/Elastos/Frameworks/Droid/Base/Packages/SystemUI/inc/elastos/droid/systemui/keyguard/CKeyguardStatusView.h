
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSTATUSVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSTATUSVIEW_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardStatusView.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/droid/widget/GridLayout.h>

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

    //TODO

    // private KeyguardUpdateMonitorCallback mInfoCallback = new KeyguardUpdateMonitorCallback() {
    //     @Override
    //     public void onTimeChanged() {
    //         refresh();
    //     }

    //     @Override
    //     public void onKeyguardVisibilityChanged(Boolean showing) {
    //         if (showing) {
    //             if (DEBUG) Slog.v(TAG, "refresh statusview showing:" + showing);
    //             refresh();
    //             updateOwnerInfo();
    //         }
    //     }

    //     @Override
    //     public void onScreenTurnedOn() {
    //         setEnableMarquee(true);
    //     }

    //     @Override
    //     public void onScreenTurnedOff(Int32 why) {
    //         setEnableMarquee(false);
    //     }

    //     @Override
    //     public void onUserSwitchComplete(Int32 userId) {
    //         refresh();
    //         updateOwnerInfo();
    //     }
    // };

public:
    CAR_OBJECT_DECL()

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
    CARAPI_(void) OnConfigurationChanged(
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
    static const String TAG;

    AutoPtr<ILockPatternUtils> mLockPatternUtils;

    AutoPtr<ITextView> mAlarmStatusView;
    AutoPtr<ITextClock> mDateView;
    AutoPtr<ITextClock> mClockView;
    AutoPtr<ITextView> mOwnerInfo;
    // AutoPtr<IKeyguardUpdateMonitorCallback> mInfoCallback;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSTATUSVIEW_H__
