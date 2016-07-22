
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/internal/telephony/cat/CatService.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/internal/telephony/cat/RilMessage.h"
#include "elastos/droid/internal/telephony/cat/CatLog.h"
#include "elastos/droid/internal/telephony/cat/CCatCmdMessage.h"
#include "elastos/droid/internal/telephony/cat/TextMessage.h"
#include "elastos/droid/internal/telephony/cat/Input.h"
#include "elastos/droid/internal/telephony/cat/Duration.h"
#include "elastos/droid/internal/telephony/uicc/CUiccControllerHelper.h"
#include "elastos/droid/internal/telephony/uicc/CIccUtils.h"
#include "elastos/droid/internal/telephony/uicc/IccRefreshResponse.h"
#include "elastos/droid/os/HandlerThread.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/CSystemProperties.h"
#include "elastos/droid/R.h"

#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::Cat::CALL_CONTROL_NOT_ALLOWED;
using Elastos::Droid::Internal::Telephony::Cat::CCatCmdMessage;
using Elastos::Droid::Internal::Telephony::Cat::TextMessage;
using Elastos::Droid::Internal::Telephony::Cat::Input;
using Elastos::Droid::Internal::Telephony::Cat::Duration;
using Elastos::Droid::Internal::Telephony::Uicc::CardState;
using Elastos::Droid::Internal::Telephony::Uicc::CARDSTATE_ABSENT;
using Elastos::Droid::Internal::Telephony::Uicc::CARDSTATE_PRESENT;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;
using Elastos::Droid::Internal::Telephony::Uicc::CIccUtils;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccControllerHelper;
using Elastos::Droid::Internal::Telephony::Uicc::CUiccControllerHelper;
using Elastos::Droid::Internal::Telephony::Uicc::IccRefreshResponse;
using Elastos::Droid::Internal::Telephony::IIccUtils;
using Elastos::Droid::Os::HandlerThread;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::R;

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::Core::IThread;
using Elastos::IO::IOutputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

//=====================================================================
//                              RilMessage
//=====================================================================
RilMessage::RilMessage(
    /* [in] */ Int32 msgId,
    /* [in] */ const String& rawData)
{
    mId = msgId;
    AutoPtr<ICharSequence> pData;
    CString::New(rawData, (ICharSequence**)&pData);
    mData = pData;
}

RilMessage::RilMessage(
    /* [in] */ RilMessage* other)
{
    mId = other->mId;
    mData = other->mData;
    mResCode = other->mResCode;
}

//=====================================================================
//                              CatService
//=====================================================================
CAR_INTERFACE_IMPL_2(CatService, Handler, IAppInterface, ICatService);

const Int32 CatService::MSG_ID_SESSION_END;
const Int32 CatService::MSG_ID_PROACTIVE_COMMAND;
const Int32 CatService::MSG_ID_EVENT_NOTIFY;
const Int32 CatService::MSG_ID_CALL_SETUP;
const Int32 CatService::MSG_ID_REFRESH;
const Int32 CatService::MSG_ID_RESPONSE;
const Int32 CatService::MSG_ID_ICC_CHANGED;
const Int32 CatService::MSG_ID_ALPHA_NOTIFY;
const Int32 CatService::MSG_ID_RIL_MSG_DECODED;
const Int32 CatService::MSG_ID_TIMEOUT;
const Int32 CatService::MSG_ID_SEND_SMS_RESULT;
const String CatService::STK_DEFAULT("Default Message");
const Int32 CatService::WAITING_SMS_RESULT;
const Int32 CatService::WAITING_SMS_RESULT_TIME;
const Int32 CatService::SMS_SEND_OK;
const Int32 CatService::SMS_SEND_FAIL;
const Int32 CatService::SMS_SEND_RETRY;
const Boolean CatService::DBG = FALSE;
const Int32 CatService::MSG_ID_ICC_REFRESH;
const Int32 CatService::DEV_ID_KEYPAD;
const Int32 CatService::DEV_ID_DISPLAY;
const Int32 CatService::DEV_ID_UICC;
const Int32 CatService::DEV_ID_TERMINAL;
const Int32 CatService::DEV_ID_NETWORK;

CatService::CatService()
    : mStkAppInstalled(FALSE)
    , mCardState(CARDSTATE_ABSENT)
{
    Handler::constructor();
}

ECode CatService::constructor(
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ IContext* context,
    /* [in] */ IIccFileHandler* fh,
    /* [in] */ Int32 slotId)
{
    if (ci == NULL || context == NULL || fh == NULL) {
        // throw new NullPointerException(
        //         "Service: Input parameters must not be null");
        return E_NULL_POINTER_EXCEPTION;
    }
    mCmdIf = ci;
    mContext = context;
    mSlotId = slotId;
    mHandlerThread = new HandlerThread(String("Cat Telephony service") + StringUtils::ToString(slotId));
    IThread::Probe(mHandlerThread)->Start();

    // Get the RilMessagesDecoder for decoding the messages.
    mMsgDecoder = RilMessageDecoder::GetInstance(this, fh, slotId);
    if (NULL == mMsgDecoder) {
        CatLog::D(ICatService::Probe(this), String("Null RilMessageDecoder instance"));
        return NOERROR;
    }
    mMsgDecoder->Start();

    // Register ril events handling.
    mCmdIf->SetOnCatSessionEnd(this, MSG_ID_SESSION_END, NULL);
    mCmdIf->SetOnCatProactiveCmd(this, MSG_ID_PROACTIVE_COMMAND, NULL);
    mCmdIf->SetOnCatEvent(this, MSG_ID_EVENT_NOTIFY, NULL);
    mCmdIf->SetOnCatCallSetUp(this, MSG_ID_CALL_SETUP, NULL);
    mCmdIf->SetOnCatSendSmsResult(this, MSG_ID_SEND_SMS_RESULT, NULL); // Samsung STK
    //mCmdIf->SetOnSimRefresh(this, MSG_ID_REFRESH, NULL);

    mCmdIf->RegisterForIccRefresh(this, MSG_ID_ICC_REFRESH, NULL);
    mCmdIf->SetOnCatCcAlphaNotify(this, MSG_ID_ALPHA_NOTIFY, NULL);

    AutoPtr<IUiccControllerHelper> hlp;
    CUiccControllerHelper::AcquireSingleton((IUiccControllerHelper**)&hlp);
    hlp->GetInstance((IUiccController**)&mUiccController);
    mUiccController->RegisterForIccChanged(this, MSG_ID_ICC_CHANGED, NULL);

    // Check if STK application is availalbe
    mStkAppInstalled = IsStkAppInstalled();

    CatLog::D(ICatService::Probe(this), String("Running CAT service on Slotid: ")
            + StringUtils::ToString(mSlotId)
            + String(". STK app installed:") + StringUtils::ToString(mStkAppInstalled));
    return NOERROR;
}

