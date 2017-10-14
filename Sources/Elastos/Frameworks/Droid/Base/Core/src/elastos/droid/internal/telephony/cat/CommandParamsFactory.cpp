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

#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/CommandParamsFactory.h"
#include "elastos/droid/internal/telephony/cat/CCatMenu.h"
#include "elastos/droid/internal/telephony/cat/RilMessageDecoder.h"
#include "elastos/droid/internal/telephony/cat/ValueParser.h"
#include "elastos/droid/internal/telephony/cat/CatLog.h"
#include "elastos/droid/internal/telephony/cat/CTextMessage.h"
#include "elastos/droid/internal/telephony/cat/CInput.h"
#include "elastos/droid/internal/telephony/CGsmAlphabet.h"
#include "elastos/droid/content/res/CResourcesHelper.h"
#include "elastos/droid/R.h"

#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Internal::Telephony::IGsmAlphabet;
using Elastos::Droid::Internal::Telephony::CGsmAlphabet;
using Elastos::Droid::R;

using Elastos::Core::StringUtils;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

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
    if (sInstance != NULL) {
        return sInstance;
    }
    if (fh != NULL) {
        AutoPtr<CommandParamsFactory> p = new CommandParamsFactory(caller, fh);
        return p;
    }
    return NULL;
}

CommandParamsFactory::CommandParamsFactory(
    /* [in] */ RilMessageDecoder* caller,
    /* [in] */ IIccFileHandler* fh)
{
    mIconLoadState = LOAD_NO_ICON;
    mloadIcon = FALSE;
    mCaller = caller;
    mIconLoader = IconLoader::GetInstance(this, fh);
}

AutoPtr<CommandDetails> CommandParamsFactory::ProcessCommandDetails(
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    AutoPtr<CommandDetails> cmdDet;

    if (ctlvs != NULL) {
        // Search for the Command Details object.
        AutoPtr<ComprehensionTlv> ctlvCmdDet = SearchForTag(
                ComprehensionTlvTag_COMMAND_DETAILS, ctlvs);
        if (ctlvCmdDet != NULL) {
            // try {
                cmdDet = ValueParser::RetrieveCommandDetails(ctlvCmdDet);
            // } catch (ResultException e) {
            //     CatLog::D(this,
            //             "processCommandDetails: Failed to procees command details e=" + e);
            // }
        }
    }
    return cmdDet;
}

ECode CommandParamsFactory::Make(
    /* [in] */ BerTlv* berTlv)
{
    if (berTlv == NULL) {
        return NOERROR;
    }
    // reset global state parameters.
    mCmdParams = NULL;
    mIconLoadState = LOAD_NO_ICON;
    // only proactive command messages are processed.
    Int32 tag = 0;
    berTlv->GetTag(&tag);
    if (tag != BerTlv::BER_PROACTIVE_COMMAND_TAG) {
        SendCmdParams(ResultCode_CMD_TYPE_NOT_UNDERSTOOD);
        return NOERROR;
    }
    Boolean cmdPending = FALSE;
    AutoPtr<IList> ctlvs;
    berTlv->GetComprehensionTlvs((IList**)&ctlvs);
    // process command dtails from the tlv list.
    AutoPtr<CommandDetails> cmdDet = ProcessCommandDetails(ctlvs);
    if (cmdDet == NULL) {
        SendCmdParams(ResultCode_CMD_TYPE_NOT_UNDERSTOOD);
        return NOERROR;
    }

    // extract command type enumeration from the raw value stored inside
    // the Command Details object.
    CommandType cmdType = cmdDet->mTypeOfCommand;
    // if (cmdType == NULL) {
    //     // This PROACTIVE COMMAND is presently not handled. Hence set
    //     // result code as BEYOND_TERMINAL_CAPABILITY in TR.
    //     mCmdParams = new CommandParams(cmdDet);
    //     sendCmdParams(ResultCode_BEYOND_TERMINAL_CAPABILITY);
    //     return;
    // }

    // proactive command length is incorrect.
    Boolean bValid = FALSE;
    berTlv->IsLengthValid(&bValid);
    if (!bValid) {
        mCmdParams = new CommandParams(cmdDet);
        SendCmdParams(ResultCode_CMD_DATA_NOT_UNDERSTOOD);
        return NOERROR;
    }

    // try {
        switch (cmdType) {
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
            SendCmdParams(ResultCode_BEYOND_TERMINAL_CAPABILITY);
            return NOERROR;
        }
    // } catch (ResultException e) {
    //     CatLog::D(this, "make: caught ResultException e=" + e);
    //     mCmdParams = new CommandParams(cmdDet);
    //     sendCmdParams(e.result());
    //     return;
    // }
    if (!cmdPending) {
        SendCmdParams(ResultCode_OK);
    }
    return NOERROR;
}

ECode CommandParamsFactory::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    switch (what) {
    case MSG_ID_LOAD_ICON_DONE:
        SendCmdParams(SetIcons(obj));
        break;
    }
    return NOERROR;
}

