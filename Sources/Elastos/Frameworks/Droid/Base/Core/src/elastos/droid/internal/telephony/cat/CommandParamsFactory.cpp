
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/CommandParamsFactory.h"
#include "elastos/droid/internal/telephony/cat/RilMessageDecoder.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

//=====================================================================
//
//=====================================================================
//=====================================================================
//                         CommandParamsFactory
//=====================================================================
const Int32 CommandParamsFactory::MSG_ID_LOAD_ICON_DONE;
const Int32 CommandParamsFactory::LOAD_NO_ICON;
const Int32 CommandParamsFactory::LOAD_SINGLE_ICON;
const Int32 CommandParamsFactory::LOAD_MULTI_ICONS;
const Int32 CommandParamsFactory::DTTZ_SETTING;
const Int32 CommandParamsFactory::LANGUAGE_SETTING;
AutoPtr<CommandParamsFactory> CommandParamsFactory::sInstance = NULL;
const Int32 CommandParamsFactory::MAX_GSM7_DEFAULT_CHARS;
const Int32 CommandParamsFactory::MAX_UCS2_CHARS;

// synchronized
AutoPtr<CommandParamsFactory> CommandParamsFactory::GetInstance(
    /* [in] */ RilMessageDecoder* caller,
    /* [in] */ IIccFileHandler* fh)
{
    // ==================before translated======================
    // if (sInstance != null) {
    //     return sInstance;
    // }
    // if (fh != null) {
    //     return new CommandParamsFactory(caller, fh);
    // }
    // return null;
    assert(0);
    AutoPtr<CommandParamsFactory> empty;
    return empty;
}

ECode CommandParamsFactory::Make(
    /* [in] */ BerTlv* berTlv)
{
    // ==================before translated======================
    // if (berTlv == null) {
    //     return;
    // }
    // // reset global state parameters.
    // mCmdParams = null;
    // mIconLoadState = LOAD_NO_ICON;
    // // only proactive command messages are processed.
    // if (berTlv.getTag() != BerTlv.BER_PROACTIVE_COMMAND_TAG) {
    //     sendCmdParams(ResultCode.CMD_TYPE_NOT_UNDERSTOOD);
    //     return;
    // }
    // boolean cmdPending = false;
    // List<ComprehensionTlv> ctlvs = berTlv.getComprehensionTlvs();
    // // process command dtails from the tlv list.
    // CommandDetails cmdDet = processCommandDetails(ctlvs);
    // if (cmdDet == null) {
    //     sendCmdParams(ResultCode.CMD_TYPE_NOT_UNDERSTOOD);
    //     return;
    // }
    //
    // // extract command type enumeration from the raw value stored inside
    // // the Command Details object.
    // AppInterface.CommandType cmdType = AppInterface.CommandType
    //         .fromInt(cmdDet.typeOfCommand);
    // if (cmdType == null) {
    //     // This PROACTIVE COMMAND is presently not handled. Hence set
    //     // result code as BEYOND_TERMINAL_CAPABILITY in TR.
    //     mCmdParams = new CommandParams(cmdDet);
    //     sendCmdParams(ResultCode.BEYOND_TERMINAL_CAPABILITY);
    //     return;
    // }
    //
    // // proactive command length is incorrect.
    // if (!berTlv.isLengthValid()) {
    //     mCmdParams = new CommandParams(cmdDet);
    //     sendCmdParams(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    //     return;
    // }
    //
    // try {
    //     switch (cmdType) {
    //     case SET_UP_MENU:
    //         cmdPending = processSelectItem(cmdDet, ctlvs);
    //         break;
    //     case SELECT_ITEM:
    //         cmdPending = processSelectItem(cmdDet, ctlvs);
    //         break;
    //     case DISPLAY_TEXT:
    //         cmdPending = processDisplayText(cmdDet, ctlvs);
    //         break;
    //      case SET_UP_IDLE_MODE_TEXT:
    //          cmdPending = processSetUpIdleModeText(cmdDet, ctlvs);
    //          break;
    //      case GET_INKEY:
    //         cmdPending = processGetInkey(cmdDet, ctlvs);
    //         break;
    //      case GET_INPUT:
    //          cmdPending = processGetInput(cmdDet, ctlvs);
    //          break;
    //      case SEND_DTMF:
    //      case SEND_SMS:
    //      case SEND_SS:
    //      case SEND_USSD:
    //          cmdPending = processEventNotify(cmdDet, ctlvs);
    //          break;
    //      case GET_CHANNEL_STATUS:
    //      case SET_UP_CALL:
    //          cmdPending = processSetupCall(cmdDet, ctlvs);
    //          break;
    //      case REFRESH:
    //          cmdPending = processEventNotify(cmdDet, ctlvs);
    //          break;
    //      case LAUNCH_BROWSER:
    //          cmdPending = processLaunchBrowser(cmdDet, ctlvs);
    //          break;
    //      case PLAY_TONE:
    //         cmdPending = processPlayTone(cmdDet, ctlvs);
    //         break;
    //      case SET_UP_EVENT_LIST:
    //          cmdPending = processSetUpEventList(cmdDet, ctlvs);
    //          break;
    //      case PROVIDE_LOCAL_INFORMATION:
    //         cmdPending = processProvideLocalInfo(cmdDet, ctlvs);
    //         break;
    //      case OPEN_CHANNEL:
    //      case CLOSE_CHANNEL:
    //      case RECEIVE_DATA:
    //      case SEND_DATA:
    //          cmdPending = processBIPClient(cmdDet, ctlvs);
    //          break;
    //     case ACTIVATE:
    //         cmdPending = processActivate(cmdDet, ctlvs);
    //         break;
    //     default:
    //         // unsupported proactive commands
    //         mCmdParams = new CommandParams(cmdDet);
    //         sendCmdParams(ResultCode.BEYOND_TERMINAL_CAPABILITY);
    //         return;
    //     }
    // } catch (ResultException e) {
    //     CatLog.d(this, "make: caught ResultException e=" + e);
    //     mCmdParams = new CommandParams(cmdDet);
    //     sendCmdParams(e.result());
    //     return;
    // }
    // if (!cmdPending) {
    //     sendCmdParams(ResultCode.OK);
    // }
    assert(0);
    return NOERROR;
}

