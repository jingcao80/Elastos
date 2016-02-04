
#ifndef  __ELASTOS_DROID_PACKAGES_SYSTEMUI_DOZE_DOZELOG_H__
#define  __ELASTOS_DROID_PACKAGES_SYSTEMUI_DOZE_DOZELOG_H__

#include <elastos/droid/ext/frameworkext.h>
#include "Elastos.CoreLibrary.Text.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Object;
using Elastos::IO::IPrintWriter;
using Elastos::Text::ISimpleDateFormat;

namespace Elastos {
namespace Droid {
namespace Packages {
namespace SystemUI {
namespace Doze {

class DozeLog: public Object
{
private:
    class SummaryStats
        : public Object
    {
    public:
        void Append();

        void Dump(
            /* [in] */ IPrintWriter* pw,
            /* [in] */ const String& type);

    private:
        Int32 mCount;
    };

    class KeyguardUpdateMonitorCallback: public Object
    {
    public:
        KeyguardUpdateMonitorCallback();

        void OnEmergencyCallAction();

        void OnKeyguardBouncerChanged(
            /* [in] */ Boolean bouncer);

        void OnScreenTurnedOn();

        void OnScreenTurnedOff(
            /* [in] */ Int32 why);

        void OnKeyguardVisibilityChanged(
            /* [in] */ Boolean showing);
    };

public:
    static CARAPI_(void) TracePickupPulse(
        /* [in] */ Boolean withinVibrationThreshold);

    static CARAPI_(void) TracePulseStart();

    static CARAPI_(void) TracePulseFinish();

    static CARAPI_(void) TraceNotificationPulse(
        /* [in] */ Int64 instance);

    static CARAPI_(void) TraceDozing(
        /* [in] */ IContext* context,
        /* [in] */ Boolean dozing);

    static CARAPI_(void) TraceFling(
        /* [in] */ Boolean expand,
        /* [in] */ Boolean aboveThreshold,
        /* [in] */ Boolean thresholdNeeded,
        /* [in] */ Boolean screenOnFromTouch);

    static CARAPI_(void) TraceEmergencyCall();

    static CARAPI_(void) TraceKeyguardBouncerChanged(
        /* [in] */ Boolean showing);

    static CARAPI_(void) TraceScreenOn();

    static CARAPI_(void) TraceScreenOff(
        /* [in] */ Int32 why);

    static CARAPI_(void) TraceKeyguard(
        /* [in] */ Boolean showing);

    static CARAPI_(void) TraceProximityResult(
        /* [in] */ Boolean near,
        /* [in] */ Int64 millis);

    static CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* pw);

private:
    static CARAPI_(void) Log(
        /* [in] */ const String& msg);

    static CARAPI_(Boolean) InitStatic();

private:
    static AutoPtr<KeyguardUpdateMonitorCallback> sKeyguardCallback;

    static String TAG;
    static Boolean DEBUG;
    static Boolean ENABLED;
    static Int32 SIZE;
    static AutoPtr<ISimpleDateFormat> FORMAT;

    static AutoPtr<ArrayOf<Int64> > sTimes;
    static AutoPtr<ArrayOf<String> > sMessages;
    static Int32 sPosition;
    static Int32 sCount;
    static Boolean sPulsing;

    static Int64 sSince;
    static AutoPtr<SummaryStats> sPickupPulseNearVibrationStats;
    static AutoPtr<SummaryStats> sPickupPulseNotNearVibrationStats;
    static AutoPtr<SummaryStats> sNotificationPulseStats;
    static AutoPtr<SummaryStats> sScreenOnPulsingStats;
    static AutoPtr<SummaryStats> sScreenOnNotPulsingStats;
    static AutoPtr<SummaryStats> sEmergencyCallStats;
    static AutoPtr<SummaryStats> sProxNearStats;
    static AutoPtr<SummaryStats> sProxFarStats;
    static Boolean sInit;
    static Object sDozeLog;
};

} // namespace Doze
} // namespace SystemUI
} // namespace Packages
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PACKAGES_SYSTEMUI_DOZE_DOZELOG_H__
