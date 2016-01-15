
#include "elastos/droid/systemui/doze/CDozeLog.h"
#include "elastos/droid/os/Build.h"

using Elastos::Droid::Os::Build;

using Elastos::Core::ISystem;

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// CDozeLog::
//===============================================================
String CDozeLog::TAG = String("DozeLog");
Boolean CDozeLog::DEBUG;// = Log.isLoggable(TAG, Log.DEBUG);
Boolean CDozeLog::ENABLED = TRUE;
Int32 CDozeLog::SIZE = Build::IS_DEBUGGABLE ? 400 : 50;
AutoPtr<ISimpleDateFormat> CDozeLog::FORMAT = new SimpleDateFormat("MM-dd HH:mm:ss.SSS");

AutoPtr<ArrayOf<Int64> > CDozeLog::sTimes;
AutoPtr<ArrayOf<String> > CDozeLog::sMessages;
Int32 CDozeLog::sPosition = 0;
Int32 CDozeLog::sCount = 0;
Boolean CDozeLog::sPulsing = FALSE;

Int64 CDozeLog::sSince = 0;
AutoPtr<SummaryStats> CDozeLog::sPickupPulseNearVibrationStats;
AutoPtr<SummaryStats> CDozeLog::sPickupPulseNotNearVibrationStats;
AutoPtr<SummaryStats> CDozeLog::sNotificationPulseStats;
AutoPtr<SummaryStats> CDozeLog::sScreenOnPulsingStats;
AutoPtr<SummaryStats> CDozeLog::sScreenOnNotPulsingStats;
AutoPtr<SummaryStats> CDozeLog::sEmergencyCallStats;
AutoPtr<SummaryStats> CDozeLog::sProxNearStats;
AutoPtr<SummaryStats> CDozeLog::sProxFarStats;

CAR_SINGLETON_IMPL(CDozeLog)

CAR_INTERFACE_IMPL(CDozeLog, Singleton, IDozeLog)

ECode CDozeLog::TracePickupPulse(
    /* [in] */ Boolean withinVibrationThreshold)
{
    if (!ENABLED) return NOERROR;
    Log("pickupPulse withinVibrationThreshold=" + withinVibrationThreshold);
    (withinVibrationThreshold ? sPickupPulseNearVibrationStats
            : sPickupPulseNotNearVibrationStats).Append();
}

ECode CDozeLog::TracePulseStart()
{
    if (!ENABLED) return NOERROR;
    sPulsing = TRUE;
    return Log("pulseStart");
}

ECode CDozeLog::TracePulseFinish()
{
    if (!ENABLED) return NOERROR;
    sPulsing = FALSE;
    return Log("pulseFinish");
}

ECode CDozeLog::TraceNotificationPulse(
    /* [in] */ Int64 instance)
{
    if (!ENABLED) return NOERROR;
    Log("notificationPulse instance=" + instance);
    sNotificationPulseStats->Append();
    return NOERROR;
}

ECode CDozeLog::TraceDozing(
    /* [in] */ IContext* context,
    /* [in] */ Boolean dozing)
{
    if (!ENABLED) return NOERROR;
    sPulsing = FALSE;
    synchronized(DozeLog.class) {
        if (dozing && sMessages == NULL) {
            sTimes = ArrayOf<Int64>::Alloc(SIZE);
            sMessages = ArrayOf<String>::Alloc(SIZE);
            sSince = System::GetCurrentTimeMillis();
            sPickupPulseNearVibrationStats = new SummaryStats();
            sPickupPulseNotNearVibrationStats = new SummaryStats();
            sNotificationPulseStats = new SummaryStats();
            sScreenOnPulsingStats = new SummaryStats();
            sScreenOnNotPulsingStats = new SummaryStats();
            sEmergencyCallStats = new SummaryStats();
            sProxNearStats = new SummaryStats();
            sProxFarStats = new SummaryStats();
            Log("init");
            KeyguardUpdateMonitor.getInstance(context).registerCallback(sKeyguardCallback);
        }
    }
    Log("dozing " + dozing);
    return NOERROR;
}

ECode CDozeLog::TraceFling(
    /* [in] */ Boolean expand,
    /* [in] */ Boolean aboveThreshold,
    /* [in] */ Boolean thresholdNeeded,
    /* [in] */ Boolean screenOnFromTouch)
{
    if (!ENABLED) return NOERROR;
    Log("fling expand=" + expand + " aboveThreshold=" + aboveThreshold + " thresholdNeeded="
            + thresholdNeeded + " screenOnFromTouch=" + screenOnFromTouch);
    return NOERROR;
}

ECode CDozeLog::TraceEmergencyCall()
{
    if (!ENABLED) return NOERROR;
    Log("emergencyCall");
    sEmergencyCallStats->Append();
    return NOERROR;
}

