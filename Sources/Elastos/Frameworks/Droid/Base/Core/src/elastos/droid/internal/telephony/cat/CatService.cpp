/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony.cat;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::ISystemProperties;

using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::ISubscriptionController;
using Elastos::Droid::Internal::Telephony::Uicc::IIccFileHandler;
using Elastos::Droid::Internal::Telephony::Uicc::IIccUtils;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardStatus::ICardState;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRefreshResponse;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;

using Elastos::IO::IByteArrayOutputStream;
using Elastos::Utility::IList;
using Elastos::Utility::ILocale;

using static com::Android::Internal::Telephony::Cat::CatCmdMessage::I
                   SetupEventListConstants.IDLE_SCREEN_AVAILABLE_EVENT;
using static com::Android::Internal::Telephony::Cat::CatCmdMessage::I
                   SetupEventListConstants.LANGUAGE_SELECTION_EVENT;
using static com::Android::Internal::Telephony::Cat::CatCmdMessage::I
                   SetupEventListConstants.HCI_CONNECTIVITY_EVENT;

class RilMessage {
    Int32 mId;
    Object mData;
    ResultCode mResCode;

    RilMessage(Int32 msgId, String rawData) {
        mId = msgId;
        mData = rawData;
    }

    RilMessage(RilMessage other) {
        mId = other.mId;
        mData = other.mData;
        mResCode = other.mResCode;
    }
}

/**
 * Class that implements SIM Toolkit Telephony Service. Interacts with the RIL
 * and application.
 *
 * {@hide}
 */
public class CatService extends Handler implements AppInterface {
    private static const Boolean DBG = FALSE;

    // Class members
    private CommandsInterface mCmdIf;
    private Context mContext;
    private CatCmdMessage mCurrntCmd = NULL;
    private CatCmdMessage mMenuCmd = NULL;

    private RilMessageDecoder mMsgDecoder = NULL;
    private Boolean mStkAppInstalled = FALSE;

    private UiccController mUiccController;
    private CardState mCardState = CardState.CARDSTATE_ABSENT;

    // Samsung STK
    private Int32 mTimeoutDest = 0;
    private Int32 mCallControlResultCode = 0;

    // Service constants.
    static const Int32 MSG_ID_SESSION_END              = 1;
    static const Int32 MSG_ID_PROACTIVE_COMMAND        = 2;
    static const Int32 MSG_ID_EVENT_NOTIFY             = 3;
    static const Int32 MSG_ID_CALL_SETUP               = 4;
    static const Int32 MSG_ID_REFRESH                  = 5;
    static const Int32 MSG_ID_RESPONSE                 = 6;
    static const Int32 MSG_ID_ICC_CHANGED              = 7;
    static const Int32 MSG_ID_ALPHA_NOTIFY             = 8;
    static const Int32 MSG_ID_RIL_MSG_DECODED          = 9;
    static const Int32 MSG_ID_TIMEOUT                  = 10; // Samsung STK
    static const Int32 MSG_ID_SEND_SMS_RESULT          = 11; // Samsung STK

    //Events to signal SIM REFRESH notificatations
    private static const Int32 MSG_ID_ICC_REFRESH  = 30;

    private static const Int32 DEV_ID_KEYPAD      = 0x01;
    private static const Int32 DEV_ID_DISPLAY     = 0x02;
    private static const Int32 DEV_ID_UICC        = 0x81;
    private static const Int32 DEV_ID_TERMINAL    = 0x82;
    private static const Int32 DEV_ID_NETWORK     = 0x83;

    static const String STK_DEFAULT = "Default Message";

    private HandlerThread mHandlerThread;
    private Int32 mSlotId;

    // Samsung STK SEND_SMS
    static const Int32 WAITING_SMS_RESULT = 2;
    static const Int32 WAITING_SMS_RESULT_TIME = 60000;

    static const Int32 SMS_SEND_OK = 0;
    static const Int32 SMS_SEND_FAIL = 32790;
    static const Int32 SMS_SEND_RETRY = 32810;