ResultCode CommandParamsFactory::SetIcons(
    /* [in] */ IInterface* data)
{
    AutoPtr<ArrayOf<IBitmap*> > icons;
    // Int32 iconIndex = 0;

    if (data == NULL) {
        if (mloadIcon) {
            CatLog::D(IHandler::Probe(this), String("Optional Icon data is NULL"));
            mCmdParams->mLoadIconFailed = TRUE;
            mloadIcon = FALSE;
            /** In case of icon load fail consider the
             ** received proactive command as valid (sending RESULT OK) */
            return ResultCode_OK;
        }
        return ResultCode_PRFRMD_ICON_NOT_DISPLAYED;
    }
    switch(mIconLoadState) {
    case LOAD_SINGLE_ICON: {
        Boolean b = FALSE;
        mCmdParams->SetIcon(IBitmap::Probe(data), &b);
        break;
    }
    case LOAD_MULTI_ICONS: {
        AutoPtr<IArrayList> pArr = IArrayList::Probe(data);
        Int32 size = 0;
        pArr->GetSize(&size);
        icons = ArrayOf<IBitmap*>::Alloc(size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> p;
            pArr->Get(i, (IInterface**)&p);
            icons->Set(i, IBitmap::Probe(p));
        }
        // set each item icon.
        for (Int32 i = 0; i < icons->GetLength(); i++) {
            AutoPtr<IBitmap> icon = (*icons)[i];
            Boolean b = FALSE;
            mCmdParams->SetIcon(icon, &b);
            if (icon == NULL && mloadIcon) {
                CatLog::D(IHandler::Probe(this), String("Optional Icon data is NULL while loading multi icons"));
                mCmdParams->mLoadIconFailed = TRUE;
            }
        }
        break;
    }
    }
    return ResultCode_OK;
}

void CommandParamsFactory::SendCmdParams(
    /* [in] */ ResultCode resCode)
{
    mCaller->SendMsgParamsDecoded(resCode, mCmdParams);
}

AutoPtr<ComprehensionTlv> CommandParamsFactory::SearchForTag(
    /* [in] */ ComprehensionTlvTag tag,
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    AutoPtr<IIterator> iter;
    ctlvs->GetIterator((IIterator**)&iter);
    return SearchForNextTag(tag, iter);
}

AutoPtr<ComprehensionTlv> CommandParamsFactory::SearchForNextTag(
    /* [in] */ ComprehensionTlvTag tag,
    /* [in] */ IIterator/*<ComprehensionTlv*>*/* iter)
{
    Int32 tagValue = tag;
    Boolean bHasNext = FALSE;
    while ((iter->HasNext(&bHasNext), bHasNext)) {
        AutoPtr<IInterface> p;
        iter->GetNext((IInterface**)&p);
        AutoPtr<ComprehensionTlv> ctlv = (ComprehensionTlv*)(IObject*)p.Get();
        Int32 tag = 0;
        ctlv->GetTag(&tag);
        if (tag == tagValue) {
            return ctlv;
        }
    }
    return NULL;
}

Boolean CommandParamsFactory::ProcessDisplayText(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    CatLog::D(IHandler::Probe(this), String("process DisplayText"));

    AutoPtr<ITextMessage> _textMsg;
    CTextMessage::New((ITextMessage**)&_textMsg);
    AutoPtr<IconId> iconId;

    AutoPtr<ComprehensionTlv> ctlv = SearchForTag(ComprehensionTlvTag_TEXT_STRING,
            ctlvs);
    AutoPtr<CTextMessage> textMsg = (CTextMessage*)_textMsg.Get();
    if (ctlv != NULL) {
        textMsg->mText = ValueParser::RetrieveTextString(ctlv);
    }
    // If the tlv object doesn't exist or the it is a NULL object reply
    // with command not understood.
    if (textMsg->mText == NULL) {
        // throw new ResultException(ResultCode_CMD_DATA_NOT_UNDERSTOOD);
        return FALSE;
    }

    ctlv = SearchForTag(ComprehensionTlvTag_IMMEDIATE_RESPONSE, ctlvs);
    if (ctlv != NULL) {
        textMsg->mResponseNeeded = FALSE;
    }
    // parse icon identifier
    ctlv = SearchForTag(ComprehensionTlvTag_ICON_ID, ctlvs);
    if (ctlv != NULL) {
        iconId = ValueParser::RetrieveIconId(ctlv);
        textMsg->mIconSelfExplanatory = iconId->mSelfExplanatory;
    }
    // parse tone duration
    ctlv = SearchForTag(ComprehensionTlvTag_DURATION, ctlvs);
    if (ctlv != NULL) {
        textMsg->mDuration = ValueParser::RetrieveDuration(ctlv);
    }

    // Parse command qualifier parameters.
    textMsg->mIsHighPriority = (cmdDet->mCommandQualifier & 0x01) != 0;
    textMsg->mUserClear = (cmdDet->mCommandQualifier & 0x80) != 0;

    mCmdParams = new DisplayTextParams(cmdDet, _textMsg);

    if (iconId != NULL) {
        mloadIcon = TRUE;
        mIconLoadState = LOAD_SINGLE_ICON;
        AutoPtr<IMessage> msg;
        ObtainMessage(MSG_ID_LOAD_ICON_DONE, (IMessage**)&msg);
        mIconLoader->LoadIcon(iconId->mRecordNumber, msg);
        return TRUE;
    }
    return FALSE;
}

Boolean CommandParamsFactory::ProcessSetUpIdleModeText(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    CatLog::D(IHandler::Probe(this), String("process SetUpIdleModeText"));

    AutoPtr<ITextMessage> _textMsg;
    CTextMessage::New((ITextMessage**)&_textMsg);
    AutoPtr<IconId> iconId;
    AutoPtr<CTextMessage> textMsg = (CTextMessage*)_textMsg.Get();

    AutoPtr<ComprehensionTlv> ctlv = SearchForTag(ComprehensionTlvTag_TEXT_STRING,
            ctlvs);
    if (ctlv != NULL) {
        textMsg->mText = ValueParser::RetrieveTextString(ctlv);
    }

    ctlv = SearchForTag(ComprehensionTlvTag_ICON_ID, ctlvs);
    if (ctlv != NULL) {
        iconId = ValueParser::RetrieveIconId(ctlv);
        textMsg->mIconSelfExplanatory = iconId->mSelfExplanatory;
    }

    /*
     * If the tlv object doesn't contain text and the icon is not self
     * explanatory then reply with command not understood.
     */
    if (textMsg->mText == NULL && iconId != NULL && !textMsg->mIconSelfExplanatory) {
        // throw new ResultException(ResultCode_CMD_DATA_NOT_UNDERSTOOD);
        return FALSE;
    }
    mCmdParams = new DisplayTextParams(cmdDet, textMsg);

    if (iconId != NULL) {
        mloadIcon = TRUE;
        mIconLoadState = LOAD_SINGLE_ICON;
        AutoPtr<IMessage> msg;
        ObtainMessage(MSG_ID_LOAD_ICON_DONE, (IMessage**)&msg);
        mIconLoader->LoadIcon(iconId->mRecordNumber, msg);
        return TRUE;
    }
    return FALSE;
}

Boolean CommandParamsFactory::ProcessGetInkey(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    CatLog::D(IHandler::Probe(this), String("process GetInkey"));

    AutoPtr<IInput> _input;
    CInput::New((IInput**)&_input);
    AutoPtr<CInput> input = (CInput*)_input.Get();
    AutoPtr<IconId> iconId;

    AutoPtr<ComprehensionTlv> ctlv = SearchForTag(ComprehensionTlvTag_TEXT_STRING,
            ctlvs);
    if (ctlv != NULL) {
        input->mText = ValueParser::RetrieveTextString(ctlv);
    }
    else {
        // throw new ResultException(ResultCode_REQUIRED_VALUES_MISSING);
        return FALSE;
    }
    // parse icon identifier
    ctlv = SearchForTag(ComprehensionTlvTag_ICON_ID, ctlvs);
    if (ctlv != NULL) {
        iconId = ValueParser::RetrieveIconId(ctlv);
    }

    // parse duration
    ctlv = SearchForTag(ComprehensionTlvTag_DURATION, ctlvs);
    if (ctlv != NULL) {
        input->mDuration = ValueParser::RetrieveDuration(ctlv);
    }

    input->mMinLen = 1;
    input->mMaxLen = 1;

    input->mDigitOnly = (cmdDet->mCommandQualifier & 0x01) == 0;
    input->mUcs2 = (cmdDet->mCommandQualifier & 0x02) != 0;
    input->mYesNo = (cmdDet->mCommandQualifier & 0x04) != 0;
    input->mHelpAvailable = (cmdDet->mCommandQualifier & 0x80) != 0;
    input->mEcho = TRUE;

    mCmdParams = new GetInputParams(cmdDet, input);

    if (iconId != NULL) {
        mloadIcon = TRUE;
        mIconLoadState = LOAD_SINGLE_ICON;
        AutoPtr<IMessage> msg;
        ObtainMessage(MSG_ID_LOAD_ICON_DONE, (IMessage**)&msg);
        mIconLoader->LoadIcon(iconId->mRecordNumber, msg);
        return TRUE;
    }
    return FALSE;
}

Boolean CommandParamsFactory::ProcessGetInput(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    CatLog::D(IHandler::Probe(this), String("process GetInput"));

    AutoPtr<IInput> _input;
    CInput::New((IInput**)&_input);
    AutoPtr<IconId> iconId;
    AutoPtr<CInput> input = (CInput*)_input.Get();

    AutoPtr<ComprehensionTlv> ctlv = SearchForTag(ComprehensionTlvTag_TEXT_STRING,
            ctlvs);
    if (ctlv != NULL) {
        input->mText = ValueParser::RetrieveTextString(ctlv);
    }
    else {
        // throw new ResultException(ResultCode_REQUIRED_VALUES_MISSING);
        return FALSE;
    }

    ctlv = SearchForTag(ComprehensionTlvTag_RESPONSE_LENGTH, ctlvs);
    if (ctlv != NULL) {
        // try {
            AutoPtr<ArrayOf<Byte> > rawValue;
            ctlv->GetRawValue((ArrayOf<Byte>**)&rawValue);
            Int32 valueIndex = 0;
            ctlv->GetValueIndex(&valueIndex);
            input->mMinLen = (*rawValue)[valueIndex] & 0xff;
            input->mMaxLen = (*rawValue)[valueIndex + 1] & 0xff;
        // } catch (IndexOutOfBoundsException e) {
        //     throw new ResultException(ResultCode_CMD_DATA_NOT_UNDERSTOOD);
        // }
    }
    else {
        // throw new ResultException(ResultCode_REQUIRED_VALUES_MISSING);
        return FALSE;
    }

    ctlv = SearchForTag(ComprehensionTlvTag_DEFAULT_TEXT, ctlvs);
    if (ctlv != NULL) {
        input->mDefaultText = ValueParser::RetrieveTextString(ctlv);
    }
    // parse icon identifier
    ctlv = SearchForTag(ComprehensionTlvTag_ICON_ID, ctlvs);
    if (ctlv != NULL) {
        iconId = ValueParser::RetrieveIconId(ctlv);
    }

    input->mDigitOnly = (cmdDet->mCommandQualifier & 0x01) == 0;
    input->mUcs2 = (cmdDet->mCommandQualifier & 0x02) != 0;
    input->mEcho = (cmdDet->mCommandQualifier & 0x04) == 0;
    input->mPacked = (cmdDet->mCommandQualifier & 0x08) != 0;
    input->mHelpAvailable = (cmdDet->mCommandQualifier & 0x80) != 0;

    // Truncate the maxLen if it exceeds the max number of chars that can
    // be encoded. Limit depends on DCS in Command Qualifier.
    if (input->mUcs2 && input->mMaxLen > MAX_UCS2_CHARS) {
        CatLog::D(IHandler::Probe(this), String("UCS2: received maxLen = ")
                + StringUtils::ToString(input->mMaxLen)
                + String(", truncating to ") + StringUtils::ToString(MAX_UCS2_CHARS));
        input->mMaxLen = MAX_UCS2_CHARS;
    }
    else if (!input->mPacked && input->mMaxLen > MAX_GSM7_DEFAULT_CHARS) {
        CatLog::D(IHandler::Probe(this), String("GSM 7Bit Default: received maxLen = ")
                + StringUtils::ToString(input->mMaxLen) + String(", truncating to ")
                + StringUtils::ToString(MAX_GSM7_DEFAULT_CHARS));
        input->mMaxLen = MAX_GSM7_DEFAULT_CHARS;
    }

    mCmdParams = new GetInputParams(cmdDet, input);

    if (iconId != NULL) {
        mloadIcon = TRUE;
        mIconLoadState = LOAD_SINGLE_ICON;
        AutoPtr<IMessage> msg;
        ObtainMessage(MSG_ID_LOAD_ICON_DONE, (IMessage**)&msg);
        mIconLoader->LoadIcon(iconId->mRecordNumber, msg);
        return TRUE;
    }
    return FALSE;
}

Boolean CommandParamsFactory::ProcessSelectItem(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    CatLog::D(IHandler::Probe(this), String("process SelectItem"));

    AutoPtr<CCatMenu> menu;
    CCatMenu::NewByFriend((CCatMenu**)&menu);
    AutoPtr<IconId> titleIconId;
    AutoPtr<ItemsIconId> itemsIconId;
    AutoPtr<IIterator> iter;
    ctlvs->GetIterator((IIterator**)&iter);

    AutoPtr<ComprehensionTlv> ctlv = SearchForTag(ComprehensionTlvTag_ALPHA_ID,
            ctlvs);
    if (ctlv != NULL) {
        menu->mTitle = ValueParser::RetrieveAlphaId(ctlv);
    }

    while (TRUE) {
        ctlv = SearchForNextTag(ComprehensionTlvTag_ITEM, iter);
        if (ctlv != NULL) {
            menu->mItems->Add(ValueParser::RetrieveItem(ctlv));
        }
        else {
            break;
        }
    }

    // We must have at least one menu item.
    Int32 size = 0;
    menu->mItems->GetSize(&size);
    if (size == 0) {
        assert(0 && "ResultException");
        // throw new ResultException(ResultCode_REQUIRED_VALUES_MISSING);
        return FALSE;
    }

    ctlv = SearchForTag(ComprehensionTlvTag_ITEM_ID, ctlvs);
    if (ctlv != NULL) {
        // CAT items are listed 1...n while list start at 0, need to
        // subtract one.
        menu->mDefaultItem = ValueParser::RetrieveItemId(ctlv) - 1;
    }

    ctlv = SearchForTag(ComprehensionTlvTag_ICON_ID, ctlvs);
    if (ctlv != NULL) {
        mIconLoadState = LOAD_SINGLE_ICON;
        titleIconId = ValueParser::RetrieveIconId(ctlv);
        menu->mTitleIconSelfExplanatory = titleIconId->mSelfExplanatory;
    }

    ctlv = SearchForTag(ComprehensionTlvTag_ITEM_ICON_ID_LIST, ctlvs);
    if (ctlv != NULL) {
        mIconLoadState = LOAD_MULTI_ICONS;
        itemsIconId = ValueParser::RetrieveItemsIconId(ctlv);
        menu->mItemsIconSelfExplanatory = itemsIconId->mSelfExplanatory;
    }

    Boolean presentTypeSpecified = (cmdDet->mCommandQualifier & 0x01) != 0;
    if (presentTypeSpecified) {
        if ((cmdDet->mCommandQualifier & 0x02) == 0) {
            menu->mPresentationType = PresentationType_DATA_VALUES;
        }
        else {
            menu->mPresentationType = PresentationType_NAVIGATION_OPTIONS;
        }
    }
    menu->mSoftKeyPreferred = (cmdDet->mCommandQualifier & 0x04) != 0;
    menu->mHelpAvailable = (cmdDet->mCommandQualifier & 0x80) != 0;

    mCmdParams = new SelectItemParams(cmdDet, menu, titleIconId != NULL);

    // Load icons data if needed.
    switch(mIconLoadState) {
        case LOAD_NO_ICON:
            return FALSE;
        case LOAD_SINGLE_ICON: {
            mloadIcon = TRUE;
            AutoPtr<IMessage> msg;
            ObtainMessage(MSG_ID_LOAD_ICON_DONE, (IMessage**)&msg);
            mIconLoader->LoadIcon(titleIconId->mRecordNumber, msg);
            break;
        }
        case LOAD_MULTI_ICONS: {
            AutoPtr<ArrayOf<Int32> > recordNumbers = itemsIconId->mRecordNumbers;
            if (titleIconId != NULL) {
                // Create a new array for all the icons (title and items).
                recordNumbers = ArrayOf<Int32>::Alloc(itemsIconId->mRecordNumbers->GetLength() + 1);
                (*recordNumbers)[0] = titleIconId->mRecordNumber;
                // System::Arraycopy(itemsIconId->mRecordNumbers, 0, recordNumbers,
                //         1, itemsIconId->mRecordNumbers->GetLength());
                itemsIconId->mRecordNumbers->Copy(0, recordNumbers, 1, itemsIconId->mRecordNumbers->GetLength());
            }
            mloadIcon = TRUE;
            AutoPtr<IMessage> msg;
            ObtainMessage(MSG_ID_LOAD_ICON_DONE, (IMessage**)&msg);
            mIconLoader->LoadIcons(recordNumbers, msg);
            break;
        }
    }
    return TRUE;
}

Boolean CommandParamsFactory::ProcessEventNotify(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    CatLog::D(IHandler::Probe(this), String("process EventNotify"));

    AutoPtr<ITextMessage> _textMsg;
    CTextMessage::New((ITextMessage**)&_textMsg);
    AutoPtr<IconId> iconId;
    AutoPtr<CTextMessage> textMsg = (CTextMessage*)_textMsg.Get();

    AutoPtr<ComprehensionTlv> ctlv = SearchForTag(ComprehensionTlvTag_ALPHA_ID,
            ctlvs);
    textMsg->mText = ValueParser::RetrieveAlphaId(ctlv);

    ctlv = SearchForTag(ComprehensionTlvTag_ICON_ID, ctlvs);
    if (ctlv != NULL) {
        iconId = ValueParser::RetrieveIconId(ctlv);
        textMsg->mIconSelfExplanatory = iconId->mSelfExplanatory;
    }

    textMsg->mResponseNeeded = FALSE;
    // Samsung STK
    CommandType cmdType = cmdDet->mTypeOfCommand;
    AutoPtr<IResourcesHelper> hlp;
    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&hlp);
    AutoPtr<IResources> resr;
    hlp->GetSystem((IResources**)&resr);
    Boolean isSamsungStk = FALSE;
    resr->GetBoolean(R::bool_::config_samsung_stk, &isSamsungStk);
    if (cmdType == SEND_SMS && isSamsungStk) {
        String smscAddress(NULL);
        String pdu(NULL);

        ctlv = SearchForTag(ComprehensionTlvTag_ADDRESS, ctlvs);
        if (ctlv != NULL) {
            smscAddress = ValueParser::RetrieveSMSCaddress(ctlv);
            CatLog::D(IHandler::Probe(this), String("The smsc address is ") + smscAddress);
        }
        else {
            CatLog::D(IHandler::Probe(this), String("The smsc address is NULL"));
        }

        ctlv = SearchForTag(ComprehensionTlvTag_SMS_TPDU, ctlvs);
        if (ctlv != NULL) {
            pdu = ValueParser::RetrieveSMSTPDU(ctlv);
            CatLog::D(IHandler::Probe(this), String("The SMS tpdu is ") + pdu);
        }
        else {
            CatLog::D(IHandler::Probe(this), String("The SMS tpdu is NULL"));
        }
        mCmdParams = new SendSMSParams(cmdDet, textMsg, smscAddress, pdu);
    }
    else if (cmdType == SEND_USSD && isSamsungStk) {
        String ussdString(NULL);
        ctlv = SearchForTag(ComprehensionTlvTag_USSD_STRING, ctlvs);
        if (ctlv != NULL) {
            ussdString = ValueParser::RetrieveUSSDString(ctlv);
            CatLog::D(IHandler::Probe(this), String("The ussd string is ") + ussdString);
        }
        else {
            CatLog::D(IHandler::Probe(this), String("The ussd string is NULL"));
        }
        mCmdParams = new SendUSSDParams(cmdDet, textMsg, ussdString);
    }
    else {
        mCmdParams = new DisplayTextParams(cmdDet, textMsg);
    }

    if (iconId != NULL) {
        mloadIcon = TRUE;
        mIconLoadState = LOAD_SINGLE_ICON;
        AutoPtr<IMessage> msg;
        ObtainMessage(MSG_ID_LOAD_ICON_DONE, (IMessage**)&msg);
        mIconLoader->LoadIcon(iconId->mRecordNumber, msg);
        return TRUE;
    }
    return FALSE;
}

Boolean CommandParamsFactory::ProcessSetUpEventList(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    CatLog::D(IHandler::Probe(this), String("process SetUpEventList"));
    AutoPtr<ComprehensionTlv> ctlv = SearchForTag(ComprehensionTlvTag_EVENT_LIST, ctlvs);
    if (ctlv != NULL) {
        // try {
            AutoPtr<ArrayOf<Byte> > rawValue;
            ctlv->GetRawValue((ArrayOf<Byte>**)&rawValue);
            Int32 valueIndex = 0;
            ctlv->GetValueIndex(&valueIndex);
            Int32 valueLen = 0;
            ctlv->GetLength(&valueLen);
            AutoPtr<ArrayOf<Int32> > eventList = ArrayOf<Int32>::Alloc(valueLen);
            Int32 eventValue = -1;
            Int32 i = 0;
            while (valueLen > 0) {
                eventValue = (*rawValue)[valueIndex] & 0xff;
                valueIndex++;
                valueLen--;

                switch (eventValue) {
                    case ICatCmdMessageSetupEventListConstants::USER_ACTIVITY_EVENT:
                    case ICatCmdMessageSetupEventListConstants::IDLE_SCREEN_AVAILABLE_EVENT:
                    case ICatCmdMessageSetupEventListConstants::LANGUAGE_SELECTION_EVENT:
                    case ICatCmdMessageSetupEventListConstants::BROWSER_TERMINATION_EVENT:
                    case ICatCmdMessageSetupEventListConstants::BROWSING_STATUS_EVENT:
                    case ICatCmdMessageSetupEventListConstants::HCI_CONNECTIVITY_EVENT:
                        (*eventList)[i] = eventValue;
                        i++;
                        break;
                    default:
                        break;
                }

            }
            mCmdParams = new SetEventListParams(cmdDet, eventList);
        // } catch (IndexOutOfBoundsException e) {
        //     CatLog.e(this, " IndexOutofBoundException in processSetUpEventList");
        // }
    }
    return FALSE;
}

Boolean CommandParamsFactory::ProcessLaunchBrowser(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    CatLog::D(IHandler::Probe(this), String("process LaunchBrowser"));

    AutoPtr<ITextMessage> _confirmMsg;
    CTextMessage::New((ITextMessage**)&_confirmMsg);
    AutoPtr<IconId> iconId;
    String url(NULL);
    AutoPtr<CTextMessage> confirmMsg = (CTextMessage*)_confirmMsg.Get();

    AutoPtr<ComprehensionTlv> ctlv = SearchForTag(ComprehensionTlvTag_URL, ctlvs);
    if (ctlv != NULL) {
        // try {
            AutoPtr<ArrayOf<Byte> > rawValue;
            ctlv->GetRawValue((ArrayOf<Byte>**)&rawValue);
            Int32 valueIndex = 0;
            ctlv->GetValueIndex(&valueIndex);
            Int32 valueLen = 0;
            ctlv->GetLength(&valueLen);
            if (valueLen > 0) {
                AutoPtr<IGsmAlphabet> gsm;
                CGsmAlphabet::AcquireSingleton((IGsmAlphabet**)&gsm);
                gsm->Gsm8BitUnpackedToString(rawValue,
                        valueIndex, valueLen, &url);
            }
            else {
                url = NULL;
            }
        // } catch (IndexOutOfBoundsException e) {
        //     throw new ResultException(ResultCode_CMD_DATA_NOT_UNDERSTOOD);
        // }
    }

    // parse alpha identifier.
    ctlv = SearchForTag(ComprehensionTlvTag_ALPHA_ID, ctlvs);
    confirmMsg->mText = ValueParser::RetrieveAlphaId(ctlv);

    // parse icon identifier
    ctlv = SearchForTag(ComprehensionTlvTag_ICON_ID, ctlvs);
    if (ctlv != NULL) {
        iconId = ValueParser::RetrieveIconId(ctlv);
        confirmMsg->mIconSelfExplanatory = iconId->mSelfExplanatory;
    }

    // parse command qualifier value.
    LaunchBrowserMode mode;
    switch (cmdDet->mCommandQualifier) {
    case 0x00:
    default:
        mode = LAUNCH_IF_NOT_ALREADY_LAUNCHED;
        break;
    case 0x02:
        mode = USE_EXISTING_BROWSER;
        break;
    case 0x03:
        mode = LAUNCH_NEW_BROWSER;
        break;
    }

    mCmdParams = new LaunchBrowserParams(cmdDet, confirmMsg, url, mode);

    if (iconId != NULL) {
        mIconLoadState = LOAD_SINGLE_ICON;
        AutoPtr<IMessage> msg;
        ObtainMessage(MSG_ID_LOAD_ICON_DONE, (IMessage**)&msg);
        mIconLoader->LoadIcon(iconId->mRecordNumber, msg);
        return TRUE;
    }
    return FALSE;
}

Boolean CommandParamsFactory::ProcessPlayTone(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    CatLog::D(IHandler::Probe(this), String("process PlayTone"));

    Tone tone = Tone_NONE;
    AutoPtr<ITextMessage> _textMsg;
    CTextMessage::New((ITextMessage**)&_textMsg);
    AutoPtr<IDuration> duration;
    AutoPtr<IconId> iconId;
    AutoPtr<CTextMessage> textMsg = (CTextMessage*)_textMsg.Get();

    AutoPtr<ComprehensionTlv> ctlv = SearchForTag(ComprehensionTlvTag_TONE, ctlvs);
    if (ctlv != NULL) {
        // Nothing to do for NULL objects.
        Int32 length = 0;
        ctlv->GetLength(&length);
        if (length > 0) {
            // try {
                AutoPtr<ArrayOf<Byte> > rawValue;
                ctlv->GetRawValue((ArrayOf<Byte>**)&rawValue);
                Int32 valueIndex = 0;
                ctlv->GetValueIndex(&valueIndex);
                Int32 toneVal = (*rawValue)[valueIndex];
                tone = toneVal;
            // } catch (IndexOutOfBoundsException e) {
            //     throw new ResultException(
            //             ResultCode_CMD_DATA_NOT_UNDERSTOOD);
            // }
        }
    }
    // parse alpha identifier
    ctlv = SearchForTag(ComprehensionTlvTag_ALPHA_ID, ctlvs);
    if (ctlv != NULL) {
        textMsg->mText = ValueParser::RetrieveAlphaId(ctlv);
        // Assign the tone message text to empty string, if alpha identifier
        // data is NULL. If no alpha identifier tlv is present, then tone
        // message text will be NULL.
        if (textMsg->mText == NULL) {
            textMsg->mText = String("");
        }
    }
    // parse tone duration
    ctlv = SearchForTag(ComprehensionTlvTag_DURATION, ctlvs);
    if (ctlv != NULL) {
        duration = ValueParser::RetrieveDuration(ctlv);
    }
    // parse icon identifier
    ctlv = SearchForTag(ComprehensionTlvTag_ICON_ID, ctlvs);
    if (ctlv != NULL) {
        iconId = ValueParser::RetrieveIconId(ctlv);
        textMsg->mIconSelfExplanatory = iconId->mSelfExplanatory;
    }

    Boolean vibrate = (cmdDet->mCommandQualifier & 0x01) != 0x00;

    textMsg->mResponseNeeded = FALSE;
    mCmdParams = new PlayToneParams(cmdDet, textMsg, tone, duration, vibrate);

    if (iconId != NULL) {
        mIconLoadState = LOAD_SINGLE_ICON;
        AutoPtr<IMessage> msg;
        ObtainMessage(MSG_ID_LOAD_ICON_DONE, (IMessage**)&msg);
        mIconLoader->LoadIcon(iconId->mRecordNumber, msg);
        return TRUE;
    }
    return FALSE;
}

Boolean CommandParamsFactory::ProcessSetupCall(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    CatLog::D(IHandler::Probe(this), String("process SetupCall"));

    AutoPtr<IIterator> iter;
    ctlvs->GetIterator((IIterator**)&iter);
    AutoPtr<ComprehensionTlv> ctlv;
    // User confirmation phase message.
    AutoPtr<ITextMessage> _confirmMsg;
    CTextMessage::New((ITextMessage**)&_confirmMsg);
    AutoPtr<CTextMessage> confirmMsg = (CTextMessage*)_confirmMsg.Get();
    // Call set up phase message.
    AutoPtr<ITextMessage> _callMsg;
    CTextMessage::New((ITextMessage**)&_callMsg);
    AutoPtr<CTextMessage> callMsg = (CTextMessage*)_callMsg.Get();
    AutoPtr<IconId> confirmIconId;
    AutoPtr<IconId> callIconId;

    // get confirmation message string.
    ctlv = SearchForNextTag(ComprehensionTlvTag_ALPHA_ID, iter);
    confirmMsg->mText = ValueParser::RetrieveAlphaId(ctlv);

    ctlv = SearchForTag(ComprehensionTlvTag_ICON_ID, ctlvs);
    if (ctlv != NULL) {
        confirmIconId = ValueParser::RetrieveIconId(ctlv);
        confirmMsg->mIconSelfExplanatory = confirmIconId->mSelfExplanatory;
    }

    // get call set up message string.
    ctlv = SearchForNextTag(ComprehensionTlvTag_ALPHA_ID, iter);
    if (ctlv != NULL) {
        callMsg->mText = ValueParser::RetrieveAlphaId(ctlv);
    }

    ctlv = SearchForTag(ComprehensionTlvTag_ICON_ID, ctlvs);
    if (ctlv != NULL) {
        callIconId = ValueParser::RetrieveIconId(ctlv);
        callMsg->mIconSelfExplanatory = callIconId->mSelfExplanatory;
    }

    mCmdParams = new CallSetupParams(cmdDet, confirmMsg, callMsg);

    if (confirmIconId != NULL || callIconId != NULL) {
        mIconLoadState = LOAD_MULTI_ICONS;
        AutoPtr<ArrayOf<Int32> > recordNumbers = ArrayOf<Int32>::Alloc(2);
        (*recordNumbers)[0] = confirmIconId != NULL
                ? confirmIconId->mRecordNumber : -1;
        (*recordNumbers)[1] = callIconId != NULL ? callIconId->mRecordNumber
                : -1;

        AutoPtr<IMessage> msg;
        ObtainMessage(MSG_ID_LOAD_ICON_DONE, (IMessage**)&msg);
        mIconLoader->LoadIcons(recordNumbers, msg);
        return TRUE;
    }
    return FALSE;
}