ECode CDozeLog::TraceKeyguardBouncerChanged(
    /* [in] */ Boolean showing)
{
    if (!ENABLED) return NOERROR;
    Log("bouncer " + showing);
    return NOERROR;
}

ECode CDozeLog::TraceScreenOn()
{
    if (!ENABLED) return NOERROR;
    Log("screenOn pulsing=" + sPulsing);
    (sPulsing ? sScreenOnPulsingStats : sScreenOnNotPulsingStats)->Append();
    sPulsing = FALSE;
    return NOERROR;
}

ECode CDozeLog::TraceScreenOff(
    /* [in] */ Int32 why)
{
    if (!ENABLED) return NOERROR;
    Log("screenOff why=" + why);
    return NOERROR;
}

ECode CDozeLog::TraceKeyguard(
    /* [in] */ Boolean showing)
{
    if (!ENABLED) return NOERROR;
    Log("keyguard " + showing);
    if (!showing) {
        sPulsing = FALSE;
    }
    return NOERROR;
}

ECode CDozeLog::TraceProximityResult(
    /* [in] */ Boolean near,
    /* [in] */ Int64 millis)
{
    if (!ENABLED) return NOERROR;
    Log("proximityResult near=" + near + " millis=" + millis);
    (near ? sProxNearStats : sProxFarStats)->Append();
    return NOERROR;
}

ECode CDozeLog::Dump(
    /* [in] */ IPrintWriter* pw)
{
    synchronized(DozeLog.class) {
        if (sMessages == NULL) return NOERROR;
        pw->Println("  Doze log:");
        Int32 start = (sPosition - sCount + SIZE) % SIZE;
        for (Int32 i = 0; i < sCount; i++) {
            Int32 j = (start + i) % SIZE;
            pw->Print("    ");
            pw->Print(FORMAT.format(new Date(sTimes[j])));
            pw->Print(' ');
            pw->Println(sMessages[j]);
        }
        pw->Print("  Doze summary stats (for ");
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        Int64 when = 0;
        system->GetCurrentTimeMillis(&when);
        TimeUtils::FormatDuration(when - sSince, pw);
        pw->Println("):");
        sPickupPulseNearVibrationStats->Dump(pw, "Pickup pulse (near vibration)");
        sPickupPulseNotNearVibrationStats->Dump(pw, "Pickup pulse (not near vibration)");
        sNotificationPulseStats->Dump(pw, "Notification pulse");
        sScreenOnPulsingStats->Dump(pw, "Screen on (pulsing)");
        sScreenOnNotPulsingStats->Dump(pw, "Screen on (not pulsing)");
        sEmergencyCallStats->Dump(pw, "Emergency call");
        sProxNearStats->Dump(pw, "Proximity (near)");
        sProxFarStats->Dump(pw, "Proximity (far)");
    }
    return NOERROR;
}

ECode CDozeLog::Log(
    /* [in] */ const String& msg)
{
//    synchronized(DozeLog.class) {
        if (sMessages == NULL) return NOERROR;
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        system->GetCurrentTimeMillis(&((*sTimes)[sPosition]));
        (*sMessages)[sPosition] = msg;
        sPosition = (sPosition + 1) % SIZE;
        sCount = Elastos::Core::Math::Min(sCount + 1, SIZE);
//    }
    if (DEBUG) Logger::D(TAG, msg);
}

//===============================================================
// CDozeLog::SummaryStats::
//===============================================================

void CDozeLog::SummaryStats::Append()
{
    mCount++;
}

void CDozeLog::SummaryStats::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& type)
{
    pw->Print("    ");
    pw->Print(type);
    pw->Print(": n=");
    pw->Print(mCount);
    pw->Print(" (");
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 mills = 0;
    system->GetCurrentTimeMillis(&mills);
    double perHr = (double) mCount / (mills - sSince)
            * 1000 * 60 * 60;
    pw->Print(perHr);
    pw->Print("/hr)");
    pw->Println();
}

//===============================================================
// CDozeLog::KeyguardUpdateMonitorCallback::
//===============================================================

ECode CDozeLog::KeyguardUpdateMonitorCallback::OnEmergencyCallAction()
{
    TraceEmergencyCall();
}

ECode CDozeLog::KeyguardUpdateMonitorCallback::OnKeyguardBouncerChanged(
    /* [in] */ Boolean bouncer)
{
    TraceKeyguardBouncerChanged(bouncer);
}

ECode CDozeLog::KeyguardUpdateMonitorCallback::OnScreenTurnedOn()
{
    TraceScreenOn();
}

ECode CDozeLog::KeyguardUpdateMonitorCallback::OnScreenTurnedOff(
    /* [in] */ Int32 why)
{
    TraceScreenOff(why);
}

ECode CDozeLog::KeyguardUpdateMonitorCallback::OnKeyguardVisibilityChanged(
    /* [in] */ Boolean showing)
{
    TraceKeyguard(showing);
}
