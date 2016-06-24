
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMSERVICESTATETRACKER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMSERVICESTATETRACKER_H__

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Telephony.h"
#include "_Elastos_Droid_Internal_Telephony_Gsm_CGsmServiceStateTracker.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/internal/telephony/ServiceStateTracker.h"
#include "elastos/droid/os/AsyncResult.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::INotification;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Internal::Telephony::DataConnection::IDcTrackerBase;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::ServiceStateTracker;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Telephony::ICellLocation;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::Gsm::IGsmCellLocation;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::ITimeZone;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

/**
 * {@hide}
 */
CarClass(CGsmServiceStateTracker)
    , public ServiceStateTracker
    , public IGsmServiceStateTracker
{
private:
    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        MyBroadcastReceiver(
            /* [in] */ CGsmServiceStateTracker* host)
            : mHost(host)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CGsmServiceStateTracker* mHost;
    };

    class AutoTimeContentObserver
        : public ContentObserver
    {
    public:
        AutoTimeContentObserver(
            /* [in] */ IHandler* handler,
            /* [in] */ CGsmServiceStateTracker* host)
            : mHost(host)
        {
            ContentObserver::constructor(handler);
        }

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        CGsmServiceStateTracker* mHost;
    };

    class AutoTimeZoneContentObserver
        : public ContentObserver
    {
    public:
        AutoTimeZoneContentObserver(
            /* [in] */ IHandler* handler,
            /* [in] */ CGsmServiceStateTracker* host)
            : mHost(host)
        {
            ContentObserver::constructor(handler);
        }

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        CGsmServiceStateTracker* mHost;
    };

public:
    CGsmServiceStateTracker();

    virtual ~CGsmServiceStateTracker();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IGSMPhone* phone);

    CARAPI Dispose();

    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    /**
     * A complete "service state" from our perspective is
     * composed of a handful of separate requests to the radio.
     *
     * We make all of these requests at once, but then abandon them
     * and start over again if the radio notifies us that some
     * event has changed
     */
    CARAPI PollState();

    /**
     * @return The current GPRS state. IN_SERVICE is the same as "attached"
     * and OUT_OF_SERVICE is the same as detached.
     */
    CARAPI GetCurrentDataConnectionState(
        /* [out] */ Int32* result);

    /**
     * @return true if phone is camping on a technology (eg UMTS)
     * that could support voice and data simultaneously or
     * concurrent services support indicator is set to '1'.
     */
    CARAPI IsConcurrentVoiceAndDataAllowed(
        /* [out] */ Boolean* result);

    /**
     * @return the current cell location information. Prefer Gsm location
     * information if available otherwise return LTE location information
     */
    CARAPI GetCellLocation(
        /* [out] */ ICellLocation** result);

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    /**
     * Clean up existing voice and data connection then turn off radio power.
     *
     * Hang up the existing voice calls to decrease call drop rate.
     */
    CARAPI PowerOffRadioSafely(
        /* [in] */ IDcTrackerBase* dcTracker);

    CARAPI SetImsRegistrationState(
        /* [in] */ Boolean registered);

protected:
    CARAPI GetPhone(
        /* [out] */ IPhone** result);

    CARAPI SetPowerStateToDesired();

    CARAPI HangupAndPowerOff();

    CARAPI UpdateSpnDisplay();

    /**
     * Handle the result of one of the pollState()-related requests
     */
    CARAPI HandlePollStateResult(
        /* [in] */ Int32 what,
        /* [in] */ AsyncResult* ar);

    CARAPI OnUpdateIccAvailability();

    CARAPI Log(
        /* [in] */ const String& s);

    CARAPI Loge(
        /* [in] */ const String& s);