    /* For multisim catservice should not be singleton */
    CatService(CommandsInterface ci, Context context, IccFileHandler fh, Int32 slotId) {
        If (ci == NULL || context == NULL || fh == NULL) {
            throw new NullPointerException(
                    "Service: Input parameters must not be NULL");
        }
        mCmdIf = ci;
        mContext = context;
        mSlotId = slotId;
        mHandlerThread = new HandlerThread("Cat Telephony service" + slotId);
        mHandlerThread->Start();

        // Get the RilMessagesDecoder for decoding the messages.
        mMsgDecoder = RilMessageDecoder->GetInstance(this, fh, slotId);
        If (NULL == mMsgDecoder) {
            CatLogger::D(this, "Null RilMessageDecoder instance");
            return;
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

        mUiccController = UiccController->GetInstance();
        mUiccController->RegisterForIccChanged(this, MSG_ID_ICC_CHANGED, NULL);

        // Check if STK application is availalbe
        mStkAppInstalled = IsStkAppInstalled();

        CatLogger::D(this, "Running CAT service on Slotid: " + mSlotId +
                ". STK app installed:" + mStkAppInstalled);
    }

    CARAPI Dispose() {
            CatLogger::D(this, "Disposing CatService object for slot: " + mSlotId);

            // Clean up stk icon if dispose is called
            BroadcastCardStateAndIccRefreshResp(CardState.CARDSTATE_ABSENT, NULL);

            mCmdIf->UnSetOnCatSessionEnd(this);
            mCmdIf->UnSetOnCatProactiveCmd(this);
            mCmdIf->UnSetOnCatEvent(this);
            mCmdIf->UnSetOnCatCallSetUp(this);
            mCmdIf->UnSetOnCatCcAlphaNotify(this);
            mCmdIf->UnSetOnCatSendSmsResult(this);

            mCmdIf->UnregisterForIccRefresh(this);
            If (mUiccController != NULL) {
                mUiccController->UnregisterForIccChanged(this);
                mUiccController = NULL;
            }
            If (mMsgDecoder != NULL) {
                mMsgDecoder->Dispose(mSlotId);
                mMsgDecoder = NULL;
            }
            mHandlerThread->Quit();
            mHandlerThread = NULL;
            RemoveCallbacksAndMessages(NULL);
    }

    //@Override
    protected void Finalize() {
        CatLogger::D(this, "Service finalized");
    }

    private void HandleRilMsg(RilMessage rilMsg) {
        If (rilMsg == NULL) {
            return;
        }

        // dispatch messages
        CommandParams cmdParams = NULL;
        Switch (rilMsg.mId) {
        case MSG_ID_EVENT_NOTIFY:
            If (rilMsg.mResCode == ResultCode.OK) {
                cmdParams = (CommandParams) rilMsg.mData;
                If (cmdParams != NULL) {
                    HandleCommand(cmdParams, FALSE);
                }
            }
            break;
        case MSG_ID_PROACTIVE_COMMAND:
            try {
                cmdParams = (CommandParams) rilMsg.mData;
            } Catch (ClassCastException e) {
                // for error handling : cast exception
                CatLogger::D(this, "Fail to parse proactive command");
                // Don't send Terminal Resp if command detail is not available
                If (mCurrntCmd != NULL) {
                    SendTerminalResponse(mCurrntCmd.mCmdDet, ResultCode.CMD_DATA_NOT_UNDERSTOOD,
                                     FALSE, 0x00, NULL);
                }
                break;
            }
            If (cmdParams != NULL) {
                If (rilMsg.mResCode == ResultCode.OK) {
                    HandleCommand(cmdParams, TRUE);
                } else {
                    // for proactive commands that couldn't be decoded
                    // successfully respond with the code generated by the
                    // message decoder.
                    SendTerminalResponse(cmdParams.mCmdDet, rilMsg.mResCode,
                            FALSE, 0, NULL);
                }
            }
            break;
        case MSG_ID_REFRESH:
            cmdParams = (CommandParams) rilMsg.mData;
            If (cmdParams != NULL) {
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

    /**
     * This function validates the events in SETUP_EVENT_LIST which are currently
     * supported by the Android framework. In case of SETUP_EVENT_LIST has NULL events
     * or no events, all the events need to be reset.
     */
    private Boolean IsSupportedSetupEventCommand(CatCmdMessage cmdMsg) {
        Boolean flag = TRUE;

        For (Int32 eventVal: cmdMsg->GetSetEventList().eventList) {
            CatLogger::D(this,"Event: " + eventVal);
            Switch (eventVal) {
                /* Currently android is supporting only the below events in SetupEventList
                 * Idle Screen Available,
                 * Language Selection and
                 * HCI Connectivity.
                 */
                case IDLE_SCREEN_AVAILABLE_EVENT:
                case LANGUAGE_SELECTION_EVENT:
                case HCI_CONNECTIVITY_EVENT:
                    break;
                default:
                    flag = FALSE;
            }
        }
        return flag;
    }

    /**
     * Handles RIL_UNSOL_STK_EVENT_NOTIFY or RIL_UNSOL_STK_PROACTIVE_COMMAND command
     * from RIL.
     * Sends valid proactive command data to the application using intents.
     * RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE will be send back if the command is
     * from RIL_UNSOL_STK_PROACTIVE_COMMAND.
     */
    private void HandleCommand(CommandParams cmdParams, Boolean isProactiveCmd) {
        CatLogger::D(this, cmdParams->GetCommandType()->Name());

        CharSequence message;
        ResultCode resultCode;
        CatCmdMessage cmdMsg = new CatCmdMessage(cmdParams);
        Switch (cmdParams->GetCommandType()) {
            case SET_UP_MENU:
                If (RemoveMenu(cmdMsg->GetMenu())) {
                    mMenuCmd = NULL;
                } else {
                    mMenuCmd = cmdMsg;
                }
                resultCode = cmdParams.mLoadIconFailed ? ResultCode.PRFRMD_ICON_NOT_DISPLAYED
                                                                            : ResultCode.OK;
                If (isProactiveCmd) {
                    SendTerminalResponse(cmdParams.mCmdDet, resultCode, FALSE, 0, NULL);
                }
                break;
            case DISPLAY_TEXT:
                break;
            case REFRESH:
                //Stk app service displays alpha id to user if it is present, nothing to do here
                CatLogger::D(this, "Pass Refresh to Stk app");
                break;
            case SET_UP_IDLE_MODE_TEXT:
                resultCode = cmdParams.mLoadIconFailed ? ResultCode.PRFRMD_ICON_NOT_DISPLAYED
                                                                            : ResultCode.OK;
                If (isProactiveCmd) {
                    SendTerminalResponse(cmdParams.mCmdDet,resultCode, FALSE, 0, NULL);
                }
                break;
            case SET_UP_EVENT_LIST:
                If (isProactiveCmd) {
                    If (IsSupportedSetupEventCommand(cmdMsg)) {
                        SendTerminalResponse(cmdParams.mCmdDet, ResultCode.OK, FALSE, 0, NULL);
                    } else {
                        SendTerminalResponse(cmdParams.mCmdDet,
                                ResultCode.BEYOND_TERMINAL_CAPABILITY, FALSE, 0, NULL);
                    }
                }
                break;
            case PROVIDE_LOCAL_INFORMATION:
                ResponseData resp;
                Switch (cmdParams.mCmdDet.commandQualifier) {
                    case CommandParamsFactory.DTTZ_SETTING:
                        resp = new DTTZResponseData(NULL);
                        SendTerminalResponse(cmdParams.mCmdDet, ResultCode.OK, FALSE, 0, resp);
                        break;
                    case CommandParamsFactory.LANGUAGE_SETTING:
                        resp = new LanguageResponseData(Locale->GetDefault()->GetLanguage());
                        SendTerminalResponse(cmdParams.mCmdDet, ResultCode.OK, FALSE, 0, resp);
                        break;
                    default:
                        SendTerminalResponse(cmdParams.mCmdDet, ResultCode.OK, FALSE, 0, NULL);
                }
                // No need to start STK app here.
                return;
            case LAUNCH_BROWSER:
                If ((((LaunchBrowserParams) cmdParams).mConfirmMsg.text != NULL)
                        && (((LaunchBrowserParams) cmdParams).mConfirmMsg.text->Equals(STK_DEFAULT))) {
                    message = mContext->GetText(R::string::launchBrowserDefault);
                    ((LaunchBrowserParams) cmdParams).mConfirmMsg.text = message->ToString();
                }
                break;
            case SELECT_ITEM:
            case GET_INPUT:
            case GET_INKEY:
                break;
            case SEND_DTMF:
            case SEND_SMS:
                // Samsung STK
                If (cmdParams instanceof SendSMSParams) {
                    HandleProactiveCommandSendSMS((SendSMSParams) cmdParams);
                }
                // Fall through
            case SEND_SS:
            case SEND_USSD:
                // Samsung STK
                If (cmdParams instanceof SendUSSDParams) {
                    HandleProactiveCommandSendUSSD((SendUSSDParams) cmdParams);
                }
                If ((((DisplayTextParams)cmdParams).mTextMsg.text != NULL)
                        && (((DisplayTextParams)cmdParams).mTextMsg.text->Equals(STK_DEFAULT))) {
                    message = mContext->GetText(R::string::sending);
                    ((DisplayTextParams)cmdParams).mTextMsg.text = message->ToString();
                }
                break;
            case PLAY_TONE:
                break;
            case SET_UP_CALL:
                If ((((CallSetupParams) cmdParams).mConfirmMsg.text != NULL)
                        && (((CallSetupParams) cmdParams).mConfirmMsg.text->Equals(STK_DEFAULT))) {
                    message = mContext->GetText(R::string::SetupCallDefault);
                    ((CallSetupParams) cmdParams).mConfirmMsg.text = message->ToString();
                }
                break;
            case OPEN_CHANNEL:
            case CLOSE_CHANNEL:
            case RECEIVE_DATA:
            case SEND_DATA:
                BIPClientParams cmd = (BIPClientParams) cmdParams;
                /* Per 3GPP specification 102.223,
                 * if the alpha identifier is not provided by the UICC,
                 * the terminal MAY give information to the user
                 * noAlphaUsrCnf defines if you need to show user confirmation or not
                 */
                Boolean noAlphaUsrCnf = FALSE;
                try {
                    noAlphaUsrCnf = mContext->GetResources()->GetBoolean(
                            R.bool.config_stkNoAlphaUsrCnf);
                } Catch (NotFoundException e) {
                    noAlphaUsrCnf = FALSE;
                }
                If ((cmd.mTextMsg.text == NULL) && (cmd.mHasAlphaId || noAlphaUsrCnf)) {
                    CatLogger::D(this, "cmd " + cmdParams->GetCommandType() + " with NULL alpha id");
                    // If alpha length is zero, we just respond with OK.
                    If (isProactiveCmd) {
                        SendTerminalResponse(cmdParams.mCmdDet, ResultCode.OK, FALSE, 0, NULL);
                    } else If (cmdParams->GetCommandType() == CommandType.OPEN_CHANNEL) {
                        mCmdIf->HandleCallSetupRequestFromSim(TRUE, NULL);
                    }
                    return;
                }
                // Respond with permanent failure to avoid retry if STK app is not present.
                If (!mStkAppInstalled) {
                    CatLogger::D(this, "No STK application found.");
                    If (isProactiveCmd) {
                        SendTerminalResponse(cmdParams.mCmdDet,
                                             ResultCode.BEYOND_TERMINAL_CAPABILITY,
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
                If (isProactiveCmd &&
                    ((cmdParams->GetCommandType() == CommandType.CLOSE_CHANNEL) ||
                     (cmdParams->GetCommandType() == CommandType.RECEIVE_DATA) ||
                     (cmdParams->GetCommandType() == CommandType.SEND_DATA))) {
                    SendTerminalResponse(cmdParams.mCmdDet, ResultCode.OK, FALSE, 0, NULL);
                }
                break;
            case ACTIVATE:
                // TO DO: Retrieve the target of the ACTIVATE cmd from the cmd.
                // Target : '01' = UICC-CFL interface according to TS 102 613 [39];
                //          '00' and '02' to 'FF' = RFU (Reserved for Future Use).
                resultCode = ResultCode.OK;
                SendTerminalResponse(cmdParams.mCmdDet, resultCode, FALSE, 0 ,NULL);
                break;
            default:
                CatLogger::D(this, "Unsupported command");
                return;
        }
        mCurrntCmd = cmdMsg;
        BroadcastCatCmdIntent(cmdMsg);
    }


    private void BroadcastCatCmdIntent(CatCmdMessage cmdMsg) {
        Intent intent = new Intent(AppInterface.CAT_CMD_ACTION);
        intent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
        intent->PutExtra("STK CMD", cmdMsg);
        intent->PutExtra("SLOT_ID", mSlotId);
        CatLogger::D(this, "Sending CmdMsg: " + cmdMsg+ " on slotid:" + mSlotId);
        mContext->SendBroadcast(intent, AppInterface.STK_PERMISSION);
    }

    /**
     * Handles RIL_UNSOL_STK_SESSION_END unsolicited command from RIL.
     *
     */
    private void HandleSessionEnd() {
        CatLogger::D(this, "SESSION END on "+ mSlotId);

        mCurrntCmd = mMenuCmd;
        Intent intent = new Intent(AppInterface.CAT_SESSION_END_ACTION);
        intent->PutExtra("SLOT_ID", mSlotId);
        intent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
        mContext->SendBroadcast(intent, AppInterface.STK_PERMISSION);
    }


    private void SendTerminalResponse(CommandDetails cmdDet,
            ResultCode resultCode, Boolean includeAdditionalInfo,
            Int32 additionalInfo, ResponseData resp) {

        If (cmdDet == NULL) {
            return;
        }
        ByteArrayOutputStream buf = new ByteArrayOutputStream();

        Input cmdInput = NULL;
        If (mCurrntCmd != NULL) {
            cmdInput = mCurrntCmd->GeInput();
        }

        // command details
        Int32 tag = ComprehensionTlvTag.COMMAND_DETAILS->Value();
        If (cmdDet.compRequired) {
            tag |= 0x80;
        }
        buf->Write(tag);
        buf->Write(0x03); // length
        buf->Write(cmdDet.commandNumber);
        buf->Write(cmdDet.typeOfCommand);
        buf->Write(cmdDet.commandQualifier);

        // device identities
        // According to TS102.223/TS31.111 section 6.8 Structure of
        // TERMINAL RESPONSE, "For all SIMPLE-TLV objects with Min=N,
        // the ME should set the CR(comprehension required) flag to
        // comprehension not required.(CR=0)"
        // Since DEVICE_IDENTITIES and DURATION TLVs have Min=N,
        // the CR flag is not set.
        tag = ComprehensionTlvTag.DEVICE_IDENTITIES->Value();
        buf->Write(tag);
        buf->Write(0x02); // length
        buf->Write(DEV_ID_TERMINAL); // source device id
        buf->Write(DEV_ID_UICC); // destination device id

        // result
        tag = ComprehensionTlvTag.RESULT->Value();
        If (cmdDet.compRequired) {
            tag |= 0x80;
        }
        buf->Write(tag);
        Int32 length = includeAdditionalInfo ? 2 : 1;
        buf->Write(length);
        buf->Write(resultCode->Value());

        // additional info
        If (includeAdditionalInfo) {
            buf->Write(additionalInfo);
        }

        // Fill optional data for each corresponding command
        If (resp != NULL) {
            resp->Format(buf);
        } else {
            EncodeOptionalTags(cmdDet, resultCode, cmdInput, buf);
        }

        Byte[] rawData = buf->ToByteArray();
        String hexString = IccUtils->BytesToHexString(rawData);
        If (DBG) {
            CatLogger::D(this, "TERMINAL RESPONSE: " + hexString);
        }

        mCmdIf->SendTerminalResponse(hexString, NULL);
    }

    private void EncodeOptionalTags(CommandDetails cmdDet,
            ResultCode resultCode, Input cmdInput, ByteArrayOutputStream buf) {
        CommandType cmdType = AppInterface.CommandType->FromInt(cmdDet.typeOfCommand);
        If (cmdType != NULL) {
            Switch (cmdType) {
                case GET_INKEY:
                    // ETSI TS 102 384,27.22.4.2.8.4.2.
                    // If it is a response for GET_INKEY command and the response timeout
                    // occured, then add DURATION TLV for variable timeout case.
                    If ((resultCode->Value() == ResultCode.NO_RESPONSE_FROM_USER->Value()) &&
                        (cmdInput != NULL) && (cmdInput.duration != NULL)) {
                        GetInKeyResponse(buf, cmdInput);
                    }
                    break;
                case PROVIDE_LOCAL_INFORMATION:
                    If ((cmdDet.commandQualifier == CommandParamsFactory.LANGUAGE_SETTING) &&
                        (resultCode->Value() == ResultCode.OK->Value())) {
                        GetPliResponse(buf);
                    }
                    break;
                default:
                    CatLogger::D(this, "EncodeOptionalTags() Unsupported Cmd details=" + cmdDet);
                    break;
            }
        } else {
            CatLogger::D(this, "EncodeOptionalTags() bad Cmd details=" + cmdDet);
        }
    }

    private void GetInKeyResponse(ByteArrayOutputStream buf, Input cmdInput) {
        Int32 tag = ComprehensionTlvTag.DURATION->Value();

        buf->Write(tag);
        buf->Write(0x02); // length
        buf->Write(cmdInput.duration.timeUnit.SECOND->Value()); // Time (Unit,Seconds)
        buf->Write(cmdInput.duration.timeInterval); // Time Duration
    }

    private void GetPliResponse(ByteArrayOutputStream buf) {

        // Locale Language Setting
        String lang = SystemProperties->Get("persist.sys.language");

        If (lang != NULL) {
            // tag
            Int32 tag = ComprehensionTlvTag.LANGUAGE->Value();
            buf->Write(tag);
            ResponseData->WriteLength(buf, lang->Length());
            buf->Write(lang->GetBytes(), 0, lang->Length());
        }
    }

    private void SendMenuSelection(Int32 menuId, Boolean helpRequired) {

        ByteArrayOutputStream buf = new ByteArrayOutputStream();

        // tag
        Int32 tag = BerTlv.BER_MENU_SELECTION_TAG;
        buf->Write(tag);

        // length
        buf->Write(0x00); // place holder

        // device identities
        tag = 0x80 | ComprehensionTlvTag.DEVICE_IDENTITIES->Value();
        buf->Write(tag);
        buf->Write(0x02); // length
        buf->Write(DEV_ID_KEYPAD); // source device id
        buf->Write(DEV_ID_UICC); // destination device id

        // item identifier
        tag = 0x80 | ComprehensionTlvTag.ITEM_ID->Value();
        buf->Write(tag);
        buf->Write(0x01); // length
        buf->Write(menuId); // menu identifier chosen

        // help request
        If (helpRequired) {
            tag = ComprehensionTlvTag.HELP_REQUEST->Value();
            buf->Write(tag);
            buf->Write(0x00); // length
        }

        Byte[] rawData = buf->ToByteArray();

        // write real length
        Int32 len = rawData.length - 2; // Minus (tag + length)
        rawData[1] = (Byte) len;

        String hexString = IccUtils->BytesToHexString(rawData);

        mCmdIf->SendEnvelope(hexString, NULL);
    }

    private void EventDownload(Int32 event, Int32 sourceId, Int32 destinationId,
            Byte[] additionalInfo, Boolean oneShot) {

        ByteArrayOutputStream buf = new ByteArrayOutputStream();

        // tag
        Int32 tag = BerTlv.BER_EVENT_DOWNLOAD_TAG;
        buf->Write(tag);

        // length
        buf->Write(0x00); // place holder, assume length < 128.

        // event list
        tag = 0x80 | ComprehensionTlvTag.EVENT_LIST->Value();
        buf->Write(tag);
        buf->Write(0x01); // length
        buf->Write(event); // event value

        // device identities
        tag = 0x80 | ComprehensionTlvTag.DEVICE_IDENTITIES->Value();
        buf->Write(tag);
        buf->Write(0x02); // length
        buf->Write(sourceId); // source device id
        buf->Write(destinationId); // destination device id

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
        Switch (event) {
            case IDLE_SCREEN_AVAILABLE_EVENT:
                CatLogger::D(this, " Sending Idle Screen Available event download to ICC");
                break;
            case LANGUAGE_SELECTION_EVENT:
                CatLogger::D(this, " Sending Language Selection event download to ICC");
                tag = 0x80 | ComprehensionTlvTag.LANGUAGE->Value();
                buf->Write(tag);
                // Language length should be 2 Byte
                buf->Write(0x02);
                break;
            case HCI_CONNECTIVITY_EVENT:
                CatLogger::D(this, " Sending HCI Connectivity event download to ICC");
                break;
            default:
                break;
        }

        // additional information
        If (additionalInfo != NULL) {
            For (Byte b : additionalInfo) {
                buf->Write(b);
            }
        }

        Byte[] rawData = buf->ToByteArray();

        // write real length
        Int32 len = rawData.length - 2; // Minus (tag + length)
        rawData[1] = (Byte) len;

        String hexString = IccUtils->BytesToHexString(rawData);

        CatLogger::D(this, "ENVELOPE COMMAND: " + hexString);

        mCmdIf->SendEnvelope(hexString, NULL);
    }

    //@Override
    CARAPI HandleMessage(Message msg) {
        CatLogger::D(this, "handleMessage[" + msg.what + "]");

        Switch (msg.what) {
        case MSG_ID_SESSION_END:
        case MSG_ID_PROACTIVE_COMMAND:
        case MSG_ID_EVENT_NOTIFY:
        case MSG_ID_REFRESH:
            CatLogger::D(this, "ril message arrived,slotid:" + mSlotId);
            String data = NULL;
            If (msg.obj != NULL) {
                AsyncResult ar = (AsyncResult) msg.obj;
                If (ar != NULL && ar.result != NULL) {
                    try {
                        data = (String) ar.result;
                    } Catch (ClassCastException e) {
                        break;
                    }
                }
            }
            mMsgDecoder->SendStartDecodingMessageParams(new RilMessage(msg.what, data));
            break;
        case MSG_ID_CALL_SETUP:
            mMsgDecoder->SendStartDecodingMessageParams(new RilMessage(msg.what, NULL));
            break;
        case MSG_ID_RIL_MSG_DECODED:
            HandleRilMsg((RilMessage) msg.obj);
            break;
        case MSG_ID_RESPONSE:
            HandleCmdResponse((CatResponseMessage) msg.obj);
            break;
        case MSG_ID_ICC_CHANGED:
            CatLogger::D(this, "MSG_ID_ICC_CHANGED");
            UpdateIccAvailability();
            break;
        case MSG_ID_ICC_REFRESH:
            If (msg.obj != NULL) {
                AsyncResult ar = (AsyncResult) msg.obj;
                If (ar != NULL && ar.result != NULL) {
                    BroadcastCardStateAndIccRefreshResp(CardState.CARDSTATE_PRESENT,
                                  (IccRefreshResponse) ar.result);
                } else {
                    CatLogger::D(this,"Icc REFRESH with exception: " + ar.exception);
                }
            } else {
                CatLogger::D(this, "IccRefresh Message is NULL");
            }
            break;
        case MSG_ID_ALPHA_NOTIFY:
            CatLogger::D(this, "Received CAT CC Alpha message from card");
            If (msg.obj != NULL) {
                AsyncResult ar = (AsyncResult) msg.obj;
                If (ar != NULL && ar.result != NULL) {
                    BroadcastAlphaMessage((String)ar.result);
                } else {
                    CatLogger::D(this, "CAT Alpha message: ar.result is NULL");
                }
            } else {
                CatLogger::D(this, "CAT Alpha message: msg.obj is NULL");
            }
            break;
        case MSG_ID_TIMEOUT: // Should only be called for Samsung STK
            If (mTimeoutDest == WAITING_SMS_RESULT) {
                CatLogger::D(this, "SMS SEND TIMEOUT");
                If (CallControlResult->FromInt(mCallControlResultCode) ==
                        CallControlResult.CALL_CONTROL_NOT_ALLOWED)
                    SendTerminalResponse(mCurrntCmd.mCmdDet,
                            ResultCode.USIM_CALL_CONTROL_PERMANENT, TRUE, 1, NULL);
                else
                    SendTerminalResponse(mCurrntCmd.mCmdDet,
                            ResultCode.TERMINAL_CRNTLY_UNABLE_TO_PROCESS, FALSE, 0, NULL);
                break;
            }
            break;
        case MSG_ID_SEND_SMS_RESULT: // Samsung STK SEND_SMS
            If (mContext->GetResources().
                        GetBoolean(R.bool.config_samsung_stk)) {
                Int32[] sendResult;
                AsyncResult ar;
                CatLogger::D(this, "handleMsg : MSG_ID_SEND_SMS_RESULT");
                CancelTimeOut();
                CatLogger::D(this, "The Msg ID data:" + msg.what);
                ar = (AsyncResult) msg.obj;
                If (ar == NULL || ar.result == NULL || mCurrntCmd == NULL || mCurrntCmd.mCmdDet == NULL)
                    break;
                sendResult = (Int32[]) ar.result;
                If (sendResult.length == 0)
                    break;
                Switch (sendResult[0]) {
                    default:
                        CatLogger::D(this, "SMS SEND GENERIC FAIL");
                        If (CallControlResult->FromInt(mCallControlResultCode) ==
                                CallControlResult.CALL_CONTROL_NOT_ALLOWED)
                            SendTerminalResponse(mCurrntCmd.mCmdDet,
                                    ResultCode.USIM_CALL_CONTROL_PERMANENT, TRUE, 1, NULL);
                        else
                            SendTerminalResponse(mCurrntCmd.mCmdDet,
                                    ResultCode.TERMINAL_CRNTLY_UNABLE_TO_PROCESS, FALSE, 0, NULL);
                        break;
                    case SMS_SEND_OK: // '\0'
                        CatLogger::D(this, "SMS SEND OK");
                        If (CallControlResult->FromInt(mCallControlResultCode) ==
                                CallControlResult.CALL_CONTROL_NOT_ALLOWED)
                            SendTerminalResponse(mCurrntCmd.mCmdDet,
                                    ResultCode.USIM_CALL_CONTROL_PERMANENT, TRUE, 1, NULL);
                        else
                            SendTerminalResponse(mCurrntCmd.mCmdDet, ResultCode.OK, FALSE, 0, NULL);
                        break;
                    case SMS_SEND_FAIL:
                        CatLogger::D(this, "SMS SEND FAIL - MEMORY NOT AVAILABLE");
                        If (CallControlResult->FromInt(mCallControlResultCode) ==
                                CallControlResult.CALL_CONTROL_NOT_ALLOWED)
                            SendTerminalResponse(mCurrntCmd.mCmdDet,
                                    ResultCode.USIM_CALL_CONTROL_PERMANENT, TRUE, 1, NULL);
                        else
                            SendTerminalResponse(mCurrntCmd.mCmdDet,
                                    ResultCode.TERMINAL_CRNTLY_UNABLE_TO_PROCESS, FALSE, 0, NULL);
                        break;
                    case SMS_SEND_RETRY:
                        CatLogger::D(this, "SMS SEND FAIL RETRY");
                        If (CallControlResult->FromInt(mCallControlResultCode) ==
                                CallControlResult.CALL_CONTROL_NOT_ALLOWED)
                            SendTerminalResponse(mCurrntCmd.mCmdDet,
                                    ResultCode.USIM_CALL_CONTROL_PERMANENT, TRUE, 1, NULL);
                        else
                            SendTerminalResponse(mCurrntCmd.mCmdDet,
                                    ResultCode.NETWORK_CRNTLY_UNABLE_TO_PROCESS, FALSE, 0, NULL);
                        break;
                    }
            }
            break;
        default:
            throw new AssertionError("Unrecognized CAT command: " + msg.what);
        }
    }

    /**
     ** This function sends a CARD Status (ABSENT, PRESENT, REFRESH) to STK_APP.
     ** This is triggered during ICC_REFRESH or CARD STATE changes. In case
     ** REFRESH, additional information is sent in 'refresh_result'
     **
     **/
    private void  BroadcastCardStateAndIccRefreshResp(CardState cardState,
            IccRefreshResponse iccRefreshState) {
        Intent intent = new Intent(AppInterface.CAT_ICC_STATUS_CHANGE);
        intent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
        Boolean cardPresent = (cardState == CardState.CARDSTATE_PRESENT);

        If (iccRefreshState != NULL) {
            //This case is when MSG_ID_ICC_REFRESH is received.
            intent->PutExtra(AppInterface.REFRESH_RESULT, iccRefreshState.refreshResult);
            CatLogger::D(this, "Sending IccResult with Result: "
                    + iccRefreshState.refreshResult);
        }

        // This sends an intent with CARD_ABSENT (0 - FALSE) /CARD_PRESENT (1 - TRUE).
        intent->PutExtra(AppInterface.CARD_STATUS, cardPresent);
        intent->PutExtra("SLOT_ID", mSlotId);
        CatLogger::D(this, "Sending Card Status: "
                + cardState + " " + "cardPresent: " + cardPresent);
        mContext->SendBroadcast(intent, AppInterface.STK_PERMISSION);
    }

    private void BroadcastAlphaMessage(String alphaString) {
        CatLogger::D(this, "Broadcasting CAT Alpha message from card: " + alphaString);
        Intent intent = new Intent(AppInterface.CAT_ALPHA_NOTIFY_ACTION);
        intent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
        intent->PutExtra(AppInterface.ALPHA_STRING, alphaString);
        intent->PutExtra("SLOT_ID", mSlotId);
        mContext->SendBroadcast(intent, AppInterface.STK_PERMISSION);
    }

    //@Override
    public synchronized void OnCmdResponse(CatResponseMessage resMsg) {
        If (resMsg == NULL) {
            return;
        }
        // queue a response message.
        Message msg = ObtainMessage(MSG_ID_RESPONSE, resMsg);
        msg->SendToTarget();
    }

    private Boolean ValidateResponse(CatResponseMessage resMsg) {
        Boolean validResponse = FALSE;
        If ((resMsg.mCmdDet.typeOfCommand == CommandType.SET_UP_EVENT_LIST->Value())
                || (resMsg.mCmdDet.typeOfCommand == CommandType.SET_UP_MENU->Value())) {
            CatLogger::D(this, "CmdType: " + resMsg.mCmdDet.typeOfCommand);
            validResponse = TRUE;
        } else If (mCurrntCmd != NULL) {
            validResponse = resMsg.mCmdDet->CompareTo(mCurrntCmd.mCmdDet);
            CatLogger::D(this, "isResponse for last valid cmd: " + validResponse);
        }
        return validResponse;
    }

    private Boolean RemoveMenu(Menu menu) {
        try {
            If (menu.items->Size() == 1 && menu.items->Get(0) == NULL) {
                return TRUE;
            }
        } Catch (NullPointerException e) {
            CatLogger::D(this, "Unable to get Menu's items size");
            return TRUE;
        }
        return FALSE;
    }

    private void HandleCmdResponse(CatResponseMessage resMsg) {
        // Make sure the response details match the last valid command. An invalid
        // response is a one that doesn't have a corresponding proactive command
        // and sending it can "confuse" the baseband/ril.
        // One reason for out of order responses can be UI glitches. For example,
        // if the application launch an activity, and that activity is stored
        // by the framework inside the history stack. That activity will be
        // available for relaunch using the latest application dialog
        // (Int64 press on the home button). Relaunching that activity can send
        // the same command's result again to the CatService and can cause it to
        // get out of sync with the SIM. This can happen in case of
        // non-interactive type Setup Event List and SETUP_MENU proactive commands.
        // Stk framework would have already sent Terminal Response to Setup Event
        // List and SETUP_MENU proactive commands. After sometime Stk app will send
        // Envelope Command/Event Download. In which case, the response details doesn't
        // match with last valid Command (which are not related).
        // However, we should allow Stk framework to send the message to ICC.
        If (!ValidateResponse(resMsg)) {
            return;
        }
        ResponseData resp = NULL;
        Boolean helpRequired = FALSE;
        CommandDetails cmdDet = resMsg->GetCmdDetails();
        AppInterface.CommandType type = AppInterface.CommandType->FromInt(cmdDet.typeOfCommand);
        Switch (resMsg.mResCode) {
        case HELP_INFO_REQUIRED:
            helpRequired = TRUE;
            // fall through
        case OK:
        case PRFRMD_WITH_PARTIAL_COMPREHENSION:
        case PRFRMD_WITH_MISSING_INFO:
        case PRFRMD_WITH_ADDITIONAL_EFS_READ:
        case PRFRMD_ICON_NOT_DISPLAYED:
        case PRFRMD_MODIFIED_BY_NAA:
        case PRFRMD_LIMITED_SERVICE:
        case PRFRMD_WITH_MODIFICATION:
        case PRFRMD_NAA_NOT_ACTIVE:
        case PRFRMD_TONE_NOT_PLAYED:
        case LAUNCH_BROWSER_ERROR:
        case TERMINAL_CRNTLY_UNABLE_TO_PROCESS:
            Switch (type) {
            case SET_UP_MENU:
                helpRequired = resMsg.mResCode == ResultCode.HELP_INFO_REQUIRED;
                SendMenuSelection(resMsg.mUsersMenuSelection, helpRequired);
                return;
            case SELECT_ITEM:
                resp = new SelectItemResponseData(resMsg.mUsersMenuSelection);
                break;
            case GET_INPUT:
            case GET_INKEY:
                Input input = mCurrntCmd->GeInput();
                If (!input.yesNo) {
                    // when help is requested there is no need to send the text
                    // string object.
                    If (!helpRequired) {
                        resp = new GetInkeyInputResponseData(resMsg.mUsersInput,
                                input.ucs2, input.packed);
                    }
                } else {
                    resp = new GetInkeyInputResponseData(
                            resMsg.mUsersYesNoSelection);
                }
                break;
            case DISPLAY_TEXT:
                If (resMsg.mResCode == ResultCode.TERMINAL_CRNTLY_UNABLE_TO_PROCESS) {
                    // For screenbusy case there will be addtional information in the terminal
                    // response. And the value of the additional information Byte is 0x01.
                    resMsg->SetAdditionalInfo(0x01);
                } else {
                    resMsg.mIncludeAdditionalInfo = FALSE;
                    resMsg.mAdditionalInfo = 0;
                }
                break;
            case LAUNCH_BROWSER:
                break;
            // 3GPP TS.102.223: Open Channel alpha confirmation should not send TR
            case OPEN_CHANNEL:
            case SET_UP_CALL:
                mCmdIf->HandleCallSetupRequestFromSim(resMsg.mUsersConfirm, NULL);
                // No need to send terminal response for SET UP CALL. The user's
                // confirmation result is send back using a dedicated ril message
                // invoked by the CommandInterface call above.
                mCurrntCmd = NULL;
                return;
            case SET_UP_EVENT_LIST:
                If (IDLE_SCREEN_AVAILABLE_EVENT == resMsg.mEventValue) {
                    EventDownload(resMsg.mEventValue, DEV_ID_DISPLAY, DEV_ID_UICC,
                            resMsg.mAddedInfo, FALSE);
                } else {
                    EventDownload(resMsg.mEventValue, DEV_ID_TERMINAL, DEV_ID_UICC,
                            resMsg.mAddedInfo, FALSE);
                }
                // No need to send the terminal response after event download.
                return;
            default:
                break;
            }
            break;
        case BACKWARD_MOVE_BY_USER:
        case USER_NOT_ACCEPT:
            // if the user dismissed the alert dialog for a
            // setup call/open channel, consider that as the user
            // rejecting the call. Use dedicated API for this, rather than
            // sending a terminal response.
            If (type == CommandType.SET_UP_CALL || type == CommandType.OPEN_CHANNEL) {
                mCmdIf->HandleCallSetupRequestFromSim(FALSE, NULL);
                mCurrntCmd = NULL;
                return;
            } else {
                resp = NULL;
            }
            break;
        case NO_RESPONSE_FROM_USER:
            // No need to send terminal response for SET UP CALL on user timeout.
            If (type == CommandType.SET_UP_CALL) {
                mCurrntCmd = NULL;
                return;
            }
        case UICC_SESSION_TERM_BY_USER:
            resp = NULL;
            break;
        default:
            return;
        }
        SendTerminalResponse(cmdDet, resMsg.mResCode, resMsg.mIncludeAdditionalInfo,
                resMsg.mAdditionalInfo, resp);
        mCurrntCmd = NULL;
    }

    private Boolean IsStkAppInstalled() {
        Intent intent = new Intent(AppInterface.CAT_CMD_ACTION);
        PackageManager pm = mContext->GetPackageManager();
        List<ResolveInfo> broadcastReceivers =
                            pm->QueryBroadcastReceivers(intent, PackageManager.GET_META_DATA);
        Int32 numReceiver = broadcastReceivers == NULL ? 0 : broadcastReceivers->Size();

        Return (numReceiver > 0);
    }

    void UpdateIccAvailability() {
        If (NULL == mUiccController) {
            return;
        }

        CardState newState = CardState.CARDSTATE_ABSENT;
        UiccCard newCard = mUiccController->GetUiccCard(mSlotId);
        If (newCard != NULL) {
            newState = newCard->GetCardState();
        }
        CardState oldState = mCardState;
        mCardState = newState;
        CatLogger::D(this,"New Card State = " + newState + " " + "Old Card State = " + oldState);
        If (oldState == CardState.CARDSTATE_PRESENT &&
                newState != CardState.CARDSTATE_PRESENT) {
            BroadcastCardStateAndIccRefreshResp(newState, NULL);
        } else If (oldState != CardState.CARDSTATE_PRESENT &&
                newState == CardState.CARDSTATE_PRESENT) {
            // Card moved to PRESENT STATE.
            mCmdIf->ReportStkServiceIsRunning(NULL);
        }
    }

    /**
     * Samsung STK SEND_SMS
     * @param cmdPar
     */
    private void HandleProactiveCommandSendSMS(SendSMSParams cmdPar) {
        CatLogger::D(this, "The smscaddress is: " + cmdPar.smscAddress);
        CatLogger::D(this, "The SMS tpdu is: " + cmdPar.pdu);
        mCmdIf->SendSMS(cmdPar.smscAddress, cmdPar.pdu, NULL);
        StartTimeOut(WAITING_SMS_RESULT, WAITING_SMS_RESULT_TIME);
    }

    /**
     * Samsung STK SEND_USSD
     * @param cmdPar
     */
    private void HandleProactiveCommandSendUSSD(SendUSSDParams cmdPar) {
        CatLogger::D(this, "The USSD is: " + cmdPar.ussdString);
        mCmdIf->SendUSSD(cmdPar.ussdString, NULL);
        // Sent USSD, let framework handle the rest
    }

    private void CancelTimeOut() {
        RemoveMessages(MSG_ID_TIMEOUT);
        mTimeoutDest = 0;
    }

    private void StartTimeOut(Int32 timeout, Int32 delay) {
        CancelTimeOut();
        mTimeoutDest = timeout;
        SendMessageDelayed(ObtainMessage(MSG_ID_TIMEOUT), delay);
    }
}
