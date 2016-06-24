
#include "elastos/droid/phone/CPhoneInterfaceManager.h"
#include "elastos/droid/phone/PhoneUtils.h"
#include <elastos/droid/Manifest.h>
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/ServiceManager.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Preference.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/text/TextUtils.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Internal::Telephony::EIID_IITelephony;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::PhoneConstantsState;
using Elastos::Droid::Internal::Telephony::PhoneConstantsState_IDLE;
using Elastos::Droid::Internal::Telephony::PhoneConstantsState_OFFHOOK;
using Elastos::Droid::Internal::Telephony::PhoneConstantsState_RINGING;
using Elastos::Droid::Internal::Telephony::Uicc::CIccIoResult;
using Elastos::Droid::Internal::Telephony::Uicc::CUiccControllerHelper;
using Elastos::Droid::Internal::Telephony::Uicc::IIccIoResult;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccControllerHelper;
using Elastos::Droid::Manifest;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Preference::CPreferenceManagerHelper;
using Elastos::Droid::Preference::IPreferenceManagerHelper;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Telephony::CIccOpenLogicalChannelResponse;
using Elastos::Droid::Telephony::CSubscriptionManager;
using Elastos::Droid::Telephony::CTelephonyManagerHelper;
using Elastos::Droid::Telephony::ICellLocation;
using Elastos::Droid::Telephony::IIccOpenLogicalChannelResponse;
using Elastos::Droid::Telephony::ITelephonyManagerWifiCallingChoices;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CPair;
using Elastos::Droid::Utility::IPair;
using Elastos::Core::AutoLock;
using Elastos::Core::CBoolean;
using Elastos::Core::CInteger32;
using Elastos::Core::CInteger64;
using Elastos::Core::CoreUtils;
using Elastos::Core::CString;
using Elastos::Core::IBoolean;
using Elastos::Core::IByte;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Phone {

const String CPhoneInterfaceManager::TAG("PhoneInterfaceManager");
const Boolean CPhoneInterfaceManager::DBG = (IPhoneGlobals::DBG_LEVEL >= 2);
const Boolean CPhoneInterfaceManager::DBG_LOC = FALSE;
const Int32 CPhoneInterfaceManager::CMD_HANDLE_PIN_MMI = 1;
const Int32 CPhoneInterfaceManager::CMD_HANDLE_NEIGHBORING_CELL = 2;
const Int32 CPhoneInterfaceManager::EVENT_NEIGHBORING_CELL_DONE = 3;
const Int32 CPhoneInterfaceManager::CMD_ANSWER_RINGING_CALL = 4;
const Int32 CPhoneInterfaceManager::CMD_END_CALL = 5;  // not used yet
const Int32 CPhoneInterfaceManager::CMD_TRANSMIT_APDU_LOGICAL_CHANNEL = 7;
const Int32 CPhoneInterfaceManager::EVENT_TRANSMIT_APDU_LOGICAL_CHANNEL_DONE = 8;
const Int32 CPhoneInterfaceManager::CMD_OPEN_CHANNEL = 9;
const Int32 CPhoneInterfaceManager::EVENT_OPEN_CHANNEL_DONE = 10;
const Int32 CPhoneInterfaceManager::CMD_CLOSE_CHANNEL = 11;
const Int32 CPhoneInterfaceManager::EVENT_CLOSE_CHANNEL_DONE = 12;
const Int32 CPhoneInterfaceManager::CMD_NV_READ_ITEM = 13;
const Int32 CPhoneInterfaceManager::EVENT_NV_READ_ITEM_DONE = 14;
const Int32 CPhoneInterfaceManager::CMD_NV_WRITE_ITEM = 15;
const Int32 CPhoneInterfaceManager::EVENT_NV_WRITE_ITEM_DONE = 16;
const Int32 CPhoneInterfaceManager::CMD_NV_WRITE_CDMA_PRL = 17;
const Int32 CPhoneInterfaceManager::EVENT_NV_WRITE_CDMA_PRL_DONE = 18;
const Int32 CPhoneInterfaceManager::CMD_NV_RESET_CONFIG = 19;
const Int32 CPhoneInterfaceManager::EVENT_NV_RESET_CONFIG_DONE = 20;
const Int32 CPhoneInterfaceManager::CMD_GET_PREFERRED_NETWORK_TYPE = 21;
const Int32 CPhoneInterfaceManager::EVENT_GET_PREFERRED_NETWORK_TYPE_DONE = 22;
const Int32 CPhoneInterfaceManager::CMD_SET_PREFERRED_NETWORK_TYPE = 23;
const Int32 CPhoneInterfaceManager::EVENT_SET_PREFERRED_NETWORK_TYPE_DONE = 24;
const Int32 CPhoneInterfaceManager::CMD_SEND_ENVELOPE = 25;
const Int32 CPhoneInterfaceManager::EVENT_SEND_ENVELOPE_DONE = 26;
const Int32 CPhoneInterfaceManager::CMD_INVOKE_OEM_RIL_REQUEST_RAW = 27;
const Int32 CPhoneInterfaceManager::EVENT_INVOKE_OEM_RIL_REQUEST_RAW_DONE = 28;
const Int32 CPhoneInterfaceManager::CMD_TRANSMIT_APDU_BASIC_CHANNEL = 29;
const Int32 CPhoneInterfaceManager::EVENT_TRANSMIT_APDU_BASIC_CHANNEL_DONE = 30;
const Int32 CPhoneInterfaceManager::CMD_EXCHANGE_SIM_IO = 31;
const Int32 CPhoneInterfaceManager::EVENT_EXCHANGE_SIM_IO_DONE = 32;
const Int32 CPhoneInterfaceManager::UnlockSim::SUPPLY_PIN_COMPLETE = 100;

CPhoneInterfaceManager::MainThreadHandler::MainThreadHandler(
    /* [in] */ CPhoneInterfaceManager* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode CPhoneInterfaceManager::MainThreadHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<MainThreadRequest> request;
    AutoPtr<IMessage> onCompleted;
    AsyncResult* ar = NULL;

    AutoPtr<IUiccControllerHelper> helper;
    CUiccControllerHelper::AcquireSingleton((IUiccControllerHelper**)&helper);
    AutoPtr<IUiccController> controller;
    helper->GetInstance((IUiccController**)&controller);
    AutoPtr<IUiccCard> uiccCard;
    controller->GetUiccCard((IUiccCard**)&uiccCard);
    IccAPDUArgument* iccArgument = NULL;

    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case CMD_HANDLE_PIN_MMI: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            request = (MainThreadRequest*)IObject::Probe(obj);
            AutoPtr<ICharSequence> cs = ICharSequence::Probe(request->mArgument);
            String s;
            cs->ToString(&s);
            Boolean tmp = FALSE;
            mHost->mPhone->HandlePinMmi(s, &tmp);
            AutoPtr<IBoolean> r;
            CBoolean::New(tmp, (IBoolean**)&r);
            request->mResult = r;
            // Wake up the requesting thread

            {
                AutoLock syncLock(request);
                request->NotifyAll();
            }
            break;
        }
        case CMD_HANDLE_NEIGHBORING_CELL: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            request = (MainThreadRequest*)IObject::Probe(obj);
            ObtainMessage(EVENT_NEIGHBORING_CELL_DONE,
                    request->Probe(EIID_IInterface), (IMessage**)&onCompleted);
            mHost->mPhone->GetNeighboringCids(onCompleted);
            break;
        }
        case EVENT_NEIGHBORING_CELL_DONE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            request = (MainThreadRequest*) IObject::Probe(ar->mUserObj);
            if (ar->mException == NULL && ar->mResult != NULL) {
                request->mResult = ar->mResult;
            }
            else {
                // create an empty list to notify the waiting thread
                AutoPtr<IList> list;
                CArrayList::New(0, (IList**)&list);
                request->mResult = TO_IINTERFACE(list);
            }
            // Wake up the requesting thread
            {
                AutoLock syncLock(request);
                request->NotifyAll();
            }
            break;
        }
        case CMD_ANSWER_RINGING_CALL:
            mHost->AnswerRingingCallInternal();
            break;

        case CMD_END_CALL: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            request = (MainThreadRequest*)IObject::Probe(obj);
            Boolean hungUp;
            Int32 phoneType;
            mHost->mPhone->GetPhoneType(&phoneType);
            if (phoneType == IPhoneConstants::PHONE_TYPE_CDMA) {
                // CDMA: If the user presses the Power button we treat it as
                // ending the complete call session
                hungUp = PhoneUtils::HangupRingingAndActive(mHost->mPhone);
            }
            else if (phoneType == IPhoneConstants::PHONE_TYPE_GSM) {
                // GSM: End the call as per the Phone state
                hungUp = PhoneUtils::Hangup(mHost->mCM);
            }
            else {
                //throw new IllegalStateException("Unexpected phone type: " + phoneType);
                Logger::E("CPhoneInterfaceManager", "Unexpected phone type: %d", phoneType);
                return E_ILLEGAL_STATE_EXCEPTION;
            }
            if (DBG) {
                StringBuilder sb;
                sb += "CMD_END_CALL: ";
                sb += (hungUp ? "hung up!" : "no call to hang up");
                Log(sb.ToString());
            }
            AutoPtr<IBoolean> r;
            CBoolean::New(hungUp, (IBoolean**)&r);
            request->mResult = r;
            // Wake up the requesting thread
            {
                AutoLock syncLock(request);
                request->NotifyAll();
            }
            break;
        }
        case CMD_TRANSMIT_APDU_LOGICAL_CHANNEL: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            request = (MainThreadRequest*)IObject::Probe(obj);

            iccArgument = (IccAPDUArgument*) IObject::Probe(request->mArgument);
            if (uiccCard == NULL) {
                Loge(String("iccTransmitApduLogicalChannel: No UICC"));
                AutoPtr<IIccIoResult> ir;
                CIccIoResult::New(0x6F, 0, NULL, (IIccIoResult**)&ir);
                request->mResult = ir;
                {
                    AutoLock syncLock(request);
                    request->NotifyAll();
                }
            }
            else {
                ObtainMessage(EVENT_TRANSMIT_APDU_LOGICAL_CHANNEL_DONE,
                    request->Probe(EIID_IInterface), (IMessage**)&onCompleted);
                uiccCard->IccTransmitApduLogicalChannel(
                    iccArgument->mChannel, iccArgument->mCla, iccArgument->mCommand,
                    iccArgument->mP1, iccArgument->mP2, iccArgument->mP3, iccArgument->mData,
                    onCompleted);
            }
            break;
        }
        case EVENT_TRANSMIT_APDU_LOGICAL_CHANNEL_DONE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*)IAsyncResult::Probe(obj);

            request = (MainThreadRequest*) IObject::Probe(ar->mUserObj);
            if (ar->mException == NULL && ar->mResult != NULL) {
                request->mResult = ar->mResult;
            }
            else {
                AutoPtr<IIccIoResult> ir;
                CIccIoResult::New(0x6F, 0, NULL, (IIccIoResult**)&ir);
                request->mResult = ir;
                Logger::D(TAG, "TODO CommandException");
                if (ar->mResult == NULL) {
                    Loge(String("iccTransmitApduLogicalChannel: Empty response"));
                }
                // else if (ar->mException instanceof CommandException) {
                //     Loge("iccTransmitApduLogicalChannel: CommandException: " +
                //             ar.exception);
                // }
                else {
                    Loge(String("iccTransmitApduLogicalChannel: Unknown exception"));
                }
            }
            {
                AutoLock syncLock(request);
                request->NotifyAll();
            }
            break;
        }
        case CMD_TRANSMIT_APDU_BASIC_CHANNEL: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            request = (MainThreadRequest*)IObject::Probe(obj);

            iccArgument = (IccAPDUArgument*) IObject::Probe(request->mArgument);
            if (uiccCard == NULL) {
                Loge(String("iccTransmitApduBasicChannel: No UICC"));
                AutoPtr<IIccIoResult> ir;
                CIccIoResult::New(0x6F, 0, NULL, (IIccIoResult**)&ir);
                request->mResult = ir;
                {
                    AutoLock syncLock(request);
                    request->NotifyAll();
                }
            }
            else {
                ObtainMessage(EVENT_TRANSMIT_APDU_BASIC_CHANNEL_DONE,
                    request->Probe(EIID_IInterface), (IMessage**)&onCompleted);
                uiccCard->IccTransmitApduBasicChannel(
                    iccArgument->mCla, iccArgument->mCommand, iccArgument->mP1, iccArgument->mP2,
                    iccArgument->mP3, iccArgument->mData, onCompleted);
            }
            break;
        }
        case EVENT_TRANSMIT_APDU_BASIC_CHANNEL_DONE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*)IAsyncResult::Probe(obj);

            request = (MainThreadRequest*) IObject::Probe(ar->mUserObj);
            if (ar->mException == NULL && ar->mResult != NULL) {
                request->mResult = ar->mResult;
            }
            else {
                AutoPtr<IIccIoResult> ir;
                CIccIoResult::New(0x6F, 0, NULL, (IIccIoResult**)&ir);
                request->mResult = ir;
                Logger::D(TAG, "TODO CommandException");
                if (ar->mResult == NULL) {
                    Loge(String("iccTransmitApduBasicChannel: Empty response"));
                }
                // else if (ar.exception instanceof CommandException) {
                //     Loge(String("iccTransmitApduBasicChannel: CommandException: ") +
                //             ar.exception);
                // }
                else {
                    Loge(String("iccTransmitApduBasicChannel: Unknown exception"));
                }
            }
            {
                AutoLock syncLock(request);
                request->NotifyAll();
            }
            break;
        }
        case CMD_EXCHANGE_SIM_IO: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            request = (MainThreadRequest*)IObject::Probe(obj);

            iccArgument = (IccAPDUArgument*) IObject::Probe(request->mArgument);
            if (uiccCard == NULL) {
                Loge(String("iccExchangeSimIO: No UICC"));
                AutoPtr<IIccIoResult> ir;
                CIccIoResult::New(0x6F, 0, NULL, (IIccIoResult**)&ir);
                request->mResult = ir;
                {
                    AutoLock syncLock(request);
                    request->NotifyAll();
                }
            }
            else {
                ObtainMessage(EVENT_EXCHANGE_SIM_IO_DONE,
                        request->Probe(EIID_IInterface), (IMessage**)&onCompleted);
                uiccCard->IccExchangeSimIO(iccArgument->mCla, /* fileID */
                        iccArgument->mCommand, iccArgument->mP1, iccArgument->mP2, iccArgument->mP3,
                        iccArgument->mData, onCompleted);
            }
            break;
        }
        case EVENT_EXCHANGE_SIM_IO_DONE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*)IAsyncResult::Probe(obj);

            request = (MainThreadRequest*) IObject::Probe(ar->mUserObj);
            if (ar->mException == NULL && ar->mResult != NULL) {
                request->mResult = ar->mResult;
            }
            else {
                AutoPtr<IIccIoResult> ir;
                CIccIoResult::New(0x6F, 0, NULL, (IIccIoResult**)&ir);
                request->mResult = ir;
            }
            {
                AutoLock syncLock(request);
                request->NotifyAll();
            }
            break;
        }
        case CMD_SEND_ENVELOPE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            request = (MainThreadRequest*)IObject::Probe(obj);

            if (uiccCard == NULL) {
                Loge(String("sendEnvelopeWithStatus: No UICC"));
                AutoPtr<IIccIoResult> ir;
                CIccIoResult::New(0x6F, 0, NULL, (IIccIoResult**)&ir);
                request->mResult = ir;
                {
                    AutoLock syncLock(request);
                    request->NotifyAll();
                }
            }
            else {
                ObtainMessage(EVENT_SEND_ENVELOPE_DONE, request->Probe(EIID_IInterface), (IMessage**)&onCompleted);
                AutoPtr<ICharSequence> cs = ICharSequence::Probe(request->mArgument);
                String s;
                cs->ToString(&s);
                uiccCard->SendEnvelopeWithStatus(s, onCompleted);
            }
            break;
        }
        case EVENT_SEND_ENVELOPE_DONE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*)IAsyncResult::Probe(obj);

            request = (MainThreadRequest*) IObject::Probe(ar->mUserObj);
            if (ar->mException == NULL && ar->mResult != NULL) {
                request->mResult = ar->mResult;
            }
            else {
                AutoPtr<IIccIoResult> ir;
                CIccIoResult::New(0x6F, 0, NULL, (IIccIoResult**)&ir);
                request->mResult = ir;
                Logger::D(TAG, "TODO CommandException");
                if (ar->mResult == NULL) {
                    Loge(String("sendEnvelopeWithStatus: Empty response"));
                }
                // else if (ar.exception instanceof CommandException) {
                //     Loge(String("sendEnvelopeWithStatus: CommandException: ") +
                //             ar.exception);
                // }
                else {
                    Loge(String("sendEnvelopeWithStatus: exception:") + TO_CSTR(ar->mException));
                }
            }
            {
                AutoLock syncLock(request);
                request->NotifyAll();
            }
            break;
        }
        case CMD_OPEN_CHANNEL: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            request = (MainThreadRequest*)IObject::Probe(obj);

            if (uiccCard == NULL) {
                Loge(String("iccOpenLogicalChannel: No UICC"));
                AutoPtr<IIccIoResult> ir;
                CIccIoResult::New(0x6F, 0, NULL, (IIccIoResult**)&ir);
                request->mResult = ir;
                {
                    AutoLock syncLock(request);
                    request->NotifyAll();
                }
            }
            else {
                ObtainMessage(EVENT_OPEN_CHANNEL_DONE, request->Probe(EIID_IInterface), (IMessage**)&onCompleted);
                String s;
                ICharSequence::Probe(request->mArgument)->ToString(&s);
                uiccCard->IccOpenLogicalChannel(s, onCompleted);
            }
            break;
        }
        case EVENT_OPEN_CHANNEL_DONE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*)IAsyncResult::Probe(obj);

            request = (MainThreadRequest*) IObject::Probe(ar->mUserObj);
            AutoPtr<IIccOpenLogicalChannelResponse> openChannelResp;
            if (ar->mException == NULL && ar->mResult != NULL) {
                AutoPtr<ArrayOf<Int32> > result;
                assert(0 && "TODO");
                // result = (int[]) ar.result;
                Int32 channelId = (*result)[0];
                AutoPtr<ArrayOf<Byte> > selectResponse;
                if (result->GetLength() > 1) {
                    selectResponse = ArrayOf<Byte>::Alloc(result->GetLength() - 1);
                    for (Int32 i = 1; i < result->GetLength(); ++i) {
                        (*selectResponse)[i - 1] = (Byte)(*result)[i];
                    }
                }
                CIccOpenLogicalChannelResponse::New(channelId,
                    IIccOpenLogicalChannelResponse::STATUS_NO_ERROR, selectResponse,
                    (IIccOpenLogicalChannelResponse**)&openChannelResp);
            }
            else {
                if (ar->mResult == NULL) {
                    Loge(String("iccOpenLogicalChannel: Empty response"));
                }
                if (ar->mException != NULL) {
                    Loge(String("iccOpenLogicalChannel: Exception: ") + TO_CSTR(ar->mException));
                }

                Int32 errorCode = IIccOpenLogicalChannelResponse::STATUS_UNKNOWN_ERROR;
                assert(0 && "TODO CommandException");
                // if ((ar.exception != NULL) && (ar.exception instanceof CommandException)) {
                //     if (ar.exception.getMessage().compareTo("MISSING_RESOURCE") == 0) {
                //         errorCode = IIccOpenLogicalChannelResponse::STATUS_MISSING_RESOURCE;
                //     } else if (ar.exception.getMessage().compareTo("NO_SUCH_ELEMENT") == 0) {
                //         errorCode = IIccOpenLogicalChannelResponse::STATUS_NO_SUCH_ELEMENT;
                //     }
                // }
                CIccOpenLogicalChannelResponse::New(
                    IIccOpenLogicalChannelResponse::INVALID_CHANNEL, errorCode, NULL,
                    (IIccOpenLogicalChannelResponse**)&openChannelResp);
            }
            request->mResult = openChannelResp;
            {
                AutoLock syncLock(request);
                request->NotifyAll();
            }
            break;
        }
        case CMD_CLOSE_CHANNEL: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            request = (MainThreadRequest*)IObject::Probe(obj);

            if (uiccCard == NULL) {
                Loge(String("iccCloseLogicalChannel: No UICC"));
                AutoPtr<IIccIoResult> ir;
                CIccIoResult::New(0x6F, 0, NULL, (IIccIoResult**)&ir);
                request->mResult = ir;
                {
                    AutoLock syncLock(request);
                    request->NotifyAll();
                }
            }
            else {
                ObtainMessage(EVENT_CLOSE_CHANNEL_DONE, request->Probe(EIID_IInterface), (IMessage**)&onCompleted);
                Int32 v;
                IInteger32::Probe(request->mArgument)->GetValue(&v);
                uiccCard->IccCloseLogicalChannel(v, onCompleted);
            }
            break;
        }
        case EVENT_CLOSE_CHANNEL_DONE:
            HandleNullReturnEvent(msg, String("iccCloseLogicalChannel"));
            break;

        case CMD_NV_READ_ITEM: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            request = (MainThreadRequest*)IObject::Probe(obj);

            ObtainMessage(EVENT_NV_READ_ITEM_DONE, request->Probe(EIID_IInterface), (IMessage**)&onCompleted);\
            Int32 v;
            IInteger32::Probe(request->mArgument)->GetValue(&v);
            mHost->mPhone->NvReadItem(v, onCompleted);
            break;
        }
        case EVENT_NV_READ_ITEM_DONE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*)IAsyncResult::Probe(obj);

            request = (MainThreadRequest*) IObject::Probe(ar->mUserObj);
            if (ar->mException == NULL && ar->mResult != NULL) {
                request->mResult = ar->mResult;     // String
            }
            else {
                AutoPtr<ICharSequence> cs;
                CString::New(String(""), (ICharSequence**)&cs);
                request->mResult = cs;
                Logger::D(TAG, "TODO CommandException");
                if (ar->mResult == NULL) {
                    Loge(String("nvReadItem: Empty response"));
                }
                // else if (ar.exception instanceof CommandException) {
                //     Loge(String("nvReadItem: CommandException: ") +
                //             ar.exception);
                // }
                else {
                    Loge(String("nvReadItem: Unknown exception"));
                }
            }
            {
                AutoLock syncLock(request);
                request->NotifyAll();
            }
            break;
        }
        case CMD_NV_WRITE_ITEM: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            request = (MainThreadRequest*)IObject::Probe(obj);

            ObtainMessage(EVENT_NV_WRITE_ITEM_DONE, request->Probe(EIID_IInterface), (IMessage**)&onCompleted);
            AutoPtr<IPair> idValue = IPair::Probe(request->mArgument);
            AutoPtr<IInterface> o;
            idValue->GetFirst((IInterface**)&o);
            Int32 first = 0;
            IInteger32::Probe(o)->GetValue(&first);

            o = NULL;
            idValue->GetSecond((IInterface**)&o);
            String second;
            ICharSequence::Probe(o)->ToString(&second);
            mHost->mPhone->NvWriteItem(first, second, onCompleted);
            break;
        }
        case EVENT_NV_WRITE_ITEM_DONE:
            HandleNullReturnEvent(msg, String("nvWriteItem"));
            break;

        case CMD_NV_WRITE_CDMA_PRL: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            request = (MainThreadRequest*)IObject::Probe(obj);

            ObtainMessage(EVENT_NV_WRITE_CDMA_PRL_DONE, request->Probe(EIID_IInterface), (IMessage**)&onCompleted);
            assert(0 && "TODO");
            // mHost->mPhone->NvWriteCdmaPrl((byte[]) request.argument, onCompleted);
            break;
        }
        case EVENT_NV_WRITE_CDMA_PRL_DONE:
            HandleNullReturnEvent(msg, String("nvWriteCdmaPrl"));
            break;

        case CMD_NV_RESET_CONFIG: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            request = (MainThreadRequest*)IObject::Probe(obj);

            ObtainMessage(EVENT_NV_RESET_CONFIG_DONE, request->Probe(EIID_IInterface), (IMessage**)&onCompleted);
            Int32 v;
            IInteger32::Probe(request->mArgument)->GetValue(&v);
            mHost->mPhone->NvResetConfig(v, onCompleted);
            break;
        }
        case EVENT_NV_RESET_CONFIG_DONE:
            HandleNullReturnEvent(msg, String("nvResetConfig"));
            break;

        case CMD_GET_PREFERRED_NETWORK_TYPE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            request = (MainThreadRequest*)IObject::Probe(obj);

            ObtainMessage(EVENT_GET_PREFERRED_NETWORK_TYPE_DONE, request->Probe(EIID_IInterface), (IMessage**)&onCompleted);
            mHost->mPhone->GetPreferredNetworkType(onCompleted);
            break;
        }
        case EVENT_GET_PREFERRED_NETWORK_TYPE_DONE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*)IAsyncResult::Probe(obj);

            request = (MainThreadRequest*) IObject::Probe(ar->mUserObj);
            if (ar->mException == NULL && ar->mResult != NULL) {
                request->mResult = ar->mResult;     // Integer
            }
            else {
                AutoPtr<IInteger32> io;
                CInteger32::New(-1, (IInteger32**)&io);
                request->mResult = io;
                Logger::D(TAG, "TODO CommandException");
                if (ar->mResult == NULL) {
                    Loge(String("getPreferredNetworkType: Empty response"));
                }
                // else if (ar.exception instanceof CommandException) {
                //     Loge(String("getPreferredNetworkType: CommandException: ") +
                //             ar.exception);
                // }
                else {
                    Loge(String("getPreferredNetworkType: Unknown exception"));
                }
            }
            {
                AutoLock syncLock(request);
                request->NotifyAll();
            }
            break;
        }
        case CMD_SET_PREFERRED_NETWORK_TYPE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            request = (MainThreadRequest*)IObject::Probe(obj);

            ObtainMessage(EVENT_SET_PREFERRED_NETWORK_TYPE_DONE, request->Probe(EIID_IInterface), (IMessage**)&onCompleted);
            Int32 networkType = 0;
            IInteger32::Probe(request->mArgument)->GetValue(&networkType);
            mHost->mPhone->SetPreferredNetworkType(networkType, onCompleted);
            break;
        }
        case EVENT_SET_PREFERRED_NETWORK_TYPE_DONE:
            HandleNullReturnEvent(msg, String("setPreferredNetworkType"));
            break;

        case CMD_INVOKE_OEM_RIL_REQUEST_RAW: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            request = (MainThreadRequest*)IObject::Probe(obj);

            ObtainMessage(EVENT_INVOKE_OEM_RIL_REQUEST_RAW_DONE, request->Probe(EIID_IInterface), (IMessage**)&onCompleted);
            assert(0 && "TODO");
            // mHost->mPhone->InvokeOemRilRequestRaw((byte[])request.argument, onCompleted);
            break;
        }
        case EVENT_INVOKE_OEM_RIL_REQUEST_RAW_DONE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*)IAsyncResult::Probe(obj);

            request = (MainThreadRequest*)IObject::Probe(ar->mUserObj);
            request->mResult = ar->Probe(EIID_IInterface);
            {
                AutoLock syncLock(request);
                request->NotifyAll();
            }
            break;
        }
        default:
            Logger::W(TAG, "MainThreadHandler: unexpected message code: %d", what);
            break;
    }
    return NOERROR;
}

