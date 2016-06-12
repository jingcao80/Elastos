
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_RIL_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_RIL_H__

#include "_Elastos.Droid.Internal.h"
#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/internal/telephony/BaseCommands.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Hardware::Display::IDisplayListener;
using Elastos::Droid::Internal::Telephony::BaseCommands;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::ICommandsInterfaceRadioState;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaSmsBroadcastConfigInfo;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaInformationRecords;
using Elastos::Droid::Internal::Telephony::DataConnection::IDataProfile;
using Elastos::Droid::Internal::Telephony::DataConnection::IDataCallResponse;
using Elastos::Droid::Internal::Telephony::Gsm::ISmsBroadcastConfigInfo;
using Elastos::Droid::Net::ILocalSocket;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::View::IDisplay;

using Elastos::Core::Object;
using Elastos::Core::IRunnable;
using Elastos::Core::IInteger32;
using Elastos::Core::IThread;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::IO::IInputStream;
using Elastos::IO::IByteBuffer;
using Elastos::Utility::IRandom;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * {@hide}
 */
class RILRequest
    : public Object
{
public:
    /**
     * Retrieves a new RILRequest instance from the pool.
     *
     * @param request RIL_REQUEST_*
     * @param result sent when operation completes
     * @return a RILRequest instance from the pool.
     */
    static AutoPtr<RILRequest> Obtain(
        /* [in] */ Int32 request,
        /* [in] */ IMessage* result);

    /**
     * Returns a RILRequest instance to the pool.
     *
     * Note: This should only be called once per use.
     */
    UInt32 Release();

    static void ResetSerial();

    String SerialString();

    void OnError(
        /* [in] */ Int32 error,
        /* [in] */ IInterface* ret);

private:
    RILRequest();

public:
    // static const String LOG_TAG;

    //***** Class Variables
    static AutoPtr<IRandom> sRandom;
    static AutoPtr<IAtomicInteger32> sNextSerial;

    //***** Instance Variables
    Int32 mSerial;
    Int32 mRequest;
    AutoPtr<IMessage> mResult;
    AutoPtr<IParcel> mParcel;
    AutoPtr<RILRequest> mNext;

private:
    static Object sPoolSync;
    static AutoPtr<RILRequest> sPool;
    static Int32 sPoolSize;
    static Int32 MAX_POOL_SIZE;
    AutoPtr<IContext> mContext;
};

/**
 * RIL implementation of the CommandsInterface.
 *
 * {@hide}
 */
class RIL
    : public BaseCommands
{
public:
    class RILSender
        : public Handler
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        RILSender(
            /* [in] */ ILooper* looper,
            /* [in] */ RIL* host);

        //***** Runnable implementation
        CARAPI Run();


        //***** Handler implementation
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        // Only allocated once
        AutoPtr<ArrayOf<Byte> > mDataLength;
        RIL* mHost;
    };

    class RILReceiver
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        RILReceiver(
            /* [in] */ RIL* host);

        CARAPI Run();

    public:
        AutoPtr<ArrayOf<Byte> > mBuffer;
        RIL* mHost;
    };

    class UnsolOemHookBuffer
        : public Object
    {
    public:
        UnsolOemHookBuffer(
            /* [in] */ Int32 rilInstance,
            /* [in] */ ArrayOf<Byte>* data);

        CARAPI GetRilInstance(
            /* [out] */ Int32* result);

        CARAPI GetUnsolOemHookBuffer(
            /* [out] */ ArrayOf<Byte>** result);

    private:
        Int32 mRilInstance;
        AutoPtr<ArrayOf<Byte> > mData;
    };

private:
    class DisplayListener
        : public Object
        , public IDisplayListener
    {
    public:
        CAR_INTERFACE_DECL()

        DisplayListener(
            /* [in] */ RIL* host);

        CARAPI OnDisplayAdded(
            /* [in] */ Int32 displayId);

        CARAPI OnDisplayRemoved(
            /* [in] */ Int32 displayId);

        CARAPI OnDisplayChanged(
            /* [in] */ Int32 displayId);

    public:
        RIL* mHost;
    };