ECode CatService::Dispose()
{
    CatLog::D(ICatService::Probe(this), String("Disposing CatService object for slot: ")
            + StringUtils::ToString(mSlotId));

    // Clean up stk icon if dispose is called
    BroadcastCardStateAndIccRefreshResp(CARDSTATE_ABSENT, NULL);

    mCmdIf->UnSetOnCatSessionEnd(this);
    mCmdIf->UnSetOnCatProactiveCmd(this);
    mCmdIf->UnSetOnCatEvent(this);
    mCmdIf->UnSetOnCatCallSetUp(this);
    mCmdIf->UnSetOnCatCcAlphaNotify(this);
    mCmdIf->UnSetOnCatSendSmsResult(this);

    mCmdIf->UnregisterForIccRefresh(this);
    if (mUiccController != NULL) {
        mUiccController->UnregisterForIccChanged(this);
        mUiccController = NULL;
    }
    if (mMsgDecoder != NULL) {
        mMsgDecoder->Dispose(mSlotId);
        mMsgDecoder = NULL;
    }
    Boolean bQ = FALSE;
    mHandlerThread->Quit(&bQ);
    mHandlerThread = NULL;
    RemoveCallbacksAndMessages(NULL);
    return NOERROR;
}

ECode CatService::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    CatLog::D(ICatService::Probe(this), String("handleMessage[") + StringUtils::ToString(what) + String("]"));

    switch (what) {
    case MSG_ID_SESSION_END:
    case MSG_ID_PROACTIVE_COMMAND:
    case MSG_ID_EVENT_NOTIFY:
    case MSG_ID_REFRESH: {
        CatLog::D(ICatService::Probe(this), String("ril message arrived,slotid:")
                + StringUtils::ToString(mSlotId));
        String data;
        if (obj != NULL) {
            AutoPtr<AsyncResult> ar = (AsyncResult*)(IObject::Probe(obj));
            if (ar != NULL && ar->mResult != NULL) {
                // try {
                ICharSequence::Probe(ar->mResult)->ToString(&data);
                // } catch (ClassCastException e) {
                //     break;
                // }
            }
        }
        AutoPtr<RilMessage> rilMsg = new RilMessage(what, data);
        mMsgDecoder->SendStartDecodingMessageParams(rilMsg);
        break;
    }
    case MSG_ID_CALL_SETUP: {
        AutoPtr<RilMessage> rilMsg = new RilMessage(what, String(NULL));
        mMsgDecoder->SendStartDecodingMessageParams(rilMsg);
        break;
    }
    case MSG_ID_RIL_MSG_DECODED:
        HandleRilMsg((RilMessage*)(IObject*)obj.Get());
        break;
    case MSG_ID_RESPONSE:
        HandleCmdResponse((CatResponseMessage*)(IObject*)obj.Get());
        break;
    case MSG_ID_ICC_CHANGED: {
        CatLog::D(ICatService::Probe(this), String("MSG_ID_ICC_CHANGED"));
        UpdateIccAvailability();
        break;
    }
    case MSG_ID_ICC_REFRESH: {
        if (obj != NULL) {
            AutoPtr<AsyncResult> ar = (AsyncResult*)(IObject*)obj.Get();
            if (ar != NULL && ar->mResult != NULL) {
                BroadcastCardStateAndIccRefreshResp(CARDSTATE_PRESENT,
                                IIccRefreshResponse::Probe(ar->mResult));
            }
            else {
                assert(0 && "TODO");
                // CatLog::D(ICatService::Probe(this), String("Icc REFRESH with exception: ") + ar->mException);
            }
        }
        else {
            CatLog::D(ICatService::Probe(this), String("IccRefresh Message is NULL"));
        }
        break;
    }
    case MSG_ID_ALPHA_NOTIFY: {
        CatLog::D(ICatService::Probe(this), String("Received CAT CC Alpha message from card"));
        if (obj != NULL) {
            AutoPtr<AsyncResult> ar = (AsyncResult*)(IObject*)obj.Get();
            if (ar != NULL && ar->mResult != NULL) {
                String str;
                ICharSequence::Probe(ar->mResult)->ToString(&str);
                BroadcastAlphaMessage(str);
            }
            else {
                CatLog::D(ICatService::Probe(this), String("CAT Alpha message: ar.result is NULL"));
            }
        }
        else {
            CatLog::D(ICatService::Probe(this), String("CAT Alpha message: msg.obj is NULL"));
        }
        break;
    }
    case MSG_ID_TIMEOUT: { // Should only be called for Samsung STK
        if (mTimeoutDest == WAITING_SMS_RESULT) {
            CatLog::D(ICatService::Probe(this), String("SMS SEND TIMEOUT"));
            if (mCallControlResultCode ==
                    CALL_CONTROL_NOT_ALLOWED) {
                SendTerminalResponse(mCurrntCmd->mCmdDet,
                        ResultCode_USIM_CALL_CONTROL_PERMANENT, TRUE, 1, NULL);
            }
            else {
                SendTerminalResponse(mCurrntCmd->mCmdDet,
                        ResultCode_TERMINAL_CRNTLY_UNABLE_TO_PROCESS, FALSE, 0, NULL);
            }
            break;
        }
        break;
    }
    case MSG_ID_SEND_SMS_RESULT: { // Samsung STK SEND_SMS
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        Boolean b = FALSE;
        res->GetBoolean(R::bool_::config_samsung_stk, &b);
        if (b) {
            AutoPtr<ArrayOf<Int32> > sendResult;
            AutoPtr<AsyncResult> ar;
            CatLog::D(ICatService::Probe(this), String("handleMsg : MSG_ID_SEND_SMS_RESULT"));
            CancelTimeOut();
            CatLog::D(ICatService::Probe(this), String("The Msg ID data:") + StringUtils::ToString(what));
            ar = (AsyncResult*)(IObject*)obj.Get();
            if (ar == NULL || ar->mResult == NULL || mCurrntCmd == NULL || mCurrntCmd->mCmdDet == NULL)
                break;
            assert(0 && "TODO");
            // sendResult = (int[]) ar.result;
            if (sendResult->GetLength() == 0)
                break;
            switch ((*sendResult)[0]) {
                default:
                    CatLog::D(ICatService::Probe(this), String("SMS SEND GENERIC FAIL"));
                    if (mCallControlResultCode ==
                            CALL_CONTROL_NOT_ALLOWED) {
                        SendTerminalResponse(mCurrntCmd->mCmdDet,
                                ResultCode_USIM_CALL_CONTROL_PERMANENT, TRUE, 1, NULL);
                    }
                    else {
                        SendTerminalResponse(mCurrntCmd->mCmdDet,
                                ResultCode_TERMINAL_CRNTLY_UNABLE_TO_PROCESS, FALSE, 0, NULL);
                    }
                    break;
                case SMS_SEND_OK: // '\0'
                    CatLog::D(ICatService::Probe(this), String("SMS SEND OK"));
                    if (mCallControlResultCode ==
                            CALL_CONTROL_NOT_ALLOWED) {
                        SendTerminalResponse(mCurrntCmd->mCmdDet,
                                ResultCode_USIM_CALL_CONTROL_PERMANENT, TRUE, 1, NULL);
                    }
                    else {
                        SendTerminalResponse(mCurrntCmd->mCmdDet, ResultCode_OK, FALSE, 0, NULL);
                    }
                    break;
                case SMS_SEND_FAIL:
                    CatLog::D(ICatService::Probe(this), String("SMS SEND FAIL - MEMORY NOT AVAILABLE"));
                    if (mCallControlResultCode ==
                            CALL_CONTROL_NOT_ALLOWED) {
                        SendTerminalResponse(mCurrntCmd->mCmdDet,
                                ResultCode_USIM_CALL_CONTROL_PERMANENT, TRUE, 1, NULL);
                    }
                    else {
                        SendTerminalResponse(mCurrntCmd->mCmdDet,
                                ResultCode_TERMINAL_CRNTLY_UNABLE_TO_PROCESS, FALSE, 0, NULL);
                    }
                    break;
                case SMS_SEND_RETRY:
                    CatLog::D(ICatService::Probe(this), String("SMS SEND FAIL RETRY"));
                    if (mCallControlResultCode ==
                            CALL_CONTROL_NOT_ALLOWED) {
                        SendTerminalResponse(mCurrntCmd->mCmdDet,
                                ResultCode_USIM_CALL_CONTROL_PERMANENT, TRUE, 1, NULL);
                    }
                    else {
                        SendTerminalResponse(mCurrntCmd->mCmdDet,
                                ResultCode_NETWORK_CRNTLY_UNABLE_TO_PROCESS, FALSE, 0, NULL);
                    }
                    break;
                }
        }
        break;
    }
    default:
        // throw new AssertionError("Unrecognized CAT command: " + msg.what);
        return E_ASSERTION_ERROR;
    }
    return NOERROR;
}

