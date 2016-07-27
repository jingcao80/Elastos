#ifndef  __ELASTOS_DROID_PHONE_CPHONEINTERFACEMANAGER_H__
#define  __ELASTOS_DROID_PHONE_CPHONEINTERFACEMANAGER_H__

#include "_Elastos_Droid_TeleService_Phone_CPhoneInterfaceManager.h"
#include "elastos/droid/teleservice/phone/PhoneGlobals.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Internal::Telephony::ICallManager;
using Elastos::Droid::Internal::Telephony::IIccCard;
using Elastos::Droid::Internal::Telephony::IITelephony;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Telephony::IIccOpenLogicalChannelResponse;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

class PhoneGlobals;

/**
 * Implementation of the ITelephony interface.
 */
CarClass(CPhoneInterfaceManager)
    , public Object
    , public IITelephony
    , public IBinder
{
private:
    /**
     * A request object to use for transmitting data to an ICC.
     */
    class IccAPDUArgument
        : public Object
    {
    public:
        TO_STRING_IMPL("CPhoneInterfaceManager::IccAPDUArgument")

        IccAPDUArgument(
            /* [in] */ Int32 channel,
            /* [in] */ Int32 cla,
            /* [in] */ Int32 command,
            /* [in] */ Int32 p1,
            /* [in] */ Int32 p2,
            /* [in] */ Int32 p3,
            /* [in] */ const String& data)
            : mChannel(channel)
            , mCla(cla)
            , mCommand(command)
            , mP1(p1)
            , mP2(p2)
            , mP3(p3)
            , mData(data)
        {}

    public:
        Int32 mChannel;
        Int32 mCla;
        Int32 mCommand;
        Int32 mP1;
        Int32 mP2;
        Int32 mP3;
        String mData;
    };

    /**
     * A request object to use for transmitting data to an ICC.
     */
    class MainThreadRequest
        : public Object
    {
    public:
        TO_STRING_IMPL("CPhoneInterfaceManager::MainThreadRequest")

        MainThreadRequest(
            /* [in] */ IInterface* argument)
            : mArgument(argument)
        {}

    public:
        /** The argument to use for the request */
        AutoPtr<IInterface> mArgument;
        /** The result of the request that is run on the main thread */
        AutoPtr<IInterface> mResult;
    };

    class IncomingThirdPartyCallArgs
       : public Object
    {
    public:
        IncomingThirdPartyCallArgs(
            /* [in] */ IComponentName* component,
            /* [in] */ const String& callId,
            /* [in] */ const String& callerDisplayName)
            : mComponent(component)
            , mCallId(callId)
            , mCallerDisplayName(callerDisplayName)
        {}

    public:
        AutoPtr<IComponentName> mComponent;
        String mCallId;
        String mCallerDisplayName;
    };

    /**
     * A handler that processes messages on the main thread in the phone process. Since many
     * of the Phone calls are not thread safe this is needed to shuttle the requests from the
     * inbound binder threads to the main thread in the phone process.  The Binder thread
     * may provide a {@link MainThreadRequest} object in the msg.obj field that they are waiting
     * on, which will be notified when the operation completes and will contain the result of the
     * request.
     *
     * <p>If a MainThreadRequest object is provided in the msg.obj field,
     * note that request.result must be set to something non-null for the calling thread to
     * unblock.
     */
    class MainThreadHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CPhoneInterfaceManager::MainThreadHandler")

        MainThreadHandler(
            /* [in] */ CPhoneInterfaceManager* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CARAPI_(void) HandleNullReturnEvent(
            /* [in] */ IMessage* msg,
            /* [in] */ const String& command);

    private:
        CPhoneInterfaceManager* mHost;
    };

    class UnlockSim;

    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CPhoneInterfaceManager::MyHandler")

        MyHandler(
            /* [in] */ CPhoneInterfaceManager::UnlockSim* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CPhoneInterfaceManager::UnlockSim* mHost;
    };

    /**
     * Helper thread to turn async call to SimCard#supplyPin into
     * a synchronous one.
     */
    class UnlockSim
        : public Thread
    {
    public:
        TO_STRING_IMPL("CPhoneInterfaceManager::UnlockSim")

        UnlockSim(
            /* [in] */ CPhoneInterfaceManager* host,
            /* [in] */ IIccCard* simCard);

        //@Override
        CARAPI Run();

        /*
         * Use PIN or PUK to unlock SIM card
         *
         * If PUK is null, unlock SIM card with PIN
         *
         * If PUK is not null, unlock SIM card with PUK and set PIN code
         */
        CARAPI MyUnlockSim(
            /* [in] */ const String& puk,
            /* [in] */ const String& pin,
            /* [out, callee] */ ArrayOf<Int32>** array);

    public:
        CPhoneInterfaceManager* mHost;

        AutoPtr<IIccCard> mSimCard;

        Boolean mDone;
        Int32 mResult;
        Int32 mRetryCount;

        // For replies from SimCard interface
        AutoPtr<IHandler> mHandler;

        // For async handler to identify request type
        static const Int32 SUPPLY_PIN_COMPLETE;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CARAPI constructor(
        /* [in] */ IPhoneGlobals* pg,
        /* [in] */ IPhone* phone);

    /**
     * Initialize the singleton PhoneInterfaceManager instance.
     * This is only done once, at startup, from PhoneApp.onCreate().
     */
    static CARAPI_(AutoPtr<CPhoneInterfaceManager>) Init(
        /* [in] */ PhoneGlobals* app,
        /* [in] */ IPhone* phone);

    //
    // Implementation of the ITelephony interface.
    //

    CARAPI Dial(
        /* [in] */ const String& number);

    CARAPI DialForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& number);

    CARAPI Call(
        /* [in] */ const String& callingPackage,
        /* [in] */ const String& number);

    CARAPI CallForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& callingPackage,
        /* [in] */ const String& number);

    /**
     * End a call based on call state
     * @return true is a call was ended
     */
    CARAPI EndCall(
        /* [out] */ Boolean* result);

    /**
     * End a call based on the call state of the subId
     * @return true is a call was ended
     */
    CARAPI EndCallForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ Boolean* result);

    CARAPI AnswerRingingCall();

    CARAPI AnswerRingingCallForSubscriber(
        /* [in] */ Int64 subId);

    /**
     * This method is no longer used and can be removed once TelephonyManager stops referring to it.
     */
    CARAPI SilenceRinger();

    CARAPI IsOffhook(
        /* [out] */ Boolean* result);

    CARAPI IsOffhookForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ Boolean* result);

    CARAPI IsRinging(
        /* [out] */ Boolean* result);

    CARAPI IsRingingForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ Boolean* result);

    CARAPI IsIdle(
        /* [out] */ Boolean* result);

    CARAPI IsIdleForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ Boolean* result);

    CARAPI IsSimPinEnabled(
        /* [out] */ Boolean* result);

    CARAPI SupplyPin(
        /* [in] */ const String& pin,
        /* [out] */ Boolean* result);

    CARAPI SupplyPinForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& pin,
        /* [out] */ Boolean* result);

    CARAPI SupplyPuk(
        /* [in] */ const String& puk,
        /* [in] */ const String& pin,
        /* [out] */ Boolean* result);

    CARAPI SupplyPukForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& puk,
        /* [in] */ const String& pin,
        /* [out] */ Boolean* result);

    /** {@hide} */
    CARAPI SupplyPinReportResult(
        /* [in] */ const String& pin,
        /* [out, callee] */ ArrayOf<Int32>** array);

    CARAPI SupplyPinReportResultForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& pin,
        /* [out, callee] */ ArrayOf<Int32>** array);

    /** {@hide} */
    CARAPI SupplyPukReportResult(
        /* [in] */ const String& puk,
        /* [in] */ const String& pin,
        /* [out, callee] */ ArrayOf<Int32>** array);

    CARAPI SupplyPukReportResultForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& puk,
        /* [in] */ const String& pin,
        /* [out, callee] */ ArrayOf<Int32>** array);

    CARAPI UpdateServiceLocation();

    CARAPI UpdateServiceLocationForSubscriber(
        /* [in] */ Int64 subId);

    CARAPI IsRadioOn(
        /* [out] */ Boolean* result);

    CARAPI IsRadioOnForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ Boolean* result);

    CARAPI ToggleRadioOnOff();

    CARAPI ToggleRadioOnOffForSubscriber(
        /* [in] */ Int64 subId);

    CARAPI SetRadio(
        /* [in] */ Boolean turnOn,
        /* [out] */ Boolean* result);

    CARAPI SetRadioForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ Boolean turnOn,
        /* [out] */ Boolean* result);

    CARAPI NeedMobileRadioShutdown(
        /* [out] */ Boolean* result);

    CARAPI ShutdownMobileRadios();

    CARAPI SetRadioPower(
        /* [in] */ Boolean turnOn,
        /* [out] */ Boolean* result);

    CARAPI SetRadioPowerForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ Boolean turnOn,
        /* [out] */ Boolean* result);

    // FIXME: subId version needed
    CARAPI EnableDataConnectivity(
        /* [out] */ Boolean* result);

    // FIXME: subId version needed
    CARAPI DisableDataConnectivity(
        /* [out] */ Boolean* result);

    // FIXME: subId version needed
    CARAPI IsDataConnectivityPossible(
        /* [out] */ Boolean* result);

    CARAPI HandlePinMmi(
        /* [in] */ const String& dialString,
        /* [out] */ Boolean* result);

    CARAPI HandlePinMmiForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& dialString,
        /* [out] */ Boolean* result);

    CARAPI GetCallState(
        /* [out] */ Int32* result);

    CARAPI GetCallStateForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    CARAPI GetDataState(
        /* [out] */ Int32* result);

    CARAPI GetDataActivity(
        /* [out] */ Int32* result);

    //@Override
    CARAPI GetCellLocation(
        /* [out] */ IBundle** bundle);

    //@Override
    CARAPI EnableLocationUpdates();

    CARAPI EnableLocationUpdatesForSubscriber(
        /* [in] */ Int64 subId);

    //@Override
    CARAPI DisableLocationUpdates();

    CARAPI DisableLocationUpdatesForSubscriber(
        /* [in] */ Int64 subId);

    //@Override
    //@SuppressWarnings("unchecked")
    CARAPI GetNeighboringCellInfo(
        /* [in] */ const String& callingPackage,
        /* [out] */ IList** list);

    //@Override
    CARAPI GetAllCellInfo(
        /* [out] */ IList** list);

    //@Override
    CARAPI SetCellInfoListRate(
        /* [in] */ Int32 rateInMillis);

    CARAPI GetActivePhoneType(
        /* [out] */ Int32* result);

    CARAPI GetActivePhoneTypeForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    /**
     * Returns the CDMA ERI icon index to display
     */
    CARAPI GetCdmaEriIconIndex(
        /* [out] */ Int32* result);

    CARAPI GetCdmaEriIconIndexForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    /**
     * Returns the CDMA ERI icon mode,
     * 0 - ON
     * 1 - FLASHING
     */
    CARAPI GetCdmaEriIconMode(
        /* [out] */ Int32* result);

    CARAPI GetCdmaEriIconModeForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    /**
     * Returns the CDMA ERI text,
     */
    CARAPI GetCdmaEriText(
        /* [out] */ String* str);

    CARAPI GetCdmaEriTextForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ String* str);

    /**
     * Returns the CDMA MDN.
     */
    CARAPI GetCdmaMdn(
        /* [in] */ Int64 subId,
        /* [out] */ String* str);

    /**
     * Returns the CDMA MIN.
     */
    CARAPI GetCdmaMin(
        /* [in] */ Int64 subId,
        /* [out] */ String* str);

    /**
     * Returns true if CDMA provisioning needs to run.
     */
    CARAPI NeedsOtaServiceProvisioning(
        /* [out] */ Boolean* result);

    /**
     * Returns the unread count of voicemails
     */
    CARAPI GetVoiceMessageCount(
        /* [out] */ Int32* result);

    /**
     * Returns the unread count of voicemails for a subId
     */
    CARAPI GetVoiceMessageCountForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    /**
     * Returns the data network type
     *
     * @Deprecated to be removed Q3 2013 use {@link #getDataNetworkType}.
     */
    //@Override
    CARAPI GetNetworkType(
        /* [out] */ Int32* result);

    /**
     * Returns the network type for a subId
     */
    //@Override
    CARAPI GetNetworkTypeForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    /**
     * Returns the data network type
     */
    //@Override
    CARAPI GetDataNetworkType(
        /* [out] */ Int32* result);

    /**
     * Returns the data network type for a subId
     */
    //@Override
    CARAPI GetDataNetworkTypeForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    /**
     * Returns the data network type
     */
    //@Override
    CARAPI GetVoiceNetworkType(
        /* [out] */ Int32* result);

    /**
     * Returns the Voice network type for a subId
     */
    //@Override
    CARAPI GetVoiceNetworkTypeForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    /**
     * @return true if a ICC card is present
     */
    CARAPI HasIccCard(
        /* [out] */ Boolean* result);

    /**
     * @return true if a ICC card is present for a slotId
     */
    CARAPI HasIccCardUsingSlotId(
        /* [in] */ Int64 slotId,
        /* [out] */ Boolean* result);

    /**
     * Return if the current radio is LTE on CDMA. This
     * is a tri-state return value as for a period of time
     * the mode may be unknown.
     *
     * @return {@link Phone#LTE_ON_CDMA_UNKNOWN}, {@link Phone#LTE_ON_CDMA_FALSE}
     * or {@link Phone#LTE_ON_CDMA_TRUE}
     */
    CARAPI GetLteOnCdmaMode(
        /* [out] */ Int32* result);

    CARAPI GetLteOnCdmaModeForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    CARAPI SetPhone(
        /* [in] */ IPhone* phone);

    /**
     * @see android.telephony.TelephonyManager.WifiCallingChoices
     */
    CARAPI GetWhenToMakeWifiCalls(
        /* [out] */ Int32* result);

    /**
     * get operator numeric value from icc records
     */
    CARAPI GetIccOperatorNumeric(
        /* [in] */ Int64 subId,
        /* [out] */ String* result);

    /**
     * @see android.telephony.TelephonyManager.WifiCallingChoices
     */
    CARAPI SetWhenToMakeWifiCalls(
        /* [in] */ Int32 preference);

    //@Override
    CARAPI IccOpenLogicalChannel(
        /* [in] */ const String& AID,
        /* [out] */ IIccOpenLogicalChannelResponse** respanse);

    //@Override
    CARAPI IccCloseLogicalChannel(
        /* [in] */ Int32 channel,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI IccTransmitApduLogicalChannel(
        /* [in] */ Int32 channel,
        /* [in] */ Int32 cla,
        /* [in] */ Int32 command,
        /* [in] */ Int32 p1,
        /* [in] */ Int32 p2,
        /* [in] */ Int32 p3,
        /* [in] */ const String& data,
        /* [out] */ String* result);

    //@Override
    CARAPI IccTransmitApduBasicChannel(
        /* [in] */ Int32 cla,
        /* [in] */ Int32 command,
        /* [in] */ Int32 p1,
        /* [in] */ Int32 p2,
        /* [in] */ Int32 p3,
        /* [in] */ const String& data,
        /* [out] */ String* result);

    //@Override
    CARAPI IccExchangeSimIO(
        /* [in] */ Int32 fileID,
        /* [in] */ Int32 command,
        /* [in] */ Int32 p1,
        /* [in] */ Int32 p2,
        /* [in] */ Int32 p3,
        /* [in] */ const String& filePath,
        /* [out] */ ArrayOf<Byte>** array);

    //@Override
    CARAPI SendEnvelopeWithStatus(
        /* [in] */ const String& content,
        /* [out] */ String* result);

    /**
     * Read one of the NV items defined in {@link com.android.internal.telephony.RadioNVItems}
     * and {@code ril_nv_items.h}. Used for device configuration by some CDMA operators.
     *
     * @param itemID the ID of the item to read
     * @return the NV item as a String, or null on error.
     */
    //@Override
    CARAPI NvReadItem(
        /* [in] */ Int32 itemID,
        /* [out] */ String* result);

    /**
     * Write one of the NV items defined in {@link com.android.internal.telephony.RadioNVItems}
     * and {@code ril_nv_items.h}. Used for device configuration by some CDMA operators.
     *
     * @param itemID the ID of the item to read
     * @param itemValue the value to write, as a String
     * @return true on success; false on any failure
     */
    //@Override
    CARAPI NvWriteItem(
        /* [in] */ Int32 itemID,
        /* [in] */ const String& itemValue,
        /* [out] */ Boolean* result);

    /**
     * Update the CDMA Preferred Roaming List (PRL) in the radio NV storage.
     * Used for device configuration by some CDMA operators.
     *
     * @param preferredRoamingList byte array containing the new PRL
     * @return true on success; false on any failure
     */
    //@Override
    CARAPI NvWriteCdmaPrl(
        /* [in] */ ArrayOf<Byte>* preferredRoamingList,
        /* [out] */ Boolean* result);

    /**
     * Perform the specified type of NV config reset.
     * Used for device configuration by some CDMA operators.
     *
     * @param resetType the type of reset to perform (1 == factory reset; 2 == NV-only reset)
     * @return true on success; false on any failure
     */
    //@Override
    CARAPI NvResetConfig(
        /* [in] */ Int32 resetType,
        /* [out] */ Boolean* result);

    /**
     * {@hide}
     * Returns Default sim, 0 in the case of single standby.
     */
    CARAPI GetDefaultSim(
        /* [out] */ Int32* result);

    CARAPI GetPcscfAddress(
        /* [in] */ const String& apnType,
        /* [out] */ ArrayOf<String>** array);

    CARAPI SetImsRegistrationState(
        /* [in] */ Boolean registered);

    /**
     * Get the calculated preferred network type.
     * Used for debugging incorrect network type.
     *
     * @return the preferred network type, defined in RILConstants.java.
     */
    //@Override
    CARAPI GetCalculatedPreferredNetworkType(
        /* [out] */ Int32* result);

    /**
     * Get the preferred network type.
     * Used for device configuration by some CDMA operators.
     *
     * @return the preferred network type, defined in RILConstants.java.
     */
    //@Override
    CARAPI GetPreferredNetworkType(
        /* [out] */ Int32* result);

    /**
     * Set the preferred network type.
     * Used for device configuration by some CDMA operators.
     *
     * @param networkType the preferred network type, defined in RILConstants.java.
     * @return true on success; false on any failure.
     */
    //@Override
    CARAPI SetPreferredNetworkType(
        /* [in] */ Int32 networkType,
        /* [out] */ Boolean* result);

    /**
     * Set mobile data enabled
     * Used by the user through settings etc to turn on/off mobile data
     *
     * @param enable {@code true} turn turn data on, else {@code false}
     */
    //@Override
    CARAPI SetDataEnabled(
        /* [in] */ Boolean enable);

    /**
     * Get whether mobile data is enabled.
     *
     * Note that this used to be available from ConnectivityService, gated by
     * ACCESS_NETWORK_STATE permission, so this will accept either that or
     * our MODIFY_PHONE_STATE.
     *
     * @return {@code true} if data is enabled else {@code false}
     */
    //@Override
    CARAPI GetDataEnabled(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI HasCarrierPrivileges(
        /* [out] */ Int32* result);

    //@Override
    CARAPI CheckCarrierPrivilegesForPackage(
        /* [in] */ const String& pkgname,
        /* [out] */ Int32* result);

    //@Override
    CARAPI GetCarrierPackageNamesForIntent(
        /* [in] */ IIntent* intent,
        /* [out] */ IList** result);

    //@Override
    CARAPI EnableSimplifiedNetworkSettingsForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ Boolean enable);

    //@Override
    CARAPI GetSimplifiedNetworkSettingsEnabledForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI SetLine1NumberForDisplayForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& alphaTag,
        /* [in] */ const String& number);

    //@Override
    CARAPI GetLine1NumberForDisplay(
        /* [in] */ Int64 subId,
        /* [out] */ String* result);

    //@Override
    CARAPI GetLine1AlphaTagForDisplay(
        /* [in] */ Int64 subId,
        /* [out] */ String* result);

    //@Override
    CARAPI SetOperatorBrandOverride(
        /* [in] */ const String& brand,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI InvokeOemRilRequestRaw(
        /* [in] */ ArrayOf<Byte>* oemReq,
        /* [out, callee] */ ArrayOf<Byte>** oemResp,
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    /**
     * Posts the specified command to be executed on the main thread,
     * waits for the request to complete, and returns the result.
     * @see #sendRequestAsync
     */
    CARAPI SendRequest(
        /* [in] */ Int32 command,
        /* [in] */ IInterface* argument,
        /* [out] */ IInterface** result);

    /**
     * Posts the specified command to be executed on the main thread,
     * waits for the request to complete, and returns the result.
     * @see #sendRequestAsync
     */
    CARAPI SendRequest(
        /* [in] */ Int32 command,
        /* [in] */ IInterface* argument,
        /* [in] */ IInterface* argument2,
        /* [out] */ IInterface** result);

    /**
     * Asynchronous ("fire and forget") version of sendRequest():
     * Posts the specified command to be executed on the main thread, and
     * returns immediately.
     * @see #sendRequest
     */
    CARAPI_(void) SendRequestAsync(
        /* [in] */ Int32 command);

    /**
     * Same as {@link #sendRequestAsync(int)} except it takes an argument.
     * @see {@link #sendRequest(int,Object)}
     */
    CARAPI_(void) SendRequestAsync(
        /* [in] */ Int32 command,
        /* [in] */ IInterface* argument);

    CARAPI_(void) Publish();

    // returns phone associated with the subId.
    // getPhone(0) returns default phone in single SIM mode.
    CARAPI_(AutoPtr<IPhone>) GetPhone(
        /* [in] */ Int64 subId);

    /**
     * Make the actual telephony calls to implement answerRingingCall().
     * This should only be called from the main thread of the Phone app.
     * @see #answerRingingCall
     *
     * TODO: it would be nice to return true if we answered the call, or
     * false if there wasn't actually a ringing incoming call, or some
     * other error occurred.  (In other words, pass back the return value
     * from PhoneUtils.answerCall() or PhoneUtils.answerAndEndActive().)
     * But that would require calling this method via sendRequest() rather
     * than sendRequestAsync(), and right now we don't actually *need* that
     * return value, so let's just return void for now.
     */
    CARAPI_(void) AnswerRingingCallInternal();

    CARAPI_(void) ShutdownRadioUsingPhoneId(
        /* [in] */ Int32 phoneId);

    //
    // Internal helper methods.
    //
    static CARAPI_(Boolean) CheckIfCallerIsSelfOrForegroundUser();

    /**
     * Make sure the caller has the READ_PHONE_STATE permission.
     *
     * @throws SecurityException if the caller does not have the required permission
     */
    CARAPI EnforceReadPermission();

    /**
     * Make sure the caller has the MODIFY_PHONE_STATE permission.
     *
     * @throws SecurityException if the caller does not have the required permission
     */
    CARAPI EnforceModifyPermission();

    /**
     * Make sure either system app or the caller has carrier privilege.
     *
     * @throws SecurityException if the caller does not have the required permission/privilege
     */
    CARAPI EnforceModifyPermissionOrCarrierPrivilege();

    /**
     * Make sure the caller has carrier privilege.
     *
     * @throws SecurityException if the caller does not have the required permission
     */
    CARAPI EnforceCarrierPrivilege();

    /**
     * Make sure the caller has the CALL_PHONE permission.
     *
     * @throws SecurityException if the caller does not have the required permission
     */
    CARAPI EnforceCallPermission();

    /**
     * Make sure the caller has the READ_PRIVILEGED_PHONE_STATE permission.
     *
     * @throws SecurityException if the caller does not have the required permission
     */
    CARAPI EnforcePrivilegedPhoneStatePermission();

    CARAPI_(String) CreateTelUrl(
        /* [in] */ const String& number);

    static CARAPI_(void) Log(
        /* [in] */ const String& msg);

    static CARAPI_(void) Logv(
        /* [in] */ const String& msg);

    static CARAPI_(void) Loge(
        /* [in] */ const String& msg);

    /**
     * {@hide}
     * Returns Default subId, 0 in the case of single standby.
     */
    CARAPI_(Int64) GetDefaultSubscription();

    CARAPI_(Int64) GetPreferredVoiceSubscription();

    static CARAPI_(Int32) GetWhenToMakeWifiCallsDefaultPreference();

    CARAPI_(String) GetIccId(
        /* [in] */ Int64 subId);

private:
    static const String TAG;
    static const Boolean DBG;
    static const Boolean DBG_LOC;

    // Message codes used with mMainThreadHandler
    static const Int32 CMD_HANDLE_PIN_MMI;
    static const Int32 CMD_HANDLE_NEIGHBORING_CELL;
    static const Int32 EVENT_NEIGHBORING_CELL_DONE;
    static const Int32 CMD_ANSWER_RINGING_CALL;
    static const Int32 CMD_END_CALL;  // not used yet
    static const Int32 CMD_TRANSMIT_APDU_LOGICAL_CHANNEL;
    static const Int32 EVENT_TRANSMIT_APDU_LOGICAL_CHANNEL_DONE;
    static const Int32 CMD_OPEN_CHANNEL;
    static const Int32 EVENT_OPEN_CHANNEL_DONE;
    static const Int32 CMD_CLOSE_CHANNEL;
    static const Int32 EVENT_CLOSE_CHANNEL_DONE;
    static const Int32 CMD_NV_READ_ITEM;
    static const Int32 EVENT_NV_READ_ITEM_DONE;
    static const Int32 CMD_NV_WRITE_ITEM;
    static const Int32 EVENT_NV_WRITE_ITEM_DONE;
    static const Int32 CMD_NV_WRITE_CDMA_PRL;
    static const Int32 EVENT_NV_WRITE_CDMA_PRL_DONE;
    static const Int32 CMD_NV_RESET_CONFIG;
    static const Int32 EVENT_NV_RESET_CONFIG_DONE;
    static const Int32 CMD_GET_PREFERRED_NETWORK_TYPE;
    static const Int32 EVENT_GET_PREFERRED_NETWORK_TYPE_DONE;
    static const Int32 CMD_SET_PREFERRED_NETWORK_TYPE;
    static const Int32 EVENT_SET_PREFERRED_NETWORK_TYPE_DONE;
    static const Int32 CMD_SEND_ENVELOPE;
    static const Int32 EVENT_SEND_ENVELOPE_DONE;
    static const Int32 CMD_INVOKE_OEM_RIL_REQUEST_RAW;
    static const Int32 EVENT_INVOKE_OEM_RIL_REQUEST_RAW_DONE;
    static const Int32 CMD_TRANSMIT_APDU_BASIC_CHANNEL;
    static const Int32 EVENT_TRANSMIT_APDU_BASIC_CHANNEL_DONE;
    static const Int32 CMD_EXCHANGE_SIM_IO;
    static const Int32 EVENT_EXCHANGE_SIM_IO_DONE;

    /** The singleton instance. */
    static AutoPtr<CPhoneInterfaceManager> sInstance;

    PhoneGlobals* mApp;
    AutoPtr<IPhone> mPhone;
    AutoPtr<ICallManager> mCM;
    AutoPtr<IAppOpsManager> mAppOps;
    AutoPtr<MainThreadHandler> mMainThreadHandler;

    AutoPtr<ISharedPreferences> mCarrierPrivilegeConfigs;
    static const String PREF_CARRIERS_ALPHATAG_PREFIX;
    static const String PREF_CARRIERS_NUMBER_PREFIX ;
    static const String PREF_CARRIERS_SIMPLIFIED_NETWORK_SETTINGS_PREFIX;
    static Object THIS;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CPHONEINTERFACEMANAGER_H__
