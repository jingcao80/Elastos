
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/CatCmdMessage.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

////=====================================================================
////                CatCmdMessage::BrowserSettings
////=====================================================================
CAR_INTERFACE_IMPL(CatCmdMessage::BrowserSettings, Object, IBrowserSettings);

ECode CatCmdMessage::BrowserSettings::SetUrl(
    /* [in] */ const String& _url)
{
    url = _url;
    return NOERROR;
}

ECode CatCmdMessage::BrowserSettings::GetUrl(
    /* [out] */ String* _url)
{
    VALIDATE_NOT_NULL(_url);
    *_url = url;
    return NOERROR;
}

ECode CatCmdMessage::BrowserSettings::SetMode(
    /* [in] */ LaunchBrowserMode _mode)
{
    mode = _mode;
    return NOERROR;
}

ECode CatCmdMessage::BrowserSettings::GetMode(
    /* [out] */ LaunchBrowserMode* _mode)
{
    VALIDATE_NOT_NULL(_mode);
    *_mode = mode;
    return NOERROR;
}

////=====================================================================
////                CatCmdMessage::CallSettings
////=====================================================================
CAR_INTERFACE_IMPL(CatCmdMessage::CallSettings, Object, ICallSettings);

ECode CatCmdMessage::CallSettings::SetConfirmMsg(
    /* [in] */ ITextMessage* _confirmMsg)
{
    confirmMsg = _confirmMsg;
    return NOERROR;
}