// synchronized
ECode CatService::OnCmdResponse(
    /* [in] */ ICatResponseMessage* resMsg)
{
    if (resMsg == NULL) {
        return NOERROR;
    }
    // queue a response message.
    AutoPtr<IMessage> msg;
    ObtainMessage(MSG_ID_RESPONSE, resMsg, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CatService::UpdateIccAvailability()
{
    if (NULL == mUiccController) {
        return NOERROR;
    }

    CardState newState = CARDSTATE_ABSENT;
    AutoPtr<IUiccCard> newCard;
    mUiccController->GetUiccCard(mSlotId, (IUiccCard**)&newCard);
    if (newCard != NULL) {
        newCard->GetCardState(&newState);
    }
    CardState oldState = mCardState;
    mCardState = newState;
    CatLog::D(ICatService::Probe(this), String("New Card State = ") + StringUtils::ToString(newState)
            + String(" Old Card State = ") + StringUtils::ToString(oldState));
    if (oldState == CARDSTATE_PRESENT &&
            newState != CARDSTATE_PRESENT) {
        BroadcastCardStateAndIccRefreshResp(newState, NULL);
    }
    else if (oldState != CARDSTATE_PRESENT &&
            newState == CARDSTATE_PRESENT) {
        // Card moved to PRESENT STATE.
        mCmdIf->ReportStkServiceIsRunning(NULL);
    }
    return NOERROR;
}

ECode CatService::Finalize()
{
    CatLog::D(ICatService::Probe(this), String("Service finalized"));
    return NOERROR;
}

void CatService::HandleRilMsg(
    /* [in] */ RilMessage* rilMsg)
{
    if (rilMsg == NULL) {
        return;
    }

    // dispatch messages
    AutoPtr<CommandParams> cmdParams;
    switch (rilMsg->mId) {
    case MSG_ID_EVENT_NOTIFY:
        if (rilMsg->mResCode == ResultCode_OK) {
            cmdParams = (CommandParams*)(IObject*)(rilMsg->mData.Get());
            if (cmdParams != NULL) {
                HandleCommand(cmdParams, FALSE);
            }
        }
        break;
    case MSG_ID_PROACTIVE_COMMAND:
        // try {
            cmdParams = (CommandParams*)(IObject*)(rilMsg->mData.Get());
        // } catch (ClassCastException e) {
        //     // for error handling : cast exception
        //     CatLog::D(this, "Fail to parse proactive command");
        //     // Don't send Terminal Resp if command detail is not available
        //     if (mCurrntCmd != NULL) {
        //         sendTerminalResponse(mCurrntCmd.mCmdDet, ResultCode.CMD_DATA_NOT_UNDERSTOOD,
        //                          FALSE, 0x00, NULL);
        //     }
        //     break;
        // }
        if (cmdParams != NULL) {
            if (rilMsg->mResCode == ResultCode_OK) {
                HandleCommand(cmdParams, TRUE);
            }
            else {
                // for proactive commands that couldn't be decoded
                // successfully respond with the code generated by the
                // message decoder.
                SendTerminalResponse(cmdParams->mCmdDet, rilMsg->mResCode,
                        FALSE, 0, NULL);
            }
        }
        break;
    case MSG_ID_REFRESH:
        cmdParams = (CommandParams*)(IObject*)(rilMsg->mData.Get());
        if (cmdParams != NULL) {
            HandleCommand(cmdParams, FALSE);
        }
        break;
    case MSG_ID_SESSION_END:
        HandleSessionEnd();
        break;
    case MSG_ID_CALL_SETUP:
        // prior event notify command supplied all the information
        // needed for set up call processing.
        break;
    }
}

Boolean CatService::IsSupportedSetupEventCommand(
    /* [in] */ ICatCmdMessage* cmdMsg)
{
    Boolean flag = TRUE;

    AutoPtr<ISetupEventListSettings> ls;
    cmdMsg->GetSetEventList((ISetupEventListSettings**)&ls);
    AutoPtr<ArrayOf<Int32> > eventList;
    ls->GetEventList((ArrayOf<Int32>**)&eventList);
    for (Int32 i = 0; i < eventList->GetLength(); i++) {
        Int32 eventVal = (*eventList)[i];
        CatLog::D(ICatService::Probe(this), String("Event: ") + StringUtils::ToString(eventVal));
        switch (eventVal) {
            /* Currently android is supporting only the below events in SetupEventList
             * Idle Screen Available,
             * Language Selection and
             * HCI Connectivity.
             */
            case ICatCmdMessageSetupEventListConstants::IDLE_SCREEN_AVAILABLE_EVENT:
            case ICatCmdMessageSetupEventListConstants::LANGUAGE_SELECTION_EVENT:
            case ICatCmdMessageSetupEventListConstants::HCI_CONNECTIVITY_EVENT:
                break;
            default:
                flag = FALSE;
        }
    }
    return flag;
}

void CatService::HandleCommand(
    /* [in] */ CommandParams* cmdParams,
    /* [in] */ Boolean isProactiveCmd)
{
    CommandType type;
    cmdParams->GetCommandType(&type);
    assert(0 && "TODO");
    // CatLog::D(ICatService::Probe(this), type.Name());

    AutoPtr<ICharSequence> message;
    ResultCode resultCode;
    AutoPtr<ICatCmdMessage> cmdMsg;
    CCatCmdMessage::New((IObject*)cmdParams, (ICatCmdMessage**)&cmdMsg);
    switch (type) {
        case SET_UP_MENU: {
            AutoPtr<IMenu> menu;
            cmdMsg->GetMenu((IMenu**)&menu);
            if (RemoveMenu(menu)) {
                mMenuCmd = NULL;
            }
            else {
                mMenuCmd = (CatCmdMessage*)cmdMsg.Get();
            }
            resultCode = cmdParams->mLoadIconFailed ? ResultCode_PRFRMD_ICON_NOT_DISPLAYED
                                                                        : ResultCode_OK;
            if (isProactiveCmd) {
                SendTerminalResponse(cmdParams->mCmdDet, resultCode, FALSE, 0, NULL);
            }
            break;
        }
        case DISPLAY_TEXT:
            break;
        case REFRESH: {
            //Stk app service displays alpha id to user if it is present, nothing to do here
            CatLog::D(ICatService::Probe(this), String("Pass Refresh to Stk app"));
            break;
        }
        case SET_UP_IDLE_MODE_TEXT: {
            resultCode = cmdParams->mLoadIconFailed ? ResultCode_PRFRMD_ICON_NOT_DISPLAYED
                                                                        : ResultCode_OK;
            if (isProactiveCmd) {
                SendTerminalResponse(cmdParams->mCmdDet, resultCode, FALSE, 0, NULL);
            }
            break;
        }
        case SET_UP_EVENT_LIST: {
            if (isProactiveCmd) {
                if (IsSupportedSetupEventCommand(cmdMsg)) {
                    SendTerminalResponse(cmdParams->mCmdDet, ResultCode_OK, FALSE, 0, NULL);
                }
                else {
                    SendTerminalResponse(cmdParams->mCmdDet,
                            ResultCode_BEYOND_TERMINAL_CAPABILITY, FALSE, 0, NULL);
                }
            }
            break;
        }
        case PROVIDE_LOCAL_INFORMATION: {
            assert(0 && "TODO");
            // ResponseData resp;
            // switch (cmdParams.mCmdDet.commandQualifier) {
            //     case CommandParamsFactory::DTTZ_SETTING:
            //         resp = new DTTZResponseData(NULL);
            //         SendTerminalResponse(cmdParams->mCmdDet, ResultCode_OK, FALSE, 0, resp);
            //         break;
            //     case CommandParamsFactory::LANGUAGE_SETTING:
            //         AutoPtr<ILocaleHelper> hlp;
            //         CLocaleHelper::AcquireSingleton((ILocaleHelper**)&hlp);
            //         AutoPtr<ILocale> loc;
            //         hlp->GetDefault((ILocale**)&loc);
            //         String lang;
            //         loc->GetLanguage(&lang);
            //         resp = new LanguageResponseData(lang);
            //         SendTerminalResponse(cmdParams->mCmdDet, ResultCode_OK, FALSE, 0, resp);
            //         break;
            //     default:
            //         SendTerminalResponse(cmdParams->mCmdDet, ResultCode_OK, FALSE, 0, NULL);
            // }
            // // No need to start STK app here.
            return;
        }
        case LAUNCH_BROWSER: {
            AutoPtr<TextMessage> tm = (TextMessage*)(((LaunchBrowserParams*) cmdParams)->mConfirmMsg.Get());
            if ((tm->mText != NULL)
                    && (tm->mText.Equals(STK_DEFAULT))) {
                mContext->GetText(R::string::launchBrowserDefault, (ICharSequence**)&message);
                message->ToString(&(tm->mText));
            }
            break;
        }
        case SELECT_ITEM:
        case GET_INPUT:
        case GET_INKEY:
            break;
        case SEND_DTMF:
        case SEND_SMS: {
            // Samsung STK
            if ((SendSMSParams*)cmdParams != NULL) {
                HandleProactiveCommandSendSMS((SendSMSParams*)cmdParams);
            }
            // Fall through
        }
        case SEND_SS:
        case SEND_USSD: {
            // Samsung STK
            if ((SendUSSDParams*)cmdParams != NULL) {
                HandleProactiveCommandSendUSSD((SendUSSDParams*)cmdParams);
            }
            AutoPtr<TextMessage> tm = (TextMessage*)(((DisplayTextParams*) cmdParams)->mTextMsg.Get());
            if ((tm->mText != NULL)
                    && (tm->mText.Equals(STK_DEFAULT))) {
                mContext->GetText(R::string::sending, (ICharSequence**)&message);
                message->ToString(&(tm->mText));
            }
            break;
        }
        case PLAY_TONE:
            break;
        case SET_UP_CALL: {
            AutoPtr<TextMessage> tm = (TextMessage*)(((CallSetupParams*) cmdParams)->mConfirmMsg.Get());
            if ((tm->mText != NULL)
                    && (tm->mText.Equals(STK_DEFAULT))) {
                mContext->GetText(R::string::SetupCallDefault, (ICharSequence**)&message);
                message->ToString(&(tm->mText));
            }
            break;
        }
        case OPEN_CHANNEL:
        case CLOSE_CHANNEL:
        case RECEIVE_DATA:
        case SEND_DATA: {
            AutoPtr<BIPClientParams> cmd = (BIPClientParams*) cmdParams;
            /* Per 3GPP specification 102.223,
             * if the alpha identifier is not provided by the UICC,
             * the terminal MAY give information to the user
             * noAlphaUsrCnf defines if you need to show user confirmation or not
             */
            Boolean noAlphaUsrCnf = FALSE;
            // try {
                AutoPtr<IResources> res;
                mContext->GetResources((IResources**)&res);
                res->GetBoolean(
                        R::bool_::config_stkNoAlphaUsrCnf, &noAlphaUsrCnf);
            // } catch (NotFoundException e) {
            //     noAlphaUsrCnf = FALSE;
            // }
            AutoPtr<TextMessage> tm = (TextMessage*)(cmd->mTextMsg.Get());
            if ((tm->mText == NULL) && (cmd->mHasAlphaId || noAlphaUsrCnf)) {
                CommandType type;
                cmdParams->GetCommandType(&type);
                CatLog::D(ICatService::Probe(this), String("cmd ")
                        + StringUtils::ToString(type) + String(" with NULL alpha id"));
                // If alpha length is zero, we just respond with OK.
                if (isProactiveCmd) {
                    SendTerminalResponse(cmdParams->mCmdDet, ResultCode_OK, FALSE, 0, NULL);
                }
                else if (type == OPEN_CHANNEL) {
                    mCmdIf->HandleCallSetupRequestFromSim(TRUE, NULL);
                }
                return;
            }
            // Respond with permanent failure to avoid retry if STK app is not present.
            if (!mStkAppInstalled) {
                CatLog::D(ICatService::Probe(this), String("No STK application found."));
                if (isProactiveCmd) {
                    SendTerminalResponse(cmdParams->mCmdDet,
                                        ResultCode_BEYOND_TERMINAL_CAPABILITY,
                                        FALSE, 0, NULL);
                    return;
                }
            }
            /*
             * CLOSE_CHANNEL, RECEIVE_DATA and SEND_DATA can be delivered by
             * either PROACTIVE_COMMAND or EVENT_NOTIFY.
             * If PROACTIVE_COMMAND is used for those commands, send terminal
             * response here.
             */
            if (isProactiveCmd &&
                ((type == CLOSE_CHANNEL) ||
                 (type == RECEIVE_DATA) ||
                 (type == SEND_DATA))) {
                SendTerminalResponse(cmdParams->mCmdDet, ResultCode_OK, FALSE, 0, NULL);
            }
            break;
        }
        case ACTIVATE: {
            // TO DO: Retrieve the target of the ACTIVATE cmd from the cmd.
            // Target : '01' = UICC-CFL interface according to TS 102 613 [39];
            //          '00' and '02' to 'FF' = RFU (Reserved for Future Use).
            resultCode = ResultCode_OK;
            SendTerminalResponse(cmdParams->mCmdDet, resultCode, FALSE, 0, NULL);
            break;
        }
        default:
            CatLog::D(ICatService::Probe(this), String("Unsupported command"));
            return;
    }
    mCurrntCmd = (CatCmdMessage*)cmdMsg.Get();
    BroadcastCatCmdIntent(cmdMsg);
}

void CatService::BroadcastCatCmdIntent(
    /* [in] */ ICatCmdMessage* cmdMsg)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IAppInterface::CAT_CMD_ACTION, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
    intent->PutExtra(String("STK CMD"), IParcelable::Probe(cmdMsg));
    intent->PutExtra(String("SLOT_ID"), mSlotId);
    assert(0 && "TODO");
    // CatLog::D(ICatService::Probe(this), String("Sending CmdMsg: ") + cmdMsg
    //         + String(" on slotid:") + StringUtils::ToString(mSlotId));
    mContext->SendBroadcast(intent, IAppInterface::STK_PERMISSION);
}

void CatService::HandleSessionEnd()
{
    CatLog::D(ICatService::Probe(this), String("SESSION END on ") + StringUtils::ToString(mSlotId));

    mCurrntCmd = mMenuCmd;
    AutoPtr<IIntent> intent;
    CIntent::New(IAppInterface::CAT_SESSION_END_ACTION, (IIntent**)&intent);
    intent->PutExtra(String("SLOT_ID"), mSlotId);
    intent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
    mContext->SendBroadcast(intent, IAppInterface::STK_PERMISSION);
}

void CatService::SendTerminalResponse(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ ResultCode resultCode,
    /* [in] */ Boolean includeAdditionalInfo,
    /* [in] */ Int32 additionalInfo,
    /* [in] */ ResponseData* resp)
{
    if (cmdDet == NULL) {
        return;
    }
    AutoPtr<IByteArrayOutputStream> buf;
    CByteArrayOutputStream::New((IByteArrayOutputStream**)&buf);

    AutoPtr<IInput> cmdInput;
    if (mCurrntCmd != NULL) {
        mCurrntCmd->GeInput((IInput**)&cmdInput);
    }

    // command details
    Int32 tag = ComprehensionTlvTag_COMMAND_DETAILS;
    if (cmdDet->mCompRequired) {
        tag |= 0x80;
    }
    IOutputStream::Probe(buf)->Write(tag);
    IOutputStream::Probe(buf)->Write(0x03); // length
    IOutputStream::Probe(buf)->Write(cmdDet->mCommandNumber);
    IOutputStream::Probe(buf)->Write(cmdDet->mTypeOfCommand);
    IOutputStream::Probe(buf)->Write(cmdDet->mCommandQualifier);

    // device identities
    // According to TS102.223/TS31.111 section 6.8 Structure of
    // TERMINAL RESPONSE, "For all SIMPLE-TLV objects with Min=N,
    // the ME should set the CR(comprehension required) flag to
    // comprehension not required.(CR=0)"
    // Since DEVICE_IDENTITIES and DURATION TLVs have Min=N,
    // the CR flag is not set.
    tag = ComprehensionTlvTag_DEVICE_IDENTITIES;
    IOutputStream::Probe(buf)->Write(tag);
    IOutputStream::Probe(buf)->Write(0x02); // length
    IOutputStream::Probe(buf)->Write(DEV_ID_TERMINAL); // source device id
    IOutputStream::Probe(buf)->Write(DEV_ID_UICC); // destination device id

    // result
    tag = ComprehensionTlvTag_RESULT;
    if (cmdDet->mCompRequired) {
        tag |= 0x80;
    }
    IOutputStream::Probe(buf)->Write(tag);
    Int32 length = includeAdditionalInfo ? 2 : 1;
    IOutputStream::Probe(buf)->Write(length);
    IOutputStream::Probe(buf)->Write(resultCode);

    // additional info
    if (includeAdditionalInfo) {
        IOutputStream::Probe(buf)->Write(additionalInfo);
    }

    // Fill optional data for each corresponding command
    if (resp != NULL) {
        resp->Format(buf);
    }
    else {
        EncodeOptionalTags(cmdDet, resultCode, cmdInput, buf);
    }

    AutoPtr<ArrayOf<Byte> > rawData;
    buf->ToByteArray((ArrayOf<Byte>**)&rawData);
    AutoPtr<IIccUtils> iccu;
    assert(0 && "TODO");
    // CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
    String hexString;
    iccu->BytesToHexString(rawData, &hexString);
    if (DBG) {
        CatLog::D(ICatService::Probe(this), String("TERMINAL RESPONSE: ") + hexString);
    }

    mCmdIf->SendTerminalResponse(hexString, NULL);
}

void CatService::EncodeOptionalTags(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ ResultCode resultCode,
    /* [in] */ IInput* cmdInput,
    /* [in] */ IByteArrayOutputStream* buf)
{
    CommandType cmdType = cmdDet->mTypeOfCommand;
    // if (cmdType != NULL) {
        switch (cmdType) {
            case GET_INKEY:
                // ETSI TS 102 384,27.22.4.2.8.4.2.
                // If it is a response for GET_INKEY command and the response timeout
                // occured, then add DURATION TLV for variable timeout case.
                assert(0 && "TODO");
                // if ((resultCode == ResultCode_NO_RESPONSE_FROM_USER) &&
                //     (cmdInput != NULL) && (cmdInput->mDuration != NULL)) {
                //     GetInKeyResponse(buf, cmdInput);
                // }
                break;
            case PROVIDE_LOCAL_INFORMATION:
                if ((cmdDet->mCommandQualifier == CommandParamsFactory::LANGUAGE_SETTING) &&
                    (resultCode == ResultCode_OK)) {
                    GetPliResponse(buf);
                }
                break;
            default:
                assert(0 && "TODO");
                // CatLog::D(ICatService::Probe(this), String("encodeOptionalTags() Unsupported Cmd details=")
                //         + cmdDet);
                break;
        }
    // }
    // else {
    //     CatLog::D(ICatService::Probe(this), String("encodeOptionalTags() bad Cmd details=") + cmdDet);
    // }
}

void CatService::GetInKeyResponse(
    /* [in] */ IByteArrayOutputStream* buf,
    /* [in] */ IInput* cmdInput)
{
    Int32 tag = ComprehensionTlvTag_DURATION;

    IOutputStream::Probe(buf)->Write(tag);
    IOutputStream::Probe(buf)->Write(0x02); // length
    AutoPtr<Input> cinput = (Input*)cmdInput;
    AutoPtr<Duration> cd = (Duration*)(cinput->mDuration.Get());
    assert(0 && "TODO");
    // IOutputStream::Probe(buf)->Write(cd->timeUnit.TimeUnit_SECOND); // Time (Unit,Seconds)
    IOutputStream::Probe(buf)->Write(cd->mTimeInterval); // Time Duration
}

void CatService::GetPliResponse(
    /* [in] */ IByteArrayOutputStream* buf)
{
    // Locale Language Setting
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String lang;
    sp->Get(String("persist.sys.language"), &lang);

    if (!lang.IsNull()) {
        // tag
        Int32 tag = ComprehensionTlvTag_LANGUAGE;
        IOutputStream::Probe(buf)->Write(tag);
        ResponseData::WriteLength(buf, lang.GetLength());
        IOutputStream::Probe(buf)->Write(lang.GetBytes(), 0, lang.GetLength());
    }
}

void CatService::SendMenuSelection(
    /* [in] */ Int32 menuId,
    /* [in] */ Boolean helpRequired)
{
    AutoPtr<IByteArrayOutputStream> buf;
    CByteArrayOutputStream::New((IByteArrayOutputStream**)&buf);

    // tag
    Int32 tag = BerTlv::BER_MENU_SELECTION_TAG;
    IOutputStream::Probe(buf)->Write(tag);

    // length
    IOutputStream::Probe(buf)->Write(0x00); // place holder

    // device identities
    tag = 0x80 | ComprehensionTlvTag_DEVICE_IDENTITIES;
    IOutputStream::Probe(buf)->Write(tag);
    IOutputStream::Probe(buf)->Write(0x02); // length
    IOutputStream::Probe(buf)->Write(DEV_ID_KEYPAD); // source device id
    IOutputStream::Probe(buf)->Write(DEV_ID_UICC); // destination device id

    // item identifier
    tag = 0x80 | ComprehensionTlvTag_ITEM_ID;
    IOutputStream::Probe(buf)->Write(tag);
    IOutputStream::Probe(buf)->Write(0x01); // length
    IOutputStream::Probe(buf)->Write(menuId); // menu identifier chosen

    // help request
    if (helpRequired) {
        tag = ComprehensionTlvTag_HELP_REQUEST;
        IOutputStream::Probe(buf)->Write(tag);
        IOutputStream::Probe(buf)->Write(0x00); // length
    }

    AutoPtr<ArrayOf<Byte> > rawData;
    buf->ToByteArray((ArrayOf<Byte>**)&rawData);

    // write real length
    Int32 len = rawData->GetLength() - 2; // minus (tag + length)
    (*rawData)[1] = (Byte) len;

    AutoPtr<IIccUtils> iccu;
    assert(0 && "TODO");
    // CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
    String hexString;
    iccu->BytesToHexString(rawData, &hexString);

    mCmdIf->SendEnvelope(hexString, NULL);
}

void CatService::EventDownload(
    /* [in] */ Int32 event,
    /* [in] */ Int32 sourceId,
    /* [in] */ Int32 destinationId,
    /* [in] */ ArrayOf<Byte>* additionalInfo,
    /* [in] */ Boolean oneShot)
{
    AutoPtr<IByteArrayOutputStream> buf;
    CByteArrayOutputStream::New((IByteArrayOutputStream**)&buf);

    // tag
    Int32 tag = BerTlv::BER_EVENT_DOWNLOAD_TAG;
    IOutputStream::Probe(buf)->Write(tag);

    // length
    IOutputStream::Probe(buf)->Write(0x00); // place holder, assume length < 128.

    // event list
    tag = 0x80 | ComprehensionTlvTag_EVENT_LIST;
    IOutputStream::Probe(buf)->Write(tag);
    IOutputStream::Probe(buf)->Write(0x01); // length
    IOutputStream::Probe(buf)->Write(event); // event value

    // device identities
    tag = 0x80 | ComprehensionTlvTag_DEVICE_IDENTITIES;
    IOutputStream::Probe(buf)->Write(tag);
    IOutputStream::Probe(buf)->Write(0x02); // length
    IOutputStream::Probe(buf)->Write(sourceId); // source device id
    IOutputStream::Probe(buf)->Write(destinationId); // destination device id

    /*
     * Check for type of event download to be sent to UICC - Browser
     * termination,Idle screen available, User activity, Language selection
     * etc as mentioned under ETSI TS 102 223 section 7.5
     */

    /*
     * Currently the below events are supported:
     * Idle Screen Available,
     * Language Selection Event and
     * HCI Connectivity.
     * Other event download commands should be encoded similar way
     */
    /* TODO: eventDownload should be extended for other Envelope Commands */
    switch (event) {
        case ICatCmdMessageSetupEventListConstants::IDLE_SCREEN_AVAILABLE_EVENT:
            CatLog::D(ICatService::Probe(this), String(" Sending Idle Screen Available event download to ICC"));
            break;
        case ICatCmdMessageSetupEventListConstants::LANGUAGE_SELECTION_EVENT:
            CatLog::D(ICatService::Probe(this), String(" Sending Language Selection event download to ICC"));
            tag = 0x80 | ComprehensionTlvTag_LANGUAGE;
            IOutputStream::Probe(buf)->Write(tag);
            // Language length should be 2 byte
            IOutputStream::Probe(buf)->Write(0x02);
            break;
        case ICatCmdMessageSetupEventListConstants::HCI_CONNECTIVITY_EVENT:
            CatLog::D(ICatService::Probe(this), String(" Sending HCI Connectivity event download to ICC"));
            break;
        default:
            break;
    }

    // additional information
    if (additionalInfo != NULL) {
        for (Int32 i = 0; i < additionalInfo->GetLength(); i++) {
            Byte b = (*additionalInfo)[i];
            IOutputStream::Probe(buf)->Write(b);
        }
    }

    AutoPtr<ArrayOf<Byte> > rawData;
    buf->ToByteArray((ArrayOf<Byte>**)&rawData);

    // write real length
    Int32 len = rawData->GetLength() - 2; // minus (tag + length)
    (*rawData)[1] = (Byte) len;

    AutoPtr<IIccUtils> iccu;
    assert(0 && "TODO");
    // CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
    String hexString;
    iccu->BytesToHexString(rawData, &hexString);

    CatLog::D(ICatService::Probe(this), String("ENVELOPE COMMAND: ") + hexString);

    mCmdIf->SendEnvelope(hexString, NULL);
}

void CatService::BroadcastCardStateAndIccRefreshResp(
    /* [in] */ CardState cardState,
    /* [in] */ IIccRefreshResponse* iccRefreshState)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IAppInterface::CAT_ICC_STATUS_CHANGE, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
    Boolean cardPresent = (cardState == CARDSTATE_PRESENT);

    if (iccRefreshState != NULL) {
        //This case is when MSG_ID_ICC_REFRESH is received.
        AutoPtr<IccRefreshResponse> cState = (IccRefreshResponse*)iccRefreshState;
        intent->PutExtra(IAppInterface::REFRESH_RESULT, cState->mRefreshResult);
        CatLog::D(ICatService::Probe(this), String("Sending IccResult with Result: ")
                + StringUtils::ToString(cState->mRefreshResult));
    }

    // This sends an intent with CARD_ABSENT (0 - FALSE) /CARD_PRESENT (1 - TRUE).
    intent->PutExtra(IAppInterface::CARD_STATUS, cardPresent);
    intent->PutExtra(String("SLOT_ID"), mSlotId);
    CatLog::D(ICatService::Probe(this), String("Sending Card Status: ")
            + StringUtils::ToString(cardState) + String(" ")
            + String("cardPresent: ") + StringUtils::ToString(cardPresent));
    mContext->SendBroadcast(intent, IAppInterface::STK_PERMISSION);
}

