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

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;

using Elastos::Droid::Internal::Telephony::IGsmAlphabet;
using Elastos::Droid::Internal::Telephony::Uicc::IIccFileHandler;

using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using static com::Android::Internal::Telephony::Cat::CatCmdMessage::I
                   SetupEventListConstants.USER_ACTIVITY_EVENT;
using static com::Android::Internal::Telephony::Cat::CatCmdMessage::I
                   SetupEventListConstants.IDLE_SCREEN_AVAILABLE_EVENT;
using static com::Android::Internal::Telephony::Cat::CatCmdMessage::I
                   SetupEventListConstants.LANGUAGE_SELECTION_EVENT;
using static com::Android::Internal::Telephony::Cat::CatCmdMessage::I
                   SetupEventListConstants.BROWSER_TERMINATION_EVENT;
using static com::Android::Internal::Telephony::Cat::CatCmdMessage::I
                   SetupEventListConstants.BROWSING_STATUS_EVENT;
using static com::Android::Internal::Telephony::Cat::CatCmdMessage::I
                   SetupEventListConstants.HCI_CONNECTIVITY_EVENT;
/**
 * Factory class, used for decoding raw Byte arrays, received from baseband,
 * into a CommandParams object.
 *
 */
class CommandParamsFactory extends Handler {
    private static CommandParamsFactory sInstance = NULL;
    private IconLoader mIconLoader;
    private CommandParams mCmdParams = NULL;
    private Int32 mIconLoadState = LOAD_NO_ICON;
    private RilMessageDecoder mCaller = NULL;
    private Boolean mloadIcon = FALSE;

    // constants
    static const Int32 MSG_ID_LOAD_ICON_DONE = 1;

    // loading icons state parameters.
    static const Int32 LOAD_NO_ICON           = 0;
    static const Int32 LOAD_SINGLE_ICON       = 1;
    static const Int32 LOAD_MULTI_ICONS       = 2;

    // Command Qualifier values for PLI command
    static const Int32 DTTZ_SETTING                           = 0x03;
    static const Int32 LANGUAGE_SETTING                       = 0x04;

    // As per TS 102.223 Annex C, Structure of CAT communications,
    // the APDU length can be max 255 bytes. This leaves only 239 bytes for user
    // input string. CMD details TLV + Device IDs TLV + Result TLV + Other
    // details of TextString TLV not including user input take 16 bytes.
    //
    // If UCS2 encoding is used, maximum 118 UCS2 chars can be encoded in 238 bytes.
    // Each UCS2 Char32 takes 2 bytes. Byte Order Mask(BOM), 0xFEFF takes 2 bytes.
    //
    // If GSM 7 bit Default(use 8 bits to represent a 7 bit Char32) format is used,
    // maximum 239 chars can be encoded in 239 bytes since each Char32 takes 1 Byte.
    //
    // No issues for GSM 7 bit packed format encoding.

    private static const Int32 MAX_GSM7_DEFAULT_CHARS = 239;
    private static const Int32 MAX_UCS2_CHARS = 118;

    static synchronized CommandParamsFactory GetInstance(RilMessageDecoder caller,
            IccFileHandler fh) {
        If (sInstance != NULL) {
            return sInstance;
        }
        If (fh != NULL) {
            return new CommandParamsFactory(caller, fh);
        }
        return NULL;
    }

    private CommandParamsFactory(RilMessageDecoder caller, IccFileHandler fh) {
        mCaller = caller;
        mIconLoader = IconLoader->GetInstance(this, fh);
    }

    private CommandDetails ProcessCommandDetails(List<ComprehensionTlv> ctlvs) {
        CommandDetails cmdDet = NULL;

        If (ctlvs != NULL) {
            // Search for the Command Details object.
            ComprehensionTlv ctlvCmdDet = SearchForTag(
                    ComprehensionTlvTag.COMMAND_DETAILS, ctlvs);
            If (ctlvCmdDet != NULL) {
                try {
                    cmdDet = ValueParser->RetrieveCommandDetails(ctlvCmdDet);
                } Catch (ResultException e) {
                    CatLogger::D(this,
                            "processCommandDetails: Failed to procees command details e=" + e);
                }
            }
        }
        return cmdDet;
    }