private:
    /**
     * Consider dataRegState if voiceRegState is OOS to determine SPN to be
     * displayed
     */
    CARAPI_(Int32) GetCombinedRegState();

    CARAPI_(void) SetSignalStrengthDefaultValues();

    CARAPI_(void) PollStateDone();

    /**
     * Check if GPRS got registered while voice is registered.
     *
     * @param dataRegState i.e. CGREG in GSM
     * @param voiceRegState i.e. CREG in GSM
     * @return FALSE if device only register to voice but not gprs
     */
    CARAPI_(Boolean) IsGprsConsistent(
        /* [in] */ Int32 dataRegState,
        /* [in] */ Int32 voiceRegState);

    /**
     * Returns a TimeZone object based only on parameters from the NITZ string.
     */
    CARAPI_(AutoPtr<ITimeZone>) GetNitzTimeZone(
        /* [in] */ Int32 offset,
        /* [in] */ Boolean dst,
        /* [in] */ Int64 when);

    CARAPI_(AutoPtr<ITimeZone>) FindTimeZone(
        /* [in] */ Int32 offset,
        /* [in] */ Boolean dst,
        /* [in] */ Int64 when);

    CARAPI_(void) QueueNextSignalStrengthPoll();

    /**
     * Set restricted state based on the OnRestrictedStateChanged notification
     * If any voice or packet restricted state changes, trigger a UI
     * notification and notify registrants when sim is ready.
     *
     * @param ar an Int32 value of RIL_RESTRICTED_STATE_*
     */
    CARAPI_(void) OnRestrictedStateChanged(
        /* [in] */ AsyncResult* ar);

    /** code is registration state 0-5 from TS 27.007 7.2 */
    CARAPI_(Int32) RegCodeToServiceState(
        /* [in] */ Int32 code);

    /**
     * code is registration state 0-5 from TS 27.007 7.2
     * returns true if registered roam, FALSE otherwise
     */
    CARAPI_(Boolean) RegCodeIsRoaming(
        /* [in] */ Int32 code);

    /**
     * Set roaming state if operator mcc is the same as sim mcc
     * and ons is different from spn
     *
     * @param s ServiceState hold current ons
     * @return true if same operator
     */
    CARAPI_(Boolean) IsSameNamedOperators(
        /* [in] */ IServiceState* s);

    /**
     * Compare SIM MCC with Operator MCC
     *
     * @param s ServiceState hold current ons
     * @return true if both are same
     */
    CARAPI_(Boolean) CurrentMccEqualsSimMcc(
        /* [in] */ IServiceState* s);

    /**
     * Do not set roaming state in case of oprators considered non-roaming.
     *
     + Can use mcc or mcc+mnc as item of config_operatorConsideredNonRoaming.
     * For example, 302 or 21407. If mcc or mcc+mnc match with operator,
     * don't set roaming state.
     *
     * @param s ServiceState hold current ons
     * @return FALSE for roaming state set
     */
    CARAPI_(Boolean) IsOperatorConsideredNonRoaming(
        /* [in] */ IServiceState* s);

    CARAPI_(Boolean) IsOperatorConsideredRoaming(
        /* [in] */ IServiceState* s);

    /**
     * nitzReceiveTime is time_t that the NITZ time was posted
     */
    CARAPI_(void) SetTimeFromNITZString(
        /* [in] */ const String& nitz,
        /* [in] */ Int64 nitzReceiveTime);

    CARAPI_(Boolean) GetAutoTime();

    CARAPI_(Boolean) GetAutoTimeZone();

    CARAPI_(void) SaveNitzTimeZone(
        /* [in] */ const String& zoneId);

    CARAPI_(void) SaveNitzTime(
        /* [in] */ Int64 time);

    /**
     * Set the timezone and send out a sticky broadcast so the system can
     * determine if the timezone was set by the carrier.
     *
     * @param zoneId timezone set by carrier
     */
    CARAPI_(void) SetAndBroadcastNetworkSetTimeZone(
        /* [in] */ const String& zoneId);

    /**
     * Set the time and Send out a sticky broadcast so the system can determine
     * if the time was set by the carrier.
     *
     * @param time time set by network
     */
    CARAPI_(void) SetAndBroadcastNetworkSetTime(
        /* [in] */ Int64 time);

    CARAPI_(void) RevertToNitzTime();

    CARAPI_(void) RevertToNitzTimeZone();

    /**
     * Post a notification to NotificationManager for restricted state
     *
     * @param notifyType is one state of PS/CS_*_ENABLE/DISABLE
     */
    CARAPI_(void) SetNotification(
        /* [in] */ Int32 notifyType);

    CARAPI_(AutoPtr<IUiccCardApplication>) GetUiccCardApplication();

