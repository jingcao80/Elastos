
#ifndef __ELASTOS_DROID_SERVER_TELECOM_TELECOMSERVICEIMPL_H__
#define __ELASTOS_DROID_SERVER_TELECOM_TELECOMSERVICEIMPL_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include <Elastos.Droid.App.h>
#include "elastos/droid/server/telecom/CallsManager.h"
#include "elastos/droid/server/telecom/MissedCallNotifier.h"
#include "elastos/droid/server/telecom/PhoneAccountRegistrar.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/os/Handler.h>

using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Internal::Telecom::IITelecomService;
using Elastos::Droid::Telecom::IPhoneAccount;
using Elastos::Droid::Telecom::IPhoneAccountHandle;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Implementation of the ITelecom interface.
 */
class TelecomServiceImpl
    : public Object
    , public IITelecomService
    , public IBinder
{
private:
    /**
     * A request object for use with {@link MainThreadHandler}. Requesters should wait() on the
     * request after sending. The main thread will notify the request when it is complete.
     */
    class MainThreadRequest
        : public Object
        , public ITelecomServiceImplMainThreadRequest
    {
    public:
        CAR_INTERFACE_DECL()

        /** The result of the request that is run on the main thread */
        AutoPtr<IObject> mResult;
    };

    /**
     * A handler that processes messages on the main thread in the phone process. Since many
     * of the Phone calls are not thread safe this is needed to shuttle the requests from the
     * inbound binder threads to the main thread in the phone process.
     */
    class MainThreadHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("TelecomServiceImpl::MainThreadHandler")

        MainThreadHandler(
            /* [in] */ TelecomServiceImpl* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        TelecomServiceImpl* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    TelecomServiceImpl();

    /** ${inheritDoc} */
    // @Override
    CARAPI AsBinder(
        /* [out] */ IBinder** result);

    CARAPI constructor(
        /* [in] */ IInterface* missedCallNotifier,// MissedCallNotifier
        /* [in] */ IInterface* phoneAccountRegistrar,// PhoneAccountRegistrar
        /* [in] */ IInterface* callsManager,// CallsManager
        /* [in] */ IContext* context);

    //
    // @Override
    CARAPI GetDefaultOutgoingPhoneAccount(
        /* [in] */ const String& uriScheme,
        /* [out] */ IPhoneAccountHandle** result);

    // @Override
    CARAPI GetUserSelectedOutgoingPhoneAccount(
        /* [out] */ IPhoneAccountHandle** result);

    // @Override
    CARAPI SetUserSelectedOutgoingPhoneAccount(
        /* [in] */ IPhoneAccountHandle* accountHandle);

    // @Override
    CARAPI GetCallCapablePhoneAccounts(
        /* [out] */ IList** result);

    // @Override
    CARAPI GetPhoneAccountsSupportingScheme(
        /* [in] */ const String& uriScheme,
        /* [out] */ IList** result);

    // @Override
    CARAPI GetPhoneAccountsForPackage(
        /* [in] */ const String& packageName,
        /* [out] */ IList** result);

    // @Override
    CARAPI GetPhoneAccount(
        /* [in] */ IPhoneAccountHandle* accountHandle,
        /* [out] */ IPhoneAccount** result);

    // @Override
    CARAPI GetAllPhoneAccountsCount(
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetAllPhoneAccounts(
        /* [out] */ IList** result);

    // @Override
    CARAPI GetAllPhoneAccountHandles(
        /* [out] */ IList** result);

    // @Override
    CARAPI GetSimCallManager(
        /* [out] */ IPhoneAccountHandle** result);

    // @Override
    CARAPI SetSimCallManager(
        /* [in] */ IPhoneAccountHandle* accountHandle);

    // @Override
    CARAPI GetSimCallManagers(
        /* [out] */ IList** result);

    // @Override
    CARAPI RegisterPhoneAccount(
        /* [in] */ IPhoneAccount* account);

    // @Override
    CARAPI UnregisterPhoneAccount(
        /* [in] */ IPhoneAccountHandle* accountHandle);

    // @Override
    CARAPI ClearAccounts(
        /* [in] */ const String& packageName);

    /**
     * @see android.telecom.TelecomManager#silenceRinger
     */
    // @Override
    CARAPI SilenceRinger();

    /**
     * @see android.telecom.TelecomManager#getDefaultPhoneApp
     */
    // @Override
    CARAPI GetDefaultPhoneApp(
        /* [out] */ IComponentName** result);

    /**
     * @see android.telecom.TelecomManager#isInCall
     */
    // @Override
    CARAPI IsInCall(
        /* [out] */ Boolean* result);

    /**
     * @see android.telecom.TelecomManager#isRinging
     */
    // @Override
    CARAPI IsRinging(
        /* [out] */ Boolean* result);

    /**
     * @see TelecomManager#getCallState
     */
    // @Override
    CARAPI GetCallState(
        /* [out] */ Int32* result);

    /**
     * @see android.telecom.TelecomManager#getActiveSubscription
     */
    CARAPI GetActiveSubscription(
        /* [out] */ Int64* result);

    /**
     * @see android.telecom.TelecomManager#switchToOtherActiveSub
     */
    CARAPI SwitchToOtherActiveSub(
        /* [in] */ Int64 subId);

    /**
     * @see android.telecom.TelecomManager#endCall
     */
    // @Override
    CARAPI EndCall(
        /* [out] */ Boolean* result);

    /**
     * @see android.telecom.TelecomManager#acceptRingingCall
     */
    // @Override
    CARAPI AcceptRingingCall();

    /**
     * @see android.telecom.TelecomManager#showInCallScreen
     */
    // @Override
    CARAPI ShowInCallScreen(
        /* [in] */ Boolean showDialpad);

    /**
     * @see android.telecom.TelecomManager#cancelMissedCallsNotification
     */
    // @Override
    CARAPI CancelMissedCallsNotification();

    /**
     * @see android.telecom.TelecomManager#handleMmi
     */
    // @Override
    CARAPI HandlePinMmi(
        /* [in] */ const String& dialString,
        /* [out] */ Boolean* result);

    /**
     * @see android.telecom.TelecomManager#isTtySupported
     */
    // @Override
    CARAPI IsTtySupported(
        /* [out] */ Boolean* result);

    /**
     * @see android.telecom.TelecomManager#getCurrentTtyMode
     */
    // @Override
    CARAPI GetCurrentTtyMode(
        /* [out] */ Int32* result);

    /**
     * @see android.telecom.TelecomManager#addNewIncomingCall
     */
    // @Override
    CARAPI AddNewIncomingCall(
        /* [in] */ IPhoneAccountHandle* phoneAccountHandle,
        /* [in] */ IBundle* extras);

    /**
     * @see android.telecom.TelecomManager#addNewUnknownCall
     */
    // @Override
    CARAPI AddNewUnknownCall(
        /* [in] */ IPhoneAccountHandle* phoneAccountHandle,
        /* [in] */ IBundle* extras);

    // @Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* writer,
        /* [in] */ ArrayOf<String>* args);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    //
    // Supporting methods for the ITelecomService interface implementation.
    //
    CARAPI AcceptRingingCallInternal();

    CARAPI EndCallInternal(
        /* [out] */ Boolean* result);

    CARAPI EnforcePhoneAccountModificationForPackage(
        /* [in] */ const String& packageName);

    CARAPI EnforceReadPermissionOrDefaultDialer();

    CARAPI EnforceModifyPermissionOrDefaultDialer();

    CARAPI EnforceCallingPackage(
        /* [in] */ const String& packageName);

    CARAPI EnforceConnectionServiceFeature();

    CARAPI EnforceRegisterProviderOrSubscriptionPermission();

    CARAPI EnforceRegisterConnectionManagerPermission();

    CARAPI EnforceReadPermission();

    CARAPI EnforceModifyPermission();

    CARAPI EnforcePermission(
        /* [in] */ const String& permission);

    CARAPI EnforceFeature(
        /* [in] */ const String& feature);

    CARAPI IsDefaultDialerCalling(
        /* [out] */ Boolean* result);

    CARAPI GetTelephonyManager(
        /* [out] */ ITelephonyManager** result);

    CARAPI SendRequestAsync(
        /* [in] */ Int32 command,
        /* [in] */ Int32 arg1,
        /* [out] */ ITelecomServiceImplMainThreadRequest** result);

    /**
     * Posts the specified command to be executed on the main thread, waits for the request to
     * complete, and returns the result.
     */
    CARAPI SendRequest(
        /* [in] */ Int32 command,
        /* [out] */ IInterface** result);

private:
    static const String REGISTER_PROVIDER_OR_SUBSCRIPTION;

    static const String REGISTER_CONNECTION_MANAGER;

    /** The context. */
    AutoPtr<IContext> mContext;

    static const String TAG;

    static const String SERVICE_NAME;

    static const Int32 MSG_SILENCE_RINGER;

    static const Int32 MSG_SHOW_CALL_SCREEN;

    static const Int32 MSG_END_CALL;

    static const Int32 MSG_ACCEPT_RINGING_CALL;

    static const Int32 MSG_CANCEL_MISSED_CALLS_NOTIFICATION;

    static const Int32 MSG_IS_TTY_SUPPORTED;

    static const Int32 MSG_GET_CURRENT_TTY_MODE;

    /** The singleton instance. */
    static AutoPtr<TelecomServiceImpl> sInstance;

    AutoPtr<MainThreadHandler> mMainThreadHandler;

    AutoPtr<CallsManager> mCallsManager;

    AutoPtr<MissedCallNotifier> mMissedCallNotifier;

    AutoPtr<PhoneAccountRegistrar> mPhoneAccountRegistrar;

    AutoPtr<IAppOpsManager> mAppOpsManager;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_TELECOMSERVICEIMPL_H__