    void Make(BerTlv berTlv) {
        If (berTlv == NULL) {
            return;
        }
        // reset global state parameters.
        mCmdParams = NULL;
        mIconLoadState = LOAD_NO_ICON;
        // only proactive command messages are processed.
        If (berTlv->GetTag() != BerTlv.BER_PROACTIVE_COMMAND_TAG) {
            SendCmdParams(ResultCode.CMD_TYPE_NOT_UNDERSTOOD);
            return;
        }
        Boolean cmdPending = FALSE;
        List<ComprehensionTlv> ctlvs = berTlv->GetComprehensionTlvs();
        // process command dtails from the tlv list.
        CommandDetails cmdDet = ProcessCommandDetails(ctlvs);
        If (cmdDet == NULL) {
            SendCmdParams(ResultCode.CMD_TYPE_NOT_UNDERSTOOD);
            return;
        }

        // extract command type enumeration from the raw value stored inside
        // the Command Details object.
        AppInterface.CommandType cmdType = AppInterface.CommandType
                .FromInt(cmdDet.typeOfCommand);
        If (cmdType == NULL) {
            // This PROACTIVE COMMAND is presently not handled. Hence set
            // result code as BEYOND_TERMINAL_CAPABILITY in TR.
            mCmdParams = new CommandParams(cmdDet);
            SendCmdParams(ResultCode.BEYOND_TERMINAL_CAPABILITY);
            return;
        }

        // proactive command length is incorrect.
        If (!berTlv->IsLengthValid()) {
            mCmdParams = new CommandParams(cmdDet);
            SendCmdParams(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
            return;
        }

        try {
            Switch (cmdType) {
            case SET_UP_MENU:
                cmdPending = ProcessSelectItem(cmdDet, ctlvs);
                break;
            case SELECT_ITEM:
                cmdPending = ProcessSelectItem(cmdDet, ctlvs);
                break;
            case DISPLAY_TEXT:
                cmdPending = ProcessDisplayText(cmdDet, ctlvs);
                break;
             case SET_UP_IDLE_MODE_TEXT:
                 cmdPending = ProcessSetUpIdleModeText(cmdDet, ctlvs);
                 break;
             case GET_INKEY:
                cmdPending = ProcessGetInkey(cmdDet, ctlvs);
                break;
             case GET_INPUT:
                 cmdPending = ProcessGetInput(cmdDet, ctlvs);
                 break;
             case SEND_DTMF:
             case SEND_SMS:
             case SEND_SS:
             case SEND_USSD:
                 cmdPending = ProcessEventNotify(cmdDet, ctlvs);
                 break;
             case GET_CHANNEL_STATUS:
             case SET_UP_CALL:
                 cmdPending = ProcessSetupCall(cmdDet, ctlvs);
                 break;
             case REFRESH:
                 cmdPending = ProcessEventNotify(cmdDet, ctlvs);
                 break;
             case LAUNCH_BROWSER:
                 cmdPending = ProcessLaunchBrowser(cmdDet, ctlvs);
                 break;
             case PLAY_TONE:
                cmdPending = ProcessPlayTone(cmdDet, ctlvs);
                break;
             case SET_UP_EVENT_LIST:
                 cmdPending = ProcessSetUpEventList(cmdDet, ctlvs);
                 break;
             case PROVIDE_LOCAL_INFORMATION:
                cmdPending = ProcessProvideLocalInfo(cmdDet, ctlvs);
                break;
             case OPEN_CHANNEL:
             case CLOSE_CHANNEL:
             case RECEIVE_DATA:
             case SEND_DATA:
                 cmdPending = ProcessBIPClient(cmdDet, ctlvs);
                 break;
            case ACTIVATE:
                cmdPending = ProcessActivate(cmdDet, ctlvs);
                break;
            default:
                // unsupported proactive commands
                mCmdParams = new CommandParams(cmdDet);
                SendCmdParams(ResultCode.BEYOND_TERMINAL_CAPABILITY);
                return;
            }
        } Catch (ResultException e) {
            CatLogger::D(this, "make: caught ResultException e=" + e);
            mCmdParams = new CommandParams(cmdDet);
            SendCmdParams(e->Result());
            return;
        }
        If (!cmdPending) {
            SendCmdParams(ResultCode.OK);
        }
    }

    //@Override
    CARAPI HandleMessage(Message msg) {
        Switch (msg.what) {
        case MSG_ID_LOAD_ICON_DONE:
            SendCmdParams(SetIcons(msg.obj));
            break;
        }
    }

    private ResultCode SetIcons(Object data) {
        Bitmap[] icons = NULL;
        Int32 iconIndex = 0;

        If (data == NULL) {
            If (mloadIcon) {
                CatLogger::D(this, "Optional Icon data is NULL");
                mCmdParams.mLoadIconFailed = TRUE;
                mloadIcon = FALSE;
                /** In case of icon load fail consider the
                 ** received proactive command as Valid (sending RESULT OK) */
                return ResultCode.OK;
            }
            return ResultCode.PRFRMD_ICON_NOT_DISPLAYED;
        }
        Switch(mIconLoadState) {
        case LOAD_SINGLE_ICON:
            mCmdParams->SetIcon((Bitmap) data);
            break;
        case LOAD_MULTI_ICONS:
            icons = (Bitmap[]) data;
            // set each item icon.
            For (Bitmap icon : icons) {
                mCmdParams->SetIcon(icon);
                If (icon == NULL && mloadIcon) {
                    CatLogger::D(this, "Optional Icon data is NULL while loading multi icons");
                    mCmdParams.mLoadIconFailed = TRUE;
                }
            }
            break;
        }
        return ResultCode.OK;
    }

    private void SendCmdParams(ResultCode resCode) {
        mCaller->SendMsgParamsDecoded(resCode, mCmdParams);
    }

    /**
     * Search for a COMPREHENSION-TLV object with the given tag from a list
     *
     * @param tag A tag to search for
     * @param ctlvs List of ComprehensionTlv objects used to search in
     *
     * @return A ComprehensionTlv object that has the tag value of {@code tag}.
     *         If no object is found with the tag, NULL is returned.
     */
    private ComprehensionTlv SearchForTag(ComprehensionTlvTag tag,
            List<ComprehensionTlv> ctlvs) {
        Iterator<ComprehensionTlv> iter = ctlvs->Iterator();
        return SearchForNextTag(tag, iter);
    }

    /**
     * Search for the next COMPREHENSION-TLV object with the given tag from a
     * list iterated by {@code iter}. {@code iter} points to the object next to
     * the found object when this method returns. Used for searching the same
     * list for similar tags, usually item id.
     *
     * @param tag A tag to search for
     * @param iter Iterator for ComprehensionTlv objects used for search
     *
     * @return A ComprehensionTlv object that has the tag value of {@code tag}.
     *         If no object is found with the tag, NULL is returned.
     */
    private ComprehensionTlv SearchForNextTag(ComprehensionTlvTag tag,
            Iterator<ComprehensionTlv> iter) {
        Int32 tagValue = tag->Value();
        While (iter->HasNext()) {
            ComprehensionTlv ctlv = iter->Next();
            If (ctlv->GetTag() == tagValue) {
                return ctlv;
            }
        }
        return NULL;
    }

    /**
     * Processes DISPLAY_TEXT proactive command from the SIM card.
     *
     * @param cmdDet Command Details container object.
     * @param ctlvs List of ComprehensionTlv objects following Command Details
     *        object and Device Identities object within the proactive command
     * @return TRUE if the command is processing is pending and additional
     *         asynchronous processing is required.
     * @throws ResultException
     */
    private Boolean ProcessDisplayText(CommandDetails cmdDet,
            List<ComprehensionTlv> ctlvs)
            throws ResultException {

        CatLogger::D(this, "process DisplayText");

        TextMessage textMsg = new TextMessage();
        IconId iconId = NULL;

        ComprehensionTlv ctlv = SearchForTag(ComprehensionTlvTag.TEXT_STRING,
                ctlvs);
        If (ctlv != NULL) {
            textMsg.text = ValueParser->RetrieveTextString(ctlv);
        }
        // If the tlv object doesn't exist or the it is a NULL object reply
        // with command not understood.
        If (textMsg.text == NULL) {
            throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
        }

        ctlv = SearchForTag(ComprehensionTlvTag.IMMEDIATE_RESPONSE, ctlvs);
        If (ctlv != NULL) {
            textMsg.responseNeeded = FALSE;
        }
        // parse icon identifier
        ctlv = SearchForTag(ComprehensionTlvTag.ICON_ID, ctlvs);
        If (ctlv != NULL) {
            iconId = ValueParser->RetrieveIconId(ctlv);
            textMsg.iconSelfExplanatory = iconId.selfExplanatory;
        }
        // parse tone duration
        ctlv = SearchForTag(ComprehensionTlvTag.DURATION, ctlvs);
        If (ctlv != NULL) {
            textMsg.duration = ValueParser->RetrieveDuration(ctlv);
        }

        // Parse command qualifier parameters.
        textMsg.isHighPriority = (cmdDet.commandQualifier & 0x01) != 0;
        textMsg.userClear = (cmdDet.commandQualifier & 0x80) != 0;

        mCmdParams = new DisplayTextParams(cmdDet, textMsg);

        If (iconId != NULL) {
            mloadIcon = TRUE;
            mIconLoadState = LOAD_SINGLE_ICON;
            mIconLoader->LoadIcon(iconId.recordNumber, this
                    .ObtainMessage(MSG_ID_LOAD_ICON_DONE));
            return TRUE;
        }
        return FALSE;
    }

    /**
     * Processes SET_UP_IDLE_MODE_TEXT proactive command from the SIM card.
     *
     * @param cmdDet Command Details container object.
     * @param ctlvs List of ComprehensionTlv objects following Command Details
     *        object and Device Identities object within the proactive command
     * @return TRUE if the command is processing is pending and additional
     *         asynchronous processing is required.
     * @throws ResultException
     */
    private Boolean ProcessSetUpIdleModeText(CommandDetails cmdDet,
            List<ComprehensionTlv> ctlvs) throws ResultException {

        CatLogger::D(this, "process SetUpIdleModeText");

        TextMessage textMsg = new TextMessage();
        IconId iconId = NULL;

        ComprehensionTlv ctlv = SearchForTag(ComprehensionTlvTag.TEXT_STRING,
                ctlvs);
        If (ctlv != NULL) {
            textMsg.text = ValueParser->RetrieveTextString(ctlv);
        }

        ctlv = SearchForTag(ComprehensionTlvTag.ICON_ID, ctlvs);
        If (ctlv != NULL) {
            iconId = ValueParser->RetrieveIconId(ctlv);
            textMsg.iconSelfExplanatory = iconId.selfExplanatory;
        }

        /*
         * If the tlv object doesn't contain text and the icon is not self
         * explanatory then reply with command not understood.
         */

        If (textMsg.text == NULL && iconId != NULL && !textMsg.iconSelfExplanatory) {
            throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
        }
        mCmdParams = new DisplayTextParams(cmdDet, textMsg);

        If (iconId != NULL) {
            mloadIcon = TRUE;
            mIconLoadState = LOAD_SINGLE_ICON;
            mIconLoader->LoadIcon(iconId.recordNumber, this
                    .ObtainMessage(MSG_ID_LOAD_ICON_DONE));
            return TRUE;
        }
        return FALSE;
    }

    /**
     * Processes GET_INKEY proactive command from the SIM card.
     *
     * @param cmdDet Command Details container object.
     * @param ctlvs List of ComprehensionTlv objects following Command Details
     *        object and Device Identities object within the proactive command
     * @return TRUE if the command is processing is pending and additional
     *         asynchronous processing is required.
     * @throws ResultException
     */
    private Boolean ProcessGetInkey(CommandDetails cmdDet,
            List<ComprehensionTlv> ctlvs) throws ResultException {

        CatLogger::D(this, "process GetInkey");

        Input input = new Input();
        IconId iconId = NULL;

        ComprehensionTlv ctlv = SearchForTag(ComprehensionTlvTag.TEXT_STRING,
                ctlvs);
        If (ctlv != NULL) {
            input.text = ValueParser->RetrieveTextString(ctlv);
        } else {
            throw new ResultException(ResultCode.REQUIRED_VALUES_MISSING);
        }
        // parse icon identifier
        ctlv = SearchForTag(ComprehensionTlvTag.ICON_ID, ctlvs);
        If (ctlv != NULL) {
            iconId = ValueParser->RetrieveIconId(ctlv);
        }

        // parse duration
        ctlv = SearchForTag(ComprehensionTlvTag.DURATION, ctlvs);
        If (ctlv != NULL) {
            input.duration = ValueParser->RetrieveDuration(ctlv);
        }

        input.minLen = 1;
        input.maxLen = 1;

        input.digitOnly = (cmdDet.commandQualifier & 0x01) == 0;
        input.ucs2 = (cmdDet.commandQualifier & 0x02) != 0;
        input.yesNo = (cmdDet.commandQualifier & 0x04) != 0;
        input.helpAvailable = (cmdDet.commandQualifier & 0x80) != 0;
        input.echo = TRUE;

        mCmdParams = new GetInputParams(cmdDet, input);

        If (iconId != NULL) {
            mloadIcon = TRUE;
            mIconLoadState = LOAD_SINGLE_ICON;
            mIconLoader->LoadIcon(iconId.recordNumber, this
                    .ObtainMessage(MSG_ID_LOAD_ICON_DONE));
            return TRUE;
        }
        return FALSE;
    }

    /**
     * Processes GET_INPUT proactive command from the SIM card.
     *
     * @param cmdDet Command Details container object.
     * @param ctlvs List of ComprehensionTlv objects following Command Details
     *        object and Device Identities object within the proactive command
     * @return TRUE if the command is processing is pending and additional
     *         asynchronous processing is required.
     * @throws ResultException
     */
    private Boolean ProcessGetInput(CommandDetails cmdDet,
            List<ComprehensionTlv> ctlvs) throws ResultException {

        CatLogger::D(this, "process GetInput");

        Input input = new Input();
        IconId iconId = NULL;

        ComprehensionTlv ctlv = SearchForTag(ComprehensionTlvTag.TEXT_STRING,
                ctlvs);
        If (ctlv != NULL) {
            input.text = ValueParser->RetrieveTextString(ctlv);
        } else {
            throw new ResultException(ResultCode.REQUIRED_VALUES_MISSING);
        }

        ctlv = SearchForTag(ComprehensionTlvTag.RESPONSE_LENGTH, ctlvs);
        If (ctlv != NULL) {
            try {
                Byte[] rawValue = ctlv->GetRawValue();
                Int32 valueIndex = ctlv->GetValueIndex();
                input.minLen = rawValue[valueIndex] & 0xff;
                input.maxLen = rawValue[valueIndex + 1] & 0xff;
            } Catch (IndexOutOfBoundsException e) {
                throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
            }
        } else {
            throw new ResultException(ResultCode.REQUIRED_VALUES_MISSING);
        }

        ctlv = SearchForTag(ComprehensionTlvTag.DEFAULT_TEXT, ctlvs);
        If (ctlv != NULL) {
            input.defaultText = ValueParser->RetrieveTextString(ctlv);
        }
        // parse icon identifier
        ctlv = SearchForTag(ComprehensionTlvTag.ICON_ID, ctlvs);
        If (ctlv != NULL) {
            iconId = ValueParser->RetrieveIconId(ctlv);
        }

        input.digitOnly = (cmdDet.commandQualifier & 0x01) == 0;
        input.ucs2 = (cmdDet.commandQualifier & 0x02) != 0;
        input.echo = (cmdDet.commandQualifier & 0x04) == 0;
        input.packed = (cmdDet.commandQualifier & 0x08) != 0;
        input.helpAvailable = (cmdDet.commandQualifier & 0x80) != 0;

        // Truncate the maxLen if it exceeds the max number of chars that can
        // be encoded. Limit depends on DCS in Command Qualifier.
        If (input.ucs2 && input.maxLen > MAX_UCS2_CHARS) {
            CatLogger::D(this, "UCS2: received maxLen = " + input.maxLen +
                  ", truncating to " + MAX_UCS2_CHARS);
            input.maxLen = MAX_UCS2_CHARS;
        } else If (!input.packed && input.maxLen > MAX_GSM7_DEFAULT_CHARS) {
            CatLogger::D(this, "GSM 7Bit Default: received maxLen = " + input.maxLen +
                  ", truncating to " + MAX_GSM7_DEFAULT_CHARS);
            input.maxLen = MAX_GSM7_DEFAULT_CHARS;
        }

        mCmdParams = new GetInputParams(cmdDet, input);

        If (iconId != NULL) {
            mloadIcon = TRUE;
            mIconLoadState = LOAD_SINGLE_ICON;
            mIconLoader->LoadIcon(iconId.recordNumber, this
                    .ObtainMessage(MSG_ID_LOAD_ICON_DONE));
            return TRUE;
        }
        return FALSE;
    }

    /**
     * Processes SELECT_ITEM proactive command from the SIM card.
     *
     * @param cmdDet Command Details container object.
     * @param ctlvs List of ComprehensionTlv objects following Command Details
     *        object and Device Identities object within the proactive command
     * @return TRUE if the command is processing is pending and additional
     *         asynchronous processing is required.
     * @throws ResultException
     */
    private Boolean ProcessSelectItem(CommandDetails cmdDet,
            List<ComprehensionTlv> ctlvs) throws ResultException {

        CatLogger::D(this, "process SelectItem");

        Menu menu = new Menu();
        IconId titleIconId = NULL;
        ItemsIconId itemsIconId = NULL;
        Iterator<ComprehensionTlv> iter = ctlvs->Iterator();

        ComprehensionTlv ctlv = SearchForTag(ComprehensionTlvTag.ALPHA_ID,
                ctlvs);
        If (ctlv != NULL) {
            menu.title = ValueParser->RetrieveAlphaId(ctlv);
        }

        While (TRUE) {
            ctlv = SearchForNextTag(ComprehensionTlvTag.ITEM, iter);
            If (ctlv != NULL) {
                menu.items->Add(ValueParser->RetrieveItem(ctlv));
            } else {
                break;
            }
        }

        // We must have at least one menu item.
        If (menu.items->Size() == 0) {
            throw new ResultException(ResultCode.REQUIRED_VALUES_MISSING);
        }

        ctlv = SearchForTag(ComprehensionTlvTag.ITEM_ID, ctlvs);
        If (ctlv != NULL) {
            // CAT items are listed 1...n while list start at 0, need to
            // subtract one.
            menu.defaultItem = ValueParser->RetrieveItemId(ctlv) - 1;
        }

        ctlv = SearchForTag(ComprehensionTlvTag.ICON_ID, ctlvs);
        If (ctlv != NULL) {
            mIconLoadState = LOAD_SINGLE_ICON;
            titleIconId = ValueParser->RetrieveIconId(ctlv);
            menu.titleIconSelfExplanatory = titleIconId.selfExplanatory;
        }

        ctlv = SearchForTag(ComprehensionTlvTag.ITEM_ICON_ID_LIST, ctlvs);
        If (ctlv != NULL) {
            mIconLoadState = LOAD_MULTI_ICONS;
            itemsIconId = ValueParser->RetrieveItemsIconId(ctlv);
            menu.itemsIconSelfExplanatory = itemsIconId.selfExplanatory;
        }

        Boolean presentTypeSpecified = (cmdDet.commandQualifier & 0x01) != 0;
        If (presentTypeSpecified) {
            If ((cmdDet.commandQualifier & 0x02) == 0) {
                menu.presentationType = PresentationType.DATA_VALUES;
            } else {
                menu.presentationType = PresentationType.NAVIGATION_OPTIONS;
            }
        }
        menu.softKeyPreferred = (cmdDet.commandQualifier & 0x04) != 0;
        menu.helpAvailable = (cmdDet.commandQualifier & 0x80) != 0;

        mCmdParams = new SelectItemParams(cmdDet, menu, titleIconId != NULL);

        // Load icons data if needed.
        Switch(mIconLoadState) {
        case LOAD_NO_ICON:
            return FALSE;
        case LOAD_SINGLE_ICON:
            mloadIcon = TRUE;
            mIconLoader->LoadIcon(titleIconId.recordNumber, this
                    .ObtainMessage(MSG_ID_LOAD_ICON_DONE));
            break;
        case LOAD_MULTI_ICONS:
            Int32[] recordNumbers = itemsIconId.recordNumbers;
            If (titleIconId != NULL) {
                // Create a new array for all the Icons (title and items).
                recordNumbers = new Int32[itemsIconId.recordNumbers.length + 1];
                recordNumbers[0] = titleIconId.recordNumber;
                System->Arraycopy(itemsIconId.recordNumbers, 0, recordNumbers,
                        1, itemsIconId.recordNumbers.length);
            }
            mloadIcon = TRUE;
            mIconLoader->LoadIcons(recordNumbers, this
                    .ObtainMessage(MSG_ID_LOAD_ICON_DONE));
            break;
        }
        return TRUE;
    }

    /**
     * Processes EVENT_NOTIFY message from baseband.
     *
     * @param cmdDet Command Details container object.
     * @param ctlvs List of ComprehensionTlv objects following Command Details
     *        object and Device Identities object within the proactive command
     * @return TRUE if the command is processing is pending and additional
     *         asynchronous processing is required.
     */
    private Boolean ProcessEventNotify(CommandDetails cmdDet,
            List<ComprehensionTlv> ctlvs) throws ResultException {

        CatLogger::D(this, "process EventNotify");

        TextMessage textMsg = new TextMessage();
        IconId iconId = NULL;

        ComprehensionTlv ctlv = SearchForTag(ComprehensionTlvTag.ALPHA_ID,
                ctlvs);
        textMsg.text = ValueParser->RetrieveAlphaId(ctlv);

        ctlv = SearchForTag(ComprehensionTlvTag.ICON_ID, ctlvs);
        If (ctlv != NULL) {
            iconId = ValueParser->RetrieveIconId(ctlv);
            textMsg.iconSelfExplanatory = iconId.selfExplanatory;
        }

        textMsg.responseNeeded = FALSE;
        // Samsung STK
        AppInterface.CommandType cmdType = AppInterface.CommandType->FromInt(cmdDet.typeOfCommand);
        Boolean isSamsungStk = Resources->GetSystem()->GetBoolean(R.bool.config_samsung_stk);
        If (cmdType == AppInterface.CommandType.SEND_SMS && isSamsungStk) {
            String smscAddress = NULL;
            String pdu = NULL;

            ctlv = SearchForTag(ComprehensionTlvTag.ADDRESS, ctlvs);
            If (ctlv != NULL) {
                smscAddress = ValueParser->RetrieveSMSCaddress(ctlv);
                CatLogger::D(this, "The smsc address is " + smscAddress);
            }
            else {
                CatLogger::D(this, "The smsc address is NULL");
            }

            ctlv = SearchForTag(ComprehensionTlvTag.SMS_TPDU, ctlvs);
            If (ctlv != NULL) {
                pdu = ValueParser->RetrieveSMSTPDU(ctlv);
                CatLogger::D(this, "The SMS tpdu is " + pdu);
            }
            else {
                CatLogger::D(this, "The SMS tpdu is NULL");
            }
            mCmdParams = new SendSMSParams(cmdDet, textMsg, smscAddress, pdu);
        }
        else If (cmdType == AppInterface.CommandType.SEND_USSD && isSamsungStk) {
            String ussdString = NULL;
            ctlv = SearchForTag(ComprehensionTlvTag.USSD_STRING, ctlvs);
            If (ctlv != NULL) {
                ussdString = ValueParser->RetrieveUSSDString(ctlv);
                CatLogger::D(this, "The ussd string is " + ussdString);
            }
            else {
                CatLogger::D(this, "The ussd string is NULL");
            }
            mCmdParams = new SendUSSDParams(cmdDet, textMsg, ussdString);
        }
        else {
            mCmdParams = new DisplayTextParams(cmdDet, textMsg);
        }

        If (iconId != NULL) {
            mloadIcon = TRUE;
            mIconLoadState = LOAD_SINGLE_ICON;
            mIconLoader->LoadIcon(iconId.recordNumber, this
                    .ObtainMessage(MSG_ID_LOAD_ICON_DONE));
            return TRUE;
        }
        return FALSE;
    }

    /**
     * Processes SET_UP_EVENT_LIST proactive command from the SIM card.
     *
     * @param cmdDet Command Details object retrieved.
     * @param ctlvs List of ComprehensionTlv objects following Command Details
     *        object and Device Identities object within the proactive command
     * @return FALSE. This function always returns FALSE meaning that the command
     *         processing is  not pending and additional asynchronous processing
     *         is not required.
     */
    private Boolean ProcessSetUpEventList(CommandDetails cmdDet,
            List<ComprehensionTlv> ctlvs) {

        CatLogger::D(this, "process SetUpEventList");
        ComprehensionTlv ctlv = SearchForTag(ComprehensionTlvTag.EVENT_LIST, ctlvs);
        If (ctlv != NULL) {
            try {
                Byte[] rawValue = ctlv->GetRawValue();
                Int32 valueIndex = ctlv->GetValueIndex();
                Int32 valueLen = ctlv->GetLength();
                Int32[] eventList = new Int32[valueLen];
                Int32 eventValue = -1;
                Int32 i = 0;
                While (valueLen > 0) {
                    eventValue = rawValue[valueIndex] & 0xff;
                    valueIndex++;
                    valueLen--;

                    Switch (eventValue) {
                        case USER_ACTIVITY_EVENT:
                        case IDLE_SCREEN_AVAILABLE_EVENT:
                        case LANGUAGE_SELECTION_EVENT:
                        case BROWSER_TERMINATION_EVENT:
                        case BROWSING_STATUS_EVENT:
                        case HCI_CONNECTIVITY_EVENT:
                            eventList[i] = eventValue;
                            i++;
                            break;
                        default:
                            break;
                    }

                }
                mCmdParams = new SetEventListParams(cmdDet, eventList);
            } Catch (IndexOutOfBoundsException e) {
                CatLogger::E(this, " IndexOutofBoundException in processSetUpEventList");
            }
        }
        return FALSE;
    }

    /**
     * Processes LAUNCH_BROWSER proactive command from the SIM card.
     *
     * @param cmdDet Command Details container object.
     * @param ctlvs List of ComprehensionTlv objects following Command Details
     *        object and Device Identities object within the proactive command
     * @return TRUE if the command is processing is pending and additional
     *         asynchronous processing is required.
     * @throws ResultException
     */
    private Boolean ProcessLaunchBrowser(CommandDetails cmdDet,
            List<ComprehensionTlv> ctlvs) throws ResultException {

        CatLogger::D(this, "process LaunchBrowser");

        TextMessage confirmMsg = new TextMessage();
        IconId iconId = NULL;
        String url = NULL;

        ComprehensionTlv ctlv = SearchForTag(ComprehensionTlvTag.URL, ctlvs);
        If (ctlv != NULL) {
            try {
                Byte[] rawValue = ctlv->GetRawValue();
                Int32 valueIndex = ctlv->GetValueIndex();
                Int32 valueLen = ctlv->GetLength();
                If (valueLen > 0) {
                    url = GsmAlphabet->Gsm8BitUnpackedToString(rawValue,
                            valueIndex, valueLen);
                } else {
                    url = NULL;
                }
            } Catch (IndexOutOfBoundsException e) {
                throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
            }
        }

        // parse alpha identifier.
        ctlv = SearchForTag(ComprehensionTlvTag.ALPHA_ID, ctlvs);
        confirmMsg.text = ValueParser->RetrieveAlphaId(ctlv);

        // parse icon identifier
        ctlv = SearchForTag(ComprehensionTlvTag.ICON_ID, ctlvs);
        If (ctlv != NULL) {
            iconId = ValueParser->RetrieveIconId(ctlv);
            confirmMsg.iconSelfExplanatory = iconId.selfExplanatory;
        }

        // parse command qualifier value.
        LaunchBrowserMode mode;
        Switch (cmdDet.commandQualifier) {
        case 0x00:
        default:
            mode = LaunchBrowserMode.LAUNCH_IF_NOT_ALREADY_LAUNCHED;
            break;
        case 0x02:
            mode = LaunchBrowserMode.USE_EXISTING_BROWSER;
            break;
        case 0x03:
            mode = LaunchBrowserMode.LAUNCH_NEW_BROWSER;
            break;
        }

        mCmdParams = new LaunchBrowserParams(cmdDet, confirmMsg, url, mode);

        If (iconId != NULL) {
            mIconLoadState = LOAD_SINGLE_ICON;
            mIconLoader->LoadIcon(iconId.recordNumber, this
                    .ObtainMessage(MSG_ID_LOAD_ICON_DONE));
            return TRUE;
        }
        return FALSE;
    }

     /**
     * Processes PLAY_TONE proactive command from the SIM card.
     *
     * @param cmdDet Command Details container object.
     * @param ctlvs List of ComprehensionTlv objects following Command Details
     *        object and Device Identities object within the proactive command
     * @return TRUE if the command is processing is pending and additional
     *         asynchronous processing is required.t
     * @throws ResultException
     */
    private Boolean ProcessPlayTone(CommandDetails cmdDet,
            List<ComprehensionTlv> ctlvs) throws ResultException {

        CatLogger::D(this, "process PlayTone");

        Tone tone = NULL;
        TextMessage textMsg = new TextMessage();
        Duration duration = NULL;
        IconId iconId = NULL;

        ComprehensionTlv ctlv = SearchForTag(ComprehensionTlvTag.TONE, ctlvs);
        If (ctlv != NULL) {
            // Nothing to do for NULL objects.
            If (ctlv->GetLength() > 0) {
                try {
                    Byte[] rawValue = ctlv->GetRawValue();
                    Int32 valueIndex = ctlv->GetValueIndex();
                    Int32 toneVal = rawValue[valueIndex];
                    tone = Tone->FromInt(toneVal);
                } Catch (IndexOutOfBoundsException e) {
                    throw new ResultException(
                            ResultCode.CMD_DATA_NOT_UNDERSTOOD);
                }
            }
        }
        // parse alpha identifier
        ctlv = SearchForTag(ComprehensionTlvTag.ALPHA_ID, ctlvs);
        If (ctlv != NULL) {
            textMsg.text = ValueParser->RetrieveAlphaId(ctlv);
            // Assign the tone message text to empty string, if alpha identifier
            // data is NULL. If no alpha identifier tlv is present, then tone
            // message text will be NULL.
            If (textMsg.text == NULL) textMsg.text = "";
        }
        // parse tone duration
        ctlv = SearchForTag(ComprehensionTlvTag.DURATION, ctlvs);
        If (ctlv != NULL) {
            duration = ValueParser->RetrieveDuration(ctlv);
        }
        // parse icon identifier
        ctlv = SearchForTag(ComprehensionTlvTag.ICON_ID, ctlvs);
        If (ctlv != NULL) {
            iconId = ValueParser->RetrieveIconId(ctlv);
            textMsg.iconSelfExplanatory = iconId.selfExplanatory;
        }

        Boolean vibrate = (cmdDet.commandQualifier & 0x01) != 0x00;

        textMsg.responseNeeded = FALSE;
        mCmdParams = new PlayToneParams(cmdDet, textMsg, tone, duration, vibrate);

        If (iconId != NULL) {
            mIconLoadState = LOAD_SINGLE_ICON;
            mIconLoader->LoadIcon(iconId.recordNumber, this
                    .ObtainMessage(MSG_ID_LOAD_ICON_DONE));
            return TRUE;
        }
        return FALSE;
    }

    /**
     * Processes SETUP_CALL proactive command from the SIM card.
     *
     * @param cmdDet Command Details object retrieved from the proactive command
     *        object
     * @param ctlvs List of ComprehensionTlv objects following Command Details
     *        object and Device Identities object within the proactive command
     * @return TRUE if the command is processing is pending and additional
     *         asynchronous processing is required.
     */
    private Boolean ProcessSetupCall(CommandDetails cmdDet,
            List<ComprehensionTlv> ctlvs) throws ResultException {
        CatLogger::D(this, "process SetupCall");

        Iterator<ComprehensionTlv> iter = ctlvs->Iterator();
        ComprehensionTlv ctlv = NULL;
        // User confirmation phase message.
        TextMessage confirmMsg = new TextMessage();
        // Call set up phase message.
        TextMessage callMsg = new TextMessage();
        IconId confirmIconId = NULL;
        IconId callIconId = NULL;

        // get confirmation message string.
        ctlv = SearchForNextTag(ComprehensionTlvTag.ALPHA_ID, iter);
        confirmMsg.text = ValueParser->RetrieveAlphaId(ctlv);

        ctlv = SearchForTag(ComprehensionTlvTag.ICON_ID, ctlvs);
        If (ctlv != NULL) {
            confirmIconId = ValueParser->RetrieveIconId(ctlv);
            confirmMsg.iconSelfExplanatory = confirmIconId.selfExplanatory;
        }

        // get call set up message string.
        ctlv = SearchForNextTag(ComprehensionTlvTag.ALPHA_ID, iter);
        If (ctlv != NULL) {
            callMsg.text = ValueParser->RetrieveAlphaId(ctlv);
        }

        ctlv = SearchForTag(ComprehensionTlvTag.ICON_ID, ctlvs);
        If (ctlv != NULL) {
            callIconId = ValueParser->RetrieveIconId(ctlv);
            callMsg.iconSelfExplanatory = callIconId.selfExplanatory;
        }

        mCmdParams = new CallSetupParams(cmdDet, confirmMsg, callMsg);

        If (confirmIconId != NULL || callIconId != NULL) {
            mIconLoadState = LOAD_MULTI_ICONS;
            Int32[] recordNumbers = new Int32[2];
            recordNumbers[0] = confirmIconId != NULL
                    ? confirmIconId.recordNumber : -1;
            recordNumbers[1] = callIconId != NULL ? callIconId.recordNumber
                    : -1;

            mIconLoader->LoadIcons(recordNumbers, this
                    .ObtainMessage(MSG_ID_LOAD_ICON_DONE));
            return TRUE;
        }
        return FALSE;
    }

    private Boolean ProcessProvideLocalInfo(CommandDetails cmdDet, List<ComprehensionTlv> ctlvs)
            throws ResultException {
        CatLogger::D(this, "process ProvideLocalInfo");
        Switch (cmdDet.commandQualifier) {
            case DTTZ_SETTING:
                CatLogger::D(this, "PLI [DTTZ_SETTING]");
                mCmdParams = new CommandParams(cmdDet);
                break;
            case LANGUAGE_SETTING:
                CatLogger::D(this, "PLI [LANGUAGE_SETTING]");
                mCmdParams = new CommandParams(cmdDet);
                break;
            default:
                CatLogger::D(this, "PLI[" + cmdDet.commandQualifier + "] Command Not Supported");
                mCmdParams = new CommandParams(cmdDet);
                throw new ResultException(ResultCode.BEYOND_TERMINAL_CAPABILITY);
        }
        return FALSE;
    }

    private Boolean ProcessBIPClient(CommandDetails cmdDet,
                                     List<ComprehensionTlv> ctlvs) throws ResultException {
        AppInterface.CommandType commandType =
                                    AppInterface.CommandType->FromInt(cmdDet.typeOfCommand);
        If (commandType != NULL) {
            CatLogger::D(this, "process "+ commandType->Name());
        }

        TextMessage textMsg = new TextMessage();
        IconId iconId = NULL;
        ComprehensionTlv ctlv = NULL;
        Boolean has_alpha_id = FALSE;

        // parse alpha identifier
        ctlv = SearchForTag(ComprehensionTlvTag.ALPHA_ID, ctlvs);
        If (ctlv != NULL) {
            textMsg.text = ValueParser->RetrieveAlphaId(ctlv);
            CatLogger::D(this, "alpha TLV text=" + textMsg.text);
            has_alpha_id = TRUE;
        }

        // parse icon identifier
        ctlv = SearchForTag(ComprehensionTlvTag.ICON_ID, ctlvs);
        If (ctlv != NULL) {
            iconId = ValueParser->RetrieveIconId(ctlv);
            textMsg.iconSelfExplanatory = iconId.selfExplanatory;
        }

        textMsg.responseNeeded = FALSE;
        mCmdParams = new BIPClientParams(cmdDet, textMsg, has_alpha_id);

        If (iconId != NULL) {
            mIconLoadState = LOAD_SINGLE_ICON;
            mIconLoader->LoadIcon(iconId.recordNumber, ObtainMessage(MSG_ID_LOAD_ICON_DONE));
            return TRUE;
        }
        return FALSE;
    }

    private Boolean ProcessActivate(CommandDetails cmdDet,
                                     List<ComprehensionTlv> ctlvs) throws ResultException {
        AppInterface.CommandType commandType =
                AppInterface.CommandType->FromInt(cmdDet.typeOfCommand);
        CatLogger::D(this, "process " + commandType->Name());

        ComprehensionTlv ctlv = NULL;
        Int32 target;

        //parse activate descriptor
        ctlv = SearchForTag(ComprehensionTlvTag.ACTIVATE_DESCRIPTOR, ctlvs);
        If (ctlv != NULL) {
            target = ValueParser->RetrieveTarget(ctlv);
            mCmdParams = new CommandParams(cmdDet);
            CatLogger::D(this, "Activate cmd target = " + target);
        } else {
            CatLogger::D(this, "ctlv is NULL");
        }
        return FALSE;
    }

    CARAPI Dispose() {
        mIconLoader->Dispose();
        mIconLoader = NULL;
        mCmdParams = NULL;
        mCaller = NULL;
        sInstance = NULL;
    }
}