ECode CommandParamsFactory::HandleMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // switch (msg.what) {
    // case MSG_ID_LOAD_ICON_DONE:
    //     sendCmdParams(setIcons(msg.obj));
    //     break;
    // }
    assert(0);
    return NOERROR;
}

ECode CommandParamsFactory::Dispose()
{
    // ==================before translated======================
    // mIconLoader.dispose();
    // mIconLoader = null;
    // mCmdParams = null;
    // mCaller = null;
    // sInstance = null;
    assert(0);
    return NOERROR;
}

CommandParamsFactory::CommandParamsFactory(
    /* [in] */ RilMessageDecoder* caller,
    /* [in] */ IIccFileHandler* fh)
{
    // ==================before translated======================
    // mCaller = caller;
    // mIconLoader = IconLoader.getInstance(this, fh);
}

AutoPtr<CommandDetails> CommandParamsFactory::ProcessCommandDetails(
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    // ==================before translated======================
    // CommandDetails cmdDet = null;
    //
    // if (ctlvs != null) {
    //     // Search for the Command Details object.
    //     ComprehensionTlv ctlvCmdDet = searchForTag(
    //             ComprehensionTlvTag.COMMAND_DETAILS, ctlvs);
    //     if (ctlvCmdDet != null) {
    //         try {
    //             cmdDet = ValueParser.retrieveCommandDetails(ctlvCmdDet);
    //         } catch (ResultException e) {
    //             CatLog.d(this,
    //                     "processCommandDetails: Failed to procees command details e=" + e);
    //         }
    //     }
    // }
    // return cmdDet;
    assert(0);
    AutoPtr<CommandDetails> empty;
    return empty;
}

ResultCode CommandParamsFactory::SetIcons(
    /* [in] */ IInterface* data)
{
    // ==================before translated======================
    // Bitmap[] icons = null;
    // int iconIndex = 0;
    //
    // if (data == null) {
    //     if (mloadIcon) {
    //         CatLog.d(this, "Optional Icon data is NULL");
    //         mCmdParams.mLoadIconFailed = true;
    //         mloadIcon = false;
    //         /** In case of icon load fail consider the
    //          ** received proactive command as valid (sending RESULT OK) */
    //         return ResultCode.OK;
    //     }
    //     return ResultCode.PRFRMD_ICON_NOT_DISPLAYED;
    // }
    // switch(mIconLoadState) {
    // case LOAD_SINGLE_ICON:
    //     mCmdParams.setIcon((Bitmap) data);
    //     break;
    // case LOAD_MULTI_ICONS:
    //     icons = (Bitmap[]) data;
    //     // set each item icon.
    //     for (Bitmap icon : icons) {
    //         mCmdParams.setIcon(icon);
    //         if (icon == null && mloadIcon) {
    //             CatLog.d(this, "Optional Icon data is NULL while loading multi icons");
    //             mCmdParams.mLoadIconFailed = true;
    //         }
    //     }
    //     break;
    // }
    // return ResultCode.OK;
    assert(0);
    return 0;
}

void CommandParamsFactory::SendCmdParams(
    /* [in] */ ResultCode resCode)
{
    // ==================before translated======================
    // mCaller.sendMsgParamsDecoded(resCode, mCmdParams);
    assert(0);
}

AutoPtr<ComprehensionTlv> CommandParamsFactory::SearchForTag(
    /* [in] */ ComprehensionTlvTag tag,
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    // ==================before translated======================
    // Iterator<ComprehensionTlv> iter = ctlvs.iterator();
    // return searchForNextTag(tag, iter);
    assert(0);
    AutoPtr<ComprehensionTlv> empty;
    return empty;
}

