//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_RIL_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_RIL_H__

#include "_Elastos.Droid.Internal.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Net.h"
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "elastos/droid/internal/telephony/BaseCommands.h"
#include "elastos/droid/internal/telephony/RILParcel.h"
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
    static CARAPI_(AutoPtr<RILRequest>) Obtain(
        /* [in] */ Int32 request,
        /* [in] */ IMessage* result);

    /**
     * Returns a RILRequest instance to the pool.
     *
     * Note: This should only be called once per use.
     */
    CARAPI_(UInt32) ReleaseRequest();

    static CARAPI_(void) ResetSerial();

    CARAPI_(String) SerialString();

    CARAPI_(void) OnError(
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
    AutoPtr<RILParcel> mParcel;
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
    , public IRIL
{
public:
    class RILSender
        : public Handler
        , public IRunnable
    {
    public:
        TO_STRING_IMPL("RILSender")

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
    CAR_INTERFACE_DECL();

    RIL();

    //***** Constructors
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 preferredNetworkType,
        /* [in] */ Int32 cdmaSubscription);

    CARAPI constructor(
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
        /* [in] */ const String& pin,
        /* [in] */ IMessage* result);

    CARAPI SupplyIccPinForApp(
        /* [in] */ const String& pin,
        /* [in] */ const String& aid,
        /* [in] */ IMessage* result);

    CARAPI SupplyIccPuk(
        /* [in] */ const String& puk,
        /* [in] */ const String& newPin,
        /* [in] */ IMessage* result);

    CARAPI SupplyIccPukForApp(
        /* [in] */ const String& puk,
        /* [in] */ const String& newPin,
        /* [in] */ const String& aid,
        /* [in] */ IMessage* result);

    CARAPI SupplyIccPin2(
        /* [in] */ const String& pin,
        /* [in] */ IMessage* result);

    CARAPI SupplyIccPin2ForApp(
        /* [in] */ const String& pin,
        /* [in] */ const String& aid,
        /* [in] */ IMessage* result);

    CARAPI SupplyIccPuk2(
        /* [in] */ const String& puk2,
        /* [in] */ const String& newPin2,
        /* [in] */ IMessage* result);

    CARAPI SupplyIccPuk2ForApp(
        /* [in] */ const String& puk,
        /* [in] */ const String& newPin2,
        /* [in] */ const String& aid,
        /* [in] */ IMessage* result);

    CARAPI ChangeIccPin(
        /* [in] */ const String& oldPin,
        /* [in] */ const String& newPin,
        /* [in] */ IMessage* result);

    CARAPI ChangeIccPinForApp(
        /* [in] */ const String& oldPin,
        /* [in] */ const String& newPin,
        /* [in] */ const String& aid,
        /* [in] */ IMessage* result);

    CARAPI ChangeIccPin2(
        /* [in] */ const String& oldPin2,
        /* [in] */ const String& newPin2,
        /* [in] */ IMessage* result);

    CARAPI ChangeIccPin2ForApp(
        /* [in] */ const String& oldPin2,
        /* [in] */ const String& newPin2,
        /* [in] */ const String& aid,
        /* [in] */ IMessage* result);

    CARAPI ChangeBarringPassword(
        /* [in] */ const String& facility,
        /* [in] */ const String& oldPwd,
        /* [in] */ const String& newPwd,
        /* [in] */ IMessage* result);

    CARAPI SupplyDepersonalization(
        /* [in] */ const String& netpin,
        /* [in] */ const String& type,
        /* [in] */ IMessage* result);

    CARAPI GetCurrentCalls(
        /* [in] */ IMessage* result);

    CARAPI GetPDPContextList(
        /* [in] */ IMessage* result);

    CARAPI GetDataCallList(
        /* [in] */ IMessage* result);

    CARAPI Dial(
        /* [in] */ const String& address,
        /* [in] */ Int32 clirMode,
        /* [in] */ IMessage* result);

    CARAPI Dial(
        /* [in] */ const String& address,
        /* [in] */ Int32 clirMode,
        /* [in] */ IUUSInfo* uusInfo,
        /* [in] */ IMessage* result);

    CARAPI GetIMSI(
        /* [in] */ IMessage* result);

    CARAPI GetIMSIForApp(
        /* [in] */ const String& aid,
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
        /* [in] */ const String& dtmfString,
        /* [in] */ Int32 on,
        /* [in] */ Int32 off,
        /* [in] */ IMessage* result);

    CARAPI SendSMS(
        /* [in] */ const String& smscPDU,
        /* [in] */ const String& pdu,
        /* [in] */ IMessage* result);

    CARAPI SendSMSExpectMore(
        /* [in] */ const String& smscPDU,
        /* [in] */ const String& pdu,
        /* [in] */ IMessage* result);

    CARAPI SendCdmaSms(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [in] */ IMessage* result);

    CARAPI SendImsGsmSms(
        /* [in] */ const String& smscPDU,
        /* [in] */ const String& pdu,
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
        /* [in] */ const String& smsc,
        /* [in] */ const String& pdu,
        /* [in] */ IMessage* response);

    CARAPI WriteSmsToRuim(
        /* [in] */ Int32 status,
        /* [in] */ const String& pdu,
        /* [in] */ IMessage* response);

    CARAPI SetupDataCall(
        /* [in] */ const String& radioTechnology,
        /* [in] */ const String& profile,
        /* [in] */ const String& apn,
        /* [in] */ const String& user,
        /* [in] */ const String& password,
        /* [in] */ const String& authType,
        /* [in] */ const String& protocol,
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
        /* [in] */ const String& ackPdu,
        /* [in] */ IMessage* result);

    CARAPI IccIO(
        /* [in] */ Int32 command,
        /* [in] */ Int32 fileid,
        /* [in] */ const String& path,
        /* [in] */ Int32 p1,
        /* [in] */ Int32 p2,
        /* [in] */ Int32 p3,
        /* [in] */ const String& data,
        /* [in] */ const String& pin2,
        /* [in] */ IMessage* result);

    CARAPI IccIOForApp(
        /* [in] */ Int32 command,
        /* [in] */ Int32 fileid,
        /* [in] */ const String& path,
        /* [in] */ Int32 p1,
        /* [in] */ Int32 p2,
        /* [in] */ Int32 p3,
        /* [in] */ const String& data,
        /* [in] */ const String& pin2,
        /* [in] */ const String& aid,
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
        /* [in] */ const String& operatorNumeric,
        /* [in] */ IMessage* response);

    CARAPI GetNetworkSelectionMode(
        /* [in] */ IMessage* response);

    CARAPI GetAvailableNetworks(
        /* [in] */ IMessage* response);

    CARAPI SetCallForward(
        /* [in] */ Int32 action,
        /* [in] */ Int32 cfReason,
        /* [in] */ Int32 serviceClass,
        /* [in] */ const String& number,
        /* [in] */ Int32 timeSeconds,
        /* [in] */ IMessage* response);

    CARAPI QueryCallForwardStatus(
        /* [in] */ Int32 cfReason,
        /* [in] */ Int32 serviceClass,
        /* [in] */ const String& number,
        /* [in] */ IMessage* response);

    CARAPI QueryCLIP(
        /* [in] */ IMessage* response);

    CARAPI GetBasebandVersion(
        /* [in] */ IMessage* response);

    CARAPI QueryFacilityLock(
        /* [in] */ const String& facility,
        /* [in] */ const String& password,
        /* [in] */ Int32 serviceClass,
        /* [in] */ IMessage* response);

    CARAPI QueryFacilityLockForApp(
        /* [in] */ const String& facility,
        /* [in] */ const String& password,
        /* [in] */ Int32 serviceClass,
        /* [in] */ const String& appId,
        /* [in] */ IMessage* response);

    CARAPI SetFacilityLock(
        /* [in] */ const String& facility,
        /* [in] */ Boolean lockState,
        /* [in] */ const String& password,
        /* [in] */ Int32 serviceClass,
        /* [in] */ IMessage* response);

    CARAPI SetFacilityLockForApp(
        /* [in] */ const String& facility,
        /* [in] */ Boolean lockState,
        /* [in] */ const String& password,
        /* [in] */ Int32 serviceClass,
        /* [in] */ const String& appId,
        /* [in] */ IMessage* response);

    CARAPI SendUSSD(
        /* [in] */ const String& ussdString,
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
        /* [in] */ const String& contents,
        /* [in] */ IMessage* response);

    /**
     * {@inheritDoc}
     */
    CARAPI SendEnvelope(
        /* [in] */ const String& contents,
        /* [in] */ IMessage* response);

    /**
     * {@inheritDoc}
     */
    CARAPI SendEnvelopeWithStatus(
        /* [in] */ const String& contents,
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
        /* [in] */ const String& address,
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
        /* [in] */ const String& feature,
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
        /* [in] */ const String& FeatureCode,
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
        /* [in] */ const String& nonce,
        /* [in] */ IMessage* response);

    CARAPI RequestIccSimAuthentication(
        /* [in] */ Int32 authContext,
        /* [in] */ const String& data,
        /* [in] */ const String& aid,
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
        /* [in] */ const String& apn,
        /* [in] */ const String& protocol,
        /* [in] */ Int32 authType,
        /* [in] */ const String& username,
        /* [in] */ const String& password,
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
        /* [in] */ const String& AID,
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
        /* [in] */ const String& data,
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
        /* [in] */ const String& data,
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
        /* [in] */ const String& itemValue,
        /* [in] */ IMessage* response);

    CARAPI NvWriteCdmaPrl(
        /* [in] */ ArrayOf<Byte>* preferredRoamingList,
        /* [in] */ IMessage* response);

    CARAPI NvResetConfig(
        /* [in] */ Int32 resetType,
        /* [in] */ IMessage* response);

protected:
    CARAPI_(void) SendScreenState(
        /* [in] */ Boolean on);

    CARAPI OnRadioAvailable();

    CARAPI GetRadioStateFromInt(
        /* [in] */ Int32 stateInt,
        /* [out] */ ICommandsInterfaceRadioState* state);

    CARAPI_(void) SwitchToRadioState(
        /* [in] */ ICommandsInterfaceRadioState newState);

    CARAPI_(void) Send(
        /* [in] */ RILRequest* rr);

    CARAPI_(void) ProcessResponse(
        /* [in] */ RILParcel* p);

    /**
     * Release each request in mRequestList then clear the list
     * @param error is the RIL_Errno sent back
     * @param loggable true means to print all requests in mRequestList
     */
    CARAPI_(void) ClearRequestList(
        /* [in] */ Int32 error,
        /* [in] */ Boolean loggable);

    CARAPI_(AutoPtr<RILRequest>) FindAndRemoveRequestFromList(
        /* [in] */ Int32 serial);

    CARAPI_(AutoPtr<RILRequest>) ProcessSolicited(
        /* [in] */ RILParcel* p);

    static CARAPI_(String) RetToString(
        /* [in] */ Int32 req,
        /* [in] */ IInterface* ret);

    CARAPI_(void) ProcessUnsolicited(
        /* [in] */ RILParcel* p);

    /**
     * Notifiy all registrants that the ril has connected or disconnected.
     *
     * @param rilVer is the version of the ril or -1 if disconnected.
     */
    CARAPI_(void) NotifyRegistrantsRilConnectionChanged(
        /* [in] */ Int32 rilVer);

    /** Notify registrants of WWAN coexistence event. */
    CARAPI_(void) NotifyWwanIwlanCoexist(
        /* [in] */ ArrayOf<Byte>* data);

    /** Notify registrants of SIM_REFRESH event. */
    CARAPI_(void) NotifySimRefresh(
        /* [in] */ ArrayOf<Byte>* data);

    /** Notify registrants of MODEM_CAPABILITY event. */
    CARAPI_(void) NotifyModemCap(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ IInteger32* phoneId);

    CARAPI_(AutoPtr<IInterface>) ResponseInts(
        /* [in] */ RILParcel* p);

    CARAPI_(AutoPtr<IInterface>) ResponseVoid(
        /* [in] */ RILParcel* p);

    CARAPI_(AutoPtr<IInterface>) ResponseCallForward(
        /* [in] */ RILParcel* p);

    CARAPI_(AutoPtr<IInterface>) ResponseSuppServiceNotification(
        /* [in] */ RILParcel* p);

    CARAPI_(AutoPtr<IInterface>) ResponseCdmaSms(
        /* [in] */ RILParcel* p);

    CARAPI_(AutoPtr<IInterface>) ResponseString(
        /* [in] */ RILParcel* p);

    CARAPI_(AutoPtr<IInterface>) ResponseStrings(
        /* [in] */ RILParcel* p);

    CARAPI_(AutoPtr<IInterface>) ResponseRaw(
        /* [in] */ RILParcel* p);

    CARAPI_(AutoPtr<IInterface>) ResponseSMS(
        /* [in] */ RILParcel* p);

    CARAPI_(AutoPtr<IInterface>) ResponseICC_IO(
        /* [in] */ RILParcel* p);

    CARAPI_(AutoPtr<IInterface>) ResponseIccCardStatus(
        /* [in] */ RILParcel* p);

    CARAPI_(AutoPtr<IInterface>) ResponseSimRefresh(
        /* [in] */ RILParcel* p);

    CARAPI_(AutoPtr<IInterface>) ResponseCallList(
        /* [in] */ RILParcel* p);

    CARAPI_(AutoPtr<IDataCallResponse>) GetDataCallResponse(
        /* [in] */ RILParcel* p,
        /* [in] */ Int32 version);

    CARAPI_(AutoPtr<IInterface>) ResponseDataCallList(
        /* [in] */ RILParcel* p);

    CARAPI_(AutoPtr<IInterface>) ResponseSetupDataCall(
        /* [in] */ RILParcel* p);

    CARAPI_(AutoPtr<IInterface>) ResponseOperatorInfos(
        /* [in] */ RILParcel* p);

    CARAPI_(AutoPtr<IInterface>) ResponseCellList(
        /* [in] */ RILParcel* p);

    CARAPI_(AutoPtr<IInterface>) ResponseGetPreferredNetworkType(
        /* [in] */ RILParcel* p);

    CARAPI_(AutoPtr<IInterface>) ResponseGmsBroadcastConfig(
        /* [in] */ RILParcel* p);

    CARAPI_(AutoPtr<IInterface>) ResponseCdmaBroadcastConfig(
        /* [in] */ RILParcel* p);

    CARAPI_(AutoPtr<IInterface>) ResponseSignalStrength(
        /* [in] */ RILParcel* p);

    CARAPI_(AutoPtr<IArrayList>) ResponseCdmaInformationRecord(
        /* [in] */ RILParcel* p);

    CARAPI_(AutoPtr<IInterface>) ResponseCdmaCallWaiting(
        /* [in] */ RILParcel* p);

    CARAPI_(AutoPtr<IInterface>) ResponseCallRing(
        /* [in] */ RILParcel* p);

    CARAPI_(void) NotifyRegistrantsCdmaInfoRec(
        /* [in] */ ICdmaInformationRecords* infoRec);

    CARAPI_(AutoPtr<IArrayList>) ResponseCellInfoList(
        /* [in] */ RILParcel* p);

    static CARAPI_(String) RequestToString(
        /* [in] */ Int32 request);

    static CARAPI_(String) ResponseToString(
        /* [in] */ Int32 request);

    CARAPI_(void) RiljLog(
        /* [in] */ const String& msg);

    CARAPI_(void) RiljLogv(
        /* [in] */ const String& msg);

    CARAPI_(void) UnsljLog(
        /* [in] */ Int32 response);

    CARAPI_(void) UnsljLogMore(
        /* [in] */ Int32 response,
        /* [in] */ const String& more);

    CARAPI_(void) UnsljLogRet(
        /* [in] */ Int32 response,
        /* [in] */ IInterface* ret);

    CARAPI_(void) UnsljLogvRet(
        /* [in] */ Int32 response,
        /* [in] */ IInterface* ret);

    CARAPI_(AutoPtr<IInterface>) ResponseSsData(
        /* [in] */ RILParcel* p);

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
    static CARAPI_(Int32) ReadRilMessage(
        /* [in] */ IInputStream* is,
        /* [in] */ ArrayOf<Byte>* buffer);

    CARAPI_(void) ConstructGsmSendSmsRilRequest(
        /* [in] */ RILRequest* rr,
        /* [in] */ const String& smscPDU,
        /* [in] */ const String& pdu);

    CARAPI_(void) ConstructCdmaSendSmsRilRequest(
        /* [in] */ RILRequest* rr,
        /* [in] */ ArrayOf<Byte>* pdu);

    /**
     *  Restructures PDU data so that it is consistent with RIL
     *  data structure.
     *
     *  @param pdu The data to be written to the RUIM card.
     */
    CARAPI_(void) ConstructCdmaWriteSmsRilRequest(
        /* [in] */ RILRequest* rr,
        /* [in] */ ArrayOf<Byte>* pdu);

    /**
     *  Translates EF_SMS status bits to a status value compatible with
     *  SMS AT commands.  See TS 27.005 3.1.
     */
    CARAPI_(Int32) TranslateStatus(
        /* [in] */ Int32 status);

    CARAPI_(void) SendOemRilRequestRaw(
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
    CARAPI_(void) UpdateScreenState();

    /**
     * Holds a PARTIAL_WAKE_LOCK whenever
     * a) There is outstanding RIL request sent to RIL deamon and no replied
     * b) There is a request pending to be sent out.
     *
     * There is a WAKE_LOCK_TIMEOUT to release the lock, though it shouldn't
     * happen often.
     */
    CARAPI_(void) AcquireWakeLock();

    CARAPI_(void) DecrementWakeLock();

    // true if we had the wakelock
    CARAPI_(Boolean) ClearWakeLock();

    CARAPI_(Boolean) IsQcUnsolOemHookResp(
        /* [in] */ IByteBuffer* oemHookResponse);

    CARAPI_(void) ProcessUnsolOemhookResponse(
        /* [in] */ IByteBuffer* oemHookResponse);

    CARAPI_(AutoPtr<IInterface>) ResponseICC_IOBase64(
        /* [in] */ RILParcel* p);

    CARAPI_(AutoPtr<IArrayList>) ResponseGetDataCallProfile(
        /* [in] */ RILParcel* p);

    CARAPI_(AutoPtr<IInterface>) ResponseHardwareConfig(
        /* [in] */ RILParcel* p);

    /*
     * Helper function for the iccTransmitApdu* commands above.
     */
    CARAPI_(void) IccTransmitApduHelper(
        /* [in] */ Int32 rilCommand,
        /* [in] */ Int32 channel,
        /* [in] */ Int32 cla,
        /* [in] */ Int32 instruction,
        /* [in] */ Int32 p1,
        /* [in] */ Int32 p2,
        /* [in] */ Int32 p3,
        /* [in] */ const String& data,
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
    Int32 mWakeLockTimeout;
    // The number of wakelock requests currently active.  Don't release the lock
    // until dec'd to 0
    Int32 mWakeLockCount;

    AutoPtr<ISparseArray> mRequestList;
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