void CPhoneInterfaceManager::MainThreadHandler::HandleNullReturnEvent(
    /* [in] */ IMessage* msg,
    /* [in] */ const String& command)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AsyncResult* ar = (AsyncResult*)IAsyncResult::Probe(obj);

    AutoPtr<MainThreadRequest> request = (MainThreadRequest*) IObject::Probe(ar->mUserObj);
    if (ar->mException == NULL) {
        AutoPtr<IBoolean> bo;
        CBoolean::New(TRUE, (IBoolean**)&bo);
        request->mResult = bo;
    }
    else {
        AutoPtr<IBoolean> bo;
        CBoolean::New(FALSE, (IBoolean**)&bo);
        request->mResult = bo;
        Logger::D(TAG, "TODO CommandException");
        // if (ar.exception instanceof CommandException) {
        //     StringBuilder sb;
        //     sb += command;
        //     sb += ": CommandException: ";
        //     sb += ar.exception;
        //     Loge(sb.ToString());
        // }
        // else {
        //     StringBuilder sb;
        //     sb += command;
        //     sb += ": Unknown exception";
        //     Loge(sb.ToString());
        // }
    }

    {
        AutoLock syncLock(request);
        request->NotifyAll();
    }
}

CPhoneInterfaceManager::MyHandler::MyHandler(
    /* [in] */ CPhoneInterfaceManager::UnlockSim* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode CPhoneInterfaceManager::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AsyncResult* ar = (AsyncResult*)IAsyncResult::Probe(obj);

    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case UnlockSim::SUPPLY_PIN_COMPLETE:
            Logger::D(TAG, "SUPPLY_PIN_COMPLETE");
            {
                AutoLock syncLock(mHost);
                msg->GetArg1(&(mHost->mRetryCount));
                if (ar->mException != NULL) {
                    Logger::D(TAG, "TODO CommandException");
                    // if (ar.exception instanceof CommandException &&
                    //         ((CommandException)(ar.exception)).getCommandError()
                    //         == CommandException.Error.PASSWORD_INCORRECT) {
                    //     mResult = IPhoneConstants::PIN_PASSWORD_INCORRECT;
                    // }
                    // else {
                    //     mResult = IPhoneConstants::PIN_GENERAL_FAILURE;
                    // }
                }
                else {
                    mHost->mResult = IPhoneConstants::PIN_RESULT_SUCCESS;
                }
                mHost->mDone = TRUE;
                mHost->NotifyAll();
            }
            break;
    }
    return NOERROR;
}

