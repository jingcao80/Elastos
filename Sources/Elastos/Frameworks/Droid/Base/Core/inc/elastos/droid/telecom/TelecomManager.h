#ifndef __ELASTOS_DROID_TELECOM_TELECOMMANAGER_H__
#define __ELASTOS_DROID_TELECOM_TELECOMMANAGER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Telecom.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Internal::Telecom::IITelecomService;
using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Telecom {

/**
 * Provides access to Telecom-related functionality.
 * TODO: Move this all into PhoneManager.
 */
class TelecomManager
    : public Object
    , public ITelecomManager
{
public:
    CAR_INTERFACE_DECL()

    /**
     * @hide
     */
    static CARAPI From(
        /* [in] */ IContext* context,
        /* [out] */ ITelecomManager** result);

    /**
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Return the {@link PhoneAccount} which is the user-chosen default for making outgoing phone
     * calls with a specified URI scheme.
     * <p>
     * Apps must be prepared for this method to return {@code null}, indicating that there currently
     * exists no user-chosen default {@code PhoneAccount}.
     * <p>
     * @param uriScheme The URI scheme.
     * @return The {@link PhoneAccountHandle} corresponding to the user-chosen default for outgoing
     * phone calls for a specified URI scheme.
     *
     * @hide
     */
    CARAPI GetDefaultOutgoingPhoneAccount(
        /* [in] */ const String& uriScheme,
        /* [out] */ IPhoneAccountHandle** result);

    /**
     * Return the {@link PhoneAccount} which is the user-chosen default for making outgoing phone
     * calls. This {@code PhoneAccount} will always be a member of the list which is returned from
     * calling {@link #getCallCapablePhoneAccounts()}
     *
     * Apps must be prepared for this method to return {@code null}, indicating that there currently
     * exists no user-chosen default {@code PhoneAccount}.
     *
     * @return The user outgoing phone account selected by the user.
     * @hide
     */
    CARAPI GetUserSelectedOutgoingPhoneAccount(
        /* [out] */ IPhoneAccountHandle** result);

    /**
     * Sets the default account for making outgoing phone calls.
     * @hide
     */
    CARAPI SetUserSelectedOutgoingPhoneAccount(
        /* [in] */ IPhoneAccountHandle* accountHandle);

    /**
     * Returns the current SIM call manager. Apps must be prepared for this method to return
     * {@code null}, indicating that there currently exists no user-chosen default
     * {@code PhoneAccount}.
     * @return The phone account handle of the current sim call manager.
     * @hide
     */
    CARAPI GetSimCallManager(
        /* [out] */ IPhoneAccountHandle** result);

    /**
     * Sets the SIM call manager to the specified phone account.
     * @param accountHandle The phone account handle of the account to set as the sim call manager.
     * @hide
     */
    CARAPI SetSimCallManager(
        /* [in] */ IPhoneAccountHandle* accountHandle);

    /**
     * Returns the list of registered SIM call managers.
     * @return List of registered SIM call managers.
     * @hide
     */
    CARAPI GetSimCallManagers(
        /* [out] */ IList** result);

    /**
     * Returns the current connection manager. Apps must be prepared for this method to return
     * {@code null}, indicating that there currently exists no user-chosen default
     * {@code PhoneAccount}.
     *
     * @return The phone account handle of the current connection manager.
     * @hide
     */
    CARAPI GetConnectionManager(
        /* [out] */ IPhoneAccountHandle** result);

    /**
     * Returns a list of the {@link PhoneAccountHandle}s which can be used to make and receive phone
     * calls which support the specified URI scheme.
     * <P>
     * For example, invoking with {@code "tel"} will find all {@link PhoneAccountHandle}s which
     * support telephone calls (e.g. URIs such as {@code tel:555-555-1212}).  Invoking with
     * {@code "sip"} will find all {@link PhoneAccountHandle}s which support SIP calls (e.g. URIs
     * such as {@code sip:example@sipexample.com}).
     *
     * @param uriScheme The URI scheme.
     * @return A list of {@code PhoneAccountHandle} objects supporting the URI scheme.
     *
     * @hide
     */
    CARAPI GetPhoneAccountsSupportingScheme(
        /* [in] */ const String& uriScheme,
        /* [out] */ IList** result);

    /**
     * Return a list of {@link PhoneAccountHandle}s which can be used to make and receive phone
     * calls.
     *
     * @see #EXTRA_PHONE_ACCOUNT_HANDLE
     * @return A list of {@code PhoneAccountHandle} objects.
     *
     * @hide
     */
    CARAPI GetCallCapablePhoneAccounts(
        /* [out] */ IList** result);

    /**
     * Determine whether the device has more than one account registered that can make and receive
     * phone calls.
     *
     * @return {@code true} if the device has more than one account registered and {@code false}
     * otherwise.
     * @hide
     */
    CARAPI HasMultipleCallCapableAccounts(
        /* [out] */ Boolean* result);

    /**
     *  Returns a list of all {@link PhoneAccount}s registered for the calling package.
     *
     * @return A list of {@code PhoneAccountHandle} objects.
     * @hide
     */
    CARAPI GetPhoneAccountsForPackage(
        /* [out] */ IList** result);

    /**
     * Return the {@link PhoneAccount} for a specified {@link PhoneAccountHandle}. Object includes
     * resources which can be used in a user interface.
     *
     * @param account The {@link PhoneAccountHandle}.
     * @return The {@link PhoneAccount} object.
     * @hide
     */
    CARAPI GetPhoneAccount(
        /* [in] */ IPhoneAccountHandle* account,
        /* [out] */ IPhoneAccount** result);

    /**
     * Returns a count of all {@link PhoneAccount}s.
     *
     * @return The count of {@link PhoneAccount}s.
     * @hide
     */
    CARAPI GetAllPhoneAccountsCount(
        /* [out] */ Int32* result);

    /**
     * Returns a list of all {@link PhoneAccount}s.
     *
     * @return All {@link PhoneAccount}s.
     * @hide
     */
    CARAPI GetAllPhoneAccounts(
        /* [out] */ IList** result);

    /**
     * Returns a list of all {@link PhoneAccountHandle}s.
     *
     * @return All {@link PhoneAccountHandle}s.
     * @hide
     */
    CARAPI GetAllPhoneAccountHandles(
        /* [out] */ IList** result);

    /**
     * Register a {@link PhoneAccount} for use by the system.
     *
     * @param account The complete {@link PhoneAccount}.
     * @hide
     */
    CARAPI RegisterPhoneAccount(
        /* [in] */ IPhoneAccount* account);

    /**
     * Remove a {@link PhoneAccount} registration from the system.
     *
     * @param accountHandle A {@link PhoneAccountHandle} for the {@link PhoneAccount} to unregister.
     * @hide
     */
    CARAPI UnregisterPhoneAccount(
        /* [in] */ IPhoneAccountHandle* accountHandle);

    /**
     * Remove all Accounts that belong to the calling package from the system.
     * @hide
     */
    CARAPI ClearAccounts();

    /**
     * @hide
     */
    CARAPI GetDefaultPhoneApp(
        /* [out] */ IComponentName** result);

    /**
     * Returns whether there is an ongoing phone call (can be in dialing, ringing, active or holding
     * states).
     * <p>
     * Requires permission: {@link android.Manifest.permission#READ_PHONE_STATE}
     * </p>
     */
    CARAPI IsInCall(
        /* [out] */ Boolean* result);

    /**
     * Returns one of the following constants that represents the current state of Telecom:
     *
     * {@link TelephonyManager#CALL_STATE_RINGING}
     * {@link TelephonyManager#CALL_STATE_OFFHOOK}
     * {@link TelephonyManager#CALL_STATE_IDLE}
     * @hide
     */
    CARAPI GetCallState(
        /* [out] */ Int32* result);

    /**
     * Returns whether there currently exists is a ringing incoming-call.
     *
     * @hide
     */
    CARAPI IsRinging(
        /* [out] */ Boolean* result);

    /**
     * Ends an ongoing call.
     * TODO: L-release - need to convert all invocations of ITelecomService#endCall to use this
     * method (clockwork & gearhead).
     * @hide
     */
    CARAPI EndCall(
        /* [out] */ Boolean* result);

    /**
     * If there is a ringing incoming call, this method accepts the call on behalf of the user.
     * TODO: L-release - need to convert all invocation of ITelecmmService#answerRingingCall to use
     * this method (clockwork & gearhead).
     *
     * @hide
     */
    CARAPI AcceptRingingCall();

    /**
     * Silences the ringer if a ringing call exists.
     *
     * @hide
     */
    CARAPI SilenceRinger();

    /**
     * Returns whether TTY is supported on this device.
     *
     * @hide
     */
    CARAPI IsTtySupported(
        /* [out] */ Boolean* result);

    /**
     * Returns the current TTY mode of the device. For TTY to be on the user must enable it in
     * settings and have a wired headset plugged in.
     * Valid modes are:
     * - {@link TelecomManager#TTY_MODE_OFF}
     * - {@link TelecomManager#TTY_MODE_FULL}
     * - {@link TelecomManager#TTY_MODE_HCO}
     * - {@link TelecomManager#TTY_MODE_VCO}
     * @hide
     */
    CARAPI GetCurrentTtyMode(
        /* [out] */ Int32* result);

    /**
     * Registers a new incoming call. A {@link ConnectionService} should invoke this method when it
     * has an incoming call. The specified {@link PhoneAccountHandle} must have been registered
     * with {@link #registerPhoneAccount}. Once invoked, this method will cause the system to bind
     * to the {@link ConnectionService} associated with the {@link PhoneAccountHandle} and request
     * additional information about the call (See
     * {@link ConnectionService#onCreateIncomingConnection}) before starting the incoming call UI.
     *
     * @param phoneAccount A {@link PhoneAccountHandle} registered with
     *            {@link #registerPhoneAccount}.
     * @param extras A bundle that will be passed through to
     *            {@link ConnectionService#onCreateIncomingConnection}.
     * @hide
     */
    CARAPI AddNewIncomingCall(
        /* [in] */ IPhoneAccountHandle* phoneAccount,
        /* [in] */ IBundle* extras);

    /**
     * Registers a new unknown call with Telecom. This can only be called by the system Telephony
     * service. This is invoked when Telephony detects a new unknown connection that was neither
     * a new incoming call, nor an user-initiated outgoing call.
     *
     * @param phoneAccount A {@link PhoneAccountHandle} registered with
     *            {@link #registerPhoneAccount}.
     * @param extras A bundle that will be passed through to
     *            {@link ConnectionService#onCreateIncomingConnection}.
     * @hide
     */
    CARAPI AddNewUnknownCall(
        /* [in] */ IPhoneAccountHandle* phoneAccount,
        /* [in] */ IBundle* extras);

    /**
     * Processes the specified dial string as an MMI code.
     * MMI codes are any sequence of characters entered into the dialpad that contain a "*" or "#".
     * Some of these sequences launch special behavior through handled by Telephony.
     * <p>
     * Requires that the method-caller be set as the system dialer app.
     * </p>
     *
     * @param dialString The digits to dial.
     * @return True if the digits were processed as an MMI code, false otherwise.
     */
    CARAPI HandleMmi(
        /* [in] */ const String& dialString,
        /* [out] */ Boolean* result);

    /**
     * Removes the missed-call notification if one is present.
     * <p>
     * Requires that the method-caller be set as the system dialer app.
     * </p>
     */
    CARAPI CancelMissedCallsNotification();

    /**
     * Brings the in-call screen to the foreground if there is an ongoing call. If there is
     * currently no ongoing call, then this method does nothing.
     * <p>
     * Requires that the method-caller be set as the system dialer app or have the
     * {@link android.Manifest.permission#READ_PHONE_STATE} permission.
     * </p>
     *
     * @param showDialpad Brings up the in-call dialpad as part of showing the in-call screen.
     */
    CARAPI ShowInCallScreen(
        /* [in] */ Boolean showDialpad);

private:
    AutoPtr<IITelecomService> GetTelecomService();

    Boolean IsServiceConnected();

private:
    static String TAG;

    IContext* mContext;
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_TELECOMMANAGER_H__