ECode CatCmdMessage::CallSettings::GetConfirmMsg(
    /* [out] */ ITextMessage** result)
{
    VALIDATE_NOT_NULL(result);
    *result = confirmMsg;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CatCmdMessage::CallSettings::SetCallMsg(
    /* [in] */ ITextMessage* _callMsg)
{
    callMsg = _callMsg;
    return NOERROR;
}

ECode CatCmdMessage::CallSettings::GetCallMsg(
    /* [out] */ ITextMessage** result)
{
    VALIDATE_NOT_NULL(result);
    *result = callMsg;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

////=====================================================================
////                CatCmdMessage::SetupEventListSettings
////=====================================================================
CAR_INTERFACE_IMPL(CatCmdMessage::SetupEventListSettings, Object, ISetupEventListSettings);

ECode CatCmdMessage::SetupEventListSettings::SetEventList(
    /* [in] */ ArrayOf<Int32>* _eventList)
{
    eventList = _eventList;
    return NOERROR;
}

ECode CatCmdMessage::SetupEventListSettings::GetEventList(
    /* [out] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = eventList;
    REFCOUNT_ADD(*result);
    return NOERROR;
}


//const Int32 CatCmdMessage::SetupEventListConstants::USER_ACTIVITY_EVENT;
//const Int32 CatCmdMessage::SetupEventListConstants::IDLE_SCREEN_AVAILABLE_EVENT;
//const Int32 CatCmdMessage::SetupEventListConstants::LANGUAGE_SELECTION_EVENT;
//const Int32 CatCmdMessage::SetupEventListConstants::BROWSER_TERMINATION_EVENT;
//const Int32 CatCmdMessage::SetupEventListConstants::BROWSING_STATUS_EVENT;
//const Int32 CatCmdMessage::SetupEventListConstants::HCI_CONNECTIVITY_EVENT;
//const Int32 CatCmdMessage::BrowserTerminationCauses::USER_TERMINATION;
//const Int32 CatCmdMessage::BrowserTerminationCauses::ERROR_TERMINATION;

////=====================================================================
////                CatCmdMessage::InnerParcelableCreator
////=====================================================================
//CatCmdMessage::InnerParcelableCreator::InnerParcelableCreator(
//    /* [in] */ CatCmdMessage* owner)
//    : mOwner(owner)
//{
//    // ==================before translated======================
//    // mOwner = owner;
//}
//
//ECode CatCmdMessage::InnerParcelableCreator::CreateFromParcel(
//    /* [in] */ IParcel* in,
//    /* [out] */ CatCmdMessage** result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return new CatCmdMessage(in);
//    assert(0);
//    return NOERROR;
//}
//
//ECode CatCmdMessage::InnerParcelableCreator::NewArray(
//    /* [in] */ Int32 size,
//    /* [out] */ CatCmdMessage[]** result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return new CatCmdMessage[size];
//    assert(0);
//    return NOERROR;
//}

//=====================================================================
//                            CatCmdMessage
//=====================================================================
CAR_INTERFACE_IMPL_2(CatCmdMessage, Object, ICatCmdMessage, IParcelable);

const Int32 CatCmdMessage::REFRESH_NAA_INIT_AND_FULL_FILE_CHANGE;
const Int32 CatCmdMessage::REFRESH_NAA_INIT_AND_FILE_CHANGE;
const Int32 CatCmdMessage::REFRESH_NAA_INIT;
const Int32 CatCmdMessage::REFRESH_UICC_RESET;
//const AutoPtr<IParcelable> AutoPtr< ::Creator<CatCmdMessage> > CatCmdMessage::CREATOR = new InnerParcelableCreator(this);

CatCmdMessage::CatCmdMessage()
{
}

ECode CatCmdMessage::constructor()
{
    return NOERROR;
}

ECode CatCmdMessage::constructor(
    /* [in] */ /*CommandParams*/IInterface* _cmdParams)
{
    AutoPtr<CommandParams> cmdParams = (CommandParams*)(IObject::Probe(_cmdParams));
    // ==================before translated======================
    // mCmdDet = cmdParams.mCmdDet;
    // mLoadIconFailed =  cmdParams.mLoadIconFailed;
    // switch(getCmdType()) {
    // case SET_UP_MENU:
    // case SELECT_ITEM:
    //     mMenu = ((SelectItemParams) cmdParams).mMenu;
    //     break;
    // case DISPLAY_TEXT:
    // case SET_UP_IDLE_MODE_TEXT:
    // case SEND_DTMF:
    // case SEND_SMS:
    // case REFRESH:
    // case SEND_SS:
    // case SEND_USSD:
    //     mTextMsg = ((DisplayTextParams) cmdParams).mTextMsg;
    //     break;
    // case GET_INPUT:
    // case GET_INKEY:
    //     mInput = ((GetInputParams) cmdParams).mInput;
    //     break;
    // case LAUNCH_BROWSER:
    //     mTextMsg = ((LaunchBrowserParams) cmdParams).mConfirmMsg;
    //     mBrowserSettings = new BrowserSettings();
    //     mBrowserSettings.url = ((LaunchBrowserParams) cmdParams).mUrl;
    //     mBrowserSettings.mode = ((LaunchBrowserParams) cmdParams).mMode;
    //     break;
    // case PLAY_TONE:
    //     PlayToneParams params = (PlayToneParams) cmdParams;
    //     mToneSettings = params.mSettings;
    //     mTextMsg = params.mTextMsg;
    //     break;
    // case GET_CHANNEL_STATUS:
    //     mTextMsg = ((CallSetupParams) cmdParams).mConfirmMsg;
    //     break;
    // case SET_UP_CALL:
    //     mCallSettings = new CallSettings();
    //     mCallSettings.confirmMsg = ((CallSetupParams) cmdParams).mConfirmMsg;
    //     mCallSettings.callMsg = ((CallSetupParams) cmdParams).mCallMsg;
    //     break;
    // case OPEN_CHANNEL:
    // case CLOSE_CHANNEL:
    // case RECEIVE_DATA:
    // case SEND_DATA:
    //     BIPClientParams param = (BIPClientParams) cmdParams;
    //     mTextMsg = param.mTextMsg;
    //     break;
    // case SET_UP_EVENT_LIST:
    //     mSetupEventListSettings = new SetupEventListSettings();
    //     mSetupEventListSettings.eventList = ((SetEventListParams) cmdParams).mEventInfo;
    //     break;
    // case ACTIVATE:
    // case PROVIDE_LOCAL_INFORMATION:
    // default:
    //     break;
    // }
    return NOERROR;
}

ECode CatCmdMessage::constructor(
    /* [in] */ IParcel* in)
{
    // ==================before translated======================
    // mCmdDet = in.readParcelable(null);
    // mTextMsg = in.readParcelable(null);
    // mMenu = in.readParcelable(null);
    // mInput = in.readParcelable(null);
    // mLoadIconFailed = (in.readByte() == 1);
    // switch (getCmdType()) {
    // case LAUNCH_BROWSER:
    //     mBrowserSettings = new BrowserSettings();
    //     mBrowserSettings.url = in.readString();
    //     mBrowserSettings.mode = LaunchBrowserMode.values()[in.readInt()];
    //     break;
    // case PLAY_TONE:
    //     mToneSettings = in.readParcelable(null);
    //     break;
    // case SET_UP_CALL:
    //     mCallSettings = new CallSettings();
    //     mCallSettings.confirmMsg = in.readParcelable(null);
    //     mCallSettings.callMsg = in.readParcelable(null);
    //     break;
    // case SET_UP_EVENT_LIST:
    //     mSetupEventListSettings = new SetupEventListSettings();
    //     int length = in.readInt();
    //     mSetupEventListSettings.eventList = new int[length];
    //     for (int i = 0; i < length; i++) {
    //         mSetupEventListSettings.eventList[i] = in.readInt();
    //     }
    //     break;
    // default:
    //     break;
    // }
    return NOERROR;
}

ECode CatCmdMessage::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    // ==================before translated======================
    // dest.writeParcelable(mCmdDet, 0);
    // dest.writeParcelable(mTextMsg, 0);
    // dest.writeParcelable(mMenu, 0);
    // dest.writeParcelable(mInput, 0);
    // dest.writeByte((byte) (mLoadIconFailed ? 1 : 0));
    // switch(getCmdType()) {
    // case LAUNCH_BROWSER:
    //     dest.writeString(mBrowserSettings.url);
    //     dest.writeInt(mBrowserSettings.mode.ordinal());
    //     break;
    // case PLAY_TONE:
    //     dest.writeParcelable(mToneSettings, 0);
    //     break;
    // case SET_UP_CALL:
    //     dest.writeParcelable(mCallSettings.confirmMsg, 0);
    //     dest.writeParcelable(mCallSettings.callMsg, 0);
    //     break;
    // case SET_UP_EVENT_LIST:
    //     dest.writeIntArray(mSetupEventListSettings.eventList);
    //     break;
    // default:
    //     break;
    // }
    assert(0);
    return NOERROR;
}

ECode CatCmdMessage::ReadFromParcel(
        /* [in] */ IParcel* source)
{
    constructor(source);
    return NOERROR;
}

//ECode CatCmdMessage::DescribeContents(
//    /* [out] */ Int32* result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return 0;
//    assert(0);
//    return NOERROR;
//}

ECode CatCmdMessage::GetCmdType(
    /* [out] */ CommandType* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return AppInterface.CommandType.fromInt(mCmdDet.typeOfCommand);
    assert(0);
    return NOERROR;
}

ECode CatCmdMessage::GetMenu(
    /* [out] */ IMenu** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mMenu;
    assert(0);
    return NOERROR;
}

ECode CatCmdMessage::GeInput(
    /* [out] */ IInput** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mInput;
    assert(0);
    return NOERROR;
}

ECode CatCmdMessage::GeTextMessage(
    /* [out] */ ITextMessage** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mTextMsg;
    assert(0);
    return NOERROR;
}

ECode CatCmdMessage::GetBrowserSettings(
    /* [out] */ IBrowserSettings** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mBrowserSettings;
    assert(0);
    return NOERROR;
}

ECode CatCmdMessage::GetToneSettings(
    /* [out] */ IToneSettings** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mToneSettings;
    assert(0);
    return NOERROR;
}

ECode CatCmdMessage::GetCallSettings(
    /* [out] */ ICallSettings** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCallSettings;
    assert(0);
    return NOERROR;
}

ECode CatCmdMessage::HasIconLoadFailed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mLoadIconFailed;
    assert(0);
    return NOERROR;
}

ECode CatCmdMessage::IsRefreshResetOrInit(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if ((mCmdDet.commandQualifier == REFRESH_NAA_INIT_AND_FULL_FILE_CHANGE)
    //     || (mCmdDet.commandQualifier == REFRESH_NAA_INIT_AND_FILE_CHANGE )
    //     || (mCmdDet.commandQualifier == REFRESH_NAA_INIT)
    //     || (mCmdDet.commandQualifier == REFRESH_UICC_RESET)) {
    //     return true;
    // } else {
    //     return false;
    // }
    assert(0);
    return NOERROR;
}

ECode CatCmdMessage::GetSetEventList(
    /* [out] */ ISetupEventListSettings** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSetupEventListSettings;
    assert(0);
    return NOERROR;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