CPhoneInterfaceManager::UnlockSim::UnlockSim(
    /* [in] */ CPhoneInterfaceManager* host,
    /* [in] */ IIccCard* simCard)
    : mHost(host)
    , mSimCard(simCard)
    , mDone(FALSE)
    , mResult(IPhoneConstants::PIN_GENERAL_FAILURE)
    , mRetryCount(-1)
{
}

ECode CPhoneInterfaceManager::UnlockSim::Run()
{
    Looper::Prepare();

    {
        AutoLock syncLock(this);
        mHandler = new MyHandler(this);
        NotifyAll();
    }
    return Looper::Loop();
}

ECode CPhoneInterfaceManager::UnlockSim::MyUnlockSim(
    /* [in] */ const String& puk,
    /* [in] */ const String& pin,
    /* [out, callee] */ ArrayOf<Int32>** array)
{
    VALIDATE_NOT_NULL(array)
    *array = NULL;

    {
        AutoLock syncLock(this);

        while (mHandler == NULL) {
            //try {
            ECode ec = Wait();
            //} catch (InterruptedException e) {
            if (ec == (ECode)E_INTERRUPTED_EXCEPTION) {
                AutoPtr<IThread> t = Thread::GetCurrentThread();
                t->Interrupt();
            }
            //}
        }
        AutoPtr<IMessageHelper> helper;
        CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
        AutoPtr<IMessage> callback;
        helper->Obtain(mHandler, SUPPLY_PIN_COMPLETE, (IMessage**)&callback);

        if (puk == NULL) {
            mSimCard->SupplyPin(pin, callback);
        }
        else {
            mSimCard->SupplyPuk(puk, pin, callback);
        }

        while (!mDone) {
            //try {
            Logger::D(TAG, "wait for done");
            ECode ec = Wait();
            //} catch (InterruptedException e) {
                // Restore the interrupted status
            if (ec == (ECode)E_INTERRUPTED_EXCEPTION) {
                AutoPtr<IThread> t = Thread::GetCurrentThread();
                t->Interrupt();
            }
            //}
        }
        Logger::D(TAG, "done");
        AutoPtr<ArrayOf<Int32> > resultArray = ArrayOf<Int32>::Alloc(2);
        (*resultArray)[0] = mResult;
        (*resultArray)[1] = mRetryCount;
        *array = resultArray;
        REFCOUNT_ADD(*array);
        return NOERROR;
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(CPhoneInterfaceManager, Object, IITelephony, IBinder)
CAR_OBJECT_IMPL(CPhoneInterfaceManager)

AutoPtr<CPhoneInterfaceManager> CPhoneInterfaceManager::sInstance;

const String CPhoneInterfaceManager::PREF_CARRIERS_ALPHATAG_PREFIX("carrier_alphtag_");
const String CPhoneInterfaceManager::PREF_CARRIERS_NUMBER_PREFIX("carrier_number_");
const String CPhoneInterfaceManager::
        PREF_CARRIERS_SIMPLIFIED_NETWORK_SETTINGS_PREFIX("carrier_simplified_network_settings_");
Object CPhoneInterfaceManager::THIS;

ECode CPhoneInterfaceManager::SendRequest(
    /* [in] */ Int32 command,
    /* [in] */ IInterface* argument,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    return SendRequest(command, argument, NULL, result);
}

ECode CPhoneInterfaceManager::SendRequest(
    /* [in] */ Int32 command,
    /* [in] */ IInterface* argument,
    /* [in] */ IInterface* argument2,
    /* [out] */ IInterface** result)
{
    AutoPtr<ILooper> lopper = Looper::GetMyLooper();
    AutoPtr<ILooper> lopper2;
    mMainThreadHandler->GetLooper((ILooper**)&lopper2);
    if (TO_IINTERFACE(lopper) == TO_IINTERFACE(lopper2)) {
        //throw new RuntimeException("This method will deadlock if called from the main thread.");
        Logger::E("CPhoneInterfaceManager", "This method will deadlock if called from the main thread.");
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<MainThreadRequest> request = new MainThreadRequest(argument);
    AutoPtr<IMessage> msg;
    mMainThreadHandler->ObtainMessage(command, request->Probe(EIID_IInterface), (IMessage**)&msg);
    msg->SendToTarget();

    // Wait for the request to complete
    {
        AutoLock syncLock(request);
        while (request->mResult == NULL) {
            //try {
            request->Wait();
            // } catch (InterruptedException e) {
                // Do nothing, go back and wait until the request is complete
            //}
        }
    }
    *result = request->mResult;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

void CPhoneInterfaceManager::SendRequestAsync(
    /* [in] */ Int32 command)
{
    Boolean tmp = FALSE;
    mMainThreadHandler->SendEmptyMessage(command, &tmp);
}

void CPhoneInterfaceManager::SendRequestAsync(
    /* [in] */ Int32 command,
    /* [in] */ IInterface* argument)
{
    AutoPtr<MainThreadRequest> request = new MainThreadRequest(argument);
    AutoPtr<IMessage> msg;
    mMainThreadHandler->ObtainMessage(command, request->Probe(EIID_IInterface), (IMessage**)&msg);
    msg->SendToTarget();
}

AutoPtr<CPhoneInterfaceManager> CPhoneInterfaceManager::Init(
    /* [in] */ PhoneGlobals* app,
    /* [in] */ IPhone* phone)
{
    {
        AutoLock lock(THIS);
        if (sInstance == NULL) {
            CPhoneInterfaceManager::NewByFriend(app, phone, (CPhoneInterfaceManager**)&sInstance);
        }
        else {
            Logger::D(TAG, "init() called multiple times!  sInstance = %s", TO_CSTR(sInstance));
        }
    }
    return sInstance;
}

ECode CPhoneInterfaceManager::constructor(
    /* [in] */ IPhoneGlobals* app,
    /* [in] */ IPhone* phone)
{
    mApp = (PhoneGlobals*)app;
    mPhone = phone;
    AutoPtr<PhoneGlobals> globals;
    PhoneGlobals::GetInstance((PhoneGlobals**)&globals);
    mCM = globals->mCM;

    AutoPtr<IInterface> obj;
    IContext::Probe(app)->GetSystemService(IContext::APP_OPS_SERVICE, (IInterface**)&obj);
    mAppOps = IAppOpsManager::Probe(obj);
    mMainThreadHandler = new MainThreadHandler(this);

    AutoPtr<IContext> context;
    mPhone->GetContext((IContext**)&context);
    AutoPtr<IPreferenceManagerHelper> helper;
    CPreferenceManagerHelper::AcquireSingleton((IPreferenceManagerHelper**)&helper);
    helper->GetDefaultSharedPreferences(context, (ISharedPreferences**)&mCarrierPrivilegeConfigs);
    Publish();
    return NOERROR;
}

void CPhoneInterfaceManager::Publish()
{
    if (DBG) Log(String("publish: ") + TO_CSTR(this));

    ServiceManager::AddService(String("phone"), this->Probe(EIID_IInterface));
}

AutoPtr<IPhone> CPhoneInterfaceManager::GetPhone(
    /* [in] */ Int64 subId)
{
    // FIXME: hack for the moment
    return mPhone;
    // return PhoneUtils.getPhoneForSubscriber(subId);
}

ECode CPhoneInterfaceManager::Dial(
    /* [in] */ const String& number)
{
    Int64 num = GetPreferredVoiceSubscription();
    return DialForSubscriber(num, number);
}

ECode CPhoneInterfaceManager::DialForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& number)
{
    if (DBG) Log(String("dial: ") + number);
    // No permission check needed here: This is just a wrapper around the
    // ACTION_DIAL intent, which is available to any app since it puts up
    // the UI before it does anything.

    String url = CreateTelUrl(number);
    if (url.IsNull()) {
        return NOERROR;
    }

    // PENDING: should we just silently fail if phone is offhook or ringing?
    PhoneConstantsState state;
    mCM->GetState(subId, &state);
    if (state != PhoneConstantsState_OFFHOOK && state != PhoneConstantsState_RINGING) {
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> uri;
        helper->Parse(url, (IUri**)&uri);
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_DIAL, uri, (IIntent**)&intent);
        intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        intent->PutExtra(IPhoneConstants::SUBSCRIPTION_KEY, subId);
        mApp->StartActivity(intent);
    }
    return NOERROR;
}

ECode CPhoneInterfaceManager::Call(
    /* [in] */ const String& callingPackage,
    /* [in] */ const String& number)
{
    Int64 num = GetPreferredVoiceSubscription();
    return CallForSubscriber(num, callingPackage, number);
}

ECode CPhoneInterfaceManager::CallForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& callingPackage,
    /* [in] */ const String& number)
{
    if (DBG) Log(String("call: ") + number);

    // This is just a wrapper around the ACTION_CALL intent, but we still
    // need to do a permission check since we're calling startActivity()
    // from the context of the phone app.
    EnforceCallPermission();

    Int32 id = Binder::GetCallingUid();
    Int32 op;
    mAppOps->NoteOp(IAppOpsManager::OP_CALL_PHONE, id, callingPackage, &op);
    if (op!= IAppOpsManager::MODE_ALLOWED) {
        return NOERROR;
    }

    String url = CreateTelUrl(number);
    if (url == NULL) {
        return NOERROR;
    }

    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> uri;
    helper->Parse(url, (IUri**)&uri);
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_CALL, uri, (IIntent**)&intent);
    intent->PutExtra(IPhoneConstants::SUBSCRIPTION_KEY, subId);
    intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    return mApp->StartActivity(intent);
}

