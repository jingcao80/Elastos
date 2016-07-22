#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCCARDPROXY_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCCARDPROXY_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/RegistrantList.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaSubscriptionSourceManager;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState;
using Elastos::Droid::Internal::Telephony::ISubscription;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::RegistrantList;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  * @Deprecated use {@link UiccController}.getUiccCard instead.
  *
  * The Phone App assumes that there is only one icc card, and one icc application
  * available at a time. Moreover, it assumes such object (represented with IccCard)
  * is available all the time (whether {@link RILConstants#RIL_REQUEST_GET_SIM_STATUS} returned
  * or not, whether card has desired application or not, whether there really is a card in the
  * slot or not).
  *
  * UiccController, however, can handle multiple instances of icc objects (multiple
  * {@link UiccCardApplication}, multiple {@link IccFileHandler}, multiple {@link IccRecords})
  * created and destroyed dynamically during phone operation.
  *
  * This class implements the IccCard interface that is always available (right after default
  * phone object is constructed) to expose the current (based on voice radio technology)
  * application on the uicc card, so that external apps won't break.
  */
class IccCardProxy
    : public Handler
    , public IIccCard
    , public IIccCardProxy
{
public:
    CAR_INTERFACE_DECL();

    IccCardProxy();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ICommandsInterface* ci);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ICommandsInterface* ci,
        /* [in] */ Int32 cardIndex);

    virtual CARAPI Dispose();

    /*
      * The card application that the external world sees will be based on the
      * voice radio technology only!
      */
    virtual CARAPI SetVoiceRadioTech(
        /* [in] */ Int32 radioTech);

    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    virtual CARAPI ResetProperties();

    virtual CARAPI GetIccRecordsLoaded(
        /* [out] */ Boolean* result);

    /* IccCard interface implementation */
    // @Override
    CARAPI GetState(
        /* [out] */ IccCardConstantsState* result);

    // @Override
    CARAPI GetIccRecords(
        /* [out] */ IIccRecords** result);

    // @Override
    CARAPI GetIccFileHandler(
        /* [out] */ IIccFileHandler** result);

    /**
      * Notifies handler of any transition into State.ABSENT
      */
    // @Override
    CARAPI RegisterForAbsent(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForAbsent(
        /* [in] */ IHandler* h);

    /**
      * Notifies handler of any transition into State.PERSO_LOCKED
      */
    // @Override
    CARAPI RegisterForPersoLocked(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForPersoLocked(
        /* [in] */ IHandler* h);

    /**
      * Notifies handler of any transition into State.isPinLocked()
      */
    // @Override
    CARAPI RegisterForLocked(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForLocked(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI SupplyPin(
        /* [in] */ const String& pin,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI SupplyPuk(
        /* [in] */ const String& puk,
        /* [in] */ const String& newPin,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI SupplyPin2(
        /* [in] */ const String& pin2,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI SupplyPuk2(
        /* [in] */ const String& puk2,
        /* [in] */ const String& newPin2,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI SupplyDepersonalization(
        /* [in] */ const String& pin,
        /* [in] */ const String& type,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI GetIccLockEnabled(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetIccFdnEnabled(
        /* [out] */ Boolean* result);

    virtual CARAPI GetIccFdnAvailable(
        /* [out] */ Boolean* result);

    virtual CARAPI GetIccPin2Blocked(
        /* [out] */ Boolean* result);

    virtual CARAPI GetIccPuk2Blocked(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetIccLockEnabled(
        /* [in] */ Boolean enabled,
        /* [in] */ const String& password,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI SetIccFdnEnabled(
        /* [in] */ Boolean enabled,
        /* [in] */ const String& password,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI ChangeIccLockPassword(
        /* [in] */ const String& oldPassword,
        /* [in] */ const String& newPassword,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI ChangeIccFdnPassword(
        /* [in] */ const String& oldPassword,
        /* [in] */ const String& newPassword,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI GetServiceProviderName(
        /* [out] */ String* result);

    // @Override
    CARAPI IsApplicationOnIcc(
        /* [in] */  AppType type,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI HasIccCard(
        /* [out] */ Boolean* result);

    virtual CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:
    /**
      * This method sets the IccRecord, corresponding to the currently active
      * subscription, as the active record.
      */
    CARAPI_(void) UpdateActiveRecord();

    /**
      * In case of 3gpp2 we need to find out if subscription used is coming from
      * NV in which case we shouldn't broadcast any sim states changes.
      */
    CARAPI_(void) UpdateQuietMode();

    CARAPI_(void) OnSubscriptionActivated();

    CARAPI_(void) OnSubscriptionDeactivated();

    CARAPI_(void) OnRecordsLoaded();

    CARAPI_(void) UpdateIccAvailability();

    CARAPI_(void) UpdateExternalState();

    CARAPI_(void) RegisterUiccCardEvents();

    CARAPI_(void) UnregisterUiccCardEvents();

    CARAPI_(void) UpdateStateProperty();

    CARAPI_(void) BroadcastIccStateChangedIntent(
        /* [in] */ const String& value,
        /* [in] */ const String& reason);

    CARAPI_(void) SetExternalState(
        /* [in] */ IccCardConstantsState newState,
        /* [in] */ Boolean _override);

    CARAPI_(void) ProcessLockedState();

    CARAPI_(void) SetExternalState(
        /* [in] */ IccCardConstantsState newState);

    CARAPI_(String) GetIccStateIntentString(
        /* [in] */ IccCardConstantsState state);

    /**
      * Locked state have a reason (PIN, PUK, NETWORK, PERM_DISABLED, CARD_IO_ERROR)
      * @return reason
      */
    CARAPI_(String) GetIccStateReason(
        /* [in] */ IccCardConstantsState state);

    CARAPI_(void) SetSystemProperty(
        /* [in] */ const String& property,
        /* [in] */ Int32 slotId,
        /* [in] */ const String& value);

    CARAPI_(void) Log(
        /* [in] */ const String& s);

    CARAPI_(void) Loge(
        /* [in] */ const String& msg);

protected:
    IccCardConstantsState mExternalState;

private:
    static const Boolean DBG;
    static const String LOGTAG;
    static const Int32 EVENT_RADIO_OFF_OR_UNAVAILABLE = 1;
    static const Int32 EVENT_RADIO_ON = 2;
    static const Int32 EVENT_ICC_CHANGED = 3;
    static const Int32 EVENT_ICC_ABSENT = 4;
    static const Int32 EVENT_ICC_LOCKED = 5;
    static const Int32 EVENT_APP_READY = 6;
    static const Int32 EVENT_RECORDS_LOADED = 7;
    static const Int32 EVENT_IMSI_READY = 8;
    static const Int32 EVENT_PERSO_LOCKED = 9;
    static const Int32 EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED = 11;
    static const Int32 EVENT_ICC_RECORD_EVENTS = 500;
    static const Int32 EVENT_SUBSCRIPTION_ACTIVATED = 501;
    static const Int32 EVENT_SUBSCRIPTION_DEACTIVATED = 502;
    static const Int32 EVENT_CARRIER_PRIVILIGES_LOADED = 503;
    // FIXME Rename mCardIndex to mSlotId.
    Int32 mCardIndex;
    AutoPtr<ISubscription> mSubscriptionData;
    Object mLock;
    AutoPtr<IContext> mContext;
    AutoPtr<ICommandsInterface> mCi;
    AutoPtr<RegistrantList> mAbsentRegistrants;
    AutoPtr<RegistrantList> mPinLockedRegistrants;
    AutoPtr<RegistrantList> mPersoLockedRegistrants;
    Int32 mCurrentAppType;
    //default to 3gpp?
    AutoPtr<IUiccController> mUiccController;
    AutoPtr<IUiccCard> mUiccCard;
    AutoPtr<IUiccCardApplication> mUiccApplication;
    AutoPtr<IIccRecords> mIccRecords;
    AutoPtr<ICdmaSubscriptionSourceManager> mCdmaSSM;
    Boolean mRadioOn;
    Boolean mQuietMode;
    // when set to true IccCardProxy will not broadcast
    // ACTION_SIM_STATE_CHANGED intents
    Boolean mInitialized;
    Boolean mIsCardStatusAvailable;
    PersoSubState mPersoSubState;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCCARDPROXY_H__