Boolean CommandParamsFactory::ProcessProvideLocalInfo(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    CatLog::D(IHandler::Probe(this), String("process ProvideLocalInfo"));
    switch (cmdDet->mCommandQualifier) {
        case DTTZ_SETTING:
            CatLog::D(IHandler::Probe(this), String("PLI [DTTZ_SETTING]"));
            mCmdParams = new CommandParams(cmdDet);
            break;
        case LANGUAGE_SETTING:
            CatLog::D(IHandler::Probe(this), String("PLI [LANGUAGE_SETTING]"));
            mCmdParams = new CommandParams(cmdDet);
            break;
        default:
            CatLog::D(IHandler::Probe(this), String("PLI[") + StringUtils::ToString(cmdDet->mCommandQualifier)
                    + String("] Command Not Supported"));
            mCmdParams = new CommandParams(cmdDet);
            // throw new ResultException(ResultCode_BEYOND_TERMINAL_CAPABILITY);
    }
    return FALSE;
}

Boolean CommandParamsFactory::ProcessBIPClient(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    CommandType commandType = cmdDet->mTypeOfCommand;
    if (commandType != -1) {
        CatLog::D(this->Probe(EIID_IInterface), String("process ") + StringUtils::ToString(commandType));
    }

    AutoPtr<ITextMessage> _textMsg;
    CTextMessage::New((ITextMessage**)&_textMsg);
    AutoPtr<IconId> iconId;
    AutoPtr<ComprehensionTlv> ctlv;
    Boolean has_alpha_id = FALSE;
    AutoPtr<CTextMessage> textMsg = (CTextMessage*)_textMsg.Get();

    // parse alpha identifier
    ctlv = SearchForTag(ComprehensionTlvTag_ALPHA_ID, ctlvs);
    if (ctlv != NULL) {
        textMsg->mText = ValueParser::RetrieveAlphaId(ctlv);
        CatLog::D(IHandler::Probe(this), String("alpha TLV text=") + textMsg->mText);
        has_alpha_id = TRUE;
    }

    // parse icon identifier
    ctlv = SearchForTag(ComprehensionTlvTag_ICON_ID, ctlvs);
    if (ctlv != NULL) {
        iconId = ValueParser::RetrieveIconId(ctlv);
        textMsg->mIconSelfExplanatory = iconId->mSelfExplanatory;
    }

    textMsg->mResponseNeeded = FALSE;
    mCmdParams = new BIPClientParams(cmdDet, textMsg, has_alpha_id);

    if (iconId != NULL) {
        mIconLoadState = LOAD_SINGLE_ICON;
        AutoPtr<IMessage> msg;
        ObtainMessage(MSG_ID_LOAD_ICON_DONE, (IMessage**)&msg);
        mIconLoader->LoadIcon(iconId->mRecordNumber, msg);
        return TRUE;
    }
    return FALSE;
}

Boolean CommandParamsFactory::ProcessActivate(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs)
{
    CommandType commandType = cmdDet->mTypeOfCommand;
    assert(0 && "TODO");
    // CatLog::D(IHandler::Probe(this), String("process ") + commandType.name());

    AutoPtr<ComprehensionTlv> ctlv;
    Int32 target = 0;

    //parse activate descriptor
    ctlv = SearchForTag(ComprehensionTlvTag_ACTIVATE_DESCRIPTOR, ctlvs);
    if (ctlv != NULL) {
        target = ValueParser::RetrieveTarget(ctlv);
        mCmdParams = new CommandParams(cmdDet);
        CatLog::D(IHandler::Probe(this), String("Activate cmd target = ") + StringUtils::ToString(target));
    }
    else {
        CatLog::D(IHandler::Probe(this), String("ctlv is NULL"));
    }
    return FALSE;
}

ECode CommandParamsFactory::Dispose()
{
    mIconLoader->Dispose();
    mIconLoader = NULL;
    mCmdParams = NULL;
    mCaller = NULL;
    sInstance = NULL;
    return NOERROR;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