ECode CPhoneInterfaceManager::EndCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return EndCallForSubscriber(GetDefaultSubscription(), result);
}

ECode CPhoneInterfaceManager::EndCallForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceCallPermission();
    AutoPtr<IInterface> obj;
    AutoPtr<IInteger64> iv;
    CInteger64::New(subId, (IInteger64**)&iv);
    SendRequest(CMD_END_CALL, iv, NULL, (IInterface**)&obj);
    AutoPtr<IBoolean> value = IBoolean::Probe(obj);
    return value->GetValue(result);
}

ECode CPhoneInterfaceManager::AnswerRingingCall()
{

    return AnswerRingingCallForSubscriber(GetDefaultSubscription());
}

ECode CPhoneInterfaceManager::AnswerRingingCallForSubscriber(
    /* [in] */ Int64 subId)
{
    if (DBG) Log(String("answerRingingCall..."));
    // TODO: there should eventually be a separate "ANSWER_PHONE" permission,
    // but that can probably wait till the big TelephonyManager API overhaul.
    // For now, protect this call with the MODIFY_PHONE_STATE permission.
    EnforceModifyPermission();
    SendRequestAsync(CMD_ANSWER_RINGING_CALL);
    return NOERROR;
}

void CPhoneInterfaceManager::AnswerRingingCallInternal()
{
    AutoPtr<ICall> call;
    mPhone->GetRingingCall((ICall**)&call);
    Boolean res;
    call->IsIdle(&res);
    Boolean hasRingingCall = !res;
    if (hasRingingCall) {
        AutoPtr<ICall> call2;
        mPhone->GetForegroundCall((ICall**)&call2);
        call2->IsIdle(&res);
        Boolean hasActiveCall = !res;

        AutoPtr<ICall> call3;
        mPhone->GetBackgroundCall((ICall**)&call3);
        call3->IsIdle(&res);
        Boolean hasHoldingCall = !res;
        if (hasActiveCall && hasHoldingCall) {
            // Both lines are in use!
            // TODO: provide a flag to let the caller specify what
            // policy to use if both lines are in use.  (The current
            // behavior is hardwired to "answer incoming, end ongoing",
            // which is how the CALL button is specced to behave.)
            AutoPtr<ICall> call4;
            mCM->GetFirstActiveRingingCall((ICall**)&call4);
            PhoneUtils::AnswerAndEndActive(mCM, call4);
            return;
        }
        else {
            // answerCall() will automatically hold the current active
            // call, if there is one.
            AutoPtr<ICall> call5;
            mCM->GetFirstActiveRingingCall((ICall**)&call5);
            PhoneUtils::AnswerCall(call5);
            return;
        }
    }
    else {
        // No call was ringing.
        return;
    }
}

ECode CPhoneInterfaceManager::SilenceRinger()
{
    Logger::E(TAG, "silenseRinger not supported");
    return NOERROR;
}

ECode CPhoneInterfaceManager::IsOffhook(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return IsOffhookForSubscriber(GetDefaultSubscription(), result);
}

ECode CPhoneInterfaceManager::IsOffhookForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IPhone> phone = GetPhone(subId);
    Int32 state;
    phone->GetState(&state);
    *result = (state == PhoneConstantsState_OFFHOOK);
    return NOERROR;
}

ECode CPhoneInterfaceManager::IsRinging(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return IsRingingForSubscriber(GetDefaultSubscription(), result);
}

ECode CPhoneInterfaceManager::IsRingingForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IPhone> phone = GetPhone(subId);
    Int32 state;
    phone->GetState(&state);
    *result = (state == PhoneConstantsState_RINGING);
    return NOERROR;
}

ECode CPhoneInterfaceManager::IsIdle(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return IsIdleForSubscriber(GetDefaultSubscription(), result);
}

ECode CPhoneInterfaceManager::IsIdleForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IPhone> phone = GetPhone(subId);
    Int32 state;
    phone->GetState(&state);
    *result = (state == PhoneConstantsState_IDLE);
    return NOERROR;
}

ECode CPhoneInterfaceManager::IsSimPinEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceReadPermission();
    AutoPtr<PhoneGlobals> globals;
    PhoneGlobals::GetInstance((PhoneGlobals**)&globals);
    return globals->IsSimPinEnabled(result);
}

ECode CPhoneInterfaceManager::SupplyPin(
    /* [in] */ const String& pin,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return SupplyPinForSubscriber(GetDefaultSubscription(), pin, result);
}

ECode CPhoneInterfaceManager::SupplyPinForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& pin,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Int32> > resultArray;
    SupplyPinReportResultForSubscriber(subId, pin, (ArrayOf<Int32>**)&resultArray);
    *result = ((*resultArray)[0] == IPhoneConstants::PIN_RESULT_SUCCESS) ? TRUE : FALSE;
    return NOERROR;
}

ECode CPhoneInterfaceManager::SupplyPuk(
    /* [in] */ const String& puk,
    /* [in] */ const String& pin,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return SupplyPukForSubscriber(GetDefaultSubscription(), puk, pin, result);
}

ECode CPhoneInterfaceManager::SupplyPukForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& puk,
    /* [in] */ const String& pin,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Int32> > resultArray;
    SupplyPukReportResultForSubscriber(subId, puk, pin, (ArrayOf<Int32>**)&resultArray);
    *result = ((*resultArray)[0] == IPhoneConstants::PIN_RESULT_SUCCESS) ? TRUE : FALSE;
    return NOERROR;
}

ECode CPhoneInterfaceManager::SupplyPinReportResult(
    /* [in] */ const String& pin,
    /* [out, callee] */ ArrayOf<Int32>** array)
{
    VALIDATE_NOT_NULL(array)

    return SupplyPinReportResultForSubscriber(GetDefaultSubscription(), pin, array);
}

ECode CPhoneInterfaceManager::SupplyPinReportResultForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& pin,
    /* [out, callee] */ ArrayOf<Int32>** array)
{
    VALIDATE_NOT_NULL(array)

    EnforceModifyPermission();
    AutoPtr<IPhone> phone = GetPhone(subId);
    AutoPtr<IIccCard> iccCard;
    phone->GetIccCard((IIccCard**)&iccCard);
    AutoPtr<UnlockSim> checkSimPin = new UnlockSim(this, iccCard);
    IThread::Probe(checkSimPin)->Start();
    return checkSimPin->MyUnlockSim(String(NULL), pin, array);
}

ECode CPhoneInterfaceManager::SupplyPukReportResult(
    /* [in] */ const String& puk,
    /* [in] */ const String& pin,
    /* [out, callee] */ ArrayOf<Int32>** array)
{
    VALIDATE_NOT_NULL(array)

    return SupplyPukReportResultForSubscriber(GetDefaultSubscription(), puk, pin, array);
}

ECode CPhoneInterfaceManager::SupplyPukReportResultForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& puk,
    /* [in] */ const String& pin,
    /* [out, callee] */ ArrayOf<Int32>** array)
{
    VALIDATE_NOT_NULL(array)

    EnforceModifyPermission();
    AutoPtr<IPhone> phone = GetPhone(subId);
    AutoPtr<IIccCard> iccCard;
    phone->GetIccCard((IIccCard**)&iccCard);
    AutoPtr<UnlockSim> checkSimPuk = new UnlockSim(this, iccCard);
    IThread::Probe(checkSimPuk)->Start();
    return checkSimPuk->MyUnlockSim(puk, pin, array);
}

ECode CPhoneInterfaceManager::UpdateServiceLocation()
{
    return UpdateServiceLocationForSubscriber(GetDefaultSubscription());
}

ECode CPhoneInterfaceManager::UpdateServiceLocationForSubscriber(
    /* [in] */ Int64 subId)
{
    // No permission check needed here: this call is harmless, and it's
    // needed for the ServiceState.requestStateUpdate() call (which is
    // already intentionally exposed to 3rd parties.)
    AutoPtr<IPhone> phone = GetPhone(subId);
    return phone->UpdateServiceLocation();
}

ECode CPhoneInterfaceManager::IsRadioOn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return IsRadioOnForSubscriber(GetDefaultSubscription(), result);
}

ECode CPhoneInterfaceManager::IsRadioOnForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IPhone> phone = GetPhone(subId);
    AutoPtr<IServiceState> serviceState;
    phone->GetServiceState((IServiceState**)&serviceState);
    Int32 state;
    serviceState->GetState(&state);
    *result = state != IServiceState::STATE_POWER_OFF;
    return NOERROR;
}

ECode CPhoneInterfaceManager::ToggleRadioOnOff()
{
    return ToggleRadioOnOffForSubscriber(GetDefaultSubscription());
}

ECode CPhoneInterfaceManager::ToggleRadioOnOffForSubscriber(
    /* [in] */ Int64 subId)
{
    EnforceModifyPermission();
    AutoPtr<IPhone> phone = GetPhone(subId);
    Boolean tmp = FALSE;
    IsRadioOnForSubscriber(subId, &tmp);
    return phone->SetRadioPower(!tmp);
}