AutoPtr<ComprehensionTlv> CommandParamsFactory::SearchForNextTag(
    /* [in] */ ComprehensionTlvTag tag,
    /* [in] */ IIterator/*<ComprehensionTlv*>*/* iter)
{
    // ==================before translated======================
    // int tagValue = tag.value();
    // while (iter.hasNext()) {
    //     ComprehensionTlv ctlv = iter.next();
    //     if (ctlv.getTag() == tagValue) {
    //         return ctlv;
    //     }
    // }
    // return null;
    assert(0);
    AutoPtr<ComprehensionTlv> empty;
    return empty;
}

Boolean CommandParamsFactory::ProcessDisplayText(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    // ==================before translated======================
    //
    // CatLog.d(this, "process DisplayText");
    //
    // TextMessage textMsg = new TextMessage();
    // IconId iconId = null;
    //
    // ComprehensionTlv ctlv = searchForTag(ComprehensionTlvTag.TEXT_STRING,
    //         ctlvs);
    // if (ctlv != null) {
    //     textMsg.text = ValueParser.retrieveTextString(ctlv);
    // }
    // // If the tlv object doesn't exist or the it is a null object reply
    // // with command not understood.
    // if (textMsg.text == null) {
    //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    // }
    //
    // ctlv = searchForTag(ComprehensionTlvTag.IMMEDIATE_RESPONSE, ctlvs);
    // if (ctlv != null) {
    //     textMsg.responseNeeded = false;
    // }
    // // parse icon identifier
    // ctlv = searchForTag(ComprehensionTlvTag.ICON_ID, ctlvs);
    // if (ctlv != null) {
    //     iconId = ValueParser.retrieveIconId(ctlv);
    //     textMsg.iconSelfExplanatory = iconId.selfExplanatory;
    // }
    // // parse tone duration
    // ctlv = searchForTag(ComprehensionTlvTag.DURATION, ctlvs);
    // if (ctlv != null) {
    //     textMsg.duration = ValueParser.retrieveDuration(ctlv);
    // }
    //
    // // Parse command qualifier parameters.
    // textMsg.isHighPriority = (cmdDet.commandQualifier & 0x01) != 0;
    // textMsg.userClear = (cmdDet.commandQualifier & 0x80) != 0;
    //
    // mCmdParams = new DisplayTextParams(cmdDet, textMsg);
    //
    // if (iconId != null) {
    //     mloadIcon = true;
    //     mIconLoadState = LOAD_SINGLE_ICON;
    //     mIconLoader.loadIcon(iconId.recordNumber, this
    //             .obtainMessage(MSG_ID_LOAD_ICON_DONE));
    //     return true;
    // }
    // return false;
    assert(0);
    return FALSE;
}

Boolean CommandParamsFactory::ProcessSetUpIdleModeText(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    // ==================before translated======================
    //
    // CatLog.d(this, "process SetUpIdleModeText");
    //
    // TextMessage textMsg = new TextMessage();
    // IconId iconId = null;
    //
    // ComprehensionTlv ctlv = searchForTag(ComprehensionTlvTag.TEXT_STRING,
    //         ctlvs);
    // if (ctlv != null) {
    //     textMsg.text = ValueParser.retrieveTextString(ctlv);
    // }
    //
    // ctlv = searchForTag(ComprehensionTlvTag.ICON_ID, ctlvs);
    // if (ctlv != null) {
    //     iconId = ValueParser.retrieveIconId(ctlv);
    //     textMsg.iconSelfExplanatory = iconId.selfExplanatory;
    // }
    //
    // /*
    //  * If the tlv object doesn't contain text and the icon is not self
    //  * explanatory then reply with command not understood.
    //  */
    //
    // if (textMsg.text == null && iconId != null && !textMsg.iconSelfExplanatory) {
    //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    // }
    // mCmdParams = new DisplayTextParams(cmdDet, textMsg);
    //
    // if (iconId != null) {
    //     mloadIcon = true;
    //     mIconLoadState = LOAD_SINGLE_ICON;
    //     mIconLoader.loadIcon(iconId.recordNumber, this
    //             .obtainMessage(MSG_ID_LOAD_ICON_DONE));
    //     return true;
    // }
    // return false;
    assert(0);
    return FALSE;
}

