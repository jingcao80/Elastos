
#ifndef __ELASTOS_DROID_SYSTEMUI_DOZE_CDOZELOG_H__
#define __ELASTOS_DROID_SYSTEMUI_DOZE_CDOZELOG_H__

#include "_Elastos_Droid_Transition_CDozeLog.h"
#include <elastos/core/Singleton.h>

using Elastos::Text::ISimpleDateFormat;
using Elastos::Droid::Content::IContext;

using Elastos::Core::Singleton;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Transition {

CarClass (CDozeLog)
    , public Singleton
    , public IDozeLog
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

    class KeyguardUpdateMonitorCallback
    {
    public:
        KeyguardUpdateMonitorCallback();

        CARAPI OnEmergencyCallAction();

        CARAPI OnKeyguardBouncerChanged(
            /* [in] */ Boolean bouncer);

        CARAPI OnScreenTurnedOn();

        CARAPI OnScreenTurnedOff(
            /* [in] */ Int32 why);

        CARAPI OnKeyguardVisibilityChanged(
            /* [in] */ Boolean showing);
    };

public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI TracePickupPulse(
        /* [in] */ Boolean withinVibrationThreshold);

    CARAPI TracePulseStart();

    CARAPI TracePulseFinish();

    CARAPI TraceNotificationPulse(
        /* [in] */ Int64 instance);

    CARAPI TraceDozing(
        /* [in] */ IContext* context,
        /* [in] */ Boolean dozing);

    CARAPI TraceFling(
        /* [in] */ Boolean expand,
        /* [in] */ Boolean aboveThreshold,
        /* [in] */ Boolean thresholdNeeded,
        /* [in] */ Boolean screenOnFromTouch);

    CARAPI TraceEmergencyCall();

    CARAPI TraceKeyguardBouncerChanged(
        /* [in] */ Boolean showing);

    CARAPI TraceScreenOn();

    CARAPI TraceScreenOff(
        /* [in] */ Int32 why);

    CARAPI TraceKeyguard(
        /* [in] */ Boolean showing);

    CARAPI TraceProximityResult(
        /* [in] */ Boolean near,
        /* [in] */ Int64 millis);

    CARAPI Dump(
        /* [in] */ IPrintWriter* pw);

private:
    CARAPI Log(
        /* [in] */ const String& msg);

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
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_DOZE_CDOZELOG_H__