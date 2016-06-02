
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_SERVICESTATETRACKER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_SERVICESTATETRACKER_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

#include "elastos/droid/os/Handler.h"

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

using Elastos::Core::Object;
using Elastos::Droid::Os::Handler;


using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Internal::Telephony::Dataconnection::IDcTrackerBase;
using Elastos::Droid::Os::IAsyncResult;
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

    CARAPI constructor(
        /* [in] */ IPhoneBase* phoneBase,
        /* [in] */ ICommandsInterface* ci,
        /* [in] */ ICellInfo* cellInfo);

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
        /* [out] */ Int32* result);

    virtual CARAPI IsConcurrentVoiceAndDataAllowed(
        /* [out] */ Boolean* result);

    virtual CARAPI SetImsRegistrationState(
        /* [in] */ Boolean registered);

    virtual CARAPI PollState();

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
        /* [in] */ String property,
        /* [in] */ String defValue,
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
    ECO_LOCAL void RequestShutdown();
    ECO_LOCAL CARAPI NotifySignalStrength(Boolean * result);
    ECO_LOCAL void NotifyDataRegStateRilRadioTechnologyChanged();
    /**
     * Some operators have been known to report registration failure
     * data only devices, to fix that use DataRegState.
     */
    ECO_LOCAL void UseDataRegStateForDataOnlyDevices();
    ECO_LOCAL void UpdatePhoneObject();
    ECO_LOCAL void DisableSingleLocationUpdate();
    ECO_LOCAL Boolean OnSignalStrengthResult(AsyncResult ar, Boolean isGsm);

    /**
     * Hang up all voice call and turn off radio. Implemented by derived class.
     */
    virtual CARAPI_(void) HangupAndPowerOff() = 0;

    virtual CARAPI GetPhone(IPhone* phone) = 0;
    virtual CARAPI_(void) HandlePollStateResult(Int32 what, IAsyncResult* ar) = 0;
    virtual CARAPI_(void) UpdateSpnDisplay() = 0;
    virtual CARAPI_(void) SetPowerStateToDesired() = 0;
    virtual CARAPI_(void) OnUpdateIccAvailability() = 0;
    virtual CARAPI_(void) Log(String s) = 0;
    virtual CARAPI_(void) Loge(String s) = 0;

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
    ECO_LOCAL Boolean ShouldFixTimeZoneNow(IPhoneBase* phoneBase, String operatorNumeric,
        String prevOperatorNumeric, Boolean needToFixTimeZone);

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
    ECO_LOCAL void UpdateCarrierMccMncConfiguration(String newOp, String oldOp, IContext* context);
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
    static const Int32 EVENT_RADIO_STATE_CHANGED;
    static const Int32 EVENT_NETWORK_STATE_CHANGED;
    static const Int32 EVENT_GET_SIGNAL_STRENGTH;
    static const Int32 EVENT_POLL_STATE_REGISTRATION;
    static const Int32 EVENT_POLL_STATE_GPRS;
    static const Int32 EVENT_POLL_STATE_OPERATOR;
    static const Int32 EVENT_POLL_SIGNAL_STRENGTH;
    static const Int32 EVENT_NITZ_TIME;
    static const Int32 EVENT_SIGNAL_STRENGTH_UPDATE;
    static const Int32 EVENT_RADIO_AVAILABLE;
    static const Int32 EVENT_POLL_STATE_NETWORK_SELECTION_MODE;
    static const Int32 EVENT_GET_LOC_DONE;
    static const Int32 EVENT_SIM_RECORDS_LOADED;
    static const Int32 EVENT_SIM_READY;
    static const Int32 EVENT_LOCATION_UPDATES_ENABLED;
    static const Int32 EVENT_GET_PREFERRED_NETWORK_TYPE;
    static const Int32 EVENT_SET_PREFERRED_NETWORK_TYPE;
    static const Int32 EVENT_RESET_PREFERRED_NETWORK_TYPE;
    static const Int32 EVENT_CHECK_REPORT_GPRS;
    static const Int32 EVENT_RESTRICTED_STATE_CHANGED;

    /** CDMA events */
    static const Int32 EVENT_POLL_STATE_REGISTRATION_CDMA;
    static const Int32 EVENT_POLL_STATE_OPERATOR_CDMA;
    static const Int32 EVENT_RUIM_READY;
    static const Int32 EVENT_RUIM_RECORDS_LOADED;
    static const Int32 EVENT_POLL_SIGNAL_STRENGTH_CDMA;
    static const Int32 EVENT_GET_SIGNAL_STRENGTH_CDMA;
    static const Int32 EVENT_NETWORK_STATE_CHANGED_CDMA;
    static const Int32 EVENT_GET_LOC_DONE_CDMA;
    //static const Int32 EVENT_UNUSED;
    static const Int32 EVENT_NV_LOADED;
    static const Int32 EVENT_POLL_STATE_CDMA_SUBSCRIPTION;
    static const Int32 EVENT_NV_READY;
    static const Int32 EVENT_ERI_FILE_LOADED;
    static const Int32 EVENT_OTA_PROVISION_STATUS_CHANGE;
    static const Int32 EVENT_SET_RADIO_POWER_OFF;
    static const Int32 EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED;
    static const Int32 EVENT_CDMA_PRL_VERSION_CHANGED;
    static const Int32 EVENT_RADIO_ON;
    static const Int32 EVENT_ICC_CHANGED;
    static const Int32 EVENT_GET_CELL_INFO_LIST;
    static const Int32 EVENT_UNSOL_CELL_INFO_LIST;
    static const Int32 EVENT_CHANGE_IMS_STATE;

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
    AutoPtr<IccRecords> mIccRecords;
    AutoPtr<IPhoneBase> mPhoneBase;
    AutoPtr<IServiceState> mNewSS;
    AutoPtr<IList> mLastCellInfoList;
    // This is final as subclasses alias to a more specific type
    // so we don't want the reference to change.
    AutoPtr<ICellInfo> mCellInfo;
    AutoPtr<ISignalStrength> mSignalStrength;

    AutoPtr<IIntentFilter> mIntentFilter;
    AutoPtr<IPendingIntent> mRadioOffIntent;

    AutoPtr<IRegistrantList> mRoamingOnRegistrants;
    AutoPtr<IRegistrantList> mRoamingOffRegistrants;
    AutoPtr<IRegistrantList> mAttachedRegistrants;
    AutoPtr<IRegistrantList> mDetachedRegistrants;
    AutoPtr<IRegistrantList> mDataRegStateOrRatChangedRegistrants;
    AutoPtr<IRegistrantList> mNetworkAttachedRegistrants;
    AutoPtr<IRegistrantList> mPsRestrictEnabledRegistrants;
    AutoPtr<IRegistrantList> mPsRestrictDisabledRegistrants;

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
