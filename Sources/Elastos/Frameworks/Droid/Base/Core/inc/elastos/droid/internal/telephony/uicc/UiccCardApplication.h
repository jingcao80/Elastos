#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_UICCCARDAPPLICATION_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_UICCCARDAPPLICATION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/Registrant.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/RegistrantList.h"

// import android.telephony.Rlog;
// import com.android.internal.telephony.PhoneConstants;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::Registrant;
using Elastos::Droid::Os::RegistrantList;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::Uicc::AppState;
using Elastos::Droid::Internal::Telephony::Uicc::AppType;
using Elastos::Droid::Internal::Telephony::Uicc::PersoSubState;
using Elastos::Droid::Internal::Telephony::Uicc::PinState;
using Elastos::Droid::Internal::Telephony::Uicc::IUICCConfig;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;
using Elastos::Droid::Internal::Telephony::Uicc::IIccCardApplicationStatus;
using Elastos::Droid::Internal::Telephony::Uicc::IIccFileHandler;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  * {@hide}
  */
class UiccCardApplication
    : public Object
    , public IUiccCardApplication
{
private:
    class InnerHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("UiccCardApplication::InnerHandler")

        InnerHandler(
            /* [in] */ UiccCardApplication* owner);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        UiccCardApplication* mOwner;
    };