ECode CPhoneInterfaceManager::SetRadio(
    /* [in] */ Boolean turnOn,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return SetRadioForSubscriber(GetDefaultSubscription(), turnOn, result);
}

ECode CPhoneInterfaceManager::SetRadioForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ Boolean turnOn,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceModifyPermission();
    AutoPtr<IPhone> phone = GetPhone(subId);
    AutoPtr<IServiceState> serviceState;
    phone->GetServiceState((IServiceState**)&serviceState);
    Int32 state;
    serviceState->GetState(&state);
    if ((state != IServiceState::STATE_POWER_OFF) != turnOn) {
        ToggleRadioOnOffForSubscriber(subId);
    }
    *result = TRUE;
    return NOERROR;
}

ECode CPhoneInterfaceManager::NeedMobileRadioShutdown(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    /*
     * If any of the Radios are available, it will need to be
     * shutdown. So return true if any Radio is available.
     */
    AutoPtr<ITelephonyManagerHelper> helper;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&helper);
    AutoPtr<ITelephonyManager> manager;
    helper->GetDefault((ITelephonyManager**)&manager);
    Int32 count;
    manager->GetPhoneCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IPhone> phone;
        assert(0 && "TODO: Need PhoneFactory");
        // PhoneFactory::GetPhone(i);
        Boolean res;
        if (phone != NULL && (phone->IsRadioAvailable(&res), res)) {
            *result = TRUE;
            return NOERROR;
        }
    }
    Logv(StringUtils::ToString(count) + String(" Phones are shutdown."));
    *result = FALSE;
    return NOERROR;
}

ECode CPhoneInterfaceManager::ShutdownMobileRadios()
{
    AutoPtr<ITelephonyManagerHelper> helper;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&helper);
    AutoPtr<ITelephonyManager> manager;
    helper->GetDefault((ITelephonyManager**)&manager);
    Int32 count;
    manager->GetPhoneCount(&count);
    for (Int32 i = 0; i < count; i++) {
        Logv(String("Shutting down Phone ") + StringUtils::ToString(i));
        ShutdownRadioUsingPhoneId(i);
    }
    return NOERROR;
}

void CPhoneInterfaceManager::ShutdownRadioUsingPhoneId(
    /* [in] */ Int32 phoneId)
{
    EnforceModifyPermission();
    AutoPtr<IPhone> phone;
    assert(0 && "TODO: Need PhoneFactory");
    // PhoneFactory::GetPhone(phoneId);
    Boolean res;
    if (phone != NULL && (phone->IsRadioAvailable(&res), res)) {
        phone->ShutdownRadio();
    }
}

ECode CPhoneInterfaceManager::SetRadioPower(
    /* [in] */ Boolean turnOn,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return SetRadioPowerForSubscriber(GetDefaultSubscription(), turnOn, result);
}

ECode CPhoneInterfaceManager::SetRadioPowerForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ Boolean turnOn,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceModifyPermission();
    AutoPtr<IPhone> phone = GetPhone(subId);
    phone->SetRadioPower(turnOn);
    *result = TRUE;
    return NOERROR;
}

ECode CPhoneInterfaceManager::EnableDataConnectivity(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceModifyPermission();
    AutoPtr<ISubscriptionManager> helper;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&helper);
    Int64 subId = 0;
    helper->GetDefaultDataSubId(&subId);
    AutoPtr<IPhone> phone = GetPhone(subId);
    phone->SetDataEnabled(TRUE);
    *result = TRUE;
    return NOERROR;
}

ECode CPhoneInterfaceManager::DisableDataConnectivity(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceModifyPermission();
    AutoPtr<ISubscriptionManager> helper;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&helper);
    Int64 subId = 0;
    helper->GetDefaultDataSubId(&subId);
    AutoPtr<IPhone> phone = GetPhone(subId);
    phone->SetDataEnabled(FALSE);
    *result = TRUE;
    return NOERROR;
}

ECode CPhoneInterfaceManager::IsDataConnectivityPossible(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ISubscriptionManager> helper;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&helper);
    Int64 subId = 0;
    helper->GetDefaultDataSubId(&subId);
    AutoPtr<IPhone> phone = GetPhone(subId);
    return phone->IsDataConnectivityPossible(result);
}

ECode CPhoneInterfaceManager::HandlePinMmi(
    /* [in] */ const String& dialString,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return HandlePinMmiForSubscriber(GetDefaultSubscription(), dialString, result);
}

ECode CPhoneInterfaceManager::HandlePinMmiForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& dialString,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceModifyPermission();
    AutoPtr<ICharSequence> cs;
    CString::New(dialString, (ICharSequence**)&cs);
    AutoPtr<IInteger64> iv;
    CInteger64::New(subId, (IInteger64**)&iv);
    AutoPtr<IInterface> obj;
    SendRequest(CMD_HANDLE_PIN_MMI, cs, iv, (IInterface**)&obj);
    AutoPtr<IBoolean> value = IBoolean::Probe(obj);
    return value->GetValue(result);
}

ECode CPhoneInterfaceManager::GetCallState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    return GetCallStateForSubscriber(GetDefaultSubscription(), result);
}

ECode CPhoneInterfaceManager::GetCallStateForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IPhone> phone = GetPhone(subId);
    Int32 state;
    phone->GetState(&state);
    assert(0 && "TODO Need DefaultPhoneNotifier");
    // return DefaultPhoneNotifier::ConvertCallState(state, result);
    return NOERROR;
}

ECode CPhoneInterfaceManager::GetDataState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISubscriptionManager> helper;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&helper);
    Int64 subId = 0;
    helper->GetDefaultDataSubId(&subId);
    AutoPtr<IPhone> phone = GetPhone(subId);
    Int32 state;
    phone->GetDataConnectionState(&state);
    assert(0 && "TODO Need DefaultPhoneNotifier");
    // return DefaultPhoneNotifier::ConvertDataState(state, result);
    return NOERROR;
}

ECode CPhoneInterfaceManager::GetDataActivity(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ISubscriptionManager> helper;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&helper);
    Int64 subId = 0;
    helper->GetDefaultDataSubId(&subId);
    AutoPtr<IPhone> phone = GetPhone(subId);
    Int32 state;
    phone->GetDataActivityState(&state);
    assert(0 && "TODO Need DefaultPhoneNotifier");
    // return DefaultPhoneNotifier::ConvertDataActivityState(state, result);
    return NOERROR;
}

ECode CPhoneInterfaceManager::GetCellLocation(
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle)

    //try {
    ECode ec = mApp->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::ACCESS_FINE_LOCATION, String(NULL));
    //} catch (SecurityException e) {
    if (ec == (ECode)E_SECURITY_EXCEPTION) {
        // If we have ACCESS_FINE_LOCATION permission, skip the check for ACCESS_COARSE_LOCATION
        // A failure should throw the SecurityException from ACCESS_COARSE_LOCATION since this
        // is the weaker precondition
        mApp->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::ACCESS_COARSE_LOCATION, String(NULL));
    }
    //}

    if (CheckIfCallerIsSelfOrForegroundUser()) {
        if (DBG_LOC) Log(String("getCellLocation: is active user"));
        AutoPtr<IBundle> data;
        CBundle::New((IBundle**)&data);
        AutoPtr<ICellLocation> cl;
        mPhone->GetCellLocation((ICellLocation**)&cl);
        cl->FillInNotifierBundle(data);
        *bundle = data;
        REFCOUNT_ADD(*bundle);
        return NOERROR;
    }
    else {
        if (DBG_LOC) Log(String("getCellLocation: suppress non-active user"));
        *bundle = NULL;
        return NOERROR;
    }
    return NOERROR;
}

ECode CPhoneInterfaceManager::EnableLocationUpdates()
{
    return EnableLocationUpdatesForSubscriber(GetDefaultSubscription());
}

ECode CPhoneInterfaceManager::EnableLocationUpdatesForSubscriber(
    /* [in] */ Int64 subId)
{
    mApp->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONTROL_LOCATION_UPDATES, String(NULL));

    AutoPtr<IPhone> phone = GetPhone(subId);
    return phone->EnableLocationUpdates();
}

ECode CPhoneInterfaceManager::DisableLocationUpdates()
{
    return DisableLocationUpdatesForSubscriber(GetDefaultSubscription());
}

ECode CPhoneInterfaceManager::DisableLocationUpdatesForSubscriber(
    /* [in] */ Int64 subId)
{
    mApp->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONTROL_LOCATION_UPDATES, String(NULL));
    AutoPtr<IPhone> phone = GetPhone(subId);
    return phone->DisableLocationUpdates();
}

ECode CPhoneInterfaceManager::GetNeighboringCellInfo(
    /* [in] */ const String& callingPackage,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list)

    //try {
    ECode ec = mApp->EnforceCallingOrSelfPermission(
                Elastos::Droid::Manifest::permission::ACCESS_FINE_LOCATION, String(NULL));
    //} catch (SecurityException e) {
    if (ec == (ECode)E_SECURITY_EXCEPTION) {
        // If we have ACCESS_FINE_LOCATION permission, skip the check
        // for ACCESS_COARSE_LOCATION
        // A failure should throw the SecurityException from
        // ACCESS_COARSE_LOCATION since this is the weaker precondition
        mApp->EnforceCallingOrSelfPermission(
                Elastos::Droid::Manifest::permission::ACCESS_COARSE_LOCATION, String(NULL));
    }
    //}

    Int32 op;
    mAppOps->NoteOp(IAppOpsManager::OP_NEIGHBORING_CELLS, Binder::GetCallingUid(),
            callingPackage, &op);
    if (op != IAppOpsManager::MODE_ALLOWED) {
        *list = NULL;
        return NOERROR;
    }
    if (CheckIfCallerIsSelfOrForegroundUser()) {
        if (DBG_LOC) Log(String("getNeighboringCellInfo: is active user"));

        AutoPtr<IArrayList> cells;

        //try {
        AutoPtr<IInterface> obj;
        ECode ec = SendRequest(CMD_HANDLE_NEIGHBORING_CELL, NULL, NULL, (IInterface**)&obj);
        cells = IArrayList::Probe(obj);
        //} catch (RuntimeException e) {
        if (ec == (ECode)E_RUNTIME_EXCEPTION) {
            Logger::E(TAG, "getNeighboringCellInfo %d", ec);
        }
        //}
        *list = IList::Probe(cells);
        REFCOUNT_ADD(*list)
        return NOERROR;
    }
    else {
        if (DBG_LOC) Log(String("getNeighboringCellInfo: suppress non-active user"));
        *list = NULL;
        return NOERROR;
    }
    return NOERROR;
}

ECode CPhoneInterfaceManager::GetAllCellInfo(
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list)

    //try {
    ECode ec = mApp->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::ACCESS_FINE_LOCATION, String(NULL));
    //} catch (SecurityException e) {
    if (ec == (ECode)E_SECURITY_EXCEPTION) {
        // If we have ACCESS_FINE_LOCATION permission, skip the check for ACCESS_COARSE_LOCATION
        // A failure should throw the SecurityException from ACCESS_COARSE_LOCATION since this
        // is the weaker precondition
        mApp->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::ACCESS_COARSE_LOCATION, String(NULL));
    }
    //}

    if (CheckIfCallerIsSelfOrForegroundUser()) {
        if (DBG_LOC) Log(String("getAllCellInfo: is active user"));
        return mPhone->GetAllCellInfo(list);
    }
    else {
        if (DBG_LOC) Log(String("getAllCellInfo: suppress non-active user"));
        *list = NULL;
        return NOERROR;
    }
    return NOERROR;
}

ECode CPhoneInterfaceManager::SetCellInfoListRate(
    /* [in] */ Int32 rateInMillis)
{
    return mPhone->SetCellInfoListRate(rateInMillis);
}

