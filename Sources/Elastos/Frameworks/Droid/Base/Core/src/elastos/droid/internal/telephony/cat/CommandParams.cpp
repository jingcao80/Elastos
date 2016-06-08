
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/CommandParams.h"

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
    // ==================before translated======================
    // mCmdDet = cmdDet;
}

ECode CommandParams::GetCommandType(
    /* [out] */ CommandType* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return AppInterface.CommandType.fromInt(mCmdDet.typeOfCommand);
    assert(0);
    return NOERROR;
}

ECode CommandParams::SetIcon(
    /* [in] */ IBitmap* icon,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return true;
    assert(0);
    return NOERROR;
}

ECode CommandParams::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCmdDet.toString();
    assert(0);
    return NOERROR;
}

//=====================================================================
//                          DisplayTextParams
//=====================================================================
DisplayTextParams::DisplayTextParams(
    /* [in] */ CommandDetails* cmdDet,
    /* [in] */ ITextMessage* textMsg)
    : CommandParams(cmdDet)
{
    // ==================before translated======================
    // super(cmdDet);
    // mTextMsg = textMsg;
}

ECode DisplayTextParams::SetIcon(
    /* [in] */ IBitmap* icon,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (icon != null && mTextMsg != null) {
    //     mTextMsg.icon = icon;
    //     return true;
    // }
    // return false;
    assert(0);
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
    // ==================before translated======================
    // super(cmdDet);
    // mConfirmMsg = confirmMsg;
    // mMode = mode;
    // mUrl = url;
}

ECode LaunchBrowserParams::SetIcon(
    /* [in] */ IBitmap* icon,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (icon != null && mConfirmMsg != null) {
    //     mConfirmMsg.icon = icon;
    //     return true;
    // }
    // return false;
    assert(0);
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
    // ==================before translated======================
    // super(cmdDet);
    // this.mEventInfo = eventInfo;
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
    // ==================before translated======================
    // super(cmdDet);
    // mTextMsg = textMsg;
    // mSettings = new ToneSettings(duration, tone, vibrate);
}

ECode PlayToneParams::SetIcon(
    /* [in] */ IBitmap* icon,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (icon != null && mTextMsg != null) {
    //     mTextMsg.icon = icon;
    //     return true;
    // }
    // return false;
    assert(0);
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
    // ==================before translated======================
    // super(cmdDet);
    // mConfirmMsg = confirmMsg;
    // mCallMsg = callMsg;
}

ECode CallSetupParams::SetIcon(
    /* [in] */ IBitmap* icon,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (icon == null) {
    //     return false;
    // }
    // if (mConfirmMsg != null && mConfirmMsg.icon == null) {
    //     mConfirmMsg.icon = icon;
    //     return true;
    // } else if (mCallMsg != null && mCallMsg.icon == null) {
    //     mCallMsg.icon = icon;
    //     return true;
    // }
    // return false;
    assert(0);
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
    // ==================before translated======================
    // super(cmdDet);
    // mMenu = menu;
    // mLoadTitleIcon = loadTitleIcon;
}

ECode SelectItemParams::SetIcon(
    /* [in] */ IBitmap* icon,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (icon != null && mMenu != null) {
    //     if (mLoadTitleIcon && mMenu.titleIcon == null) {
    //         mMenu.titleIcon = icon;
    //     } else {
    //         for (Item item : mMenu.items) {
    //             if (item.icon != null) {
    //                 continue;
    //             }
    //             item.icon = icon;
    //             break;
    //         }
    //     }
    //     return true;
    // }
    // return false;
    assert(0);
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
    // ==================before translated======================
    // super(cmdDet);
    // mInput = input;
}

ECode GetInputParams::SetIcon(
    /* [in] */ IBitmap* icon,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (icon != null && mInput != null) {
    //     mInput.icon = icon;
    // }
    // return true;
    assert(0);
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
    // ==================before translated======================
    // super(cmdDet);
    // mTextMsg = textMsg;
    // mHasAlphaId = has_alpha_id;
}

ECode BIPClientParams::SetIcon(
    /* [in] */ IBitmap* icon,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (icon != null && mTextMsg != null) {
    //     mTextMsg.icon = icon;
    //     return true;
    // }
    // return false;
    assert(0);
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
    // ==================before translated======================
    // super(cmdDet);
    // mActivateTarget = target;
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
    // ==================before translated======================
    // super(cmdDet, textmessage);
    // smscAddress = smscaddress;
    // pdu = smsPdu;
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
    // ==================before translated======================
    // super(cmdDet, textmessage);
    // ussdString = ussdstring;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