public:
    static const String TAG; // = "GsmSST";
    static const Boolean VDBG = FALSE;
    AutoPtr<IGsmCellLocation> mCellLoc;
    AutoPtr<IGsmCellLocation> mNewCellLoc;
    Int32 mPreferredNetworkType;
    String mSavedTimeZone;
    Int64 mSavedTime;
    Int64 mSavedAtTime;

    /** Notification type. */
    static const Int32 PS_ENABLED = 1001;            // Access Control blocks data service
    static const Int32 PS_DISABLED = 1002;           // Access Control enables data service
    static const Int32 CS_ENABLED = 1003;            // Access Control blocks all voice/sms service
    static const Int32 CS_DISABLED = 1004;           // Access Control enables all voice/sms service
    static const Int32 CS_NORMAL_ENABLED = 1005;     // Access Control blocks normal voice/sms service
    static const Int32 CS_EMERGENCY_ENABLED = 1006;  // Access Control blocks emergency call service

    /** Notification id. */
    static const Int32 PS_NOTIFICATION = 888;  // Id to update and cancel PS restricted
    static const Int32 CS_NOTIFICATION = 999;  // Id to update and cancel CS restricted

private:
    static const Int32 EVENT_ALL_DATA_DISCONNECTED = 1001;
    static const String WAKELOCK_TAG; // = "ServiceStateTracker";
    AutoPtr<IGSMPhone> mPhone;
    Int32 mMaxDataCalls; // = 1;
    Int32 mNewMaxDataCalls; // = 1;
    Int32 mReasonDataDenied; // = -1;
    Int32 mNewReasonDataDenied; // = -1;

    /**
     * GSM roaming status solely based on TS 27.007 7.2 CREG. Only used by
     * handlePollStateResult to store CREG roaming result.
     */
    Boolean mGsmRoaming; // = FALSE;

    /**
     * Data roaming status solely based on TS 27.007 10.1.19 CGREG. Only used by
     * handlePollStateResult to store CGREG roaming result.
     */
    Boolean mDataRoaming; // = FALSE;

    /**
     * Mark when service state is in emergency call only mode
     */
    Boolean mEmergencyOnly; // = FALSE;

    /**
     * Sometimes we get the NITZ time before we know what country we
     * are in. Keep the time zone information from the NITZ string so
     * we can fix the time zone once know the country.
     */
    Boolean mNeedFixZoneAfterNitz; // = FALSE;
    Int32 mZoneOffset;
    Boolean mZoneDst;
    Int64 mZoneTime;
    Boolean mGotCountryCode; // = FALSE;
    AutoPtr<IContentResolver> mCr;

    /** Boolean is true is setTimeFromNITZString was called */
    Boolean mNitzUpdatedTime; // = FALSE;

    /** Started the recheck process after finding gprs should registered but not. */
    Boolean mStartedGprsRegCheck; // = FALSE;

    /** Already sent the event-log for no gprs register. */
    Boolean mReportedGprsNoReg; // = FALSE;

    /**
     * The Notification object given to the NotificationManager.
     */
    AutoPtr<INotification> mNotification;

    /** Wake lock used while setting time of day. */
    AutoPtr<IPowerManagerWakeLock> mWakeLock;

    /** Keep track of SPN display rules, so we only broadcast intent if something changes. */
    String mCurSpn;
    String mCurPlmn;
    Boolean mCurShowPlmn; // = FALSE;
    Boolean mCurShowSpn; // = FALSE;

    AutoPtr<IBroadcastReceiver> mIntentReceiver;

    AutoPtr<IContentObserver> mAutoTimeObserver;

    AutoPtr<IContentObserver> mAutoTimeZoneObserver;
};

} // namespace Gem
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMSERVICESTATETRACKER_H__
