#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMASERVICESTATETRACKER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMASERVICESTATETRACKER_H__

#include "Elastos.Droid.Telephony.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/RegistrantList.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/internal/telephony/ServiceStateTracker.h"

// import android.app.AlarmManager;
// import android.content.Intent;
// import android.content.res.Resources;
// import android.os.Build;
// import android.os.PowerManager;
// import android.os.SystemClock;
// import android.os.SystemProperties;
// import android.os.UserHandle;
// import android.provider.Settings;
// import android.provider.Settings.SettingNotFoundException;
// import android.telephony.CellInfoCdma;
// import android.telephony.Rlog;
// import android.telephony.SignalStrength;
// import android.telephony.TelephonyManager;
// import android.telephony.SubscriptionManager;
// import android.text.TextUtils;
// import android.util.EventLog;
// import android.util.TimeUtils;
// import com.android.internal.telephony.CommandException;
// import com.android.internal.telephony.CommandsInterface;
// import com.android.internal.telephony.CommandsInterface.RadioState;
// import com.android.internal.telephony.EventLogTags;
// import com.android.internal.telephony.MccTable;
// import com.android.internal.telephony.PhoneConstants;
// import com.android.internal.telephony.PhoneFactory;
// import com.android.internal.telephony.ServiceStateTracker;
// import com.android.internal.telephony.TelephonyIntents;
// import com.android.internal.telephony.TelephonyProperties;
// import com.android.internal.telephony.dataconnection.DcTrackerBase;
// import com.android.internal.telephony.uicc.UiccController;
// import com.android.internal.telephony.uicc.RuimRecords;
// import com.android.internal.telephony.Operators;
// import java.util.Arrays;
// import java.util.Calendar;
// import java.util.Date;
// import java.util.List;

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Os::RegistrantList;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Internal::Telephony::ServiceStateTracker;
using Elastos::Droid::Telephony::Cdma::ICdmaCellLocation;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ICellInfo;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IHbpcdUtils;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::ITimeZone;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

/**
  * {@hide}
  */
class CdmaServiceStateTracker
    : public ServiceStateTracker
    , public ICdmaServiceStateTracker
{
private:
    class InnerContentObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("CdmaServiceStateTracker::InnerContentObserver")

        InnerContentObserver(
            /* [in] */ CdmaServiceStateTracker* owner);

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        CdmaServiceStateTracker* mOwner;
    };

    class InnerContentObserver1
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("CdmaServiceStateTracker::InnerContentObserver1")

        InnerContentObserver1(
            /* [in] */ CdmaServiceStateTracker* owner);

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        CdmaServiceStateTracker* mOwner;
    };