Boolean CommandParamsFactory::ProcessGetInkey(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    // ==================before translated======================
    //
    // CatLog.d(this, "process GetInkey");
    //
    // Input input = new Input();
    // IconId iconId = null;
    //
    // ComprehensionTlv ctlv = searchForTag(ComprehensionTlvTag.TEXT_STRING,
    //         ctlvs);
    // if (ctlv != null) {
    //     input.text = ValueParser.retrieveTextString(ctlv);
    // } else {
    //     throw new ResultException(ResultCode.REQUIRED_VALUES_MISSING);
    // }
    // // parse icon identifier
    // ctlv = searchForTag(ComprehensionTlvTag.ICON_ID, ctlvs);
    // if (ctlv != null) {
    //     iconId = ValueParser.retrieveIconId(ctlv);
    // }
    //
    // // parse duration
    // ctlv = searchForTag(ComprehensionTlvTag.DURATION, ctlvs);
    // if (ctlv != null) {
    //     input.duration = ValueParser.retrieveDuration(ctlv);
    // }
    //
    // input.minLen = 1;
    // input.maxLen = 1;
    //
    // input.digitOnly = (cmdDet.commandQualifier & 0x01) == 0;
    // input.ucs2 = (cmdDet.commandQualifier & 0x02) != 0;
    // input.yesNo = (cmdDet.commandQualifier & 0x04) != 0;
    // input.helpAvailable = (cmdDet.commandQualifier & 0x80) != 0;
    // input.echo = true;
    //
    // mCmdParams = new GetInputParams(cmdDet, input);
    //
    // if (iconId != null) {
    //     mloadIcon = true;
    //     mIconLoadState = LOAD_SINGLE_ICON;
    //     mIconLoader.loadIcon(iconId.recordNumber, this
    //             .obtainMessage(MSG_ID_LOAD_ICON_DONE));
    //     return true;
    // }
    // return false;
    assert(0);
    return FALSE;
}

Boolean CommandParamsFactory::ProcessGetInput(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    // ==================before translated======================
    //
    // CatLog.d(this, "process GetInput");
    //
    // Input input = new Input();
    // IconId iconId = null;
    //
    // ComprehensionTlv ctlv = searchForTag(ComprehensionTlvTag.TEXT_STRING,
    //         ctlvs);
    // if (ctlv != null) {
    //     input.text = ValueParser.retrieveTextString(ctlv);
    // } else {
    //     throw new ResultException(ResultCode.REQUIRED_VALUES_MISSING);
    // }
    //
    // ctlv = searchForTag(ComprehensionTlvTag.RESPONSE_LENGTH, ctlvs);
    // if (ctlv != null) {
    //     try {
    //         byte[] rawValue = ctlv.getRawValue();
    //         int valueIndex = ctlv.getValueIndex();
    //         input.minLen = rawValue[valueIndex] & 0xff;
    //         input.maxLen = rawValue[valueIndex + 1] & 0xff;
    //     } catch (IndexOutOfBoundsException e) {
    //         throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    //     }
    // } else {
    //     throw new ResultException(ResultCode.REQUIRED_VALUES_MISSING);
    // }
    //
    // ctlv = searchForTag(ComprehensionTlvTag.DEFAULT_TEXT, ctlvs);
    // if (ctlv != null) {
    //     input.defaultText = ValueParser.retrieveTextString(ctlv);
    // }
    // // parse icon identifier
    // ctlv = searchForTag(ComprehensionTlvTag.ICON_ID, ctlvs);
    // if (ctlv != null) {
    //     iconId = ValueParser.retrieveIconId(ctlv);
    // }
    //
    // input.digitOnly = (cmdDet.commandQualifier & 0x01) == 0;
    // input.ucs2 = (cmdDet.commandQualifier & 0x02) != 0;
    // input.echo = (cmdDet.commandQualifier & 0x04) == 0;
    // input.packed = (cmdDet.commandQualifier & 0x08) != 0;
    // input.helpAvailable = (cmdDet.commandQualifier & 0x80) != 0;
    //
    // // Truncate the maxLen if it exceeds the max number of chars that can
    // // be encoded. Limit depends on DCS in Command Qualifier.
    // if (input.ucs2 && input.maxLen > MAX_UCS2_CHARS) {
    //     CatLog.d(this, "UCS2: received maxLen = " + input.maxLen +
    //           ", truncating to " + MAX_UCS2_CHARS);
    //     input.maxLen = MAX_UCS2_CHARS;
    // } else if (!input.packed && input.maxLen > MAX_GSM7_DEFAULT_CHARS) {
    //     CatLog.d(this, "GSM 7Bit Default: received maxLen = " + input.maxLen +
    //           ", truncating to " + MAX_GSM7_DEFAULT_CHARS);
    //     input.maxLen = MAX_GSM7_DEFAULT_CHARS;
    // }
    //
    // mCmdParams = new GetInputParams(cmdDet, input);
    //
    // if (iconId != null) {
    //     mloadIcon = true;
    //     mIconLoadState = LOAD_SINGLE_ICON;
    //     mIconLoader.loadIcon(iconId.recordNumber, this
    //             .obtainMessage(MSG_ID_LOAD_ICON_DONE));
    //     return true;
    // }
    // return false;
    assert(0);
    return FALSE;
}

Boolean CommandParamsFactory::ProcessSelectItem(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    // ==================before translated======================
    //
    // CatLog.d(this, "process SelectItem");
    //
    // Menu menu = new Menu();
    // IconId titleIconId = null;
    // ItemsIconId itemsIconId = null;
    // Iterator<ComprehensionTlv> iter = ctlvs.iterator();
    //
    // ComprehensionTlv ctlv = searchForTag(ComprehensionTlvTag.ALPHA_ID,
    //         ctlvs);
    // if (ctlv != null) {
    //     menu.title = ValueParser.retrieveAlphaId(ctlv);
    // }
    //
    // while (true) {
    //     ctlv = searchForNextTag(ComprehensionTlvTag.ITEM, iter);
    //     if (ctlv != null) {
    //         menu.items.add(ValueParser.retrieveItem(ctlv));
    //     } else {
    //         break;
    //     }
    // }
    //
    // // We must have at least one menu item.
    // if (menu.items.size() == 0) {
    //     throw new ResultException(ResultCode.REQUIRED_VALUES_MISSING);
    // }
    //
    // ctlv = searchForTag(ComprehensionTlvTag.ITEM_ID, ctlvs);
    // if (ctlv != null) {
    //     // CAT items are listed 1...n while list start at 0, need to
    //     // subtract one.
    //     menu.defaultItem = ValueParser.retrieveItemId(ctlv) - 1;
    // }
    //
    // ctlv = searchForTag(ComprehensionTlvTag.ICON_ID, ctlvs);
    // if (ctlv != null) {
    //     mIconLoadState = LOAD_SINGLE_ICON;
    //     titleIconId = ValueParser.retrieveIconId(ctlv);
    //     menu.titleIconSelfExplanatory = titleIconId.selfExplanatory;
    // }
    //
    // ctlv = searchForTag(ComprehensionTlvTag.ITEM_ICON_ID_LIST, ctlvs);
    // if (ctlv != null) {
    //     mIconLoadState = LOAD_MULTI_ICONS;
    //     itemsIconId = ValueParser.retrieveItemsIconId(ctlv);
    //     menu.itemsIconSelfExplanatory = itemsIconId.selfExplanatory;
    // }
    //
    // boolean presentTypeSpecified = (cmdDet.commandQualifier & 0x01) != 0;
    // if (presentTypeSpecified) {
    //     if ((cmdDet.commandQualifier & 0x02) == 0) {
    //         menu.presentationType = PresentationType.DATA_VALUES;
    //     } else {
    //         menu.presentationType = PresentationType.NAVIGATION_OPTIONS;
    //     }
    // }
    // menu.softKeyPreferred = (cmdDet.commandQualifier & 0x04) != 0;
    // menu.helpAvailable = (cmdDet.commandQualifier & 0x80) != 0;
    //
    // mCmdParams = new SelectItemParams(cmdDet, menu, titleIconId != null);
    //
    // // Load icons data if needed.
    // switch(mIconLoadState) {
    // case LOAD_NO_ICON:
    //     return false;
    // case LOAD_SINGLE_ICON:
    //     mloadIcon = true;
    //     mIconLoader.loadIcon(titleIconId.recordNumber, this
    //             .obtainMessage(MSG_ID_LOAD_ICON_DONE));
    //     break;
    // case LOAD_MULTI_ICONS:
    //     int[] recordNumbers = itemsIconId.recordNumbers;
    //     if (titleIconId != null) {
    //         // Create a new array for all the icons (title and items).
    //         recordNumbers = new int[itemsIconId.recordNumbers.length + 1];
    //         recordNumbers[0] = titleIconId.recordNumber;
    //         System.arraycopy(itemsIconId.recordNumbers, 0, recordNumbers,
    //                 1, itemsIconId.recordNumbers.length);
    //     }
    //     mloadIcon = true;
    //     mIconLoader.loadIcons(recordNumbers, this
    //             .obtainMessage(MSG_ID_LOAD_ICON_DONE));
    //     break;
    // }
    // return true;
    assert(0);
    return FALSE;
}

Boolean CommandParamsFactory::ProcessEventNotify(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    // ==================before translated======================
    //
    // CatLog.d(this, "process EventNotify");
    //
    // TextMessage textMsg = new TextMessage();
    // IconId iconId = null;
    //
    // ComprehensionTlv ctlv = searchForTag(ComprehensionTlvTag.ALPHA_ID,
    //         ctlvs);
    // textMsg.text = ValueParser.retrieveAlphaId(ctlv);
    //
    // ctlv = searchForTag(ComprehensionTlvTag.ICON_ID, ctlvs);
    // if (ctlv != null) {
    //     iconId = ValueParser.retrieveIconId(ctlv);
    //     textMsg.iconSelfExplanatory = iconId.selfExplanatory;
    // }
    //
    // textMsg.responseNeeded = false;
    // // Samsung STK
    // AppInterface.CommandType cmdType = AppInterface.CommandType.fromInt(cmdDet.typeOfCommand);
    // boolean isSamsungStk = Resources.getSystem().getBoolean(com.android.internal.R.bool.config_samsung_stk);
    // if (cmdType == AppInterface.CommandType.SEND_SMS && isSamsungStk) {
    //     String smscAddress = null;
    //     String pdu = null;
    //
    //     ctlv = searchForTag(ComprehensionTlvTag.ADDRESS, ctlvs);
    //     if (ctlv != null) {
    //         smscAddress = ValueParser.retrieveSMSCaddress(ctlv);
    //         CatLog.d(this, "The smsc address is " + smscAddress);
    //     }
    //     else {
    //         CatLog.d(this, "The smsc address is null");
    //     }
    //
    //     ctlv = searchForTag(ComprehensionTlvTag.SMS_TPDU, ctlvs);
    //     if (ctlv != null) {
    //         pdu = ValueParser.retrieveSMSTPDU(ctlv);
    //         CatLog.d(this, "The SMS tpdu is " + pdu);
    //     }
    //     else {
    //         CatLog.d(this, "The SMS tpdu is null");
    //     }
    //     mCmdParams = new SendSMSParams(cmdDet, textMsg, smscAddress, pdu);
    // }
    // else if (cmdType == AppInterface.CommandType.SEND_USSD && isSamsungStk) {
    //     String ussdString = null;
    //     ctlv = searchForTag(ComprehensionTlvTag.USSD_STRING, ctlvs);
    //     if (ctlv != null) {
    //         ussdString = ValueParser.retrieveUSSDString(ctlv);
    //         CatLog.d(this, "The ussd string is " + ussdString);
    //     }
    //     else {
    //         CatLog.d(this, "The ussd string is null");
    //     }
    //     mCmdParams = new SendUSSDParams(cmdDet, textMsg, ussdString);
    // }
    // else {
    //     mCmdParams = new DisplayTextParams(cmdDet, textMsg);
    // }
    //
    // if (iconId != null) {
    //     mloadIcon = true;
    //     mIconLoadState = LOAD_SINGLE_ICON;
    //     mIconLoader.loadIcon(iconId.recordNumber, this
    //             .obtainMessage(MSG_ID_LOAD_ICON_DONE));
    //     return true;
    // }
    // return false;
    assert(0);
    return FALSE;
}

Boolean CommandParamsFactory::ProcessSetUpEventList(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    // ==================before translated======================
    //
    // CatLog.d(this, "process SetUpEventList");
    // ComprehensionTlv ctlv = searchForTag(ComprehensionTlvTag.EVENT_LIST, ctlvs);
    // if (ctlv != null) {
    //     try {
    //         byte[] rawValue = ctlv.getRawValue();
    //         int valueIndex = ctlv.getValueIndex();
    //         int valueLen = ctlv.getLength();
    //         int[] eventList = new int[valueLen];
    //         int eventValue = -1;
    //         int i = 0;
    //         while (valueLen > 0) {
    //             eventValue = rawValue[valueIndex] & 0xff;
    //             valueIndex++;
    //             valueLen--;
    //
    //             switch (eventValue) {
    //                 case USER_ACTIVITY_EVENT:
    //                 case IDLE_SCREEN_AVAILABLE_EVENT:
    //                 case LANGUAGE_SELECTION_EVENT:
    //                 case BROWSER_TERMINATION_EVENT:
    //                 case BROWSING_STATUS_EVENT:
    //                 case HCI_CONNECTIVITY_EVENT:
    //                     eventList[i] = eventValue;
    //                     i++;
    //                     break;
    //                 default:
    //                     break;
    //             }
    //
    //         }
    //         mCmdParams = new SetEventListParams(cmdDet, eventList);
    //     } catch (IndexOutOfBoundsException e) {
    //         CatLog.e(this, " IndexOutofBoundException in processSetUpEventList");
    //     }
    // }
    // return false;
    assert(0);
    return FALSE;
}

Boolean CommandParamsFactory::ProcessLaunchBrowser(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    // ==================before translated======================
    //
    // CatLog.d(this, "process LaunchBrowser");
    //
    // TextMessage confirmMsg = new TextMessage();
    // IconId iconId = null;
    // String url = null;
    //
    // ComprehensionTlv ctlv = searchForTag(ComprehensionTlvTag.URL, ctlvs);
    // if (ctlv != null) {
    //     try {
    //         byte[] rawValue = ctlv.getRawValue();
    //         int valueIndex = ctlv.getValueIndex();
    //         int valueLen = ctlv.getLength();
    //         if (valueLen > 0) {
    //             url = GsmAlphabet.gsm8BitUnpackedToString(rawValue,
    //                     valueIndex, valueLen);
    //         } else {
    //             url = null;
    //         }
    //     } catch (IndexOutOfBoundsException e) {
    //         throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    //     }
    // }
    //
    // // parse alpha identifier.
    // ctlv = searchForTag(ComprehensionTlvTag.ALPHA_ID, ctlvs);
    // confirmMsg.text = ValueParser.retrieveAlphaId(ctlv);
    //
    // // parse icon identifier
    // ctlv = searchForTag(ComprehensionTlvTag.ICON_ID, ctlvs);
    // if (ctlv != null) {
    //     iconId = ValueParser.retrieveIconId(ctlv);
    //     confirmMsg.iconSelfExplanatory = iconId.selfExplanatory;
    // }
    //
    // // parse command qualifier value.
    // LaunchBrowserMode mode;
    // switch (cmdDet.commandQualifier) {
    // case 0x00:
    // default:
    //     mode = LaunchBrowserMode.LAUNCH_IF_NOT_ALREADY_LAUNCHED;
    //     break;
    // case 0x02:
    //     mode = LaunchBrowserMode.USE_EXISTING_BROWSER;
    //     break;
    // case 0x03:
    //     mode = LaunchBrowserMode.LAUNCH_NEW_BROWSER;
    //     break;
    // }
    //
    // mCmdParams = new LaunchBrowserParams(cmdDet, confirmMsg, url, mode);
    //
    // if (iconId != null) {
    //     mIconLoadState = LOAD_SINGLE_ICON;
    //     mIconLoader.loadIcon(iconId.recordNumber, this
    //             .obtainMessage(MSG_ID_LOAD_ICON_DONE));
    //     return true;
    // }
    // return false;
    assert(0);
    return FALSE;
}

Boolean CommandParamsFactory::ProcessPlayTone(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    // ==================before translated======================
    //
    // CatLog.d(this, "process PlayTone");
    //
    // Tone tone = null;
    // TextMessage textMsg = new TextMessage();
    // Duration duration = null;
    // IconId iconId = null;
    //
    // ComprehensionTlv ctlv = searchForTag(ComprehensionTlvTag.TONE, ctlvs);
    // if (ctlv != null) {
    //     // Nothing to do for null objects.
    //     if (ctlv.getLength() > 0) {
    //         try {
    //             byte[] rawValue = ctlv.getRawValue();
    //             int valueIndex = ctlv.getValueIndex();
    //             int toneVal = rawValue[valueIndex];
    //             tone = Tone.fromInt(toneVal);
    //         } catch (IndexOutOfBoundsException e) {
    //             throw new ResultException(
    //                     ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    //         }
    //     }
    // }
    // // parse alpha identifier
    // ctlv = searchForTag(ComprehensionTlvTag.ALPHA_ID, ctlvs);
    // if (ctlv != null) {
    //     textMsg.text = ValueParser.retrieveAlphaId(ctlv);
    //     // Assign the tone message text to empty string, if alpha identifier
    //     // data is null. If no alpha identifier tlv is present, then tone
    //     // message text will be null.
    //     if (textMsg.text == null) textMsg.text = "";
    // }
    // // parse tone duration
    // ctlv = searchForTag(ComprehensionTlvTag.DURATION, ctlvs);
    // if (ctlv != null) {
    //     duration = ValueParser.retrieveDuration(ctlv);
    // }
    // // parse icon identifier
    // ctlv = searchForTag(ComprehensionTlvTag.ICON_ID, ctlvs);
    // if (ctlv != null) {
    //     iconId = ValueParser.retrieveIconId(ctlv);
    //     textMsg.iconSelfExplanatory = iconId.selfExplanatory;
    // }
    //
    // boolean vibrate = (cmdDet.commandQualifier & 0x01) != 0x00;
    //
    // textMsg.responseNeeded = false;
    // mCmdParams = new PlayToneParams(cmdDet, textMsg, tone, duration, vibrate);
    //
    // if (iconId != null) {
    //     mIconLoadState = LOAD_SINGLE_ICON;
    //     mIconLoader.loadIcon(iconId.recordNumber, this
    //             .obtainMessage(MSG_ID_LOAD_ICON_DONE));
    //     return true;
    // }
    // return false;
    assert(0);
    return FALSE;
}

Boolean CommandParamsFactory::ProcessSetupCall(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    // ==================before translated======================
    // CatLog.d(this, "process SetupCall");
    //
    // Iterator<ComprehensionTlv> iter = ctlvs.iterator();
    // ComprehensionTlv ctlv = null;
    // // User confirmation phase message.
    // TextMessage confirmMsg = new TextMessage();
    // // Call set up phase message.
    // TextMessage callMsg = new TextMessage();
    // IconId confirmIconId = null;
    // IconId callIconId = null;
    //
    // // get confirmation message string.
    // ctlv = searchForNextTag(ComprehensionTlvTag.ALPHA_ID, iter);
    // confirmMsg.text = ValueParser.retrieveAlphaId(ctlv);
    //
    // ctlv = searchForTag(ComprehensionTlvTag.ICON_ID, ctlvs);
    // if (ctlv != null) {
    //     confirmIconId = ValueParser.retrieveIconId(ctlv);
    //     confirmMsg.iconSelfExplanatory = confirmIconId.selfExplanatory;
    // }
    //
    // // get call set up message string.
    // ctlv = searchForNextTag(ComprehensionTlvTag.ALPHA_ID, iter);
    // if (ctlv != null) {
    //     callMsg.text = ValueParser.retrieveAlphaId(ctlv);
    // }
    //
    // ctlv = searchForTag(ComprehensionTlvTag.ICON_ID, ctlvs);
    // if (ctlv != null) {
    //     callIconId = ValueParser.retrieveIconId(ctlv);
    //     callMsg.iconSelfExplanatory = callIconId.selfExplanatory;
    // }
    //
    // mCmdParams = new CallSetupParams(cmdDet, confirmMsg, callMsg);
    //
    // if (confirmIconId != null || callIconId != null) {
    //     mIconLoadState = LOAD_MULTI_ICONS;
    //     int[] recordNumbers = new int[2];
    //     recordNumbers[0] = confirmIconId != null
    //             ? confirmIconId.recordNumber : -1;
    //     recordNumbers[1] = callIconId != null ? callIconId.recordNumber
    //             : -1;
    //
    //     mIconLoader.loadIcons(recordNumbers, this
    //             .obtainMessage(MSG_ID_LOAD_ICON_DONE));
    //     return true;
    // }
    // return false;
    assert(0);
    return FALSE;
}

Boolean CommandParamsFactory::ProcessProvideLocalInfo(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    // ==================before translated======================
    // CatLog.d(this, "process ProvideLocalInfo");
    // switch (cmdDet.commandQualifier) {
    //     case DTTZ_SETTING:
    //         CatLog.d(this, "PLI [DTTZ_SETTING]");
    //         mCmdParams = new CommandParams(cmdDet);
    //         break;
    //     case LANGUAGE_SETTING:
    //         CatLog.d(this, "PLI [LANGUAGE_SETTING]");
    //         mCmdParams = new CommandParams(cmdDet);
    //         break;
    //     default:
    //         CatLog.d(this, "PLI[" + cmdDet.commandQualifier + "] Command Not Supported");
    //         mCmdParams = new CommandParams(cmdDet);
    //         throw new ResultException(ResultCode.BEYOND_TERMINAL_CAPABILITY);
    // }
    // return false;
    assert(0);
    return FALSE;
}

Boolean CommandParamsFactory::ProcessBIPClient(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    // ==================before translated======================
    // AppInterface.CommandType commandType =
    //                             AppInterface.CommandType.fromInt(cmdDet.typeOfCommand);
    // if (commandType != null) {
    //     CatLog.d(this, "process "+ commandType.name());
    // }
    //
    // TextMessage textMsg = new TextMessage();
    // IconId iconId = null;
    // ComprehensionTlv ctlv = null;
    // boolean has_alpha_id = false;
    //
    // // parse alpha identifier
    // ctlv = searchForTag(ComprehensionTlvTag.ALPHA_ID, ctlvs);
    // if (ctlv != null) {
    //     textMsg.text = ValueParser.retrieveAlphaId(ctlv);
    //     CatLog.d(this, "alpha TLV text=" + textMsg.text);
    //     has_alpha_id = true;
    // }
    //
    // // parse icon identifier
    // ctlv = searchForTag(ComprehensionTlvTag.ICON_ID, ctlvs);
    // if (ctlv != null) {
    //     iconId = ValueParser.retrieveIconId(ctlv);
    //     textMsg.iconSelfExplanatory = iconId.selfExplanatory;
    // }
    //
    // textMsg.responseNeeded = false;
    // mCmdParams = new BIPClientParams(cmdDet, textMsg, has_alpha_id);
    //
    // if (iconId != null) {
    //     mIconLoadState = LOAD_SINGLE_ICON;
    //     mIconLoader.loadIcon(iconId.recordNumber, obtainMessage(MSG_ID_LOAD_ICON_DONE));
    //     return true;
    // }
    // return false;
    assert(0);
    return FALSE;
}

Boolean CommandParamsFactory::ProcessActivate(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    // ==================before translated======================
    // AppInterface.CommandType commandType =
    //         AppInterface.CommandType.fromInt(cmdDet.typeOfCommand);
    // CatLog.d(this, "process " + commandType.name());
    //
    // ComprehensionTlv ctlv = null;
    // int target;
    //
    // //parse activate descriptor
    // ctlv = searchForTag(ComprehensionTlvTag.ACTIVATE_DESCRIPTOR, ctlvs);
    // if (ctlv != null) {
    //     target = ValueParser.retrieveTarget(ctlv);
    //     mCmdParams = new CommandParams(cmdDet);
    //     CatLog.d(this, "Activate cmd target = " + target);
    // } else {
    //     CatLog.d(this, "ctlv is null");
    // }
    // return false;
    assert(0);
    return FALSE;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