public:
    CAR_INTERFACE_DECL();

    UiccCardApplication();

    CARAPI constructor(
        /* [in] */ IUiccCard* uiccCard,
        /* [in] */ IIccCardApplicationStatus* as,
        /* [in] */ IContext* c,
        /* [in] */ ICommandsInterface* ci);

    virtual CARAPI Update(
        /* [in] */ IIccCardApplicationStatus* as,
        /* [in] */ IContext* c,
        /* [in] */ ICommandsInterface* ci);

    virtual CARAPI Dispose();

    /** Assumes mLock is held. */
    virtual CARAPI QueryFdn();

    virtual CARAPI OnRefresh(
        /* [in] */ IIccRefreshResponse* refreshResponse);

    virtual CARAPI RegisterForReady(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    virtual CARAPI UnregisterForReady(
        /* [in] */ IHandler* h);

    /**
      * Notifies handler of any transition into State.isPinLocked()
      */
    virtual CARAPI RegisterForLocked(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    virtual CARAPI UnregisterForLocked(
        /* [in] */ IHandler* h);

    /**
      * Notifies handler of any transition into State.PERSO_LOCKED
      */
    virtual CARAPI RegisterForPersoLocked(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    virtual CARAPI UnregisterForPersoLocked(
        /* [in] */ IHandler* h);

    virtual CARAPI GetState(
        /* [out] */ AppState* result);

    virtual CARAPI GetType(
        /* [out] */ AppType* result);

    virtual CARAPI GetAuthContext(
        /* [out] */ Int32* result);

    virtual CARAPI GetPersoSubState(
        /* [out] */ PersoSubState* result);

    virtual CARAPI GetAid(
        /* [out] */ String* result);

    virtual CARAPI GetAppLabel(
        /* [out] */ String* result);

    virtual CARAPI GetPin1State(
        /* [out] */ PinState* result);

    virtual CARAPI GetIccFileHandler(
        /* [out] */ IIccFileHandler** result);

    virtual CARAPI GetIccRecords(
        /* [out] */ IIccRecords** result);

    virtual CARAPI IsPersoLocked(
        /* [out] */ Boolean* result);

    /**
      * Supply the ICC PIN to the ICC
      *
      * When the operation is complete, onComplete will be sent to its
      * Handler.
      *
      * onComplete.obj will be an AsyncResult
      * onComplete.arg1 = remaining attempts before puk locked or -1 if unknown
      *
      * ((AsyncResult)onComplete.obj).exception == null on success
      * ((AsyncResult)onComplete.obj).exception != null on fail
      *
      * If the supplied PIN is incorrect:
      * ((AsyncResult)onComplete.obj).exception != null
      * && ((AsyncResult)onComplete.obj).exception
      *       instanceof com.android.internal.telephony.gsm.CommandException)
      * && ((CommandException)(((AsyncResult)onComplete.obj).exception))
      *          .getCommandError() == CommandException.Error.PASSWORD_INCORRECT
      */
    virtual CARAPI SupplyPin(
        /* [in] */ const String& pin,
        /* [in] */ IMessage* onComplete);

    /**
      * Supply the ICC PUK to the ICC
      *
      * When the operation is complete, onComplete will be sent to its
      * Handler.
      *
      * onComplete.obj will be an AsyncResult
      * onComplete.arg1 = remaining attempts before Icc will be permanently unusable
      * or -1 if unknown
      *
      * ((AsyncResult)onComplete.obj).exception == null on success
      * ((AsyncResult)onComplete.obj).exception != null on fail
      *
      * If the supplied PIN is incorrect:
      * ((AsyncResult)onComplete.obj).exception != null
      * && ((AsyncResult)onComplete.obj).exception
      *       instanceof com.android.internal.telephony.gsm.CommandException)
      * && ((CommandException)(((AsyncResult)onComplete.obj).exception))
      *          .getCommandError() == CommandException.Error.PASSWORD_INCORRECT
      *
      *
      */
    virtual CARAPI SupplyPuk(
        /* [in] */ const String& puk,
        /* [in] */ const String& newPin,
        /* [in] */ IMessage* onComplete);

    virtual CARAPI SupplyPin2(
        /* [in] */ const String& pin2,
        /* [in] */ IMessage* onComplete);

    virtual CARAPI SupplyPuk2(
        /* [in] */ const String& puk2,
        /* [in] */ const String& newPin2,
        /* [in] */ IMessage* onComplete);

    virtual CARAPI SupplyDepersonalization(
        /* [in] */ const String& pin,
        /* [in] */ const String& type,
        /* [in] */ IMessage* onComplete);

    /**
      * Check whether ICC pin lock is enabled
      * This is a sync call which returns the cached pin enabled state
      *
      * @return true for ICC locked enabled
      *         false for ICC locked disabled
      */
    virtual CARAPI GetIccLockEnabled(
        /* [out] */ Boolean* result);

    /**
      * Check whether ICC fdn (fixed dialing number) is enabled
      * This is a sync call which returns the cached pin enabled state
      *
      * @return true for ICC fdn enabled
      *         false for ICC fdn disabled
      */
    virtual CARAPI GetIccFdnEnabled(
        /* [out] */ Boolean* result);

    /**
      * Check whether fdn (fixed dialing number) service is available.
      * @return true if ICC fdn service available
      *         false if ICC fdn service not available
      */
    virtual CARAPI GetIccFdnAvailable(
        /* [out] */ Boolean* result);

    /**
      * Set the ICC pin lock enabled or disabled
      * When the operation is complete, onComplete will be sent to its handler
      *
      * @param enabled "true" for locked "false" for unlocked.
      * @param password needed to change the ICC pin state, aka. Pin1
      * @param onComplete
      *        onComplete.obj will be an AsyncResult
      *        ((AsyncResult)onComplete.obj).exception == null on success
      *        ((AsyncResult)onComplete.obj).exception != null on fail
      */
    virtual CARAPI SetIccLockEnabled(
        /* [in] */ Boolean enabled,
        /* [in] */ const String& password,
        /* [in] */ IMessage* onComplete);

    /**
      * Set the ICC fdn enabled or disabled
      * When the operation is complete, onComplete will be sent to its handler
      *
      * @param enabled "true" for locked "false" for unlocked.
      * @param password needed to change the ICC fdn enable, aka Pin2
      * @param onComplete
      *        onComplete.obj will be an AsyncResult
      *        ((AsyncResult)onComplete.obj).exception == null on success
      *        ((AsyncResult)onComplete.obj).exception != null on fail
      */
    virtual CARAPI SetIccFdnEnabled(
        /* [in] */ Boolean enabled,
        /* [in] */ const String& password,
        /* [in] */ IMessage* onComplete);

    /**
      * Change the ICC password used in ICC pin lock
      * When the operation is complete, onComplete will be sent to its handler
      *
      * @param oldPassword is the old password
      * @param newPassword is the new password
      * @param onComplete
      *        onComplete.obj will be an AsyncResult
      *        onComplete.arg1 = attempts remaining or -1 if unknown
      *        ((AsyncResult)onComplete.obj).exception == null on success
      *        ((AsyncResult)onComplete.obj).exception != null on fail
      */
    virtual CARAPI ChangeIccLockPassword(
        /* [in] */ const String& oldPassword,
        /* [in] */ const String& newPassword,
        /* [in] */ IMessage* onComplete);

    /**
      * Change the ICC password used in ICC fdn enable
      * When the operation is complete, onComplete will be sent to its handler
      *
      * @param oldPassword is the old password
      * @param newPassword is the new password
      * @param onComplete
      *        onComplete.obj will be an AsyncResult
      *        ((AsyncResult)onComplete.obj).exception == null on success
      *        ((AsyncResult)onComplete.obj).exception != null on fail
      */
    virtual CARAPI ChangeIccFdnPassword(
        /* [in] */ const String& oldPassword,
        /* [in] */ const String& newPassword,
        /* [in] */ IMessage* onComplete);

    /**
      * @return true if ICC card is PIN2 blocked
      */
    virtual CARAPI GetIccPin2Blocked(
        /* [out] */ Boolean* result);

    /**
      * @return true if ICC card is PUK2 blocked
      */
    virtual CARAPI GetIccPuk2Blocked(
        /* [out] */ Boolean* result);

    virtual CARAPI GetUICCConfig(
        /* [out] */ IUICCConfig** result);

    virtual CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    virtual CARAPI GetUiccCard(
        /* [out] */ IUiccCard** result);

private:
    CARAPI_(AutoPtr<IIccRecords>) CreateIccRecords(
        /* [in] */ AppType type,
        /* [in] */ IContext* c,
        /* [in] */ ICommandsInterface* ci);

    CARAPI_(AutoPtr<IIccFileHandler>) CreateIccFileHandler(
        /* [in] */ AppType type);

    /**
      * Interpret EVENT_QUERY_FACILITY_LOCK_DONE
      * @param ar is asyncResult of Query_Facility_Locked
      */
    CARAPI_(void) OnQueryFdnEnabled(
        /* [in] */ AsyncResult* ar);

    CARAPI_(void) OnChangeFdnDone(
        /* [in] */ AsyncResult* ar);

    /** REMOVE when mIccLockEnabled is not needed, assumes mLock is held */
    CARAPI_(void) QueryPin1State();

    /** REMOVE when mIccLockEnabled is not needed*/
    CARAPI_(void) OnQueryFacilityLock(
        /* [in] */ AsyncResult* ar);

    /** REMOVE when mIccLockEnabled is not needed */
    CARAPI_(void) OnChangeFacilityLock(
        /* [in] */ AsyncResult* ar);

    /**
      * Parse the error response to obtain number of attempts remaining
      */
    CARAPI_(Int32) ParsePinPukErrorResult(
        /* [in] */ AsyncResult* ar);

    /**
      * Notifies specified registrant, assume mLock is held.
      *
      * @param r Registrant to be notified. If null - all registrants will be notified
      */
    CARAPI_(void) NotifyReadyRegistrantsIfNeeded(
        /* [in] */ Registrant* r);

    /**
      * Notifies specified registrant, assume mLock is held.
      *
      * @param r Registrant to be notified. If null - all registrants will be notified
      */
    CARAPI_(void) NotifyPinLockedRegistrantsIfNeeded(
        /* [in] */ Registrant* r);

    /**
      * Notifies specified registrant, assume mLock is held.
      *
      * @param r Registrant to be notified. If null - all registrants will be notified
      */
    CARAPI_(void) NotifyPersoLockedRegistrantsIfNeeded(
        /* [in] */ Registrant* r);

    /**
      * Returns the authContext based on the type of UiccCard.
      *
      * @param appType the app type
      * @return authContext corresponding to the type or AUTH_CONTEXT_UNDEFINED if appType not
      * supported
      */
    static CARAPI_(Int32) GetAuthContext(
        /* [in] */ AppType appType);

    CARAPI_(void) Log(
        /* [in] */ const String& msg);

    CARAPI_(void) Loge(
        /* [in] */ const String& msg);

private:
    static const String LOGTAG;
    static const Boolean DBG;
    static const Int32 EVENT_PIN1_PUK1_DONE = 1;
    static const Int32 EVENT_CHANGE_PIN1_DONE = 2;
    static const Int32 EVENT_CHANGE_PIN2_DONE = 3;
    static const Int32 EVENT_QUERY_FACILITY_FDN_DONE = 4;
    static const Int32 EVENT_CHANGE_FACILITY_FDN_DONE = 5;
    static const Int32 EVENT_QUERY_FACILITY_LOCK_DONE = 6;
    static const Int32 EVENT_CHANGE_FACILITY_LOCK_DONE = 7;
    static const Int32 EVENT_PIN2_PUK2_DONE = 8;
    /*const*/ Object mLock;
    AutoPtr<IUiccCard> mUiccCard;
    //parent
    AppState mAppState;
    AppType mAppType;
    Int32 mAuthContext;
    PersoSubState mPersoSubState;
    String mAid;
    String mAppLabel;
    Boolean mPin1Replaced;
    PinState mPin1State;
    PinState mPin2State;
    Boolean mIccFdnEnabled;
    Boolean mDesiredFdnEnabled;
    Boolean mIccLockEnabled;
    Boolean mDesiredPinLocked;
    Boolean mIccFdnAvailable;
    // Default is enabled.
    AutoPtr<ICommandsInterface> mCi;
    AutoPtr<IContext> mContext;
    AutoPtr<IIccRecords> mIccRecords;
    AutoPtr<IIccFileHandler> mIccFh;
    Boolean mDestroyed;
    //set to true once this App is commanded to be disposed of.
    AutoPtr<RegistrantList> mReadyRegistrants;
    AutoPtr<RegistrantList> mPinLockedRegistrants;
    AutoPtr<RegistrantList> mPersoLockedRegistrants;
    AutoPtr<IHandler> mHandler;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_UICCCARDAPPLICATION_H__