public:
    TO_STRING_IMPL("CdmaServiceStateTracker")

    CAR_INTERFACE_DECL();

    CdmaServiceStateTracker();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ICDMAPhone* phone);

    // @Override
    CARAPI Dispose();

    /**
      * Registration point for subscription info ready
      * @param h handler to notify
      * @param what what code of message when delivered
      * @param obj placed in Message.obj
      */
    virtual CARAPI RegisterForSubscriptionInfoReady(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    virtual CARAPI UnregisterForSubscriptionInfoReady(
        /* [in] */ IHandler* h);

    // @Override
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
    // @Override
    CARAPI PollState();

    // @Override
    CARAPI GetCurrentDataConnectionState(
        /* [out] */ Int32* result);

    /**
      * @return true if phone is camping on a technology
      * that could support voice and data simultaneously.
      */
    // @Override
    CARAPI IsConcurrentVoiceAndDataAllowed(
        /* [out] */ Boolean* resutl);

    virtual CARAPI GetMdnNumber(
        /* [out] */ String* result);

    virtual CARAPI GetCdmaMin(
        /* [out] */ String* result);

    /** Returns null if NV is not yet ready */
    virtual CARAPI GetPrlVersion(
        /* [out] */ String* result);

    /**
      * Returns IMSI as MCC + MNC + MIN
      */
    virtual CARAPI GetImsi(
        /* [out] */ String* result);

    /**
      * Check if subscription data has been assigned to mMin
      *
      * return true if MIN info is ready; false otherwise.
      */
    virtual CARAPI IsMinInfoReady(
        /* [out] */ Boolean* result);

    /**
      * Returns OTASP_UNKNOWN, OTASP_NEEDED or OTASP_NOT_NEEDED
      */
    virtual CARAPI GetOtasp(
        /* [out] */ Int32* result);

    // @Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    // @Override
    CARAPI SetImsRegistrationState(
        /* [in] */ Boolean registered);

protected:
    CARAPI constructor(
        /* [in] */ ICDMAPhone* phone,
        /* [in] */ ICellInfo* cellInfo);

    // @Override
    CARAPI_(void) Finalize();

    // @Override
    CARAPI SetPowerStateToDesired();

    // @Override
    CARAPI UpdateSpnDisplay();

    // @Override
    CARAPI GetPhone(
        /* [out] */ IPhone** result);

    /**
     * Hanlde the PollStateResult message
     */
    virtual CARAPI HandlePollStateResultMessage(
        /* [in] */ Int32 what,
        /* [in] */ AsyncResult* ar);

    /**
      * Handle the result of one of the pollState() - related requests
      */
    // @Override
    CARAPI HandlePollStateResult(
        /* [in] */ Int32 what,
        /* [in] */ AsyncResult* ar);

    virtual CARAPI_(void) SetSignalStrengthDefaultValues();

    virtual CARAPI_(void) FixTimeZone(
        /* [in] */ const String& isoCountryCode);

    virtual CARAPI PollStateDone();

    virtual CARAPI_(Boolean) IsInvalidOperatorNumeric(
        /* [in] */ const String& operatorNumeric);

    virtual CARAPI_(String) FixUnknownMcc(
        /* [in] */ const String& operatorNumeric,
        /* [in] */ Int32 sid);

    virtual CARAPI_(void) SetOperatorIdd(
        /* [in] */ const String& operatorNumeric);

    virtual CARAPI_(Int32) RadioTechnologyToDataServiceState(
        /* [in] */ Int32 code);

    /** code is registration state 0-5 from TS 27.007 7.2 */
    virtual CARAPI_(Int32) RegCodeToServiceState(
        /* [in] */ Int32 code);

    /**
      * code is registration state 0-5 from TS 27.007 7.2
      * returns true if registered roam, false otherwise
      */
    virtual CARAPI_(Boolean) RegCodeIsRoaming(
        /* [in] */ Int32 code);

    virtual CARAPI_(Boolean) IsSidsAllZeros();

    // @Override
    CARAPI HangupAndPowerOff();

    virtual CARAPI_(void) ParseSidNid(
        /* [in] */ const String& sidStr,
        /* [in] */ const String& nidStr);

    virtual CARAPI_(void) UpdateOtaspState();

    virtual CARAPI_(AutoPtr<IUiccCardApplication>) GetUiccCardApplication();

    // @Override
    CARAPI OnUpdateIccAvailability();

    // @Override
    CARAPI Log(
        /* [in] */ const String& s);

    // @Override
    CARAPI Loge(
        /* [in] */ const String& s);

private:
    /**
      * Save current source of cdma subscription
      * @param source - 1 for NV, 0 for RUIM
      */
    CARAPI_(void) SaveCdmaSubscriptionSource(
        /* [in] */ Int32 source);

    CARAPI_(void) GetSubscriptionInfoAndStartPollingThreads();

    //***** Private Instance Methods
    CARAPI_(void) HandleCdmaSubscriptionSource(
        /* [in] */ Int32 newSubscriptionSource);

    /**
      * Consider dataRegState if voiceRegState is OOS to determine SPN to be
      * displayed
      */
    CARAPI_(Int32) GetCombinedRegState();

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

    /**
      * TODO: This code is exactly the same as in GsmServiceStateTracker
      * and has a TODO to not poll signal strength if screen is off.
      * This code should probably be hoisted to the base class so
      * the fix, when added, works for both.
      */
    CARAPI_(void) QueueNextSignalStrengthPoll();

    /**
      * Determine whether a roaming indicator is in the carrier-specified list of ERIs for
      * home system
      *
      * @param roamInd roaming indicator in String
      * @return true if the roamInd is in the carrier-specified list of ERIs for home network
      */
    CARAPI_(Boolean) IsRoamIndForHomeSystem(
        /* [in] */ const String& roamInd);

    /**
      * Set roaming state when cdmaRoaming is true and ons is different from spn
      * @param cdmaRoaming TS 27.007 7.2 CREG registered roaming
      * @param s ServiceState hold current ons
      * @return true for roaming state set
      */
    CARAPI_(Boolean) IsRoamingBetweenOperators(
        /* [in] */ Boolean cdmaRoaming,
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
      * Check whether a specified system ID that matches one of the home system IDs.
      */
    CARAPI_(Boolean) IsHomeSid(
        /* [in] */ Int32 sid);

    CARAPI_(void) RegisterForRuimEvents();

    CARAPI_(void) UnregisterForRuimEvents();

public:
    static const String LOGTAG;
    AutoPtr<ICDMAPhone> mPhone;
    AutoPtr<ICdmaCellLocation> mCellLoc;
    AutoPtr<ICdmaCellLocation> mNewCellLoc;
    // Current Otasp value
    Int32 mCurrentOtaspMode;
    String mSavedTimeZone;
    Int64 mSavedTime;
    Int64 mSavedAtTime;

protected:
    Boolean mDataRoaming;
    /**
      * Initially assume no data connection.
      */
    Int32 mRegistrationState;
    AutoPtr<RegistrantList> mCdmaForSubscriptionInfoReadyRegistrants;
    /**
      * Sometimes we get the NITZ time before we know what country we
      * are in. Keep the time zone information from the NITZ string so
      * we can fix the time zone once know the country.
      */
    Boolean mNeedFixZone;
    Boolean mGotCountryCode;
    /** Contains the name of the registered network in CDMA (either ONS or ERI text). */
    String mCurPlmn;
    String mCurSpn;
    Boolean mCurShowPlmn;
    Boolean mCurShowSpn;
    String mMdn;
    AutoPtr<ArrayOf<Int32> > mHomeSystemId;
    AutoPtr<ArrayOf<Int32> > mHomeNetworkId;
    String mMin;
    String mPrlVersion;
    Boolean mIsMinInfoReady;
    Boolean mIsSubscriptionFromRuim;
    static const String INVALID_MCC;
    static const String DEFAULT_MNC;
    AutoPtr<IHbpcdUtils> mHbpcdUtils;

private:
    // Min values used to by getOtasp()
    static const String UNACTIVATED_MIN2_VALUE;
    static const String UNACTIVATED_MIN_VALUE;
    static const Int32 MS_PER_HOUR = 60 * 60 * 1000;
    /** if time between NITZ updates is less than mNitzUpdateSpacing the update may be ignored. */
    static const Int32 NITZ_UPDATE_SPACING_DEFAULT = 1000 * 60 * 10;
    Int32 mNitzUpdateSpacing;
    /** If mNitzUpdateSpacing hasn't been exceeded but update is > mNitzUpdate do the update */
    static const Int32 NITZ_UPDATE_DIFF_DEFAULT = 2000;
    Int32 mNitzUpdateDiff;
    Boolean mCdmaRoaming;
    Int32 mRoamingIndicator;
    Boolean mIsInPrl;
    Int32 mDefaultRoamingIndicator;
    Int32 mZoneOffset;
    Boolean mZoneDst;
    Int64 mZoneTime;
    /** Wake lock used while setting time of day. */
    AutoPtr<IPowerManagerWakeLock> mWakeLock;
    static const String WAKELOCK_TAG;
    Boolean mIsEriTextLoaded;
    AutoPtr<ICdmaSubscriptionSourceManager> mCdmaSSM;
    /* Used only for debugging purposes. */
    String mRegistrationDeniedReason;
    AutoPtr<IContentResolver> mCr;
    String mCurrentCarrier;
    AutoPtr<IContentObserver> mAutoTimeObserver;
    AutoPtr<IContentObserver> mAutoTimeZoneObserver;
};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMASERVICESTATETRACKER_H__

