
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_SERVICESTATETRACKER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_SERVICESTATETRACKER_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>
#include "elastos/droid/internal/telephony/dataconnection/DataConnection.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/RegistrantList.h"

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

using Elastos::Core::Object;
using Elastos::Droid::Os::Handler;

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Internal::Telephony::DataConnection::IDcTrackerBase;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::RegistrantList;
using Elastos::Droid::Telephony::ICellInfo;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISignalStrength;
using Elastos::Droid::Telephony::ITelephonyManager;

using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class ServiceStateTracker
    : public Handler
    , public IServiceStateTracker
{
private:
    class ECO_LOCAL CellInfoResult
        : public Object
    {
    public:
        CellInfoResult();
    };

public:
    CAR_INTERFACE_DECL()

    ServiceStateTracker();

    CARAPI constructor(
        /* [in] */ IPhoneBase* phoneBase,
        /* [in] */ ICommandsInterface* ci,
        /* [in] */ ICellInfo* cellInfo);

    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    CARAPI Dispose();

    CARAPI GetDesiredPowerState(
        /* [out] */ Boolean* result);

    /**
     * Registration point for combined roaming on
     * combined roaming is true when roaming is true and ONS differs SPN
     *
     * @param h handler to notify
     * @param what what code of message when delivered
     * @param obj placed in Message.obj
     */
    CARAPI RegisterForRoamingOn(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI UnregisterForRoamingOn(
        /* [in] */ IHandler* h);

    /**
     * Registration point for combined roaming off
     * combined roaming is true when roaming is true and ONS differs SPN
     *
     * @param h handler to notify
     * @param what what code of message when delivered
     * @param obj placed in Message.obj
     */
    CARAPI RegisterForRoamingOff(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI UnregisterForRoamingOff(
        /* [in] */ IHandler* h);

    /**
     * Re-register network by toggling preferred network type.
     * This is a work-around to deregister and register network since there is
     * no ril api to set COPS=2 (deregister) only.
     *
     * @param onComplete is dispatched when this is complete.  it will be
     * an AsyncResult, and onComplete.obj.exception will be non-null
     * on failure.
     */
    CARAPI ReRegisterNetwork(
        /* [in] */ IMessage* onComplete);

    CARAPI SetRadioPower(
        /* [in] */ Boolean power);

    CARAPI EnableSingleLocationUpdate();

    CARAPI EnableLocationUpdates();

    CARAPI DisableLocationUpdates();

    virtual CARAPI GetCurrentDataConnectionState(
        /* [out] */ Int32* result) = 0;

    virtual CARAPI IsConcurrentVoiceAndDataAllowed(
        /* [out] */ Boolean* result) = 0;

    virtual CARAPI SetImsRegistrationState(
        /* [in] */ Boolean registered) = 0;

    virtual CARAPI PollState() = 0;

    CARAPI RegisterForDataConnectionAttached(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI UnregisterForDataConnectionAttached(
        /* [in] */ IHandler* h);

    CARAPI RegisterForDataConnectionDetached(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI UnregisterForDataConnectionDetached(
        /* [in] */ IHandler* h);

    CARAPI RegisterForDataRegStateOrRatChanged(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI UnregisterForDataRegStateOrRatChanged(
        /* [in] */ IHandler* h);

    CARAPI RegisterForNetworkAttached(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI UnregisterForNetworkAttached(
        /* [in] */ IHandler* h);

    CARAPI RegisterForPsRestrictedEnabled(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI UnregisterForPsRestrictedEnabled(
        /* [in] */ IHandler* h);

    CARAPI RegisterForPsRestrictedDisabled(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI UnregisterForPsRestrictedDisabled(
        /* [in] */ IHandler* h);

    CARAPI  PowerOffRadioSafely(
         /* [in] */ IDcTrackerBase* dcTracker);

    CARAPI ProcessPendingRadioPowerOffAfterDataOff(
        /* [out] */ Boolean* result);

    CARAPI GetSystemProperty(
        /* [in] */ const String& property,
        /* [in] */ const String& defValue,
        /* [out] */ String* result);

    CARAPI GetAllCellInfo(
        /* [out] */ IList** result);

    CARAPI GetSignalStrength(
        /* [out] */ ISignalStrength** result);

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

protected:
    ECO_LOCAL CARAPI RequestShutdown();
    ECO_LOCAL CARAPI NotifySignalStrength(Boolean * result);
    ECO_LOCAL void NotifyDataRegStateRilRadioTechnologyChanged();
    /**
     * Some operators have been known to report registration failure
     * data only devices, to fix that use DataRegState.
     */
    ECO_LOCAL void UseDataRegStateForDataOnlyDevices();
    ECO_LOCAL void UpdatePhoneObject();
    ECO_LOCAL void DisableSingleLocationUpdate();
    ECO_LOCAL Boolean OnSignalStrengthResult(AsyncResult* ar, Boolean isGsm);

    /**
     * Hang up all voice call and turn off radio. Implemented by derived class.
     */
    virtual CARAPI HangupAndPowerOff() = 0;

    virtual CARAPI GetPhone(IPhone** phone) = 0;
    virtual CARAPI HandlePollStateResult(Int32 what, AsyncResult* ar) = 0;
    virtual CARAPI UpdateSpnDisplay() = 0;
    virtual CARAPI SetPowerStateToDesired() = 0;
    virtual CARAPI OnUpdateIccAvailability() = 0;
    virtual CARAPI Log(const String& s) = 0;
    virtual CARAPI Loge(const String& s) = 0;

    /** Cancel a Pending (if any) PollState() operation */
    ECO_LOCAL void CancelPollState();

    /**
     * Return TRUE if time zone needs fixing.
     *
     * @param phoneBase
     * @param operatorNumeric
     * @param prevOperatorNumeric
     * @param needToFixTimeZone
     * @return TRUE if time zone needs to be fixed
     */
    ECO_LOCAL Boolean ShouldFixTimeZoneNow(IPhoneBase* phoneBase, const String& operatorNumeric,
        const String& prevOperatorNumeric, Boolean needToFixTimeZone);

    /**
     * Verifies the current thread is the same as the thread originally
     * used in the initialization of this instance. Throws RuntimeException
     * if not.
     *
     * @exception RuntimeException if the current thread is not
     * the thread that originally obtained this PhoneBase instance.
     */
    ECO_LOCAL ECode CheckCorrectThread();
    ECO_LOCAL Boolean IsCallerOnDifferentThread();
    ECO_LOCAL void UpdateCarrierMccMncConfiguration(const String& newOp, const String& oldOp, IContext* context);
    ECO_LOCAL Boolean IsIwlanFeatureAvailable();
    ECO_LOCAL void ProcessIwlanToWwanTransition(IServiceState* ss);

private:
    static CARAPI_(AutoPtr<ArrayOf<String> >) InitCOUNTCODE();

public:
    AutoPtr<IServiceState> mSS;
    // TODO - this should not be public, right now used externally GsmConnetion.
    AutoPtr<IRestrictedState> mRestrictedState;

protected:
    static const Boolean DBG;
    static const Boolean VDBG;
    static const String PROP_FORCE_ROAMING;

    /** Signal strength poll rate. */
    static const Int32 POLL_PERIOD_MILLIS;

    /** GSM events */
    static const Int32 EVENT_RADIO_STATE_CHANGED               = 1;
    static const Int32 EVENT_NETWORK_STATE_CHANGED             = 2;
    static const Int32 EVENT_GET_SIGNAL_STRENGTH               = 3;
    static const Int32 EVENT_POLL_STATE_REGISTRATION           = 4;
    static const Int32 EVENT_POLL_STATE_GPRS                   = 5;
    static const Int32 EVENT_POLL_STATE_OPERATOR               = 6;
    static const Int32 EVENT_POLL_SIGNAL_STRENGTH              = 10;
    static const Int32 EVENT_NITZ_TIME                         = 11;
    static const Int32 EVENT_SIGNAL_STRENGTH_UPDATE            = 12;
    static const Int32 EVENT_RADIO_AVAILABLE                   = 13;
    static const Int32 EVENT_POLL_STATE_NETWORK_SELECTION_MODE = 14;
    static const Int32 EVENT_GET_LOC_DONE                      = 15;
    static const Int32 EVENT_SIM_RECORDS_LOADED                = 16;
    static const Int32 EVENT_SIM_READY                         = 17;
    static const Int32 EVENT_LOCATION_UPDATES_ENABLED          = 18;
    static const Int32 EVENT_GET_PREFERRED_NETWORK_TYPE        = 19;
    static const Int32 EVENT_SET_PREFERRED_NETWORK_TYPE        = 20;
    static const Int32 EVENT_RESET_PREFERRED_NETWORK_TYPE      = 21;
    static const Int32 EVENT_CHECK_REPORT_GPRS                 = 22;
    static const Int32 EVENT_RESTRICTED_STATE_CHANGED          = 23;

    /** CDMA events */
    static const Int32 EVENT_POLL_STATE_REGISTRATION_CDMA      = 24;
    static const Int32 EVENT_POLL_STATE_OPERATOR_CDMA          = 25;
    static const Int32 EVENT_RUIM_READY                        = 26;
    static const Int32 EVENT_RUIM_RECORDS_LOADED               = 27;
    static const Int32 EVENT_POLL_SIGNAL_STRENGTH_CDMA         = 28;
    static const Int32 EVENT_GET_SIGNAL_STRENGTH_CDMA          = 29;
    static const Int32 EVENT_NETWORK_STATE_CHANGED_CDMA        = 30;
    static const Int32 EVENT_GET_LOC_DONE_CDMA                 = 31;
    // static const Int32 EVENT_UNUSED                            = 32;
    static const Int32 EVENT_NV_LOADED                         = 33;
    static const Int32 EVENT_POLL_STATE_CDMA_SUBSCRIPTION      = 34;
    static const Int32 EVENT_NV_READY                          = 35;
    static const Int32 EVENT_ERI_FILE_LOADED                   = 36;
    static const Int32 EVENT_OTA_PROVISION_STATUS_CHANGE       = 37;
    static const Int32 EVENT_SET_RADIO_POWER_OFF               = 38;
    static const Int32 EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED  = 39;
    static const Int32 EVENT_CDMA_PRL_VERSION_CHANGED          = 40;
    static const Int32 EVENT_RADIO_ON                          = 41;
    static const Int32 EVENT_ICC_CHANGED                       = 42;
    static const Int32 EVENT_GET_CELL_INFO_LIST                = 43;
    static const Int32 EVENT_UNSOL_CELL_INFO_LIST              = 44;
    static const Int32 EVENT_CHANGE_IMS_STATE                  = 45;

    /**
     * List of ISO codes for countries that can have an offset of
     * GMT+0 when not in daylight savings time.  This ignores some
     * small places such as the Canary Islands (Spain) and
     * Danmarkshavn (Denmark).  The list must be sorted by code.
    */
    static const AutoPtr<ArrayOf<String> > GMT_COUNTRY_CODES;

    static const String TIMEZONE_PROPERTY;
    /** Reason for registration denial. */
    static const String REGISTRATION_DENIED_GEN;
    static const String REGISTRATION_DENIED_AUTH;

    static const String ACTION_RADIO_OFF;

    AutoPtr<ICommandsInterface> mCi;
    AutoPtr<IUiccController> mUiccController;
    AutoPtr<IUiccCardApplication> mUiccApplcation;
    AutoPtr<IIccRecords> mIccRecords;
    AutoPtr<IPhoneBase> mPhoneBase;
    AutoPtr<IServiceState> mNewSS;
    AutoPtr<IList> mLastCellInfoList;
    // This is final as subclasses alias to a more specific type
    // so we don't want the reference to change.
    AutoPtr<ICellInfo> mCellInfo;
    AutoPtr<ISignalStrength> mSignalStrength;

    AutoPtr<IIntentFilter> mIntentFilter;
    AutoPtr<IPendingIntent> mRadioOffIntent;

    AutoPtr<RegistrantList> mRoamingOnRegistrants;
    AutoPtr<RegistrantList> mRoamingOffRegistrants;
    AutoPtr<RegistrantList> mAttachedRegistrants;
    AutoPtr<RegistrantList> mDetachedRegistrants;
    AutoPtr<RegistrantList> mDataRegStateOrRatChangedRegistrants;
    AutoPtr<RegistrantList> mNetworkAttachedRegistrants;
    AutoPtr<RegistrantList> mPsRestrictEnabledRegistrants;
    AutoPtr<RegistrantList> mPsRestrictDisabledRegistrants;

    /**
     * A unique identifier to track requests associated with a poll
     * and ignore stale responses.  The value is a count-down of
     * expected responses in this pollingContext.
     */
    AutoPtr<ArrayOf<Int32> > mPollingContext;

    /**
     * By default, strength polling is enabled.  However, if we're
     * getting unsolicited signal strength updates from the radio, set
     * value to TRUE and don't bother polling any more.
     */
    Boolean mDontPollSignalStrength;
    Int64 mLastCellInfoListTime;

    /* Radio power off pending flag and tag counter */
    Boolean mPendingRadioPowerOffAfterDataOff;
    Int32 mPendingRadioPowerOffAfterDataOffTag;

    Boolean mVoiceCapable;
    Boolean mDesiredPowerState;

    /* Current rat is iwlan */
    Boolean mIwlanRatAvailable;

    Boolean mImsRegistrationOnOff;
    Boolean mAlarmSwitch;
    Boolean mPowerOffDelayNeed;
    Boolean mDeviceShuttingDown;


private:
    static const Int64 LAST_CELL_INFO_LIST_MAX_AGE_MS;
    AutoPtr<ISignalStrength> mLastSignalStrength;
    /**
     * These two flags manage the behavior of the cell lock -- the
     * lock should be held if either flag is TRUE.  The intention is
     * to allow temporary acquisition of the lock to get a single
     * update.  Such a lock grab and release can thus be made to not
     * interfere with more permanent lock holds -- in other words, the
     * lock will only be released if both flags are FALSE, and so
     * releases by temporary users will only affect the lock state if
     * there is no continuous user.
     */
    Boolean mWantContinuousLocationUpdates;
    Boolean mWantSingleLocationUpdate;

};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_SERVICESTATETRACKER_H__