Boolean CPhoneInterfaceManager::CheckIfCallerIsSelfOrForegroundUser()
{
    Boolean ok;

    Boolean self = Binder::GetCallingUid() == Process::MyUid();
    if (!self) {
        // Get the caller's user id then clear the calling identity
        // which will be restored in the finally clause.
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        Int32 callingUser = 0;
        helper->GetCallingUserId(&callingUser);
        Int64 ident = Binder::ClearCallingIdentity();

        //try {
        // With calling identity cleared the current user is the foreground user.
        Int32 foregroundUser;
        ECode ec;
        AutoPtr<IActivityManagerHelper> amHelper;
        CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);
        FAIL_GOTO(ec = amHelper->GetCurrentUser(&foregroundUser), ERROR)
        ok = (foregroundUser == callingUser);
        if (DBG_LOC) {
            StringBuilder sb;
            sb += "checkIfCallerIsSelfOrForegoundUser: foregroundUser=";
            sb += foregroundUser;
            sb += " callingUser=";
            sb += callingUser;
            sb += " ok=";
            sb += ok;
            Log(sb.ToString());
        }
        //} catch (Exception ex) {
    ERROR:
        if (DBG_LOC) Loge(String("checkIfCallerIsSelfOrForegoundUser: Exception ex="));
        ok = FALSE;
        //} finally {
        Binder::RestoreCallingIdentity(ident);
        //}
    }
    else {
        if (DBG_LOC) Log(String("checkIfCallerIsSelfOrForegoundUser: is self"));
        ok = TRUE;
    }
    if (DBG_LOC) Log(String("checkIfCallerIsSelfOrForegoundUser: ret=") + StringUtils::ToString(ok));
    return ok;
}

ECode CPhoneInterfaceManager::EnforceReadPermission()
{
    return mApp->EnforceCallingOrSelfPermission(
        Elastos::Droid::Manifest::permission::READ_PHONE_STATE, String(NULL));
}

ECode CPhoneInterfaceManager::EnforceModifyPermission()
{
    return mApp->EnforceCallingOrSelfPermission(
        Elastos::Droid::Manifest::permission::MODIFY_PHONE_STATE, String(NULL));
}

ECode CPhoneInterfaceManager::EnforceModifyPermissionOrCarrierPrivilege()
{
    Int32 permission;
    mApp->CheckCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::MODIFY_PHONE_STATE, &permission);
    if (permission == IPackageManager::PERMISSION_GRANTED) {
        return NOERROR;
    }

    Log(String("No modify permission, check carrier privilege next."));
    Int32 v = 0;
    HasCarrierPrivileges(&v);
    if (v != ITelephonyManager::CARRIER_PRIVILEGE_STATUS_HAS_ACCESS) {
        Loge(String("No Carrier Privilege."));
        //throw new SecurityException("No modify permission or carrier privilege.");
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

ECode CPhoneInterfaceManager::EnforceCarrierPrivilege()
{
    Int32 v = 0;
    HasCarrierPrivileges(&v);
    if (v != ITelephonyManager::CARRIER_PRIVILEGE_STATUS_HAS_ACCESS) {
        Loge(String("No Carrier Privilege."));
        //throw new SecurityException("No Carrier Privilege.");
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

ECode CPhoneInterfaceManager::EnforceCallPermission()
{
    return mApp->EnforceCallingOrSelfPermission(
        Elastos::Droid::Manifest::permission::CALL_PHONE, String(NULL));
}

ECode CPhoneInterfaceManager::EnforcePrivilegedPhoneStatePermission()
{
    return mApp->EnforceCallingOrSelfPermission(
        Elastos::Droid::Manifest::permission::READ_PRIVILEGED_PHONE_STATE, String(NULL));
}

String CPhoneInterfaceManager::CreateTelUrl(
    /* [in] */ const String& number)
{
    if (TextUtils::IsEmpty(number)) {
        return String(NULL);
    }

    return String("tel:") + number;
}

void CPhoneInterfaceManager::Log(
    /* [in] */ const String& msg)
{
    Logger::D(TAG, String("[PhoneIntfMgr] ") + msg);
}

void CPhoneInterfaceManager::Logv(
    /* [in] */ const String& msg)
{
    Logger::V(TAG, String("[PhoneIntfMgr] ") + msg);
}

void CPhoneInterfaceManager::Loge(
    /* [in] */ const String& msg)
{
    Logger::E(TAG, String("[PhoneIntfMgr] ") + msg);
}

ECode CPhoneInterfaceManager::GetActivePhoneType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    return GetActivePhoneTypeForSubscriber(GetDefaultSubscription(), result);
}

ECode CPhoneInterfaceManager::GetActivePhoneTypeForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IPhone> phone = GetPhone(subId);
    return phone->GetPhoneType(result);
}

ECode CPhoneInterfaceManager::GetCdmaEriIconIndex(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    return GetCdmaEriIconIndexForSubscriber(GetDefaultSubscription(), result);
}

ECode CPhoneInterfaceManager::GetCdmaEriIconIndexForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IPhone> phone = GetPhone(subId);
    return phone->GetCdmaEriIconIndex(result);
}

ECode CPhoneInterfaceManager::GetCdmaEriIconMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    return GetCdmaEriIconModeForSubscriber(GetDefaultSubscription(), result);
}

ECode CPhoneInterfaceManager::GetCdmaEriIconModeForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IPhone> phone = GetPhone(subId);
    return phone->GetCdmaEriIconMode(result);
}

ECode CPhoneInterfaceManager::GetCdmaEriText(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    return GetCdmaEriTextForSubscriber(GetDefaultSubscription(), str);
}

ECode CPhoneInterfaceManager::GetCdmaEriTextForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    AutoPtr<IPhone> phone = GetPhone(subId);
    return phone->GetCdmaEriText(str);
}

ECode CPhoneInterfaceManager::GetCdmaMdn(
    /* [in] */ Int64 subId,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    EnforceModifyPermissionOrCarrierPrivilege();
    Int32 type;
    mPhone->GetPhoneType(&type);
    if (type == IPhoneConstants::PHONE_TYPE_CDMA) {
        AutoPtr<IPhone> phone = GetPhone(subId);
        return phone->GetLine1Number(str);
    }
    else {
        *str = String(NULL);
        return NOERROR;
    }
    return NOERROR;
}

ECode CPhoneInterfaceManager::GetCdmaMin(
    /* [in] */ Int64 subId,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    EnforceModifyPermissionOrCarrierPrivilege();
    Int32 type;
    mPhone->GetPhoneType(&type);
    if (type == IPhoneConstants::PHONE_TYPE_CDMA) {
        AutoPtr<IPhone> phone = GetPhone(subId);
        return phone->GetCdmaMin(str);
    }
    else {
        *str = String(NULL);
        return NOERROR;
    }
    return NOERROR;
}

ECode CPhoneInterfaceManager::NeedsOtaServiceProvisioning(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return mPhone->NeedsOtaServiceProvisioning(result);
}

ECode CPhoneInterfaceManager::GetVoiceMessageCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    return GetVoiceMessageCountForSubscriber(GetDefaultSubscription(), result);
}

ECode CPhoneInterfaceManager::GetVoiceMessageCountForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IPhone> phone = GetPhone(subId);
    return phone->GetVoiceMessageCount(result);
}

ECode CPhoneInterfaceManager::GetNetworkType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    return GetNetworkTypeForSubscriber(GetDefaultSubscription(), result);
}

ECode CPhoneInterfaceManager::GetNetworkTypeForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IPhone> phone = GetPhone(subId);
    AutoPtr<IServiceState> serviceState;
    phone->GetServiceState((IServiceState**)&serviceState);
    return serviceState->GetDataNetworkType(result);
}

ECode CPhoneInterfaceManager::GetDataNetworkType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    return GetDataNetworkTypeForSubscriber(GetDefaultSubscription(), result);
}

ECode CPhoneInterfaceManager::GetDataNetworkTypeForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IPhone> phone = GetPhone(subId);
    AutoPtr<IServiceState> serviceState;
    phone->GetServiceState((IServiceState**)&serviceState);
    return serviceState->GetDataNetworkType(result);
}

ECode CPhoneInterfaceManager::GetVoiceNetworkType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    return GetVoiceNetworkTypeForSubscriber(GetDefaultSubscription(), result);
}

ECode CPhoneInterfaceManager::GetVoiceNetworkTypeForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IPhone> phone = GetPhone(subId);
    AutoPtr<IServiceState> serviceState;
    phone->GetServiceState((IServiceState**)&serviceState);
    return serviceState->GetVoiceNetworkType(result);
}

ECode CPhoneInterfaceManager::HasIccCard(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // FIXME Make changes to pass defaultSimId of type int
    return HasIccCardUsingSlotId(GetDefaultSubscription(), result);
}

ECode CPhoneInterfaceManager::HasIccCardUsingSlotId(
    /* [in] */ Int64 slotId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IPhone> phone = GetPhone(slotId);
    AutoPtr<IIccCard> iccCard;
    phone->GetIccCard((IIccCard**)&iccCard);
    return iccCard->HasIccCard(result);
}

ECode CPhoneInterfaceManager::GetLteOnCdmaMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    return GetLteOnCdmaModeForSubscriber(GetDefaultSubscription(), result);
}

ECode CPhoneInterfaceManager::GetLteOnCdmaModeForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IPhone> phone = GetPhone(subId);
    return phone->GetLteOnCdmaMode(result);
}

ECode CPhoneInterfaceManager::SetPhone(
    /* [in] */ IPhone* phone)
{
    mPhone = phone;
    return NOERROR;
}

Int64 CPhoneInterfaceManager::GetDefaultSubscription()
{
    AutoPtr<ISubscriptionManager> helper;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&helper);
    Int64 subId = 0;
    helper->GetDefaultSubId(&subId);
    return subId;
}

Int64 CPhoneInterfaceManager::GetPreferredVoiceSubscription()
{
    AutoPtr<ISubscriptionManager> helper;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&helper);
    Int64 subId = 0;
    helper->GetDefaultVoiceSubId(&subId);
    return subId;
}

ECode CPhoneInterfaceManager::GetWhenToMakeWifiCalls(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IContext> context;
    mPhone->GetContext((IContext**)&context);
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);
    AutoPtr<ISettingsSystem> helper;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&helper);
    helper->GetInt32(contentResolver, ISettingsSystem::WHEN_TO_MAKE_WIFI_CALLS,
            GetWhenToMakeWifiCallsDefaultPreference(), result);
    return NOERROR;
}

ECode CPhoneInterfaceManager::SetWhenToMakeWifiCalls(
    /* [in] */ Int32 preference)
{
    if (DBG) Log(String("setWhenToMakeWifiCallsStr, storing setting = ")
            + StringUtils::ToString(preference));

    AutoPtr<IContext> context;
    mPhone->GetContext((IContext**)&context);
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);
    AutoPtr<ISettingsSystem> helper;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&helper);
    Boolean tmp = FALSE;
    return helper->PutInt32(contentResolver, ISettingsSystem::WHEN_TO_MAKE_WIFI_CALLS,
            preference, &tmp);
}

Int32 CPhoneInterfaceManager::GetWhenToMakeWifiCallsDefaultPreference()
{
    // TODO: Use a build property to choose this value.
    return ITelephonyManagerWifiCallingChoices::ALWAYS_USE;
}

ECode CPhoneInterfaceManager::IccOpenLogicalChannel(
    /* [in] */ const String& AID,
    /* [out] */ IIccOpenLogicalChannelResponse** respanse)
{
    VALIDATE_NOT_NULL(respanse)

    EnforceModifyPermissionOrCarrierPrivilege();

    if (DBG) Log(String("iccOpenLogicalChannel: ") + AID);

    AutoPtr<ICharSequence> cs;
    CString::New(AID, (ICharSequence**)&cs);
    AutoPtr<IInterface> obj;
    SendRequest(CMD_OPEN_CHANNEL, cs, (IInterface**)&obj);
    AutoPtr<IIccOpenLogicalChannelResponse> response = IIccOpenLogicalChannelResponse::Probe(obj);
    if (DBG) Log(String("iccOpenLogicalChannel: ") + TO_CSTR(response));
    *respanse = response;
    REFCOUNT_ADD(*respanse);
    return NOERROR;
}