public:
    //***** Constructors
    RIL(
        /* [in] */ IContext* context,
        /* [in] */ Int32 preferredNetworkType,
        /* [in] */ Int32 cdmaSubscription);

    RIL(
        /* [in] */ IContext* context,
        /* [in] */ Int32 preferredNetworkType,
        /* [in] */ Int32 cdmaSubscription,
        /* [in] */ IInteger32* instanceId);

    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ Int32 preferredNetworkType,
        /* [in] */ Int32 cdmaSubscription,
        /* [in] */ IInteger32* instanceId);

    //***** CommandsInterface implementation

    CARAPI GetVoiceRadioTechnology(
        /* [in] */ IMessage* result);

    CARAPI GetImsRegistrationState(
        /* [in] */ IMessage* result);

    CARAPI SetOnNITZTime(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI GetIccCardStatus(
        /* [in] */ IMessage* result);

    CARAPI SetUiccSubscription(
        /* [in] */ Int32 slotId,
        /* [in] */ Int32 appIndex,
        /* [in] */ Int32 subId,
        /* [in] */ Int32 subStatus,
        /* [in] */ IMessage* result);

    // FIXME This API should take an AID and slot ID
    CARAPI SetDataAllowed(
        /* [in] */ Boolean allowed,
        /* [in] */ IMessage* result);

    CARAPI GetDataCallProfile(
        /* [in] */ Int32 appType,
        /* [in] */ IMessage* result);

    CARAPI SupplyIccPin(
        /* [in] */ String pin,
        /* [in] */ IMessage* result);

    CARAPI SupplyIccPinForApp(
        /* [in] */ String pin,
        /* [in] */ String aid,
        /* [in] */ IMessage* result);

    CARAPI SupplyIccPuk(
        /* [in] */ String puk,
        /* [in] */ String newPin,
        /* [in] */ IMessage* result);

    CARAPI SupplyIccPukForApp(
        /* [in] */ String puk,
        /* [in] */ String newPin,
        /* [in] */ String aid,
        /* [in] */ IMessage* result);

    CARAPI SupplyIccPin2(
        /* [in] */ String pin,
        /* [in] */ IMessage* result);

    CARAPI SupplyIccPin2ForApp(
        /* [in] */ String pin,
        /* [in] */ String aid,
        /* [in] */ IMessage* result);

    CARAPI SupplyIccPuk2(
        /* [in] */ String puk2,
        /* [in] */ String newPin2,
        /* [in] */ IMessage* result);

    CARAPI SupplyIccPuk2ForApp(
        /* [in] */ String puk,
        /* [in] */ String newPin2,
        /* [in] */ String aid,
        /* [in] */ IMessage* result);

    CARAPI ChangeIccPin(
        /* [in] */ String oldPin,
        /* [in] */ String newPin,
        /* [in] */ IMessage* result);

    CARAPI ChangeIccPinForApp(
        /* [in] */ String oldPin,
        /* [in] */ String newPin,
        /* [in] */ String aid,
        /* [in] */ IMessage* result);

    CARAPI ChangeIccPin2(
        /* [in] */ String oldPin2,
        /* [in] */ String newPin2,
        /* [in] */ IMessage* result);

    CARAPI ChangeIccPin2ForApp(
        /* [in] */ String oldPin2,
        /* [in] */ String newPin2,
        /* [in] */ String aid,
        /* [in] */ IMessage* result);

    CARAPI ChangeBarringPassword(
        /* [in] */ String facility,
        /* [in] */ String oldPwd,
        /* [in] */ String newPwd,
        /* [in] */ IMessage* result);

    CARAPI SupplyDepersonalization(
        /* [in] */ String netpin,
        /* [in] */ String type,
        /* [in] */ IMessage* result);

    CARAPI GetCurrentCalls(
        /* [in] */ IMessage* result);

    CARAPI GetPDPContextList(
        /* [in] */ IMessage* result);

    CARAPI GetDataCallList(
        /* [in] */ IMessage* result);

    CARAPI Dial(
        /* [in] */ String address,
        /* [in] */ Int32 clirMode,
        /* [in] */ IMessage* result);

    CARAPI Dial(
        /* [in] */ String address,
        /* [in] */ Int32 clirMode,
        /* [in] */ IUUSInfo* uusInfo,
        /* [in] */ IMessage* result);

    CARAPI GetIMSI(
        /* [in] */ IMessage* result);

    CARAPI GetIMSIForApp(
        /* [in] */ String aid,
        /* [in] */ IMessage* result);

    CARAPI GetIMEI(
        /* [in] */ IMessage* result);

    CARAPI GetIMEISV(
        /* [in] */ IMessage* result);

    CARAPI HangupConnection(
        /* [in] */ Int32 gsmIndex,
        /* [in] */ IMessage* result);

    CARAPI HangupWaitingOrBackground(
        /* [in] */ IMessage* result);

    CARAPI HangupForegroundResumeBackground(
        /* [in] */ IMessage* result);

    CARAPI SwitchWaitingOrHoldingAndActive(
        /* [in] */ IMessage* result);

    CARAPI Conference(
        /* [in] */ IMessage* result);

    CARAPI SetPreferredVoicePrivacy(
        /* [in] */ Boolean enable,
        /* [in] */ IMessage* result);

    CARAPI GetPreferredVoicePrivacy(
        /* [in] */ IMessage* result);

    CARAPI SeparateConnection(
        /* [in] */ Int32 gsmIndex,
        /* [in] */ IMessage* result);

    CARAPI AcceptCall(
        /* [in] */ IMessage* result);

    CARAPI RejectCall(
        /* [in] */ IMessage* result);

    CARAPI ExplicitCallTransfer(
        /* [in] */ IMessage* result);

    CARAPI GetLastCallFailCause(
        /* [in] */ IMessage* result);

    /**
     * @deprecated
     */
    CARAPI GetLastPdpFailCause(
        /* [in] */ IMessage* result);

    /**
     * The preferred new alternative to getLastPdpFailCause
     */
    CARAPI GetLastDataCallFailCause(
        /* [in] */ IMessage* result);

    CARAPI SetMute(
        /* [in] */ Boolean enableMute,
        /* [in] */ IMessage* response);

    CARAPI GetMute(
        /* [in] */ IMessage* response);

    CARAPI GetSignalStrength(
        /* [in] */ IMessage* result);

    CARAPI GetVoiceRegistrationState(
        /* [in] */ IMessage* result);

    CARAPI GetDataRegistrationState(
        /* [in] */ IMessage* result);

    CARAPI GetOperator(
        /* [in] */ IMessage* result);

    CARAPI GetHardwareConfig(
        /* [in] */ IMessage* result);

    CARAPI SendDtmf(
        /* [in] */ Char32 c,
        /* [in] */ IMessage* result);

    CARAPI StartDtmf(
        /* [in] */ Char32 c,
        /* [in] */ IMessage* result);

    CARAPI StopDtmf(
        /* [in] */ IMessage* result);

    CARAPI SendBurstDtmf(
        /* [in] */ String dtmfString,
        /* [in] */ Int32 on,
        /* [in] */ Int32 off,
        /* [in] */ IMessage* result);

    CARAPI SendSMS(
        /* [in] */ String smscPDU,
        /* [in] */ String pdu,
        /* [in] */ IMessage* result);

    CARAPI SendSMSExpectMore(
        /* [in] */ String smscPDU,
        /* [in] */ String pdu,
        /* [in] */ IMessage* result);

    CARAPI SendCdmaSms(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [in] */ IMessage* result);

    CARAPI SendImsGsmSms(
        /* [in] */ String smscPDU,
        /* [in] */ String pdu,
        /* [in] */ Int32 retry,
        /* [in] */ Int32 messageRef,
        /* [in] */ IMessage* result);

    CARAPI SendImsCdmaSms(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [in] */ Int32 retry,
        /* [in] */ Int32 messageRef,
        /* [in] */ IMessage* result);

    CARAPI DeleteSmsOnSim(
        /* [in] */ Int32 index,
        /* [in] */ IMessage* response);

    CARAPI DeleteSmsOnRuim(
        /* [in] */ Int32 index,
        /* [in] */ IMessage* response);

    CARAPI WriteSmsToSim(
        /* [in] */ Int32 status,
        /* [in] */ String smsc,
        /* [in] */ String pdu,
        /* [in] */ IMessage* response);

    CARAPI WriteSmsToRuim(
        /* [in] */ Int32 status,
        /* [in] */ String pdu,
        /* [in] */ IMessage* response);

    CARAPI SetupDataCall(
        /* [in] */ String radioTechnology,
        /* [in] */ String profile,
        /* [in] */ String apn,
        /* [in] */ String user,
        /* [in] */ String password,
        /* [in] */ String authType,
        /* [in] */ String protocol,
        /* [in] */ IMessage* result);

    CARAPI DeactivateDataCall(
        /* [in] */ Int32 cid,
        /* [in] */ Int32 reason,
        /* [in] */ IMessage* result);

    CARAPI SetRadioPower(
        /* [in] */ Boolean on,
        /* [in] */ IMessage* result);

    CARAPI RequestShutdown(
        /* [in] */ IMessage* result);

    CARAPI SetSuppServiceNotifications(
        /* [in] */ Boolean enable,
        /* [in] */ IMessage* result);

    CARAPI AcknowledgeLastIncomingGsmSms(
        /* [in] */ Boolean success,
        /* [in] */ Int32 cause,
        /* [in] */ IMessage* result);

    CARAPI AcknowledgeLastIncomingCdmaSms(
        /* [in] */ Boolean success,
        /* [in] */ Int32 cause,
        /* [in] */ IMessage* result);

    CARAPI AcknowledgeIncomingGsmSmsWithPdu(
        /* [in] */ Boolean success,
        /* [in] */ String ackPdu,
        /* [in] */ IMessage* result);

    CARAPI IccIO(
        /* [in] */ Int32 command,
        /* [in] */ Int32 fileid,
        /* [in] */ String path,
        /* [in] */ Int32 p1,
        /* [in] */ Int32 p2,
        /* [in] */ Int32 p3,
        /* [in] */ String data,
        /* [in] */ String pin2,
        /* [in] */ IMessage* result);

    CARAPI IccIOForApp(
        /* [in] */ Int32 command,
        /* [in] */ Int32 fileid,
        /* [in] */ String path,
        /* [in] */ Int32 p1,
        /* [in] */ Int32 p2,
        /* [in] */ Int32 p3,
        /* [in] */ String data,
        /* [in] */ String pin2,
        /* [in] */ String aid,
        /* [in] */ IMessage* result);

    CARAPI GetCLIR(
        /* [in] */ IMessage* result);

    CARAPI SetCLIR(
        /* [in] */ Int32 clirMode,
        /* [in] */ IMessage* result);

    CARAPI QueryCallWaiting(
        /* [in] */ Int32 serviceClass,
        /* [in] */ IMessage* response);

    CARAPI SetCallWaiting(
        /* [in] */ Boolean enable,
        /* [in] */ Int32 serviceClass,
        /* [in] */ IMessage* response);

    CARAPI SetNetworkSelectionModeAutomatic(
        /* [in] */ IMessage* response);

    CARAPI SetNetworkSelectionModeManual(
        /* [in] */ String operatorNumeric,
        /* [in] */ IMessage* response);

    CARAPI GetNetworkSelectionMode(
        /* [in] */ IMessage* response);

    CARAPI GetAvailableNetworks(
        /* [in] */ IMessage* response);

    CARAPI SetCallForward(
        /* [in] */ Int32 action,
        /* [in] */ Int32 cfReason,
        /* [in] */ Int32 serviceClass,
        /* [in] */ String number,
        /* [in] */ Int32 timeSeconds,
        /* [in] */ IMessage* response);

    CARAPI QueryCallForwardStatus(
        /* [in] */ Int32 cfReason,
        /* [in] */ Int32 serviceClass,
        /* [in] */ String number,
        /* [in] */ IMessage* response);

    CARAPI QueryCLIP(
        /* [in] */ IMessage* response);

    CARAPI GetBasebandVersion(
        /* [in] */ IMessage* response);

    CARAPI QueryFacilityLock(
        /* [in] */ String facility,
        /* [in] */ String password,
        /* [in] */ Int32 serviceClass,
        /* [in] */ IMessage* response);

    CARAPI QueryFacilityLockForApp(
        /* [in] */ String facility,
        /* [in] */ String password,
        /* [in] */ Int32 serviceClass,
        /* [in] */ String appId,
        /* [in] */ IMessage* response);

    CARAPI SetFacilityLock(
        /* [in] */ String facility,
        /* [in] */ Boolean lockState,
        /* [in] */ String password,
        /* [in] */ Int32 serviceClass,
        /* [in] */ IMessage* response);

    CARAPI SetFacilityLockForApp(
        /* [in] */ String facility,
        /* [in] */ Boolean lockState,
        /* [in] */ String password,
        /* [in] */ Int32 serviceClass,
        /* [in] */ String appId,
        /* [in] */ IMessage* response);

    CARAPI SendUSSD(
        /* [in] */ String ussdString,
        /* [in] */ IMessage* response);

    // inherited javadoc suffices
    CARAPI CancelPendingUssd(
        /* [in] */ IMessage* response);

    CARAPI ResetRadio(
        /* [in] */ IMessage* result);

    CARAPI SetLocalCallHold(
        /* [in] */ Int32 lchStatus);

    CARAPI GetModemCapability(
        /* [in] */ IMessage* response);

    CARAPI UpdateStackBinding(
        /* [in] */ Int32 stack,
        /* [in] */ Int32 enable,
        /* [in] */ IMessage* response);

    CARAPI InvokeOemRilRequestRaw(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ IMessage* response);

    CARAPI InvokeOemRilRequestStrings(
        /* [in] */ ArrayOf<String>* strings,
        /* [in] */ IMessage* response);

     /**
     * Assign a specified band for RF configuration.
     *
     * @param bandMode one of BM_*_BAND
     * @param response is callback message
     */
    CARAPI SetBandMode(
        /* [in] */ Int32 bandMode,
        /* [in] */ IMessage* response);

    /**
     * Query the list of band mode supported by RF.
     *
     * @param response is callback message
     *        ((AsyncResult)response.obj).result  is an int[] where int[0] is
     *        the size of the array and the rest of each element representing
     *        one available BM_*_BAND
     */
    CARAPI QueryAvailableBandMode(
        /* [in] */ IMessage* response);

    /**
     * {@inheritDoc}
     */
    CARAPI SendTerminalResponse(
        /* [in] */ String contents,
        /* [in] */ IMessage* response);

    /**
     * {@inheritDoc}
     */
    CARAPI SendEnvelope(
        /* [in] */ String contents,
        /* [in] */ IMessage* response);

    /**
     * {@inheritDoc}
     */
    CARAPI SendEnvelopeWithStatus(
        /* [in] */ String contents,
        /* [in] */ IMessage* response);

    /**
     * {@inheritDoc}
     */
    CARAPI HandleCallSetupRequestFromSim(
        /* [in] */ Boolean accept,
        /* [in] */ IMessage* response);

    /**
     * {@inheritDoc}
     */
    CARAPI SetPreferredNetworkType(
        /* [in] */ Int32 networkType,
        /* [in] */ IMessage* response);

    /**
     * {@inheritDoc}
     */
    CARAPI GetPreferredNetworkType(
        /* [in] */ IMessage* response);

    /**
     * {@inheritDoc}
     */
    CARAPI GetNeighboringCids(
        /* [in] */ IMessage* response);

    /**
     * {@inheritDoc}
     */
    CARAPI SetLocationUpdates(
        /* [in] */ Boolean enable,
        /* [in] */ IMessage* response);

    /**
     * {@inheritDoc}
     */
    CARAPI GetSmscAddress(
        /* [in] */ IMessage* result);

    /**
     * {@inheritDoc}
     */
    CARAPI SetSmscAddress(
        /* [in] */ String address,
        /* [in] */ IMessage* result);

    /**
     * {@inheritDoc}
     */
    CARAPI ReportSmsMemoryStatus(
        /* [in] */ Boolean available,
        /* [in] */ IMessage* result);

    /**
     * {@inheritDoc}
     */
    CARAPI ReportStkServiceIsRunning(
        /* [in] */ IMessage* result);

    /**
     * {@inheritDoc}
     */
    CARAPI GetGsmBroadcastConfig(
        /* [in] */ IMessage* response);

    /**
     * {@inheritDoc}
     */
    CARAPI SetGsmBroadcastConfig(
        /* [in] */ ArrayOf<ISmsBroadcastConfigInfo*>* config,
        /* [in] */ IMessage* response);

    /**
     * {@inheritDoc}
     */
    CARAPI SetGsmBroadcastActivation(
        /* [in] */ Boolean activate,
        /* [in] */ IMessage* response);

    CARAPI NeedsOldRilFeature(
        /* [in] */ String feature,
        /* [out] */ Boolean* result);

    // ***** Methods for CDMA support
    CARAPI GetDeviceIdentity(
        /* [in] */ IMessage* response);

    CARAPI GetCDMASubscription(
        /* [in] */ IMessage* response);

    CARAPI SetPhoneType(
        /* [in] */ Int32 phoneType);

    /**
     * {@inheritDoc}
     */
    CARAPI QueryCdmaRoamingPreference(
        /* [in] */ IMessage* response);

    /**
     * {@inheritDoc}
     */
    CARAPI SetCdmaRoamingPreference(
        /* [in] */ Int32 cdmaRoamingType,
        /* [in] */ IMessage* response);

    /**
     * {@inheritDoc}
     */
    CARAPI SetCdmaSubscriptionSource(
        /* [in] */ Int32 cdmaSubscription,
        /* [in] */ IMessage* response);

    /**
     * {@inheritDoc}
     */
    CARAPI GetCdmaSubscriptionSource(
        /* [in] */ IMessage* response);

    /**
     * {@inheritDoc}
     */
    CARAPI QueryTTYMode(
        /* [in] */ IMessage* response);

    /**
     * {@inheritDoc}
     */
    CARAPI SetTTYMode(
        /* [in] */ Int32 ttyMode,
        /* [in] */ IMessage* response);

    /**
     * {@inheritDoc}
     */
    CARAPI SendCDMAFeatureCode(
        /* [in] */ String FeatureCode,
        /* [in] */ IMessage* response);

    CARAPI GetCdmaBroadcastConfig(
        /* [in] */ IMessage* response);

    CARAPI SetCdmaBroadcastConfig(
        /* [in] */ ArrayOf<ICdmaSmsBroadcastConfigInfo*>* configs,
        /* [in] */ IMessage* response);

    CARAPI SetCdmaBroadcastActivation(
        /* [in] */ Boolean activate,
        /* [in] */ IMessage* response);

    /**
     * {@inheritDoc}
     */
    CARAPI ExitEmergencyCallbackMode(
        /* [in] */ IMessage* response);

    CARAPI RequestIsimAuthentication(
        /* [in] */ String nonce,
        /* [in] */ IMessage* response);

    CARAPI RequestIccSimAuthentication(
        /* [in] */ Int32 authContext,
        /* [in] */ String data,
        /* [in] */ String aid,
        /* [in] */ IMessage* response);

    /**
     * {@inheritDoc}
     */
    CARAPI GetCellInfoList(
        /* [in] */ IMessage* result);

    /**
     * {@inheritDoc}
     */
    CARAPI SetCellInfoListRate(
        /* [in] */ Int32 rateInMillis,
        /* [in] */ IMessage* response);

    CARAPI SetInitialAttachApn(
        /* [in] */ String apn,
        /* [in] */ String protocol,
        /* [in] */ Int32 authType,
        /* [in] */ String username,
        /* [in] */ String password,
        /* [in] */ IMessage* result);

    CARAPI SetDataProfile(
        /* [in] */ ArrayOf<IDataProfile*>* dps,
        /* [in] */ IMessage* result);

    /* (non-Javadoc)
     * @see com.android.internal.telephony.BaseCommands#testingEmergencyCall()
     */
    CARAPI TestingEmergencyCall();

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    /**
     * {@inheritDoc}
     */
    CARAPI IccOpenLogicalChannel(
        /* [in] */ String AID,
        /* [in] */ IMessage* response);

    /**
     * {@inheritDoc}
     */
    CARAPI IccCloseLogicalChannel(
        /* [in] */ Int32 channel,
        /* [in] */ IMessage* response);

    /**
     * {@inheritDoc}
     */
    CARAPI IccTransmitApduLogicalChannel(
        /* [in] */ Int32 channel,
        /* [in] */ Int32 cla,
        /* [in] */ Int32 instruction,
        /* [in] */ Int32 p1,
        /* [in] */ Int32 p2,
        /* [in] */ Int32 p3,
        /* [in] */ String data,
        /* [in] */ IMessage* response);

    /**
     * {@inheritDoc}
     */
    CARAPI IccTransmitApduBasicChannel(
        /* [in] */ Int32 cla,
        /* [in] */ Int32 instruction,
        /* [in] */ Int32 p1,
        /* [in] */ Int32 p2,
        /* [in] */ Int32 p3,
        /* [in] */ String data,
        /* [in] */ IMessage* response);

    /**
     * {@inheritDoc}
     */
    CARAPI GetAtr(
        /* [in] */ IMessage* response);

    CARAPI NvReadItem(
        /* [in] */ Int32 itemID,
        /* [in] */ IMessage* response);

    CARAPI NvWriteItem(
        /* [in] */ Int32 itemID,
        /* [in] */ String itemValue,
        /* [in] */ IMessage* response);

    CARAPI NvWriteCdmaPrl(
        /* [in] */ ArrayOf<Byte>* preferredRoamingList,
        /* [in] */ IMessage* response);

    CARAPI NvResetConfig(
        /* [in] */ Int32 resetType,
        /* [in] */ IMessage* response);

protected:
    void SendScreenState(
        /* [in] */ Boolean on);

    CARAPI OnRadioAvailable();

    ICommandsInterfaceRadioState GetRadioStateFromInt(
        /* [in] */ Int32 stateInt);

    void SwitchToRadioState(
        /* [in] */ ICommandsInterfaceRadioState newState);

    void Send(
        /* [in] */ RILRequest* rr);

    void ProcessResponse(
        /* [in] */ IParcel* p);

    /**
     * Release each request in mRequestList then clear the list
     * @param error is the RIL_Errno sent back
     * @param loggable true means to print all requests in mRequestList
     */
    void ClearRequestList(
        /* [in] */ Int32 error,
        /* [in] */ Boolean loggable);

    AutoPtr<RILRequest> FindAndRemoveRequestFromList(
        /* [in] */ Int32 serial);

    AutoPtr<RILRequest> ProcessSolicited(
        /* [in] */ IParcel* p);

    static String RetToString(
        /* [in] */ Int32 req,
        /* [in] */ IInterface* ret);

    void ProcessUnsolicited(
        /* [in] */ IParcel* p);

    /**
     * Notifiy all registrants that the ril has connected or disconnected.
     *
     * @param rilVer is the version of the ril or -1 if disconnected.
     */
    void NotifyRegistrantsRilConnectionChanged(
        /* [in] */ Int32 rilVer);

    /** Notify registrants of WWAN coexistence event. */
    void NotifyWwanIwlanCoexist(
        /* [in] */ ArrayOf<Byte>* data);

    /** Notify registrants of SIM_REFRESH event. */
    void NotifySimRefresh(
        /* [in] */ ArrayOf<Byte>* data);

    /** Notify registrants of MODEM_CAPABILITY event. */
    void NotifyModemCap(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ IInteger32* phoneId);

    AutoPtr<IInterface> ResponseInts(
        /* [in] */ IParcel* p);

    AutoPtr<IInterface> ResponseVoid(
        /* [in] */ IParcel* p);

    AutoPtr<IInterface> ResponseCallForward(
        /* [in] */ IParcel* p);

    AutoPtr<IInterface> ResponseSuppServiceNotification(
        /* [in] */ IParcel* p);

    AutoPtr<IInterface> ResponseCdmaSms(
        /* [in] */ IParcel* p);

    AutoPtr<IInterface> ResponseString(
        /* [in] */ IParcel* p);

    AutoPtr<IInterface> ResponseStrings(
        /* [in] */ IParcel* p);

    AutoPtr<IInterface> ResponseRaw(
        /* [in] */ IParcel* p);

    AutoPtr<IInterface> ResponseSMS(
        /* [in] */ IParcel* p);

    AutoPtr<IInterface> ResponseICC_IO(
        /* [in] */ IParcel* p);

    AutoPtr<IInterface> ResponseIccCardStatus(
        /* [in] */ IParcel* p);

    AutoPtr<IInterface> ResponseSimRefresh(
        /* [in] */ IParcel* p);

    AutoPtr<IInterface> ResponseCallList(
        /* [in] */ IParcel* p);

    AutoPtr<IDataCallResponse> GetDataCallResponse(
        /* [in] */ IParcel* p,
        /* [in] */ Int32 version);

    AutoPtr<IInterface> ResponseDataCallList(
        /* [in] */ IParcel* p);

    AutoPtr<IInterface> ResponseSetupDataCall(
        /* [in] */ IParcel* p);

    AutoPtr<IInterface> ResponseOperatorInfos(
        /* [in] */ IParcel* p);

    AutoPtr<IInterface> ResponseCellList(
        /* [in] */ IParcel* p);

    AutoPtr<IInterface> ResponseGetPreferredNetworkType(
        /* [in] */ IParcel* p);

    AutoPtr<IInterface> ResponseGmsBroadcastConfig(
        /* [in] */ IParcel* p);

    AutoPtr<IInterface> ResponseCdmaBroadcastConfig(
        /* [in] */ IParcel* p);

    AutoPtr<IInterface> ResponseSignalStrength(
        /* [in] */ IParcel* p);

    AutoPtr<IArrayList> ResponseCdmaInformationRecord(
        /* [in] */ IParcel* p);

    AutoPtr<IInterface> ResponseCdmaCallWaiting(
        /* [in] */ IParcel* p);

    AutoPtr<IInterface> ResponseCallRing(
        /* [in] */ IParcel* p);

    void NotifyRegistrantsCdmaInfoRec(
        /* [in] */ ICdmaInformationRecords* infoRec);

    AutoPtr<IArrayList> ResponseCellInfoList(
        /* [in] */ IParcel* p);

    static String RequestToString(
        /* [in] */ Int32 request);

    static String ResponseToString(
        /* [in] */ Int32 request);

    void RiljLog(
        /* [in] */ String msg);

    void RiljLogv(
        /* [in] */ String msg);

    void UnsljLog(
        /* [in] */ Int32 response);

    void UnsljLogMore(
        /* [in] */ Int32 response,
        /* [in] */ String more);

    void UnsljLogRet(
        /* [in] */ Int32 response,
        /* [in] */ IInterface* ret);

    void UnsljLogvRet(
        /* [in] */ Int32 response,
        /* [in] */ IInterface* ret);

    AutoPtr<IInterface> ResponseSsData(
        /* [in] */ IParcel* p);

private:
    /**
     * Reads in a single RIL message off the wire. A RIL message consists
     * of a 4-byte little-endian length and a subsequent series of bytes.
     * The final message (length header omitted) is read into
     * <code>buffer</code> and the length of the final message (less header)
     * is returned. A return value of -1 indicates end-of-stream.
     *
     * @param is non-null; Stream to read from
     * @param buffer Buffer to fill in. Must be as large as maximum
     * message size, or an ArrayOutOfBounds exception will be thrown.
     * @return Length of message less header, or -1 on end of stream.
     * @throws IOException
     */
    static Int32 ReadRilMessage(
        /* [in] */ IInputStream* is,
        /* [in] */ ArrayOf<Byte>* buffer);

    void ConstructGsmSendSmsRilRequest(
        /* [in] */ RILRequest* rr,
        /* [in] */ String smscPDU,
        /* [in] */ String pdu);

    void ConstructCdmaSendSmsRilRequest(
        /* [in] */ RILRequest* rr,
        /* [in] */ ArrayOf<Byte>* pdu);

    /**
     *  Restructures PDU data so that it is consistent with RIL
     *  data structure.
     *
     *  @param pdu The data to be written to the RUIM card.
     */
    void ConstructCdmaWriteSmsRilRequest(
        /* [in] */ RILRequest* rr,
        /* [in] */ ArrayOf<Byte>* pdu);

    /**
     *  Translates EF_SMS status bits to a status value compatible with
     *  SMS AT commands.  See TS 27.005 3.1.
     */
    Int32 TranslateStatus(
        /* [in] */ Int32 status);

    void SendOemRilRequestRaw(
        /* [in] */ Int32 requestId,
        /* [in] */ Int32 numPayload,
        /* [in] */ ArrayOf<Byte>* payload,
        /* [in] */ IMessage* response);

    // TODO(jeffbrown): Delete me.
    // The RIL should *not* be listening for screen state changes since they are
    // becoming increasingly ambiguous on our devices.  The RIL_REQUEST_SCREEN_STATE
    // message should be deleted and replaced with more precise messages to control
    // behavior such as signal strength reporting or power managements based on
    // more robust signals.
    void UpdateScreenState();

    /**
     * Holds a PARTIAL_WAKE_LOCK whenever
     * a) There is outstanding RIL request sent to RIL deamon and no replied
     * b) There is a request pending to be sent out.
     *
     * There is a WAKE_LOCK_TIMEOUT to release the lock, though it shouldn't
     * happen often.
     */
    void AcquireWakeLock();

    void DecrementWakeLock();

    // true if we had the wakelock
    Boolean ClearWakeLock();

    Boolean IsQcUnsolOemHookResp(
        /* [in] */ IByteBuffer* oemHookResponse);

    void ProcessUnsolOemhookResponse(
        /* [in] */ IByteBuffer* oemHookResponse);

    AutoPtr<IInterface> ResponseICC_IOBase64(
        /* [in] */ IParcel* p);

    AutoPtr<IArrayList> ResponseGetDataCallProfile(
        /* [in] */ IParcel* p);

    AutoPtr<IInterface> ResponseHardwareConfig(
        /* [in] */ IParcel* p);

    /*
     * Helper function for the iccTransmitApdu* commands above.
     */
    void IccTransmitApduHelper(
        /* [in] */ Int32 rilCommand,
        /* [in] */ Int32 channel,
        /* [in] */ Int32 cla,
        /* [in] */ Int32 instruction,
        /* [in] */ Int32 p1,
        /* [in] */ Int32 p2,
        /* [in] */ Int32 p3,
        /* [in] */ String data,
        /* [in] */ IMessage* response);

public:
    static String RILJ_LOG_TAG;
    static Boolean RILJ_LOGD;
    static Boolean RILJ_LOGV; // STOPSHIP if true

    /**
     * Wake lock timeout should be longer than the longest timeout in
     * the vendor ril.
     */
    static Int32 DEFAULT_WAKE_LOCK_TIMEOUT;
    static Int32 BYTE_SIZE;

    /** Starting number for OEMHOOK request and response IDs */
    static Int32 OEMHOOK_BASE;
    Int32 OEMHOOK_UNSOL_SIM_REFRESH;
    const Int32 OEMHOOK_UNSOL_WWAN_IWLAN_COEXIST;

    /** Set Local Call Hold subscription */
    static Int32 OEMHOOK_EVT_HOOK_SET_LOCAL_CALL_HOLD;
    /** Get Modem Capabilities*/
    static Int32 OEMHOOK_EVT_HOOK_GET_MODEM_CAPABILITY;
    /** Update Stack Binding*/
    static Int32 OEMHOOK_EVT_HOOK_UPDATE_SUB_BINDING;

    static Int32 INT_SIZE;
    static String OEM_IDENTIFIER;
    Int32 mHeaderSize;
    Int32 QCRIL_EVT_HOOK_UNSOL_MODEM_CAPABILITY;

    //***** Instance Variables

    AutoPtr<ILocalSocket> mSocket;
    AutoPtr<IHandlerThread> mSenderThread;
    AutoPtr<RILSender> mSender;
    AutoPtr<IThread> mReceiverThread;
    AutoPtr<RILReceiver> mReceiver;
    AutoPtr<IDisplay> mDefaultDisplay;
    Int32 mDefaultDisplayState;
    AutoPtr<IPowerManagerWakeLock> mWakeLock;
    Object wakeLock;
    Int32 mWakeLockTimeout;
    // The number of wakelock requests currently active.  Don't release the lock
    // until dec'd to 0
    Int32 mWakeLockCount;

    AutoPtr<ISparseArray> mRequestList;
    Object listLock;

    AutoPtr<IInterface> mLastNITZTimeInfo;

    // When we are testing emergency calls
    AutoPtr<IAtomicBoolean> mTestingEmergencyCall;

    AutoPtr<IInteger32> mInstanceId;

    // Number of per-network elements expected in QUERY_AVAILABLE_NETWORKS's response.
    // 4 elements is default, but many RILs actually return 5, making it impossible to
    // divide the response array without prior knowledge of the number of elements.
    Int32 mQANElements;

    //***** Events

    static const Int32 EVENT_SEND;
    static const Int32 EVENT_WAKE_LOCK_TIMEOUT;

    //***** Constants

    // match with constant in ril.cpp
    static Int32 RIL_MAX_COMMAND_BYTES;
    static Int32 RESPONSE_SOLICITED;
    static Int32 RESPONSE_UNSOLICITED;

    static AutoPtr<ArrayOf<String> > SOCKET_NAME_RIL;

    static Int32 SOCKET_OPEN_RETRY_MILLIS;

    // The number of the required config values for broadcast SMS stored in the C struct
    // RIL_CDMA_BroadcastServiceInfo
    static Int32 CDMA_BSI_NO_OF_INTS_STRUCT;

    static Int32 CDMA_BROADCAST_SMS_NO_OF_SERVICE_CATEGORIES;

    AutoPtr<IDisplayListener> mDisplayListener;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_RIL_H__
