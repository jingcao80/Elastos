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

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/CommandParams.h"
#include "elastos/droid/internal/telephony/cat/CTextMessage.h"
#include "elastos/droid/internal/telephony/cat/CInput.h"
#include "elastos/droid/internal/telephony/cat/CToneSettings.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

//=====================================================================
//                            CommandParams
//=====================================================================
CommandParams::CommandParams(
    /* [in] */ CommandDetails* cmdDet)
{
    mCmdDet = cmdDet;
}

ECode CommandParams::GetCommandType(
    /* [out] */ CommandType* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCmdDet->mTypeOfCommand;
    return NOERROR;
}

ECode CommandParams::SetIcon(
    /* [in] */ IBitmap* icon,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode CommandParams::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return mCmdDet->ToString(result);
}

//=====================================================================
//                          DisplayTextParams
//=====================================================================
DisplayTextParams::DisplayTextParams(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ ITextMessage* textMsg)
    : CommandParams(cmdDet)
{
    mTextMsg = textMsg;
}

ECode DisplayTextParams::SetIcon(
    /* [in] */ IBitmap* icon,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (icon != NULL && mTextMsg != NULL) {
        AutoPtr<CTextMessage> _mTextMsg = (CTextMessage*)mTextMsg.Get();
        _mTextMsg->mIcon = icon;
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

//=====================================================================
//                         LaunchBrowserParams
//=====================================================================
LaunchBrowserParams::LaunchBrowserParams(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ ITextMessage* confirmMsg,
    /* [in] */ const String& url,
    /* [in] */ LaunchBrowserMode mode)
    : CommandParams(cmdDet)
{
    mConfirmMsg = confirmMsg;
    mMode = mode;
    mUrl = url;
}

ECode LaunchBrowserParams::SetIcon(
    /* [in] */ IBitmap* icon,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (icon != NULL && mConfirmMsg != NULL) {
        AutoPtr<CTextMessage> _mConfirmMsg = (CTextMessage*)mConfirmMsg.Get();
        _mConfirmMsg->mIcon = icon;
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

//=====================================================================
//                          SetEventListParams
//=====================================================================
SetEventListParams::SetEventListParams(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ ArrayOf<Int32>* eventInfo)
    : CommandParams(cmdDet)
{
    mEventInfo = eventInfo;
}

//=====================================================================
//                            PlayToneParams
//=====================================================================
PlayToneParams::PlayToneParams(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ ITextMessage* textMsg,
    /* [in] */ Tone tone,
    /* [in] */ IDuration* duration,
    /* [in] */ Boolean vibrate)
    : CommandParams(cmdDet)
{
    mTextMsg = textMsg;
    CToneSettings::New(duration, tone, vibrate, (IToneSettings**)&mSettings);
}

ECode PlayToneParams::SetIcon(
    /* [in] */ IBitmap* icon,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (icon != NULL && mTextMsg != NULL) {
        AutoPtr<CTextMessage> _mTextMsg = (CTextMessage*)mTextMsg.Get();
        _mTextMsg->mIcon = icon;
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

//=====================================================================
//                           CallSetupParams
//=====================================================================
CallSetupParams::CallSetupParams(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ ITextMessage* confirmMsg,
    /* [in] */ ITextMessage* callMsg)
    : CommandParams(cmdDet)
{
    mConfirmMsg = confirmMsg;
    mCallMsg = callMsg;
}

ECode CallSetupParams::SetIcon(
    /* [in] */ IBitmap* icon,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (icon == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<CTextMessage> _mConfirmMsg = (CTextMessage*)mConfirmMsg.Get();
    AutoPtr<CTextMessage> _mCallMsg = (CTextMessage*)mCallMsg.Get();
    if (_mConfirmMsg != NULL && _mConfirmMsg->mIcon == NULL) {
        _mConfirmMsg->mIcon = icon;
        *result = TRUE;
        return NOERROR;
    }
    else if (_mCallMsg != NULL && _mCallMsg->mIcon == NULL) {
        _mCallMsg->mIcon = icon;
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

//=====================================================================
//                           SelectItemParams
//=====================================================================
SelectItemParams::SelectItemParams(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ IMenu* menu,
    /* [in] */ Boolean loadTitleIcon)
    : CommandParams(cmdDet)
{
    mMenu = menu;
    mLoadTitleIcon = loadTitleIcon;
}

ECode SelectItemParams::SetIcon(
    /* [in] */ IBitmap* icon,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (icon != NULL && mMenu != NULL) {
        assert(0 && "TODO");
        // if (mLoadTitleIcon && mMenu->mTitleIcon == NULL) {
        //     mMenu->mTitleIcon = icon;
        // }
        // else {
        //     for (Item item : mMenu->mItems) {
        //         if (item->mIcon != NULL) {
        //             continue;
        //         }
        //         item->mIcon = icon;
        //         break;
        //     }
        // }
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

//=====================================================================
//                            GetInputParams
//=====================================================================
GetInputParams::GetInputParams(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ IInput* input)
    : CommandParams(cmdDet)
{
    mInput = input;
}

ECode GetInputParams::SetIcon(
    /* [in] */ IBitmap* icon,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (icon != NULL && mInput != NULL) {
        AutoPtr<CInput> _mInput = (CInput*)mInput.Get();
        _mInput->mIcon = icon;
    }
    *result = TRUE;
    return NOERROR;
}

//=====================================================================
//                           BIPClientParams
//=====================================================================
BIPClientParams::BIPClientParams(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ ITextMessage* textMsg,
    /* [in] */ Boolean has_alpha_id)
    : CommandParams(cmdDet)
{
    mTextMsg = textMsg;
    mHasAlphaId = has_alpha_id;
}

ECode BIPClientParams::SetIcon(
    /* [in] */ IBitmap* icon,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (icon != NULL && mTextMsg != NULL) {
        AutoPtr<CTextMessage> _mTextMsg = (CTextMessage*)mTextMsg.Get();
        _mTextMsg->mIcon = icon;
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

//=====================================================================
//                            ActivateParams
//=====================================================================
ActivateParams::ActivateParams(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ Int32 target)
    : CommandParams(cmdDet)
{
    mActivateTarget = target;
}

//=====================================================================
//                            SendSMSParams
//=====================================================================
SendSMSParams::SendSMSParams(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ ITextMessage* textmessage,
    /* [in] */ const String& smscaddress,
    /* [in] */ const String& smsPdu)
    : DisplayTextParams(cmdDet, textmessage)
{
    mSmscAddress = smscaddress;
    mPdu = smsPdu;
}

//=====================================================================
//                            SendUSSDParams
//=====================================================================
SendUSSDParams::SendUSSDParams(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ ITextMessage* textmessage,
    /* [in] */ const String& ussdstring)
    : DisplayTextParams(cmdDet, textmessage)
{
    mUssdString = ussdstring;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