ECode CPhoneInterfaceManager::IccCloseLogicalChannel(
    /* [in] */ Int32 channel,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceModifyPermissionOrCarrierPrivilege();

    if (DBG) Log(String("iccCloseLogicalChannel: ") + StringUtils::ToString(channel));
    if (channel < 0) {
      *result = FALSE;
      return NOERROR;
    }

    AutoPtr<IInteger32> io;
    CInteger32::New(channel, (IInteger32**)&io);
    AutoPtr<IInterface> obj;
    SendRequest(CMD_CLOSE_CHANNEL, io, (IInterface**)&obj);
    AutoPtr<IBoolean> value = IBoolean::Probe(obj);
    Boolean success;
    value->GetValue(&success);
    if (DBG) Log(String("iccCloseLogicalChannel: ") + StringUtils::ToString(success));
    *result = success;
    return NOERROR;
}

ECode CPhoneInterfaceManager::IccTransmitApduLogicalChannel(
    /* [in] */ Int32 channel,
    /* [in] */ Int32 cla,
    /* [in] */ Int32 command,
    /* [in] */ Int32 p1,
    /* [in] */ Int32 p2,
    /* [in] */ Int32 p3,
    /* [in] */ const String& data,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceModifyPermissionOrCarrierPrivilege();

    if (DBG) {
        StringBuilder sb;
        sb += "iccTransmitApduLogicalChannel: chnl=";
        sb += channel;
        sb += " cla=";
        sb += cla;
        sb += " cmd=";
        sb += command;
        sb += " p1=";
        sb += p1;
        sb += " p2=";
        sb += p2;
        sb += " p3=";
        sb += p3;
        sb += " data=";
        sb += data;
        Log(sb.ToString());
    }

    if (channel < 0) {
        *result = String("");
        return NOERROR;
    }

    AutoPtr<IccAPDUArgument> arg = new IccAPDUArgument(channel, cla, command, p1, p2, p3, data);
    AutoPtr<IInterface> obj;
    SendRequest(CMD_TRANSMIT_APDU_LOGICAL_CHANNEL, arg->Probe(EIID_IInterface), (IInterface**)&obj);

    AutoPtr<IIccIoResult> response = IIccIoResult::Probe(obj);
    if (DBG) Log(String("iccTransmitApduLogicalChannel: ") + TO_CSTR(response));

    // Append the returned status code to the end of the response payload.
    String s;
    assert(0 && "TDOO Need IccIoResulthelper");
    // s = StringUtils::ToHexString((response->mSw1 << 8) + response->mSw2 + 0x10000).Substring(1);
    // if (response->mPayload != NULL) {
    //     s = IccUtils::BytesToHexString(response->mPayload) + s;
    // }
    *result = s;
    return NOERROR;
}

ECode CPhoneInterfaceManager::IccTransmitApduBasicChannel(
    /* [in] */ Int32 cla,
    /* [in] */ Int32 command,
    /* [in] */ Int32 p1,
    /* [in] */ Int32 p2,
    /* [in] */ Int32 p3,
    /* [in] */ const String& data,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceModifyPermissionOrCarrierPrivilege();

    if (DBG) {
        StringBuilder sb;
        sb += "iccTransmitApduBasicChannel: cla=";
        sb += cla;
        sb += " cmd=";
        sb += command;
        sb += " p1=";
        sb += p1;
        sb += " p2=";
        sb += p2;
        sb += " p3=";
        sb += p3;
        sb += " data=";
        sb += data;
        Log(sb.ToString());
    }

    AutoPtr<IccAPDUArgument> arg = new IccAPDUArgument(0, cla, command, p1, p2, p3, data);
    AutoPtr<IInterface> obj;
    SendRequest(CMD_TRANSMIT_APDU_BASIC_CHANNEL, arg->Probe(EIID_IInterface), (IInterface**)&obj);

    AutoPtr<IIccIoResult> response = IIccIoResult::Probe(obj);
    if (DBG) Log(String("iccTransmitApduBasicChannel: ") + TO_CSTR(response));

    // Append the returned status code to the end of the response payload.
    String s;
    assert(0 && "TDOO Need IccIoResulthelper");
    // s = StringUtils::ToHexString((response->mSw1 << 8) + response->mSw2 + 0x10000).Substring(1);
    // if (response->mPayload != NULL) {
    //     s = IccUtils::BytesToHexString(response->mPayload) + s;
    // }
    *result = s;
    return NOERROR;
}

ECode CPhoneInterfaceManager::IccExchangeSimIO(
    /* [in] */ Int32 fileID,
    /* [in] */ Int32 command,
    /* [in] */ Int32 p1,
    /* [in] */ Int32 p2,
    /* [in] */ Int32 p3,
    /* [in] */ const String& filePath,
    /* [out] */ ArrayOf<Byte>** array)
{
    VALIDATE_NOT_NULL(array)

    EnforceModifyPermissionOrCarrierPrivilege();

    if (DBG) {
        StringBuilder sb;
        sb += "Exchange SIM_IO ";
        sb += fileID;
        sb += ":";
        sb += command;
        sb += " ";
        sb += p1;
        sb += " ";
        sb += p2;
        sb += " ";
        sb += p3;
        sb += ":";
        sb += filePath;
        Log(sb.ToString());
    }

    AutoPtr<IccAPDUArgument> arg = new IccAPDUArgument(-1, fileID, command, p1, p2, p3, filePath);
    AutoPtr<IInterface> obj;
    SendRequest(CMD_EXCHANGE_SIM_IO, arg->Probe(EIID_IInterface), (IInterface**)&obj);

    AutoPtr<IIccIoResult> response = IIccIoResult::Probe(obj);

    if (DBG) {
      Log(String("Exchange SIM_IO [R]") + TO_CSTR(response));
    }

    AutoPtr<ArrayOf<Byte> > result;
    assert(0 && "TDOO Need IccIoResulthelper");
    // Int32 length = 2;
    // // if (response.payload != NULL) {
    // //     length = 2 + response.payload.length;
    // //     result = ArrayOf<Byte>::Alloc(length);
    // //     System.arraycopy(response.payload, 0, result, 0, response.payload.length);
    // // }
    // // else {
    // //     result = ArrayOf<Byte>::Alloc(length);
    // // }

    // (*result)[length - 1] = (Byte)response.sw2;
    // (*result)[length - 2] = (Byte)response.sw1;
    *array = result;
    REFCOUNT_ADD(*array);
    return NOERROR;
}

ECode CPhoneInterfaceManager::SendEnvelopeWithStatus(
    /* [in] */ const String& content,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceModifyPermissionOrCarrierPrivilege();

    AutoPtr<ICharSequence> cs;
    CString::New(content, (ICharSequence**)&cs);
    AutoPtr<IInterface> obj;
    SendRequest(CMD_SEND_ENVELOPE, cs, (IInterface**)&obj);
    AutoPtr<IIccIoResult> response = IIccIoResult::Probe(obj);
    assert(0 && "TDOO Need IccIoResulthelper");
    // if (response.payload == NULL) {
    //   *result = String("");
    //   return NOERROR;
    // }

    // Append the returned status code to the end of the response payload.
    String s;
    assert(0 && "TDOO Need IccIoResulthelper");
    // s = StringUtils::ToHexString((response->mSw1 << 8) + response->mSw2 + 0x10000).Substring(1);
    // s = IccUtils::BytesToHexString(response->mPayload) + s;
    *result = s;
    return NOERROR;
}

ECode CPhoneInterfaceManager::NvReadItem(
    /* [in] */ Int32 itemID,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceModifyPermissionOrCarrierPrivilege();
    if (DBG) Log(String("nvReadItem: item ") + StringUtils::ToString(itemID));

    AutoPtr<IInteger32> io;
    CInteger32::New(itemID, (IInteger32**)&io);
    AutoPtr<IInterface> obj;
    SendRequest(CMD_NV_READ_ITEM, io, (IInterface**)&obj);
    AutoPtr<ICharSequence> cchar = ICharSequence::Probe(obj);
    String value;
    cchar->ToString(&value);
    if (DBG) {
        StringBuilder sb;
        sb += "nvReadItem: item ";
        sb += itemID;
        sb += " is \"";
        sb += value;
        sb += '"';
        Log(sb.ToString());
    }
    *result = value;
    return NOERROR;
}

ECode CPhoneInterfaceManager::NvWriteItem(
    /* [in] */ Int32 itemID,
    /* [in] */ const String& itemValue,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceModifyPermissionOrCarrierPrivilege();
    if (DBG) {
        StringBuilder sb;
        sb += "nvWriteItem: item ";
        sb += itemID;
        sb += " value \"";
        sb += itemValue;
        sb += '"';
        Log(sb.ToString());
    }

    AutoPtr<IInteger32> first;
    CInteger32::New(itemID, (IInteger32**)&first);
    AutoPtr<ICharSequence> second;
    CString::New(itemValue, (ICharSequence**)&second);

    AutoPtr<IPair> p;
    CPair::New(first, second, (IPair**)&p);
    AutoPtr<IInterface> obj;
    SendRequest(CMD_NV_WRITE_ITEM, p, (IInterface**)&obj);
    AutoPtr<IBoolean> value = IBoolean::Probe(obj);
    Boolean success;
    value->GetValue(&success);
    if (DBG) {
        StringBuilder sb;
        sb += "nvWriteItem: item ";
        sb += itemID;
        sb += ' ';
        sb += success ? "ok" : "fail";
        Log(sb.ToString());
    }
    *result = success;
    return NOERROR;
}

ECode CPhoneInterfaceManager::NvWriteCdmaPrl(
    /* [in] */ ArrayOf<Byte>* preferredRoamingList,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceModifyPermissionOrCarrierPrivilege();
    if (DBG) Log(String("nvWriteCdmaPrl: value: ") + StringUtils::ToHexString(*preferredRoamingList));

    AutoPtr<IArrayOf> array = CoreUtils::ConvertByteArray(preferredRoamingList);
    AutoPtr<IInterface> obj;
    SendRequest(CMD_NV_WRITE_CDMA_PRL, array, (IInterface**)&obj);
    AutoPtr<IBoolean> value = IBoolean::Probe(obj);
    Boolean success;
    value->GetValue(&success);

    if (DBG) Log(String("nvWriteCdmaPrl: ") + (success ? String("ok") : String("fail")));
    *result = success;
    return NOERROR;
}

ECode CPhoneInterfaceManager::NvResetConfig(
    /* [in] */ Int32 resetType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceModifyPermissionOrCarrierPrivilege();
    if (DBG) Log(String("nvResetConfig: type ") + StringUtils::ToString(resetType));

    AutoPtr<IInteger32> io;
    CInteger32::New(resetType, (IInteger32**)&io);
    AutoPtr<IInterface> obj;
    SendRequest(CMD_NV_RESET_CONFIG, io, (IInterface**)&obj);
    AutoPtr<IBoolean> value = IBoolean::Probe(obj);
    Boolean success;
    value->GetValue(&success);

    if (DBG) {
        StringBuilder sb;
        sb += "nvResetConfig: type ";
        sb += resetType;
        sb += ' ';
        sb += success ? "ok" : "fail";
        Log(sb.ToString());
    }
    return success;
}

ECode CPhoneInterfaceManager::GetDefaultSim(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    //TODO Need to get it from Telephony Devcontroller
    *result = 0;
    return NOERROR;
}

ECode CPhoneInterfaceManager::GetPcscfAddress(
    /* [in] */ const String& apnType,
    /* [out] */ ArrayOf<String>** array)
{
    VALIDATE_NOT_NULL(array)

    EnforceReadPermission();
    return mPhone->GetPcscfAddress(apnType, array);
}

ECode CPhoneInterfaceManager::SetImsRegistrationState(
    /* [in] */ Boolean registered)
{
    EnforceModifyPermission();
    return mPhone->SetImsRegistrationState(registered);
}

ECode CPhoneInterfaceManager::GetCalculatedPreferredNetworkType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceReadPermission();
    AutoPtr<IContext> context;
    mPhone->GetContext((IContext**)&context);
    assert(0 && "TODO: Need PhoneFactory");
    // return PhoneFactory::CalculatePreferredNetworkType(context, result);
    return NOERROR;
}