void CatService::BroadcastAlphaMessage(
    /* [in] */ const String& alphaString)
{
    CatLog::D(ICatService::Probe(this), String("Broadcasting CAT Alpha message from card: ") + alphaString);
    AutoPtr<IIntent> intent;
    CIntent::New(IAppInterface::CAT_ALPHA_NOTIFY_ACTION, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
    intent->PutExtra(IAppInterface::ALPHA_STRING, alphaString);
    intent->PutExtra(String("SLOT_ID"), mSlotId);
    mContext->SendBroadcast(intent, IAppInterface::STK_PERMISSION);
}

Boolean CatService::ValidateResponse(
    /* [in] */ CatResponseMessage* resMsg)
{
    Boolean validResponse = FALSE;
    if ((resMsg->mCmdDet->mTypeOfCommand == SET_UP_EVENT_LIST)
            || (resMsg->mCmdDet->mTypeOfCommand == SET_UP_MENU)) {
        CatLog::D(ICatService::Probe(this), String("CmdType: ")
                + StringUtils::ToString(resMsg->mCmdDet->mTypeOfCommand));
        validResponse = TRUE;
    }
    else if (mCurrntCmd != NULL) {
        resMsg->mCmdDet->CompareTo(mCurrntCmd->mCmdDet, &validResponse);
        CatLog::D(ICatService::Probe(this), String("isResponse for last valid cmd: ")
                + StringUtils::ToString(validResponse));
    }
    return validResponse;
}

Boolean CatService::RemoveMenu(
    /* [in] */ IMenu* menu)
{
    assert(0 && "TODO");
    // if (menu->mItems->GetSize() == 1 && menu->mItems->Get(0) == NULL) {
    //     return TRUE;
    // }
    return FALSE;
}

void CatService::HandleCmdResponse(
    /* [in] */ CatResponseMessage* resMsg)
{
    // Make sure the response details match the last valid command. An invalid
    // response is a one that doesn't have a corresponding proactive command
    // and sending it can "confuse" the baseband/ril.
    // One reason for out of order responses can be UI glitches. For example,
    // if the application launch an activity, and that activity is stored
    // by the framework inside the history stack. That activity will be
    // available for relaunch using the latest application dialog
    // (long press on the home button). Relaunching that activity can send
    // the same command's result again to the CatService and can cause it to
    // get out of sync with the SIM. This can happen in case of
    // non-interactive type Setup Event List and SETUP_MENU proactive commands.
    // Stk framework would have already sent Terminal Response to Setup Event
    // List and SETUP_MENU proactive commands. After sometime Stk app will send
    // Envelope Command/Event Download. In which case, the response details doesn't
    // match with last valid command (which are not related).
    // However, we should allow Stk framework to send the message to ICC.
    if (!ValidateResponse(resMsg)) {
        return;
    }
    AutoPtr<ResponseData> resp;
    Boolean helpRequired = FALSE;
    AutoPtr<CommandDetails> cmdDet;
    resMsg->GetCmdDetails((CommandDetails**)&cmdDet);
    CommandType type = cmdDet->mTypeOfCommand;
    switch (resMsg->mResCode) {
    case ResultCode_HELP_INFO_REQUIRED:
        helpRequired = TRUE;
        // fall through
    case ResultCode_OK:
    case ResultCode_PRFRMD_WITH_PARTIAL_COMPREHENSION:
    case ResultCode_PRFRMD_WITH_MISSING_INFO:
    case ResultCode_PRFRMD_WITH_ADDITIONAL_EFS_READ:
    case ResultCode_PRFRMD_ICON_NOT_DISPLAYED:
    case ResultCode_PRFRMD_MODIFIED_BY_NAA:
    case ResultCode_PRFRMD_LIMITED_SERVICE:
    case ResultCode_PRFRMD_WITH_MODIFICATION:
    case ResultCode_PRFRMD_NAA_NOT_ACTIVE:
    case ResultCode_PRFRMD_TONE_NOT_PLAYED:
    case ResultCode_LAUNCH_BROWSER_ERROR:
    case ResultCode_TERMINAL_CRNTLY_UNABLE_TO_PROCESS: {
        switch (type) {
        case SET_UP_MENU: {
            helpRequired = resMsg->mResCode == ResultCode_HELP_INFO_REQUIRED;
            SendMenuSelection(resMsg->mUsersMenuSelection, helpRequired);
            return;
        }
        case SELECT_ITEM: {
            resp = new SelectItemResponseData(resMsg->mUsersMenuSelection);
            break;
        }
        case GET_INPUT:
        case GET_INKEY: {
            AutoPtr<IInput> input;
            mCurrntCmd->GeInput((IInput**)&input);
            AutoPtr<Input> _input = (Input*)input.Get();
            if (!_input->mYesNo) {
                // when help is requested there is no need to send the text
                // string object.
                if (!helpRequired) {
                    resp = new GetInkeyInputResponseData(resMsg->mUsersInput,
                            _input->mUcs2, _input->mPacked);
                }
            }
            else {
                resp = new GetInkeyInputResponseData(
                        resMsg->mUsersYesNoSelection);
            }
            break;
        }
        case DISPLAY_TEXT: {
            if (resMsg->mResCode == ResultCode_TERMINAL_CRNTLY_UNABLE_TO_PROCESS) {
                // For screenbusy case there will be addtional information in the terminal
                // response. And the value of the additional information byte is 0x01.
                resMsg->SetAdditionalInfo(0x01);
            }
            else {
                resMsg->mIncludeAdditionalInfo = FALSE;
                resMsg->mAdditionalInfo = 0;
            }
            break;
        }
        case LAUNCH_BROWSER:
            break;
        // 3GPP TS.102.223: Open Channel alpha confirmation should not send TR
        case OPEN_CHANNEL:
        case SET_UP_CALL: {
            mCmdIf->HandleCallSetupRequestFromSim(resMsg->mUsersConfirm, NULL);
            // No need to send terminal response for SET UP CALL. The user's
            // confirmation result is send back using a dedicated ril message
            // invoked by the CommandInterface call above.
            mCurrntCmd = NULL;
            return;
        }
        case SET_UP_EVENT_LIST: {
            if (ICatCmdMessageSetupEventListConstants::IDLE_SCREEN_AVAILABLE_EVENT == resMsg->mEventValue) {
                EventDownload(resMsg->mEventValue, DEV_ID_DISPLAY, DEV_ID_UICC,
                        resMsg->mAddedInfo, FALSE);
            }
            else {
                EventDownload(resMsg->mEventValue, DEV_ID_TERMINAL, DEV_ID_UICC,
                        resMsg->mAddedInfo, FALSE);
            }
            // No need to send the terminal response after event download.
            return;
        }
        default:
            break;
        }
        break;
    }
    case ResultCode_BACKWARD_MOVE_BY_USER:
    case ResultCode_USER_NOT_ACCEPT: {
        // if the user dismissed the alert dialog for a
        // setup call/open channel, consider that as the user
        // rejecting the call. Use dedicated API for this, rather than
        // sending a terminal response.
        if (type == SET_UP_CALL || type == OPEN_CHANNEL) {
            mCmdIf->HandleCallSetupRequestFromSim(FALSE, NULL);
            mCurrntCmd = NULL;
            return;
        }
        else {
            resp = NULL;
        }
        break;
    }
    case ResultCode_NO_RESPONSE_FROM_USER: {
        // No need to send terminal response for SET UP CALL on user timeout.
        if (type == SET_UP_CALL) {
            mCurrntCmd = NULL;
            return;
        }
    }
    case ResultCode_UICC_SESSION_TERM_BY_USER:
        resp = NULL;
        break;
    default:
        return;
    }
    SendTerminalResponse(cmdDet, resMsg->mResCode, resMsg->mIncludeAdditionalInfo,
            resMsg->mAdditionalInfo, resp);
    mCurrntCmd = NULL;
}

Boolean CatService::IsStkAppInstalled()
{
    AutoPtr<IIntent> intent;
    CIntent::New(IAppInterface::CAT_CMD_ACTION, (IIntent**)&intent);
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IList> broadcastReceivers;
    pm->QueryBroadcastReceivers(intent, IPackageManager::GET_META_DATA, (IList**)&broadcastReceivers);
    Int32 numReceiver = 0;
    if (broadcastReceivers != NULL) {
        broadcastReceivers->GetSize(&numReceiver);
    }

    return (numReceiver > 0);
}

void CatService::HandleProactiveCommandSendSMS(
    /* [in] */ SendSMSParams* cmdPar)
{
    CatLog::D(ICatService::Probe(this), String("The smscaddress is: ") + cmdPar->mSmscAddress);
    CatLog::D(ICatService::Probe(this), String("The SMS tpdu is: ") + cmdPar->mPdu);
    mCmdIf->SendSMS(cmdPar->mSmscAddress, cmdPar->mPdu, NULL);
    StartTimeOut(WAITING_SMS_RESULT, WAITING_SMS_RESULT_TIME);
}

void CatService::HandleProactiveCommandSendUSSD(
    /* [in] */ SendUSSDParams* cmdPar)
{
    CatLog::D(ICatService::Probe(this), String("The USSD is: ") + cmdPar->mUssdString);
    mCmdIf->SendUSSD(cmdPar->mUssdString, NULL);
    // Sent USSD, let framework handle the rest
}

void CatService::CancelTimeOut()
{
    RemoveMessages(MSG_ID_TIMEOUT);
    mTimeoutDest = 0;
}

void CatService::StartTimeOut(
    /* [in] */ Int32 timeout,
    /* [in] */ Int32 delay)
{
    CancelTimeOut();
    mTimeoutDest = timeout;
    AutoPtr<IMessage> msg;
    ObtainMessage(MSG_ID_TIMEOUT, (IMessage**)&msg);
    Boolean res = FALSE;
    SendMessageDelayed(msg, delay, &res);
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