ECode CPhoneInterfaceManager::GetPreferredNetworkType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceModifyPermissionOrCarrierPrivilege();
    if (DBG) Log(String("getPreferredNetworkType"));

    AutoPtr<IInterface> obj;
    SendRequest(CMD_GET_PREFERRED_NETWORK_TYPE, NULL, (IInterface**)&obj);
    AutoPtr<IArrayOf> array = IArrayOf::Probe(obj);

    Int32 networkType;
    if (array != NULL) {
        AutoPtr<IInterface> tmp;
        array->Get(0, (IInterface**)&tmp);
        AutoPtr<IInteger32> value = IInteger32::Probe(tmp);
        value->GetValue(&networkType);
    }
    else {
        networkType = -1;
    };
    if (DBG) Log(String("getPreferredNetworkType: ") + StringUtils::ToString(networkType));
    *result = networkType;
    return NOERROR;
}

ECode CPhoneInterfaceManager::SetPreferredNetworkType(
    /* [in] */ Int32 networkType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceModifyPermissionOrCarrierPrivilege();
    if (DBG) Log(String("setPreferredNetworkType: type ") + StringUtils::ToString(networkType));

    AutoPtr<IInteger32> io;
    CInteger32::New(networkType, (IInteger32**)&io);
    AutoPtr<IInterface> obj;
    SendRequest(CMD_SET_PREFERRED_NETWORK_TYPE, io, (IInterface**)&obj);
    AutoPtr<IBoolean> value = IBoolean::Probe(obj);
    Boolean success;
    value->GetValue(&success);

    if (DBG) Log(String("setPreferredNetworkType: ") + (success ? String("ok") : String("fail")));
    if (success) {
        AutoPtr<IContext> context;
        mPhone->GetContext((IContext**)&context);
        AutoPtr<IContentResolver> cr;
        context->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<ISettingsGlobal> helper;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
        Boolean tmp = FALSE;
        helper->PutInt32(cr, ISettingsGlobal::PREFERRED_NETWORK_MODE, networkType, &tmp);
    }
    *result = success;
    return NOERROR;
}

ECode CPhoneInterfaceManager::SetDataEnabled(
    /* [in] */ Boolean enable)
{
    EnforceModifyPermission();
    return mPhone->SetDataEnabled(enable);
}

ECode CPhoneInterfaceManager::GetDataEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    //try {
    ECode ec = mApp->EnforceCallingOrSelfPermission(
        Elastos::Droid::Manifest::permission::ACCESS_NETWORK_STATE, String(NULL));
    //} catch (Exception e) {
    if (FAILED(ec)) {
        mApp->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::MODIFY_PHONE_STATE, String(NULL));
    }
    //}
    return mPhone->GetDataEnabled(result);
}

ECode CPhoneInterfaceManager::HasCarrierPrivileges(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IUiccControllerHelper> helper;
    CUiccControllerHelper::AcquireSingleton((IUiccControllerHelper**)&helper);
    AutoPtr<IUiccController> controller;
    helper->GetInstance((IUiccController**)&controller);

    AutoPtr<IUiccCard> card;
    controller->GetUiccCard((IUiccCard**)&card);
    if (card == NULL) {
        Loge(String("hasCarrierPrivileges: No UICC"));
        *result = ITelephonyManager::CARRIER_PRIVILEGE_STATUS_RULES_NOT_LOADED;
        return NOERROR;
    }

    AutoPtr<IContext> context;
    mPhone->GetContext((IContext**)&context);
    AutoPtr<IPackageManager> manager;
    context->GetPackageManager((IPackageManager**)&manager);
    return card->GetCarrierPrivilegeStatusForCurrentTransaction(manager, result);
}

ECode CPhoneInterfaceManager::CheckCarrierPrivilegesForPackage(
    /* [in] */ const String& pkgname,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IUiccControllerHelper> helper;
    CUiccControllerHelper::AcquireSingleton((IUiccControllerHelper**)&helper);
    AutoPtr<IUiccController> controller;
    helper->GetInstance((IUiccController**)&controller);
    AutoPtr<IUiccCard> card;
    controller->GetUiccCard((IUiccCard**)&card);

    if (card == NULL) {
        Loge(String("checkCarrierPrivilegesForPackage: No UICC"));
        *result = ITelephonyManager::CARRIER_PRIVILEGE_STATUS_RULES_NOT_LOADED;
        return NOERROR;
    }

    AutoPtr<IContext> context;
    mPhone->GetContext((IContext**)&context);
    AutoPtr<IPackageManager> manager;
    context->GetPackageManager((IPackageManager**)&manager);
    return card->GetCarrierPrivilegeStatus(manager, pkgname, result);
}

ECode CPhoneInterfaceManager::GetCarrierPackageNamesForIntent(
    /* [in] */ IIntent* intent,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IUiccControllerHelper> helper;
    CUiccControllerHelper::AcquireSingleton((IUiccControllerHelper**)&helper);
    AutoPtr<IUiccController> controller;
    helper->GetInstance((IUiccController**)&controller);
    AutoPtr<IUiccCard> card;
    controller->GetUiccCard((IUiccCard**)&card);

    if (card == NULL) {
        Loge(String("getCarrierPackageNamesForIntent: No UICC"));
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<IContext> context;
    mPhone->GetContext((IContext**)&context);
    AutoPtr<IPackageManager> manager;
    context->GetPackageManager((IPackageManager**)&manager);
    return card->GetCarrierPackageNamesForIntent(manager, intent, result);
}

String CPhoneInterfaceManager::GetIccId(
    /* [in] */ Int64 subId)
{
    AutoPtr<IPhone> phone = GetPhone(subId);
    AutoPtr<IUiccCard> card;
    phone->GetUiccCard((IUiccCard**)&card);
    if (card == NULL) {
        Loge(String("getIccId: No UICC"));
        return String(NULL);
    }
    String iccId;
    card->GetIccId(&iccId);
    if (TextUtils::IsEmpty(iccId)) {
        Loge(String("getIccId: ICC ID is NULL or empty."));
        return String(NULL);
    }
    return iccId;
}

ECode CPhoneInterfaceManager::EnableSimplifiedNetworkSettingsForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ Boolean enable)
{
    EnforceModifyPermissionOrCarrierPrivilege();

    String iccId = GetIccId(subId);
    if (!iccId.IsNull()) {
        String snsPrefKey = PREF_CARRIERS_SIMPLIFIED_NETWORK_SETTINGS_PREFIX + iccId;
        AutoPtr<ISharedPreferencesEditor> editor;
        mCarrierPrivilegeConfigs->Edit((ISharedPreferencesEditor**)&editor);
        if (enable) {
            editor->PutBoolean(snsPrefKey, TRUE);
        }
        else {
            editor->Remove(snsPrefKey);
        }
        Boolean tmp = FALSE;
        editor->Commit(&tmp);
    }
    return NOERROR;
}

ECode CPhoneInterfaceManager::GetSimplifiedNetworkSettingsEnabledForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceReadPermission();
    String iccId = GetIccId(subId);
    if (!iccId.IsNull()) {
        String snsPrefKey = PREF_CARRIERS_SIMPLIFIED_NETWORK_SETTINGS_PREFIX + iccId;
        return mCarrierPrivilegeConfigs->GetBoolean(snsPrefKey, FALSE, result);
    }
    *result = FALSE;
    return NOERROR;
}

ECode CPhoneInterfaceManager::SetLine1NumberForDisplayForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& number)
{
    EnforceModifyPermissionOrCarrierPrivilege();

    String iccId = GetIccId(subId);
    if (!iccId.IsNull()) {
        String alphaTagPrefKey = PREF_CARRIERS_ALPHATAG_PREFIX + iccId;
        AutoPtr<ISharedPreferencesEditor> editor;
        mCarrierPrivilegeConfigs->Edit((ISharedPreferencesEditor**)&editor);
        if (alphaTag.IsNull()) {
            editor->Remove(alphaTagPrefKey);
        }
        else {
            editor->PutString(alphaTagPrefKey, alphaTag);
        }

        String numberPrefKey = PREF_CARRIERS_NUMBER_PREFIX + iccId;
        if (number.IsNull()) {
            editor->Remove(numberPrefKey);
        }
        else {
            editor->PutString(numberPrefKey, number);
        }
        Boolean tmp = FALSE;
        editor->Commit(&tmp);
    }
    return NOERROR;
}

ECode CPhoneInterfaceManager::GetLine1NumberForDisplay(
    /* [in] */ Int64 subId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceReadPermission();

    String iccId = GetIccId(subId);
    if (!iccId.IsNull()) {
        String numberPrefKey = PREF_CARRIERS_NUMBER_PREFIX + iccId;
        return mCarrierPrivilegeConfigs->GetString(numberPrefKey, String(NULL), result);
    }
    *result = String(NULL);
    return NOERROR;
}

ECode CPhoneInterfaceManager::GetLine1AlphaTagForDisplay(
    /* [in] */ Int64 subId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceReadPermission();

    String iccId = GetIccId(subId);
    if (iccId != NULL) {
        String alphaTagPrefKey = PREF_CARRIERS_ALPHATAG_PREFIX + iccId;
        return mCarrierPrivilegeConfigs->GetString(alphaTagPrefKey, String(NULL), result);
    }
    *result = String(NULL);
    return NOERROR;
}

ECode CPhoneInterfaceManager::SetOperatorBrandOverride(
    /* [in] */ const String& brand,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceModifyPermissionOrCarrierPrivilege();
    return mPhone->SetOperatorBrandOverride(brand, result);
}

ECode CPhoneInterfaceManager::InvokeOemRilRequestRaw(
    /* [in] */ ArrayOf<Byte>* oemReq,
    /* [out, callee] */ ArrayOf<Byte>** oemResp,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceModifyPermission();

    Int32 returnValue = 0;
    ECode ec = NOERROR;
    //try
    do {
        AutoPtr<IArrayOf> array = CoreUtils::ConvertByteArray(oemReq);
        AutoPtr<IInterface> obj;
        if (FAILED(ec = SendRequest(CMD_INVOKE_OEM_RIL_REQUEST_RAW, array, (IInterface**)&obj))) {
            break;
        }
        AsyncResult* result = (AsyncResult*)IAsyncResult::Probe(obj);
        if(result->mException == NULL) {
            if (result->mResult != NULL) {
                AutoPtr<IArrayOf> data = IArrayOf::Probe(result->mResult);
                assert(data != NULL);
                Int32 len = 0;
                data->GetLength(&len);
                AutoPtr<ArrayOf<Byte> > responseData = ArrayOf<Byte>::Alloc(len);
                for (Int32 i = 0; i < len; i++) {
                    AutoPtr<IInterface> o;
                    data->Get(i, (IInterface**)&o);
                    Byte b;
                    IByte::Probe(o)->GetValue(&b);
                    (*responseData)[i] = b;
                }
                if(responseData->GetLength() > (*oemResp)->GetLength()) {
                    StringBuilder sb;
                    sb += "Buffer to copy response too small: Response length is ";
                    sb += responseData->GetLength();
                    sb += "bytes. Buffer Size is ";
                    sb += (*oemResp)->GetLength();
                    sb += "bytes.";
                    Logger::W(TAG, sb.ToString());
                }
                (*oemResp)->Copy(0, responseData, 0, len);
                // System.arraycopy(responseData, 0, oemResp, 0, responseData.length);
                returnValue = responseData->GetLength();
            }
        }
        else {
            assert(0 && "TODO need CommandException");
            // CommandException ex = (CommandException) result.exception;
            // returnValue = ex.getCommandError().ordinal();
            if(returnValue > 0) returnValue *= -1;
        }
    } while (0);
    //catch (RuntimeException e) {
    if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        Logger::W(TAG, "sendOemRilRequestRaw: Runtime Exception");
        assert(0 && "TODO need CommandException");
        // returnValue = (CommandException.Error.GENERIC_FAILURE.ordinal());
        if(returnValue > 0) returnValue *= -1;
    }

    *result = returnValue;
    return NOERROR;
}

ECode CPhoneInterfaceManager::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("Elastos.Droid.Phone.CPhoneInterfaceManager");
    return NOERROR;
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos